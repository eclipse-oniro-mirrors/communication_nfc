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
#include <mutex>
#include <vector>
#include "ndef_utils.h"
#include "nfa_api.h"
#include "nfa_rw_api.h"
#include "nfc_config.h"
#include "synchronize_event.h"
#include "tag_host.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class TagNciAdapter final {
public:
    static TagNciAdapter& GetInstance();
    TagNciAdapter();
    ~TagNciAdapter();

#if (NXP_EXTNS == TRUE)
    enum TagState {
        IDLE = 0,
        SLEEP,
        ACTIVE,
        INACTIVE
    };
#else
    enum TagState { IDLE = 0, SLEEP, ACTIVE };
#endif

    typedef struct MultiTagParams {
        tNFC_RESULT_DEVT discNtf;
        tNFC_INTF_PARAMS intfParam;
        bool isSkipIsoDepAct = false;
    } MultiTagParams;

    void HandleSelectResult(uint8_t status);
    void HandleTranceiveData(uint8_t status, uint8_t* data, uint32_t dataLen);
    void HandleReadComplete(uint8_t status);
    void HandleWriteComplete(uint8_t status);
    void HandleFormatComplete(uint8_t status);
    void HandleNdefCheckResult(uint8_t status, uint32_t currentSize, uint32_t flag, uint32_t maxSize);
    void HandleActivatedResult(tNFA_CONN_EVT_DATA* data);
    void HandleDeactivatedResult(tNFA_DEACTIVATE_TYPE deactType);
    void HandleFieldCheckResult(uint8_t status);
    void HandleSetReadOnlyResult(tNFA_STATUS status);
    bool IsReconnecting();
    void BuildTagInfo(tNFA_ACTIVATED activated);
    void HandleDiscResult(tNFA_CONN_EVT_DATA* eventData);
    void SetDeactivatedStatus();
    void SetConnectStatus(bool isStatusOk);

    // tag connection and read or write.
    // interfaces for nfc host
    tNFA_STATUS Connect(uint32_t idx);
    bool Disconnect();
    bool Reconnect();
    int Transceive(const std::string& request, std::string& response);
    void SetTimeout(const uint32_t timeout, const uint32_t technology);
    uint32_t GetTimeout(uint32_t technology) const;
    void ResetTimeout();
    void ResetTag();

    // functions for ndef tag only.
    void RegisterNdefHandler();
    void ReadNdef(std::string& response);
    bool WriteNdef(std::string& ndefMessage);
    bool IsNdefFormatable();
    bool FormatNdef();
    bool SetReadOnly() const;
    bool DetectNdefInfo(std::vector<int>& ndefInfo);
    bool IsNdefFormattable();

    // functions for checking the tag field on or not.
    bool IsTagFieldOn();
    void ResetTagFieldOnFlag();
    void OnRfDiscLock();
    void OffRfDiscLock();
    bool IsTagDeactivating();

    void AbortWait();

    // functions for multiple protocol tag
    void SetIsMultiTag(bool isMultiTag);
    bool GetIsMultiTag() const;
    void SetDiscRstEvtNum(uint32_t num);
    uint32_t GetDiscRstEvtNum() const;
    void GetMultiTagTechsFromData(const tNFA_DISC_RESULT& discoveryData);
    void SelectTheFirstTag();
    void SelectTheNextTag();

    // Sync tag connection status from NfccNciAdapter
    void SetCurrRfInterface(uint32_t rfInterface);
    void SetCurrRfProtocol(uint32_t protocol);
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
    bool IsExpectedActRfProtocol(uint32_t protocol);

    /* method for SAK28 issue */
    void SetSkipMifareInterface();

    // multiple protocol tag special status
    bool isIsoDepDhReqFailed_ = false;
   
