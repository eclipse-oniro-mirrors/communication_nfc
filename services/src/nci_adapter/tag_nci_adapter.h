/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef TAG_NCI_ADAPTER_H
#define TAG_NCI_ADAPTER_H

#include <memory>
#include <mutex>
#include <vector>

#include "infc_nci.h"
#include "nfa_api.h"
#include "nfa_rw_api.h"
#include "synchronize_event.h"
#include "tag_host.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class TagNciAdapter final {
public:
    static TagNciAdapter& GetInstance();
    void SetNciAdaptations(std::shared_ptr<INfcNci> nciAdaptations);

    static void HandleSelectResult();
    static void HandleTranceiveData(unsigned char status, unsigned char* data, int dataLen);
    static void HandleReadComplete(unsigned char status);
    static void HandleWriteComplete(unsigned char status);
    static void HandleFormatComplete(unsigned char status);
    static void HandleNdefCheckResult(unsigned char status, int currentSize, uint32_t flag, int maxSize);
    static void HandleActivatedResult();
    static void HandleDeactivatedResult();
    static void HandleFieldCheckResult(unsigned char status);
    static bool IsReconnecting();
    void HandleDiscResult(tNFA_CONN_EVT_DATA* eventData);

    // tag connection and read or write.
    void BuildTagInfo(const tNFA_CONN_EVT_DATA* eventData);
    tNFA_STATUS Connect(int discId, int protocol, int tech);
    bool Disconnect();
    bool Reconnect(int discId, int protocol, int tech, bool restart);
    bool NfaDeactivateAndSelect(int discId, int protocol);
    int Transceive(std::string& request, std::string& response);
    int GetTimeout(int technology) const;
    void ResetTimeout();
    void ResetTag();

    // functions for ndef tag only.
    void RegisterNdefHandler();
    void ReadNdef(std::string& response);
    bool WriteNdef(std::string& ndefMessage);
    bool IsNdefFormatable();
    bool FormatNdef();
    bool SetReadOnly() const;
    bool IsNdefMsgContained(std::vector<int>& ndefInfo);
    bool IsNdefFormattable();

    // functions for checking the tag field on or not.
    bool IsTagFieldOn();
    void ResetTagFieldOnFlag();
    void OnRfDiscLock();
    void OffRfDiscLock();

    static void AbortWait();

    // functions for multiple protocol tag
    void SetIsMultiTag(bool isMultiTag);
    bool GetIsMultiTag() const;
    void SetDiscRstEvtNum(uint32_t num);
    uint32_t GetDiscRstEvtNum() const;
    void GetMultiTagTechsFromData(const tNFA_DISC_RESULT& discoveryData);
    void SelectTheFirstTag();
    void SelectTheNextTag();

