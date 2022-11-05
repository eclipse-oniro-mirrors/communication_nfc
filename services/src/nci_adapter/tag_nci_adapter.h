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
    void HandleDiscResult(tNFA_CONN_EVT_DATA* eventData);

    // tag connection and read or write.
    void BuildTagInfo(const tNFA_CONN_EVT_DATA* eventData);
    tNFA_STATUS Connect(int discId, int protocol, int tech);
    bool Disconnect();
    bool Reconnect(int discId, int protocol, int tech, bool restart);
    int Transceive(std::string& request, std::string& response);
    int GetTimeout(int technology) const;
    void ResetTimeout();
    void ResetTag();

    // functions for nedf tag only.
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
private:
    TagNciAdapter();
    ~TagNciAdapter();
    int GetT1tMaxMessageSize(tNFA_ACTIVATED activated) const;
    std::string GetUidFromData(tNFA_ACTIVATED activated) const;
    tNFA_INTF_TYPE GetRfInterface(int protocol) const;
    bool IsTagActive() const;

    std::string GetTechPollForTypeB(tNFC_RF_TECH_PARAMS nfcRfTechParams, int tech);
    std::string GetTechActForIsoDep(tNFA_ACTIVATED activated, tNFC_RF_TECH_PARAMS nfcRfTechParams, int tech);
    void GetTechFromData(tNFA_ACTIVATED activated);
    void GetTechPollFromData(tNFA_ACTIVATED activated);
    void GetTechActFromData(tNFA_ACTIVATED activated);
    void ParseSpecTagType(tNFA_ACTIVATED activated);
    static void NdefCallback(unsigned char event, tNFA_NDEF_EVT_DATA* eventData);

    bool Reselect(tNFA_INTF_TYPE rfInterface);
    bool SendReselectReqIfNeed(int protocol, int tech);

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
    static bool isReconnect_;
    static bool isInTransceive_;
    static int t1tMaxMessageSize_;
    static std::string receivedData_;

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
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_NCI_ADAPTER_H