private:
    uint32_t GetT1tMaxMessageSize(tNFA_ACTIVATED activated) const;
    std::string GetUidFromData(tNFA_ACTIVATED activated) const;
    tNFA_INTF_TYPE GetRfInterface(uint32_t protocol) const;
    bool IsTagActive() const;
    bool IsDiscTypeA(uint8_t discType) const;
    bool IsDiscTypeB(uint8_t discType) const;
    bool IsDiscTypeF(uint8_t discType) const;
    bool IsDiscTypeV(uint8_t discType) const;

    std::string GetTechPollForTypeB(tNFC_RF_TECH_PARAMS nfcRfTechParams, uint32_t tech);
    std::string GetTechActForIsoDep(tNFA_ACTIVATED activated, tNFC_RF_TECH_PARAMS nfcRfTechParams, uint32_t tech) const;
    void GetTechFromData(tNFA_ACTIVATED activated);
    void GetTechPollFromData(tNFA_ACTIVATED activated);
    void GetTechActFromData(tNFA_ACTIVATED activated);
    void ParseSpecTagType(tNFA_ACTIVATED activated);
    void SetMultiTagData(tNFC_RESULT_DEVT& discNtf);
    // spacial card
    bool IsT2TNackRsp(const uint8_t* response, uint32_t responseLen);

    // mifare
    bool IsMifareConnected();
    bool IsTagDetectedInTimeDiff(uint32_t timeDiff);
    tNFA_STATUS HandleMfcTransceiveData(std::string& response);

    bool NfaDeactivateAndSelect(int discId, int protocol, tNFA_INTF_TYPE rfInterface);
    bool Reselect(tNFA_INTF_TYPE rfInterface, bool isSwitchingIface);
    bool SendReselectReqIfNeed(int protocol, int tech);
    tNFA_STATUS DoSelectForMultiTag(uint32_t currIdx);
    tNFA_STATUS SendRawFrameForHaltPICC();

    // methods for SAK28 issue
    void ClearNonStdTagData();
    bool SkipProtoActivateIfNeed(tNFC_PROTOCOL protocol);
    void SetNonStdTagData();

    void DoNfaNdefRegisterEvt(tNFA_NDEF_EVT_DATA* eventData);
    void DoNfaNdefDataEvt(tNFA_NDEF_EVT_DATA* eventData);

    // static callback functions regiter to nci stack.
    static void NdefCallback(uint8_t event, tNFA_NDEF_EVT_DATA* eventData);