private:
    TagNciAdapter();
    ~TagNciAdapter();
    int GetT1tMaxMessageSize(tNFA_ACTIVATED activated) const;
    std::string GetUidFromData(tNFA_ACTIVATED activated) const;
    tNFA_INTF_TYPE GetRfInterface(int protocol) const;
    bool IsTagActive() const;
    bool IsDiscTypeA(char discType) const;
    bool IsDiscTypeB(char discType) const;
    bool IsDiscTypeF(char discType) const;
    bool IsDiscTypeV(char discType) const;

    std::string GetTechPollForTypeB(tNFC_RF_TECH_PARAMS nfcRfTechParams, int tech);
    std::string GetTechActForIsoDep(tNFA_ACTIVATED activated, tNFC_RF_TECH_PARAMS nfcRfTechParams, int tech) const;
    void GetTechFromData(tNFA_ACTIVATED activated);
    void GetTechPollFromData(tNFA_ACTIVATED activated);
    void GetTechActFromData(tNFA_ACTIVATED activated);
    void ParseSpecTagType(tNFA_ACTIVATED activated);
    static void NdefCallback(unsigned char event, tNFA_NDEF_EVT_DATA* eventData);

    bool Reselect(tNFA_INTF_TYPE rfInterface);
    bool SendReselectReqIfNeed(int protocol, int tech);
    tNFA_STATUS DoSelectForMultiTag(int currIdx);

    // synchronized lock
    static std::mutex rfDiscoveryMutex_;
    static OHOS::NFC::SynchronizeEvent transceiveEvent_;
    static OHOS::NFC::SynchronizeEvent filedCheckEvent_;
    static OHOS::NFC::SynchronizeEvent readNdefEvent_;
    static OHOS::NFC::SynchronizeEvent writeNdefEvent_;
    static OHOS::NFC::SynchronizeEvent formatNdefEvent_;
    static OHOS::NFC::SynchronizeEvent checkNdefEvent_;
    static OHOS::NFC::SynchronizeEvent selectEvent_;
    static OHOS::NFC::SynchronizeEvent activatedEvent_;
    static OHOS::NFC::SynchronizeEvent deactivatedEvent_;

    static bool isTagFieldOn_;
    static bool isReconnecting_;
    static bool isInTransceive_;
    static int t1tMaxMessageSize_;
    static std::string receivedData_;

    // const values for Mifare Ultralight
    static const int MANUFACTURER_ID_NXP = 0x04;
    static const int SAK_MIFARE_UL_1 = 0x00;
    static const int SAK_MIFARE_UL_2 = 0x04;
    static const int ATQA_MIFARE_UL_0 = 0x44;
    static const int ATQA_MIFARE_UL_1 = 0x00;

    // const values for Mifare DESFire
    static const int SAK_MIFARE_DESFIRE = 0x20;
    static const int ATQA_MIFARE_DESFIRE_0 = 0x44;
    static const int ATQA_MIFARE_DESFIRE_1 = 0x03;

    // tag technology and protocols discovery.
    static const uint32_t MAX_NUM_TECHNOLOGY = 12;
    int technologyTimeoutsTable_[MAX_NUM_TECHNOLOGY + 1] {}; // index equals to the technology value.

    static std::shared_ptr<INfcNci> nciAdaptations_;
    std::vector<int> tagTechList_ {};           // tag type
    std::vector<int> tagRfDiscIdList_ {};       // disc id
    std::vector<int> tagActivatedProtocols_ {}; // protocol
    std::vector<std::string> tagPollBytes_ {};
    std::vector<std::string> tagActivatedBytes_ {};
    uint32_t techListIndex_;                             // current tech list index
    std::vector<int> tagDiscIdListOfDiscResult_ {}; // disc id
    std::vector<int> tagProtocolsOfDiscResult_ {};  // protocol
    int tagActivatedProtocol_;
    static int connectedProtocol_;
    static int connectedTargetType_;
    static int connectedTagDiscId_;

    // spec tag type
    bool isFelicaLite_;
    bool isMifareUltralight_;
    bool isMifareDESFire_;
    tNFA_RW_PRES_CHK_OPTION presChkOption_;

    // ndef checked status.
    static int lastNdefCheckedStatus_;
    static bool isNdefCapable_;
    static int lastCheckedNdefSize_;
    static int lastCheckedNdefMaxSize_;
    static int lastCheckedNdefMode_;
    static bool isNdefWriteSuccess_;
    static bool isNdefFormatSuccess_;
    static unsigned short int ndefTypeHandle_;
    static std::string readNdefData;

    // multiple protocol tag values
    bool isMultiTag_;
    uint32_t discRstEvtNum_; // number of tag, increased with the times of NFA_DISC_RESULT_EVT
                             // and decreased while selecting next tag
    uint32_t discNtfIndex_;
    uint32_t multiTagTmpTechIdx_; // to store the last techlist index for the last tag
    unsigned int selectedTagIdx_;          // to store the last selected tag index
    int multiTagDiscId_[MAX_NUM_TECHNOLOGY] {};
    int multiTagDiscProtocol_[MAX_NUM_TECHNOLOGY] {};
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_NCI_ADAPTER_H
