/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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
    ~TagNciAdapter();

#if (NXP_EXTNS == TRUE)
    enum TagState {
        IDLE,
        SLEEP,
        ACTIVE,
        INACTIVE
    };
#else
    enum TagState { IDLE, SLEEP, ACTIVE };
#endif
    typedef struct MultiTagParams {
        tNFC_RESULT_DEVT discNtf;
        tNFC_INTF_PARAMS intfParam;
        bool isSkipIsoDepAct = false;
    } MultiTagParams;

    void SetNciAdaptations(std::shared_ptr<INfcNci> nciAdaptations);
    static void HandleSelectResult();
    static void HandleTranceiveData(unsigned char status, unsigned char* data, int dataLen);
    static void HandleReadComplete(unsigned char status);
    static void HandleWriteComplete(unsigned char status);
    static void HandleFormatComplete(unsigned char status);
    static void HandleNdefCheckResult(unsigned char status, int currentSize, uint32_t flag, int maxSize);
    static void HandleActivatedResult(tNFA_CONN_EVT_DATA* data);
    static void HandleDeactivatedResult(tNFA_DEACTIVATE_TYPE deactType);
    static void HandleFieldCheckResult(unsigned char status);
    static void HandleSetReadOnlyResult(tNFA_STATUS status);
    static bool IsReconnecting();
    static void BuildTagInfo(tNFA_ACTIVATED activated);
    
    void HandleDiscResult(tNFA_CONN_EVT_DATA* eventData);
    void SetDeactivatedStatus();
    void SetConnectStatus(bool isStatusOk);

    // tag connection and read or write.
    // interfaces for nfc host
    tNFA_STATUS Connect(int idx);
    bool Disconnect();
    bool Reconnect();
    int Transceive(std::string& request, std::string& response);
    static void SetTimeout(int& timeout, int& technology);
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
    bool IsTagDeactivating();

    static void AbortWait();

    // functions for multiple protocol tag
    void SetIsMultiTag(bool isMultiTag);
    bool GetIsMultiTag() const;
    void SetDiscRstEvtNum(uint32_t num);
    uint32_t GetDiscRstEvtNum() const;
    void GetMultiTagTechsFromData(const tNFA_DISC_RESULT& discoveryData);
    void SelectTheFirstTag();
    void SelectTheNextTag();

    // Sync tag connection status from NfccNciAdapter
    void SetCurrRfInterface(int rfInterface);
    void SetCurrRfProtocol(int protocol);
#if (NXP_EXTNS == TRUE)
    void SetCurrRfMode(uint8_t type);
    void SetNfcID0ForTypeB(uint8_t* nfcID0);
    bool IsMultiMFCTag();
    void ClearMultiMFCTagState();
#endif
    void SetTagActivated();
    void ResetTagState();
    void SetTagDeactivated(bool isSleep);
    TagState GetTagState();
    bool IsSwitchingRfIface();
    bool IsExpectedActRfProtocol(int protocol);

    /* method for SAK28 issue */
    static void SetSkipMifareInterface();

    // multiple protocol tag special status
    bool isIsoDepDhReqFailed_ = false;
   