private:
    // synchronized lock
    std::mutex rfDiscoveryMutex_;
    OHOS::NFC::SynchronizeEvent transceiveEvent_;
    OHOS::NFC::SynchronizeEvent fieldCheckEvent_;
    OHOS::NFC::SynchronizeEvent readNdefEvent_;
    OHOS::NFC::SynchronizeEvent writeNdefEvent_;
    OHOS::NFC::SynchronizeEvent formatNdefEvent_;
    OHOS::NFC::SynchronizeEvent checkNdefEvent_;
    OHOS::NFC::SynchronizeEvent selectEvent_;
    OHOS::NFC::SynchronizeEvent activatedEvent_;
    OHOS::NFC::SynchronizeEvent deactivatedEvent_;
    OHOS::NFC::SynchronizeEvent setReadOnlyEvent_;
    OHOS::NFC::SynchronizeEvent reconnectEvent_;

    bool isTagFieldOn_ = false;
    bool isReconnecting_ = false;
    bool isReconnected_ = false;
    bool isInTransceive_ = false;
    bool isTransceiveTimeout_ = false;
    uint32_t t1tMaxMessageSize_ = 0;
    bool isWaitingDeactRst_ = false; // deactive wrating state in reselect command can be modeified only in reselect

    // const values for Mifare Ultralight
    static const uint32_t MANUFACTURER_ID_NXP = 0x04;
    static const uint32_t SAK_MIFARE_UL_1 = 0x00;
    static const uint32_t SAK_MIFARE_UL_2 = 0x04;
    static const uint32_t ATQA_MIFARE_UL_0 = 0x44;
    static const uint32_t ATQA_MIFARE_UL_1 = 0x00;

    // const values for Mifare DESFire
    static const uint32_t SAK_MIFARE_DESFIRE = 0x20;
    static const uint32_t ATQA_MIFARE_DESFIRE_0 = 0x44;
    static const uint32_t ATQA_MIFARE_DESFIRE_1 = 0x03;

    // tag technology data for tag host and nfcservice.
    std::vector<int> tagTechList_ {};
    std::vector<uint32_t> tagRfDiscIdList_ {};          // disc id
    std::vector<uint32_t> tagRfProtocols_ {};           // protocol
    std::vector<std::string> tagPollBytes_ {};
    std::vector<std::string> tagActivatedBytes_ {};     // current tech list index

    // tag connection status data
    uint32_t connectedProtocol_ = 0;
    uint32_t connectedType_ = TagHost::TARGET_TYPE_UNKNOWN;
    uint32_t connectedTechIdx_ = 0;
    uint32_t connectedRfIface_ = NFA_INTERFACE_ISO_DEP;
    TagState tagState_ = IDLE;

    // data for updating connection status
    int targetType_ = 0;
    bool isSwitchingRfIface_ = false;

    // spec tag type
    bool isFelicaLite_ = false;
    bool isMifareUltralight_ = false;
    bool isMifareDESFire_ = false;
    bool isCashbee_ = false;
    tNFA_RW_PRES_CHK_OPTION presChkOption_;

    // ndef checked status.
    uint32_t lastNdefCheckedStatus_ = NFA_STATUS_FAILED;
    bool isNdefCapable_ = false;
    uint32_t lastCheckedNdefSize_ = 0;
    uint32_t lastCheckedNdefMaxSize_ = 0;
    uint32_t lastCheckedNdefMode_ = 0;
    bool isNdefWriteSuccess_ = false;
    bool isNdefFormatSuccess_ = false;
    uint16_t ndefTypeHandle_ = NFA_HANDLE_INVALID;
    std::string readNdefData_ = {};
    bool isNdefReadTimeOut_ = false;
    bool isNdefReading_ = false;
    bool isNdefChecking_ = false;

    // multiple protocol tag values
    bool isMultiTag_ = false;
    uint32_t discRstEvtNum_ = 0; // number of tag, increased with the times of NFA_DISC_RESULT_EVT
                                    // and decreased while selecting next tag
    uint32_t discNtfIndex_ = 0;
    uint32_t multiTagTmpTechIdx_ = 0; // to store the last techlist index for the last tag
    uint32_t selectedTagIdx_ = 0;          // to store the last selected tag index
    std::vector<int> multiTagDiscId_ {};
    std::vector<int> multiTagDiscProtocol_ {};
    uint32_t techListIndex_ = 0;             // current tech list index

    // special vals for special tags
    uint8_t nfcID0_[4] {}; // Type B
    uint8_t nfcID1_[10] {};
    bool isSkipNdefRead_ = false;
    bool isMultiProtoMFC_ = false;
    bool isLegacyMifareReader_ = false;
    bool isMfcTransRspErr_ = false;
    long lastTagFoundTime_ = 0;
    bool isMultiTagSupported_ = false;

    // timeout and time diffs
    int technologyTimeoutsTable_[MAX_NUM_TECHNOLOGY] = {0}; // index equals to the technology value
    std::vector<uint32_t> multiTagTimeDiff_ {};
    bool isSkipMifareActive_ = false;
    std::basic_string<uint8_t> receivedData_ {};
    // values for SAK28 issue
    int g_selectedIdx = 0;
#if (NXP_EXTNS == TRUE)
    MultiTagParams g_multiTagParams;
#endif
    uint8_t firstUid[NCI_NFCID1_MAX_LEN] = {0};
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_NCI_ADAPTER_H
