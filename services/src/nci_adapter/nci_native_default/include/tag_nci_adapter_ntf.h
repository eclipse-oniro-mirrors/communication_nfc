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
#ifndef TAG_NCI_ADAPTER_NTF_H
#define TAG_NCI_ADAPTER_NTF_H
#include <mutex>
#include <vector>
#include "ndef_utils.h"
#include "nfa_api.h"
#include "nfa_rw_api.h"
#include "nfc_config.h"
#include "synchronize_event.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class TagNciAdapterNtf final {
public:
    static TagNciAdapterNtf& GetInstance();
    TagNciAdapterNtf();
    ~TagNciAdapterNtf();

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
    void HandleActivatedResult(tNFA_CONN_EVT_DATA* data);
    void HandleSetReadOnlyResult(tNFA_STATUS status);
    bool IsReconnecting();
    void BuildTagInfo(tNFA_ACTIVATED activated);
    void HandleDiscResult(tNFA_CONN_EVT_DATA* eventData);
    void SetDeactivatedStatus();
    void SetConnectStatus(bool isStatusOk);
    void SetCurrRfInterface(uint32_t rfInterface);
    // tag connection and read or write.
    // interfaces for nfc host
    // functions for ndef tag only.
    void RegisterNdefHandler();
    // functions for multiple protocol tag
    void SetIsMultiTag(bool isMultiTag);
    bool GetIsMultiTag() const;
    void SetDiscRstEvtNum(uint32_t num);
    uint32_t GetDiscRstEvtNum() const;
    void GetMultiTagTechsFromData(const tNFA_DISC_RESULT& discoveryData);
    void SelectTheFirstTag();
    void SelectTheNextTag();
    // Sync tag connection status from NfccNciAdapter
    void SetCurrRfProtocol(uint32_t protocol);
#if (NXP_EXTNS == TRUE)
    void SetNfcID0ForTypeB(uint8_t* nfcID0);
    void SetCurrRfMode(uint8_t type);
    bool IsMultiMFCTag();
#endif
    void ResetTagState();
    bool IsSwitchingRfIface();
    bool IsExpectedActRfProtocol(uint32_t protocol);
    /* method for SAK28 issue */
    void SetSkipMifareInterface();
#if (NXP_EXTNS == TRUE)
    void ClearMultiMFCTagState();
#endif
    void SetTagActivated();
    void ResetTagFieldOnFlag();
    void SetTagDeactivated(bool isSleep);
    void HandleDeactivatedResult(tNFA_DEACTIVATE_TYPE deactType);
    void AbortWait();
    bool IsTagDeactivating();
    void HandleFieldCheckResult(uint8_t status);
    void HandleNdefCheckResult(uint8_t status, uint32_t currentSize, uint32_t flag, uint32_t maxSize);
private:
    uint32_t GetT1tMaxMessageSize(tNFA_ACTIVATED activated) const;
    std::string GetUidFromData(tNFA_ACTIVATED activated) const;
    tNFA_INTF_TYPE GetRfInterface(uint32_t protocol) const;
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
    // mifare
    bool IsTagDetectedInTimeDiff(uint32_t timeDiff);
    bool NfaDeactivateAndSelect(int discId, int protocol, tNFA_INTF_TYPE rfInterface);
    bool SendReselectReqIfNeed(int protocol, int tech);
    tNFA_STATUS DoSelectForMultiTag(uint32_t currIdx);
    void IsMultiTag(tNFC_RESULT_DEVT discoveryNtf, uint8_t *nfcID2, uint8_t nfcID2Len);
    bool IsMifareUL(tNFA_ACTIVATED activated);
    void SetIsoDepFwt(tNFA_ACTIVATED activated, uint32_t technology);

    // methods for SAK28 issue
    void ClearNonStdTagData();
    bool SkipProtoActivateIfNeed(tNFC_PROTOCOL protocol);
    void SetNonStdTagData();
    void DoNfaNdefRegisterEvt(tNFA_NDEF_EVT_DATA* eventData);
    void DoNfaNdefDataEvt(tNFA_NDEF_EVT_DATA* eventData);

    // static callback functions regiter to nci stack.
    static void NdefCallback(uint8_t event, tNFA_NDEF_EVT_DATA* eventData);

private:
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

    // data for updating connection status
    int targetType_ = 0;
    uint16_t ndefTypeHandle_ = NFA_HANDLE_INVALID;

    // special vals for special tags
    uint8_t nfcID1_[10] {};

    // timeout and time diffs
    std::vector<uint32_t> multiTagTimeDiff_ {};
    bool isSkipMifareActive_ = false;
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
#endif  // TAG_NCI_ADAPTER_NTF_H