private:
    TagNciAdapter();
    static int GetT1tMaxMessageSize(tNFA_ACTIVATED activated);
    static std::string GetUidFromData(tNFA_ACTIVATED activated);
    tNFA_INTF_TYPE GetRfInterface(int protocol) const;
    bool IsTagActive() const;
    static bool IsDiscTypeA(char discType);
    static bool IsDiscTypeB(char discType);
    static bool IsDiscTypeF(char discType);
    static bool IsDiscTypeV(char discType);
    static std::string GetTechPollForTypeB(tNFC_RF_TECH_PARAMS nfcRfTechParams, int tech);
    static std::string GetTechActForIsoDep(tNFA_ACTIVATED activated, tNFC_RF_TECH_PARAMS nfcRfTechParams, int tech);
    static void GetTechFromData(tNFA_ACTIVATED activated);
    static void GetTechPollFromData(tNFA_ACTIVATED activated);
    static void GetTechActFromData(tNFA_ACTIVATED activated);
    static void ParseSpecTagType(tNFA_ACTIVATED activated);
    void SetMultiTagData(tNFC_RESULT_DEVT& discNtf);

    // spacial card
    bool IsT2TNackRsp(const uint8_t* response, uint32_t responseLen);

    // mifare
    static bool IsMifareConnected();
    bool IsTagDetectedInTimeDiff(uint32_t timeDiff);
    tNFA_STATUS HandleMfcTransceiveData(std::string& response);
    static void NdefCallback(unsigned char event, tNFA_NDEF_EVT_DATA* eventData);

    bool NfaDeactivateAndSelect(int discId, int protocol, tNFA_INTF_TYPE rfInterface);
    bool Reselect(tNFA_INTF_TYPE rfInterface, bool isSwitchingIface);
    bool SendReselectReqIfNeed(int protocol, int tech);
    tNFA_STATUS DoSelectForMultiTag(int currIdx);
    tNFA_STATUS SendRawFrameForHaltPICC();

    // methods for SAK28 issue
    static void ClearNonStdTagData();
    static bool SkipProtoActivateIfNeed(tNFC_PROTOCOL protocol);
    static void SetNonStdTagData();

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
    static OHOS::NFC::SynchronizeEvent setReadOnlyEvent_;
    static OHOS::NFC::SynchronizeEvent reconnectEvent_;

    static bool isTagFieldOn_;
    static bool isReconnecting_;
    static bool isReconnected_;
    static bool isInTransceive_;
    static bool isTransceiveTimeout_;
    static int t1tMaxMessageSize_;
    static bool isWaitingDeactRst_; // deactive wrating state in reselect command can be modeified only in reselect

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

    static const uint32_t MAX_NUM_TECHNOLOGY = 12;

    // tag technology data for tag host and nfcservice.
    static std::vector<int> tagTechList_;
    static std::vector<int> tagRfDiscIdList_;           // disc id
    static std::vector<int> tagRfProtocols_;            // protocol
    static std::vector<std::string> tagPollBytes_;
    static std::vector<std::string> tagActivatedBytes_; // current tech list index

    static std::shared_ptr<INfcNci> nciAdaptations_;

    // tag connection status data
    static int connectedProtocol_;
    static int connectedType_;
    static int connectedTechIdx_;
    static int connectedRfIface_;
    TagState tagState_;

    // data for updating connection status
    int targetType_;
    bool isSwitchingRfIface_ = false;

    // spec tag type
    static bool isFelicaLite_;
    static bool isMifareUltralight_;
    static bool isMifareDESFire_;
    bool isCashbee_;
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
    static bool isNdefReadTimeOut_;
    static bool isNdefReading_;
    static bool isNdefChecking_;

    // multiple protocol tag values
    static bool isMultiTag_;
    static uint32_t discRstEvtNum_; // number of tag, increased with the times of NFA_DISC_RESULT_EVT
                                    // and decreased while selecting next tag
    uint32_t discNtfIndex_;
    static uint32_t multiTagTmpTechIdx_; // to store the last techlist index for the last tag
    static int selectedTagIdx_;          // to store the last selected tag index
    std::vector<int> multiTagDiscId_ {};
    std::vector<int> multiTagDiscProtocol_ {};
    static uint32_t techListIndex_;             // current tech list index

    // special vals for special tags
    uint8_t nfcID0_[4] {}; // Type B
    uint8_t nfcID1_[10] {};
    bool isSkipNdefRead_ = false;
    bool isMultiProtoMFC_ = false;
    static bool isLegacyMifareReader_;
    static bool isMfcTransRspErr_;
    long lastTagFoundTime_ = 0;
    bool isMultiTagSupported_ = false;

    // timeout and time diffs
    static int technologyTimeoutsTable_[]; // index equals to the technology value
    std::vector<uint32_t> multiTagTimeDiff_ {};
    static bool isSkipMifareActive_;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_NCI_ADAPTER_H
