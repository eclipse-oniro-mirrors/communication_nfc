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
#include "tag_nci_adapter_ntf.h"
#include "tag_nci_adapter_common.h"
#include "tag_nci_adapter_rw.h"
#include <unistd.h>
#include "nfc_brcm_defs.h"
#include "nfc_config.h"
#include "nfc_sdk_common.h"
#include "nfcc_nci_adapter.h"
#include "loghelper.h"
#include "nfa_api.h"
#include "rw_int.h"
#include "securec.h"
#include "tag_native_impl.h"

namespace OHOS {
namespace NFC {
namespace NCI {
static const uint32_t POS_NFCF_STSTEM_CODE_HIGH = 8;
static const uint32_t POS_NFCF_STSTEM_CODE_LOW = 9;
static const uint32_t TOPAZ512_MAX_MESSAGE_SIZE = 462;
static const uint32_t TOPAZ96_MAX_MESSAGE_SIZE = 90;
static const uint32_t SENSF_RES_LENGTH = 8;
static const uint32_t SENS_RES_LENGTH = 2;
static const uint32_t SENSB_RES_POLL_POS = 4;
static const uint32_t SYSTEM_CODE_SHIFT = 8;
static const uint32_t F_POLL_LENGTH = 10;
static const uint32_t I93_POLL_LENGTH = 2;
static const uint32_t I93_ACT_LENGTH = 2;
static const uint32_t INVALID_TAG_INDEX = 0xFF;
static const uint32_t TIME_MUL_100MS = 100; // ms
static const uint8_t MIN_FWI = 0;  // min waiting time integer for protocol frame
static const uint8_t MAX_FWI = 14; // max waiting time integer for protocol frame
static const uint8_t NON_STD_CARD_SAK = 0x13;

#define g_commonMultiTagDiscId (TagNciAdapterCommon::GetInstance().multiTagDiscId_)
#define g_commonMultiTagDiscProtocol (TagNciAdapterCommon::GetInstance().multiTagDiscProtocol_)
#define g_commonTechListIndex (TagNciAdapterCommon::GetInstance().techListIndex_)
#define g_commonMultiTagTmpTechIdx (TagNciAdapterCommon::GetInstance().multiTagTmpTechIdx_)
#define g_commonDiscRstEvtNum (TagNciAdapterCommon::GetInstance().discRstEvtNum_)
#define g_commonSelectedTagIdx (TagNciAdapterCommon::GetInstance().selectedTagIdx_)
#define g_commonConnectedProtocol (TagNciAdapterCommon::GetInstance().connectedProtocol_)
#define g_commonIsLegacyMifareReader (TagNciAdapterCommon::GetInstance().isLegacyMifareReader_)
#define g_commonIsNdefReadTimeOut (TagNciAdapterCommon::GetInstance().isNdefReadTimeOut_)
#define g_commonReconnectEvent (TagNciAdapterCommon::GetInstance().reconnectEvent_)
#define g_commonIsReconnecting (TagNciAdapterCommon::GetInstance().isReconnecting_)
#define g_commonIsSwitchingRfIface (TagNciAdapterCommon::GetInstance().isSwitchingRfIface_)
#define g_commonIsIsoDepDhReqFailed (TagNciAdapterCommon::GetInstance().isIsoDepDhReqFailed_)
#define g_commonConnectedTechIdx (TagNciAdapterCommon::GetInstance().connectedTechIdx_)
#define g_commonIsReconnected (TagNciAdapterCommon::GetInstance().isReconnected_)
#define g_commonReadNdefData (TagNciAdapterCommon::GetInstance().readNdefData_)
#define g_commonReadNdefEvent (TagNciAdapterCommon::GetInstance().readNdefEvent_)
#define g_commonIsNdefReading (TagNciAdapterCommon::GetInstance().isNdefReading_)
#define g_commonT1tMaxMessageSize (TagNciAdapterCommon::GetInstance().t1tMaxMessageSize_)
#define g_commonIsNdefWriteSuccess (TagNciAdapterCommon::GetInstance().isNdefWriteSuccess_)
#define g_commonWriteNdefEvent (TagNciAdapterCommon::GetInstance().writeNdefEvent_)
#define g_commonFormatNdefEvent (TagNciAdapterCommon::GetInstance().formatNdefEvent_)
#define g_commonIsNdefFormatSuccess (TagNciAdapterCommon::GetInstance().isNdefFormatSuccess_)
#define g_commonSelectEvent (TagNciAdapterCommon::GetInstance().selectEvent_)
#define g_commonSetReadOnlyEvent (TagNciAdapterCommon::GetInstance().setReadOnlyEvent_)
#define g_commonConnectedType (TagNciAdapterCommon::GetInstance().connectedType_)
#define g_commonConnectedRfIface (TagNciAdapterCommon::GetInstance().connectedRfIface_)
#define g_commonNfcID0 (TagNciAdapterCommon::GetInstance().nfcID0_)

TagNciAdapterNtf::TagNciAdapterNtf()
{
    if (NfcConfig::hasKey(NAME_NXP_NON_STD_CARD_TIMEDIFF)) {
        std::vector<uint8_t> nonStdTimeDiff = NfcConfig::getBytes(NAME_NXP_NON_STD_CARD_TIMEDIFF);
        for (uint8_t i = 0; i < nonStdTimeDiff.size(); i++) {
            multiTagTimeDiff_.push_back(nonStdTimeDiff.at(i) * TIME_MUL_100MS);
            DebugLog("TagNciAdapterNtf::TagNciAdapterNtf: timediff[%{public}d] = %{public}d", i, nonStdTimeDiff.at(i));
        }
    } else {
        DebugLog("TagNciAdapterNtf::TagNciAdapterNtf:timediff not configured, use default");
        multiTagTimeDiff_.push_back(100); // default time diff for Mifare Tag
        multiTagTimeDiff_.push_back(300); // default time diff for ISODEP
    }
#if (NXP_EXTNS == TRUE)
    errno_t err = memset_s(&g_multiTagParams, sizeof(g_multiTagParams), 0, sizeof(g_multiTagParams));
    if (err != EOK) {
        ErrorLog("TagNciAdapterNtf::TagNciAdapterNtf:memset_s for g_multiTagParams error: %{public}d", err);
    }
#endif
}

TagNciAdapterNtf::~TagNciAdapterNtf()
{
    g_commonConnectedProtocol = NCI_PROTOCOL_UNKNOWN;
    isSkipMifareActive_ = false;
};

TagNciAdapterNtf& TagNciAdapterNtf::GetInstance()
{
    static TagNciAdapterNtf tagNciAdapterNtf;
    return tagNciAdapterNtf;
}

void TagNciAdapterNtf::DoNfaNdefRegisterEvt(tNFA_NDEF_EVT_DATA* eventData)
{
    DebugLog("NdefCallback: NFA_NDEF_REGISTER_EVT; status=0x%{public}X; handle=0x%{public}X",
        eventData->ndef_reg.status, eventData->ndef_reg.ndef_type_handle);
    ndefTypeHandle_ = eventData->ndef_reg.ndef_type_handle;
}

void TagNciAdapterNtf::DoNfaNdefDataEvt(tNFA_NDEF_EVT_DATA* eventData)
{
    DebugLog("NdefCallback: NFA_NDEF_DATA_EVT; data_len = %u", eventData->ndef_data.len);
    uint32_t ndefDataLen = eventData->ndef_data.len;
    g_commonReadNdefData = KITS::NfcSdkCommon::BytesVecToHexString(
        eventData->ndef_data.p_data, ndefDataLen);
}

void TagNciAdapterNtf::NdefCallback(unsigned char event, tNFA_NDEF_EVT_DATA* eventData)
{
    DebugLog("TagNciAdapterNtf::NdefCallback");
    switch (event) {
        case NFA_NDEF_REGISTER_EVT: {
            TagNciAdapterNtf::GetInstance().DoNfaNdefRegisterEvt(eventData);
            break;
        }
        case NFA_NDEF_DATA_EVT: {
            TagNciAdapterNtf::GetInstance().DoNfaNdefDataEvt(eventData);
            break;
        }
        default: {
            ErrorLog("%{public}s: Unknown event %{public}u", "NdefCallback", event);
            break;
        }
    }
}

void TagNciAdapterNtf::RegisterNdefHandler()
{
    DebugLog("TagNciAdapterNtf::RegisterNdefHandler");
    ndefTypeHandle_ = NFA_HANDLE_INVALID;
    NFA_RegisterNDefTypeHandler(true,
                                NFA_TNF_DEFAULT,
                                reinterpret_cast<uint8_t*>(const_cast<char*>("")),
                                0, NdefCallback);
    if (g_commonIsLegacyMifareReader) {
        Extns::GetInstance().EXTNS_MfcRegisterNDefTypeHandler(NdefCallback);
    }
}

bool TagNciAdapterNtf::IsReconnecting()
{
    return g_commonIsReconnecting;
}

void TagNciAdapterNtf::SetCurrRfInterface(uint32_t rfInterface)
{
    g_commonConnectedRfIface = rfInterface;
}

void TagNciAdapterNtf::SetCurrRfProtocol(uint32_t protocol)
{
    g_commonConnectedProtocol = protocol;
}

void TagNciAdapterNtf::SetCurrRfMode(uint8_t type)
{
    if (type == NFC_DISCOVERY_TYPE_POLL_A || type == NFC_DISCOVERY_TYPE_POLL_A_ACTIVE) {
        g_commonConnectedType = TagNciAdapterCommon::TARGET_TYPE_ISO14443_3A;
    } else if (type == NFC_DISCOVERY_TYPE_POLL_B || type == NFC_DISCOVERY_TYPE_POLL_B_PRIME) {
        g_commonConnectedType = TagNciAdapterCommon::TARGET_TYPE_ISO14443_3B;
    }
}

bool TagNciAdapterNtf::IsSwitchingRfIface()
{
    return g_commonIsSwitchingRfIface;
}

bool TagNciAdapterNtf::IsExpectedActRfProtocol(uint32_t protocol)
{
    InfoLog("IsExpectedActRfProtocol: currentProtocol = %{public}d, targetProtocol = %{public}d",
            g_commonConnectedProtocol, protocol);
    if (g_commonConnectedProtocol != NFC_PROTOCOL_UNKNOWN && g_commonConnectedProtocol != protocol) {
        NFA_Deactivate(false);
        return false;
    }
    return true;
}

void TagNciAdapterNtf::HandleSelectResult(uint8_t status)
{
    DebugLog("TagNciAdapterNtf::HandleSelectResult");
    {
        NFC::SynchronizeGuard guard(g_commonSelectEvent);
        g_commonSelectEvent.NotifyOne();
    }
    if (status != NFA_STATUS_OK) {
        if (g_commonIsSwitchingRfIface) {
            SetConnectStatus(false);
        }
        ErrorLog("TagNciAdapterNtf::HandleSelectResult error: %{public}d", status);
        NFA_Deactivate(false);
    }
}

void TagNciAdapterNtf::ClearNonStdTagData()
{
    InfoLog("ClearNonStdTagData");
    errno_t err = memset_s(&g_multiTagParams, sizeof(g_multiTagParams), 0, sizeof(g_multiTagParams));
    if (err != EOK) {
        ErrorLog("TagNciAdapterNtf::ClearNonStdTagData:memset_s for g_multiTagParams error: %{public}d", err);
    }
}

void TagNciAdapterNtf::SetNonStdTagData()
{
    // skipped detecte time calculation
    tNFC_RESULT_DEVT& info = g_multiTagParams.discNtf;
    info.rf_disc_id = TagNciAdapterCommon::GetInstance().tagRfDiscIdList_[g_commonSelectedTagIdx];
    info.protocol = TagNciAdapterCommon::GetInstance().tagRfProtocols_[g_commonSelectedTagIdx];
    InfoLog("SetNonStdTagData: disc id: %{public}d", info.rf_disc_id);
}

void TagNciAdapterNtf::HandleActivatedResult(tNFA_CONN_EVT_DATA* eventData)
{
    if (eventData->activated.activate_ntf.rf_tech_param.mode >= NCI_DISCOVERY_TYPE_LISTEN_A || //not poll mode
        eventData->activated.activate_ntf.intf_param.type == NFC_INTERFACE_EE_DIRECT_RF) {     // is EE direct rf
        return;
    }
    tNFA_ACTIVATED& activated = eventData->activated;
#if (NXP_EXTNS == TRUE)
    if (TagNciAdapterCommon::GetInstance().isMultiTag_) {
        InfoLog("TagNciAdapterNtf::HandleActivatedResult: copy nonstd tag data");
        ClearNonStdTagData();
        errno_t err = EOK;
        err = memcpy_s(&g_multiTagParams.discNtf.rf_tech_param, sizeof(tNFC_RF_TECH_PARAMS),
                       &activated.activate_ntf.rf_tech_param, sizeof(tNFC_RF_TECH_PARAMS));
        if (err != EOK) {
            ErrorLog("TagNciAdapterNtf::HandleActivatedResult, memcpy rf_tech_param error: %{public}d", err);
        }
        err = memcpy_s(&g_multiTagParams.intfParam, sizeof(tNFC_INTF_PARAMS),
                       &activated.activate_ntf.intf_param, sizeof(tNFC_INTF_PARAMS));
        if (err != EOK) {
            ErrorLog("TagNciAdapterNtf::HandleActivatedResult, memcpy intfParam error: %{public}d", err);
        }
    }
#endif
    // skipped  same kovio detection
    g_commonConnectedProtocol = activated.activate_ntf.protocol;
    g_commonT1tMaxMessageSize = GetT1tMaxMessageSize(activated);
    GetTechFromData(activated);
    BuildTagInfo(activated);
}

void TagNciAdapterNtf::SetConnectStatus(bool isStatusOk)
{
    DebugLog("TagNciAdapterNtf::SetConnectStatus");
    if (Extns::GetInstance().EXTNS_GetConnectFlag() && g_commonIsLegacyMifareReader) {
        DebugLog("TagNciAdapterNtf::SetConnectStatus: ExtnsMfcActivated");
        Extns::GetInstance().EXTNS_MfcActivated();
        Extns::GetInstance().EXTNS_SetConnectFlag(false);
    }
    if (g_commonIsReconnecting) {
        g_commonIsReconnected = isStatusOk;
        g_commonIsReconnecting = false;
        NFC::SynchronizeGuard guard(g_commonReconnectEvent);
        g_commonReconnectEvent.NotifyOne();
    }
}

void TagNciAdapterNtf::SetNfcID0ForTypeB(uint8_t* nfcID0)
{
    int nfcId0Len = 4;
    int err = memcpy_s(g_commonNfcID0, nfcId0Len, &nfcID0[0], nfcId0Len);
    if (err != 0) {
        ErrorLog("TagNciAdapterNtf::SetNfcID0ForTypeB: memcpy_s error: %{public}d", err);
    }
}

void TagNciAdapterNtf::SetDeactivatedStatus()
{
    if (Extns::GetInstance().EXTNS_GetDeactivateFlag() &&
        g_commonIsLegacyMifareReader) {
        DebugLog("TagNciAdapterNtf::SetDeactivatedStatus mifare deactivate");
        Extns::GetInstance().EXTNS_MfcDisconnect();
        Extns::GetInstance().EXTNS_SetDeactivateFlag(false);
    }
    {
        NFC::SynchronizeGuard guard(g_commonReconnectEvent);
        g_commonReconnectEvent.NotifyOne();
    }
}

void TagNciAdapterNtf::HandleSetReadOnlyResult(tNFA_STATUS status)
{
    NFC::SynchronizeGuard guard(g_commonSetReadOnlyEvent);
    g_commonSetReadOnlyEvent.NotifyOne();
}

void TagNciAdapterNtf::HandleReadComplete(uint8_t status)
{
    DebugLog("TagNciAdapterNtf::HandleReadComplete, g_commonIsNdefReading = %{public}d", g_commonIsNdefReading);
    if (!g_commonIsNdefReading) {
        return;
    }
    NFC::SynchronizeGuard guard(g_commonReadNdefEvent);
    if (status != NFA_STATUS_OK) {
        ErrorLog("Read ndef fail");
        g_commonIsNdefReadTimeOut = true;
        g_commonReadNdefData = "";
    }
    g_commonReadNdefEvent.NotifyOne();
}

void TagNciAdapterNtf::HandleWriteComplete(uint8_t status)
{
    DebugLog("TagNciAdapterNtf::HandleWriteComplete");
    NFC::SynchronizeGuard guard(g_commonWriteNdefEvent);
    g_commonIsNdefWriteSuccess = (status == NFA_STATUS_OK);
    g_commonWriteNdefEvent.NotifyOne();
}

void TagNciAdapterNtf::HandleFormatComplete(uint8_t status)
{
    DebugLog("TagNciAdapterNtf::HandleFormatComplete");
    NFC::SynchronizeGuard guard(g_commonFormatNdefEvent);
    g_commonIsNdefFormatSuccess = (status == NFA_STATUS_OK);
    g_commonFormatNdefEvent.NotifyOne();
}

bool TagNciAdapterNtf::IsDiscTypeA(uint8_t discType) const
{
    if (discType == NCI_DISCOVERY_TYPE_POLL_A) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_POLL_A_ACTIVE) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_LISTEN_A) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_LISTEN_A_ACTIVE) {
        return true;
    }
    return false;
}

bool TagNciAdapterNtf::IsDiscTypeB(uint8_t discType) const
{
    if (discType == NCI_DISCOVERY_TYPE_POLL_B) {
        return true;
    }
    if (discType == NFC_DISCOVERY_TYPE_POLL_B_PRIME) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_LISTEN_B) {
        return true;
    }
    if (discType == NFC_DISCOVERY_TYPE_LISTEN_B_PRIME) {
        return true;
    }
    return false;
}

bool TagNciAdapterNtf::IsDiscTypeF(uint8_t discType) const
{
    if (discType == NCI_DISCOVERY_TYPE_POLL_F) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_POLL_F_ACTIVE) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_LISTEN_F) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_LISTEN_F_ACTIVE) {
        return true;
    }
    return false;
}

bool TagNciAdapterNtf::IsDiscTypeV(uint8_t discType) const
{
    if (discType == NCI_DISCOVERY_TYPE_POLL_V) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_LISTEN_ISO15693) {
        return true;
    }
    return false;
}

bool TagNciAdapterNtf::IsMifareUL(tNFA_ACTIVATED activated)
{
    // can also be mifare
    if (activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] == MANUFACTURER_ID_NXP &&
        (activated.activate_ntf.rf_tech_param.param.pa.sel_rsp == SAK_MIFARE_UL_1 ||
        activated.activate_ntf.rf_tech_param.param.pa.sel_rsp == SAK_MIFARE_UL_2)) {
        InfoLog("TagNciAdapterNtf::GetTechFromData: MifareUltralight");
        return true;
    }
    return false;
}

void TagNciAdapterNtf::SetIsoDepFwt(tNFA_ACTIVATED activated, uint32_t technology)
{
    if ((activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) ||
        (activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A_ACTIVE)) {
        // get frame Waiting time Integer(fwi) from activated data
        uint8_t fwi = activated.activate_ntf.intf_param.intf_param.pa_iso.fwi;
        if (fwi <= MAX_FWI) {
            // 2^MIN_FWI * 256 * 16 * 1000 / 13560000 is approximately 618
            int fwt = (1 << (fwi - MIN_FWI)) * 618;
            InfoLog("TagNciAdapterNtf::GetTechFromData timeout = %{public}d, fwi = %{public}0#x", fwt, fwi);
            TagNciAdapterRw::GetInstance().SetTimeout(fwt, technology);
        }
    }
}

void TagNciAdapterNtf::GetTechFromData(tNFA_ACTIVATED activated)
{
    uint32_t tech[MAX_NUM_TECHNOLOGY];
    if (activated.activate_ntf.protocol == NCI_PROTOCOL_T1T) {
        tech[g_commonTechListIndex] = TagNciAdapterCommon::TARGET_TYPE_ISO14443_3A;
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_T2T) {
        tech[g_commonTechListIndex] = TagNciAdapterCommon::TARGET_TYPE_ISO14443_3A;
        if (IsMifareUL(activated)) {
            g_commonTechListIndex++;
            tech[g_commonTechListIndex] = TagNciAdapterCommon::TARGET_TYPE_MIFARE_UL;
        }
#if (NXP_EXTNS == TRUE)
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_T3BT) {
        tech[g_commonTechListIndex] = TagNciAdapterCommon::TARGET_TYPE_ISO14443_3B;
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_T3T) {
#else
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_T3T) {
#endif
        tech[g_commonTechListIndex] = TagNciAdapterCommon::TARGET_TYPE_FELICA;
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_ISO_DEP) {
        tech[g_commonTechListIndex] = TagNciAdapterCommon::TARGET_TYPE_ISO14443_4;
        SetIsoDepFwt(activated, tech[g_commonTechListIndex]);
        // A OR B
        uint8_t discType = activated.activate_ntf.rf_tech_param.mode;
        if (IsDiscTypeA(discType)) {
            g_commonTechListIndex++;
            tech[g_commonTechListIndex] = TagNciAdapterCommon::TARGET_TYPE_ISO14443_3A;
        } else if (IsDiscTypeB(discType)) {
            g_commonTechListIndex++;
            tech[g_commonTechListIndex] = TagNciAdapterCommon::TARGET_TYPE_ISO14443_3B;
        }
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_15693) {
        tech[g_commonTechListIndex] = TagNciAdapterCommon::TARGET_TYPE_V;
    } else if (activated.activate_ntf.protocol == NFC_PROTOCOL_MIFARE) {
        InfoLog("TagNciAdapterNtf::GetTechFromData: MifareClassic");
        Extns::GetInstance().EXTNS_MfcInit(activated);
        tech[g_commonTechListIndex++] = TagNciAdapterCommon::TARGET_TYPE_ISO14443_3A;
        tech[g_commonTechListIndex] = TagNciAdapterCommon::TARGET_TYPE_MIFARE_CLASSIC;
    } else {
        tech[g_commonTechListIndex] = TagNciAdapterCommon::TARGET_TYPE_UNKNOWN;
    }
    g_commonTechListIndex++;

    uint32_t tagRfDiscId = activated.activate_ntf.rf_disc_id;
    uint32_t tagNtfProtocol = activated.activate_ntf.protocol;
    for (uint32_t i = g_commonMultiTagTmpTechIdx; i < g_commonTechListIndex; i++) {
        TagNciAdapterCommon::GetInstance().tagTechList_.push_back(tech[i]);
        TagNciAdapterCommon::GetInstance().tagRfDiscIdList_.push_back(tagRfDiscId);
        TagNciAdapterCommon::GetInstance().tagRfProtocols_.push_back(tagNtfProtocol);
        InfoLog("GetTechFromData: index = %{public}d, tech = %{public}d, RfDiscId = %{public}d, "
            "protocol = %{public}d", i, tech[i], tagRfDiscId, tagNtfProtocol);
    }
}

std::string TagNciAdapterNtf::GetUidFromData(tNFA_ACTIVATED activated) const
{
    std::string uid;
    tNFC_RF_TECH_PARAMS nfcRfTechParams = activated.activate_ntf.rf_tech_param;
    uint8_t discType = nfcRfTechParams.mode;
    if (IsDiscTypeA(discType)) {
        uint32_t nfcid1Len = nfcRfTechParams.param.pa.nfcid1_len;
        uid = KITS::NfcSdkCommon::BytesVecToHexString(nfcRfTechParams.param.pa.nfcid1, nfcid1Len);
    } else if (IsDiscTypeB(discType)) {
        if (activated.activate_ntf.protocol == NFA_PROTOCOL_T3BT) {
            uid = KITS::NfcSdkCommon::BytesVecToHexString(nfcRfTechParams.param.pb.pupiid, NFC_PUPIID_MAX_LEN);
        } else {
            uid = KITS::NfcSdkCommon::BytesVecToHexString(nfcRfTechParams.param.pb.nfcid0, NFC_NFCID0_MAX_LEN);
        }
    } else if (IsDiscTypeF(discType)) {
        uid = KITS::NfcSdkCommon::BytesVecToHexString(nfcRfTechParams.param.pf.nfcid2, NFC_NFCID2_LEN);
    } else if (IsDiscTypeV(discType)) {
        uint8_t* i93Uid = activated.params.i93.uid;
        uint8_t i93UidReverse[I93_UID_BYTE_LEN];
        for (uint32_t i = 0; i < I93_UID_BYTE_LEN; i++) {
            i93UidReverse[i] = i93Uid[I93_UID_BYTE_LEN - i - 1];
        }
        uid = KITS::NfcSdkCommon::BytesVecToHexString(i93UidReverse, I93_UID_BYTE_LEN);
    } else {
        uid = "";
    }
    return uid;
}

std::string TagNciAdapterNtf::GetTechPollForTypeB(tNFC_RF_TECH_PARAMS nfcRfTechParams, uint32_t tech)
{
    std::string techPoll = "";
    if (tech == TagNciAdapterCommon::TARGET_TYPE_ISO14443_3B) {
        uint32_t length = nfcRfTechParams.param.pb.sensb_res_len;
        if (length > NFC_NFCID0_MAX_LEN) {
            length = length - NFC_NFCID0_MAX_LEN;
        } else {
            WarnLog("sensb_res_len %{public}d error", length);
            length = 0;
        }
        techPoll = KITS::NfcSdkCommon::BytesVecToHexString(
            nfcRfTechParams.param.pb.sensb_res + SENSB_RES_POLL_POS, length);
    }
    return techPoll;
}

void TagNciAdapterNtf::GetTechPollFromData(tNFA_ACTIVATED activated)
{
    std::string techPoll = "";
    tNFC_RF_TECH_PARAMS nfcRfTechParams = activated.activate_ntf.rf_tech_param;
    uint8_t discType = nfcRfTechParams.mode;
    for (uint32_t i = g_commonMultiTagTmpTechIdx; i < g_commonTechListIndex; i++) {
        if (IsDiscTypeA(discType)) {
            techPoll = KITS::NfcSdkCommon::BytesVecToHexString(
                nfcRfTechParams.param.pa.sens_res, SENS_RES_LENGTH);
        } else if (IsDiscTypeB(discType)) {
            techPoll = GetTechPollForTypeB(nfcRfTechParams, TagNciAdapterCommon::GetInstance().tagTechList_[i]);
        } else if (IsDiscTypeF(discType)) {
            uint8_t fTechPoll[F_POLL_LENGTH];
            uint8_t *sensfRes = nfcRfTechParams.param.pf.sensf_res;

            // save the pmm value.
            for (uint32_t j = 0; j < SENSF_RES_LENGTH; j++) {
                fTechPoll[j] = static_cast<uint8_t>(sensfRes[j + SENSF_RES_LENGTH]);
            }

            // save the system code.
            if (activated.params.t3t.num_system_codes > 0) {
                unsigned short *pSystemCodes = activated.params.t3t.p_system_codes;
                fTechPoll[POS_NFCF_STSTEM_CODE_HIGH] =
                    static_cast<uint8_t>(*pSystemCodes >> SYSTEM_CODE_SHIFT);
                fTechPoll[POS_NFCF_STSTEM_CODE_LOW] = static_cast<uint8_t>(*pSystemCodes);
            }
            techPoll = KITS::NfcSdkCommon::BytesVecToHexString(fTechPoll, F_POLL_LENGTH);
        } else if (IsDiscTypeV(discType)) {
            uint8_t vTechPoll[2] = {activated.params.i93.afi, activated.params.i93.dsfid};
            techPoll = KITS::NfcSdkCommon::BytesVecToHexString(vTechPoll, I93_POLL_LENGTH);
        } else {
            techPoll = "";
        }
        TagNciAdapterCommon::GetInstance().tagPollBytes_.push_back(techPoll);
    }
}

std::string TagNciAdapterNtf::GetTechActForIsoDep(tNFA_ACTIVATED activated,
    tNFC_RF_TECH_PARAMS nfcRfTechParams, uint32_t tech) const
{
    std::string techAct = "";
    if (tech == TagNciAdapterCommon::TARGET_TYPE_ISO14443_4) {
        uint8_t discType = nfcRfTechParams.mode;
        if (IsDiscTypeA(discType)) {
            if (activated.activate_ntf.intf_param.type == NFC_INTERFACE_ISO_DEP) {
                tNFC_INTF_PA_ISO_DEP paIso = activated.activate_ntf.intf_param.intf_param.pa_iso;
                techAct = (paIso.his_byte_len > 0) ? KITS::NfcSdkCommon::BytesVecToHexString(
                    paIso.his_byte, paIso.his_byte_len) : "";
            }
        } else if (IsDiscTypeB(discType)) {
            if (activated.activate_ntf.intf_param.type == NFC_INTERFACE_ISO_DEP) {
                tNFC_INTF_PB_ISO_DEP pbIso = activated.activate_ntf.intf_param.intf_param.pb_iso;
                techAct = (pbIso.hi_info_len > 0) ? KITS::NfcSdkCommon::BytesVecToHexString(
                    pbIso.hi_info, pbIso.hi_info_len) : "";
            }
        }
    } else if (tech == TagNciAdapterCommon::TARGET_TYPE_ISO14443_3A) {
        techAct = KITS::NfcSdkCommon::UnsignedCharToHexString(nfcRfTechParams.param.pa.sel_rsp);
    } else {
        // do nothing
    }
    return techAct;
}

void TagNciAdapterNtf::GetTechActFromData(tNFA_ACTIVATED activated)
{
    uint8_t protocol = activated.activate_ntf.protocol;
    tNFC_RF_TECH_PARAMS nfcRfTechParams = activated.activate_ntf.rf_tech_param;
    for (uint32_t i = g_commonMultiTagTmpTechIdx; i < g_commonTechListIndex; i++) {
        std::string techAct = "";
        if (protocol == NCI_PROTOCOL_T1T) {
            techAct = KITS::NfcSdkCommon::UnsignedCharToHexString(nfcRfTechParams.param.pa.sel_rsp);
        } else if (protocol == NCI_PROTOCOL_T2T) {
            techAct = KITS::NfcSdkCommon::UnsignedCharToHexString(nfcRfTechParams.param.pa.sel_rsp);
        } else if (protocol == NCI_PROTOCOL_T3T) {
            techAct = "";
        } else if (protocol == NCI_PROTOCOL_ISO_DEP) {
            techAct = GetTechActForIsoDep(activated, nfcRfTechParams,
                TagNciAdapterCommon::GetInstance().tagTechList_[i]);
        } else if (protocol == NCI_PROTOCOL_15693) {
            uint8_t techActivated[2] = {activated.params.i93.afi, activated.params.i93.dsfid};
            techAct = KITS::NfcSdkCommon::BytesVecToHexString(techActivated, I93_ACT_LENGTH);
        } else if (protocol == NFC_PROTOCOL_MIFARE) {
            techAct = KITS::NfcSdkCommon::UnsignedCharToHexString(nfcRfTechParams.param.pa.sel_rsp);
        } else {
            // do nothing
        }
        TagNciAdapterCommon::GetInstance().tagActivatedBytes_.push_back(techAct);
    }
}

void TagNciAdapterNtf::ParseSpecTagType(tNFA_ACTIVATED activated)
{
    // parse for FelicaLite
    if (activated.activate_ntf.protocol == NFC_PROTOCOL_T3T) {
        uint32_t i = 0;
        while (i < activated.params.t3t.num_system_codes) {
            if (activated.params.t3t.p_system_codes[i++] == T3T_SYSTEM_CODE_FELICA_LITE) {
                TagNciAdapterCommon::GetInstance().isFelicaLite_ = true;
                break;
            }
        }
    }
    // parse for MifareUltralight, NFC Digital Protocol, see SENS_RES and SEL_RES
    if (activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) {
        if ((activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] == ATQA_MIFARE_UL_0) &&
            (activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] == ATQA_MIFARE_UL_1) &&
            ((activated.activate_ntf.rf_tech_param.param.pa.sel_rsp == SAK_MIFARE_UL_1) ||
            (activated.activate_ntf.rf_tech_param.param.pa.sel_rsp == SAK_MIFARE_UL_2)) &&
            (activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] == MANUFACTURER_ID_NXP)) {
            TagNciAdapterCommon::GetInstance().isMifareUltralight_ = true;
        }
    }

    // parse for MifareDESFire, one sak byte and 2 ATQA bytes
    if ((activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) ||
        (activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_LISTEN_A) ||
        (activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_LISTEN_A_ACTIVE)) {
        if ((activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] == ATQA_MIFARE_DESFIRE_0) &&
            (activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] == ATQA_MIFARE_DESFIRE_1) &&
            (activated.activate_ntf.rf_tech_param.param.pa.sel_rsp == SAK_MIFARE_DESFIRE)) {
            TagNciAdapterCommon::GetInstance().isMifareDESFire_ = true;
        }
    }
    InfoLog("isFelicaLite_ = %{public}d, isMifareUltralight_ = %{public}d, isMifareDESFire_ = %{public}d",
        TagNciAdapterCommon::GetInstance().isFelicaLite_, TagNciAdapterCommon::GetInstance().isMifareUltralight_,
        TagNciAdapterCommon::GetInstance().isMifareDESFire_);
}

void TagNciAdapterNtf::BuildTagInfo(tNFA_ACTIVATED activated)
{
    DebugLog("TagNciAdapterNtf::BuildTagInfo, g_commonDiscRstEvtNum = %{public}d", g_commonDiscRstEvtNum);
    std::string tagUid = GetUidFromData(activated);
    GetTechPollFromData(activated);
    GetTechActFromData(activated);
    ParseSpecTagType(activated);

    if (g_commonDiscRstEvtNum == 0) {
        g_commonMultiTagTmpTechIdx = 0;
        std::shared_ptr<NCI::TagHost> tagHost = std::make_shared<NCI::TagHost>(
            TagNciAdapterCommon::GetInstance().tagTechList_,
            TagNciAdapterCommon::GetInstance().tagRfDiscIdList_,
            TagNciAdapterCommon::GetInstance().tagRfProtocols_, tagUid,
            TagNciAdapterCommon::GetInstance().tagPollBytes_,
            TagNciAdapterCommon::GetInstance().tagActivatedBytes_,
            g_commonConnectedTechIdx);
        TagNativeImpl::GetInstance().OnTagDiscovered(TagNciAdapterCommon::GetInstance().tagRfDiscIdList_[0], tagHost);
    } else {
        g_commonMultiTagTmpTechIdx = g_commonTechListIndex;
        InfoLog("TagNciAdapterNtf::BuildTagInfo, select next tag if exists");
    }
    InfoLog("TagNciAdapterNtf::BuildTagInfo, g_commonMultiTagTmpTechIdx = %{public}d,"
        "g_commonTechListIndex = %{public}d", g_commonMultiTagTmpTechIdx, g_commonTechListIndex);
}

bool TagNciAdapterNtf::IsTagDetectedInTimeDiff(uint32_t timeDiff)
{
    return true;
}

void TagNciAdapterNtf::ClearMultiMFCTagState()
{
    TagNciAdapterCommon::GetInstance().ClearMultiMFCTagState();
}

void TagNciAdapterNtf::SetMultiTagData(tNFC_RESULT_DEVT& discNtf)
{
    if (discNtf.rf_tech_param.param.pa.sel_rsp == NON_STD_CARD_SAK) {
        InfoLog("TagNciAdapterNtf::SetMultiTagData: sak 13 tag detechted, set protocol to ISODEP");
        g_commonMultiTagDiscProtocol[g_commonDiscRstEvtNum] = NFC_PROTOCOL_ISO_DEP;
        return;
    }
    if (discNtf.protocol == NFC_PROTOCOL_MIFARE) {
        if (TagNciAdapterCommon::GetInstance().isMultiProtoMFC_ &&
            IsTagDetectedInTimeDiff(multiTagTimeDiff_[0])) { // 0 for Mifare
            TagNciAdapterCommon::GetInstance().isSkipNdefRead_ = true;
        } else {
            ClearMultiMFCTagState();
        }
    } else if (discNtf.protocol == NFC_PROTOCOL_ISO_DEP) {
        if (g_commonIsIsoDepDhReqFailed && IsTagDetectedInTimeDiff(multiTagTimeDiff_[1])) { // 1 for ISODEP
            g_multiTagParams.isSkipIsoDepAct = true;
        } else {
            ClearMultiMFCTagState();
        }
    } else if (discNtf.more == NCI_DISCOVER_NTF_LAST) {
        bool isMFCDetected = false;
        for (uint32_t i = 0; i < g_commonTechListIndex; i++) {
            if (TagNciAdapterCommon::GetInstance().tagRfProtocols_[i] == NFC_PROTOCOL_MIFARE) {
                isMFCDetected = true;
            }
        }
        if (!isMFCDetected) {
            ClearMultiMFCTagState();
        }
    }
}

void TagNciAdapterNtf::IsMultiTag(tNFC_RESULT_DEVT discoveryNtf, uint8_t *nfcID2, uint8_t nfcID2Len)
{
    for (uint32_t i = 0; i < TagNciAdapterCommon::GetInstance().discNtfIndex_; i++) {
        InfoLog("TagNciAdapterNtf::HandleDiscResult, index: %{public}d, discId: %{public}d, protocl: %{public}d",
                i, g_commonMultiTagDiscId[i], g_commonMultiTagDiscProtocol[i]);
    }
    if (discoveryNtf.rf_disc_id > 2) { // multitag has more than 2 uids
        InfoLog("TagNciAdapterNtf::HandleDiscResult, this multiTag has more than 2 uids");
    } else if (discoveryNtf.rf_disc_id == 2) { // this multiTag has 2 uids
        if (memcmp(nfcID1_, nfcID2, sizeof(nfcID1_)) == 0) {
            InfoLog("TagNciAdapterNtf::HandleDiscResult, this multiTag has 2 same uids");
            TagNciAdapterCommon::GetInstance().isMultiTag_ = false;
        } else {
            InfoLog("TagNciAdapterNtf::HandleDiscResult, this multiTag has 2 different uids");
        }
    } else {
        InfoLog("TagNciAdapterNtf::HandleDiscResult, this multiTag has 1 uid");
    }
}

/**
 * @brief Parse rf discover ntf.
 * @param eventData The rf discover ntf.
 */
void TagNciAdapterNtf::HandleDiscResult(tNFA_CONN_EVT_DATA* eventData)
{
    if (eventData == nullptr) {
        WarnLog("HandleDiscResult invalid eventData.");
        return;
    }
    if (eventData->disc_result.status != NFA_STATUS_OK) {
        ErrorLog("TagNciAdapterNtf::HandleDiscResult, status error: %{public}d", eventData->disc_result.status);
        return;
    }
    tNFC_RESULT_DEVT& discoveryNtf = eventData->disc_result.discovery_ntf;
    DebugLog("TagNciAdapterNtf::HandleDiscResult, discId: %{public}d, protocol: %{public}d, discNtfIndex_: %{public}d",
        discoveryNtf.rf_disc_id, discoveryNtf.protocol, TagNciAdapterCommon::GetInstance().discNtfIndex_);
    uint8_t nfcID2[NCI_NFCID1_MAX_LEN] = {0};

    if (discoveryNtf.rf_disc_id == 1) { // first UID
        (void)memset_s(nfcID1_, sizeof(nfcID1_), 0, sizeof(nfcID1_));
        if (discoveryNtf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) {
            errno_t err = memcpy_s(nfcID1_, sizeof(nfcID1_), discoveryNtf.rf_tech_param.param.pa.nfcid1,
                           discoveryNtf.rf_tech_param.param.pa.nfcid1_len);
            if (err != EOK) {
                ErrorLog("TagNciAdapterNtf::HandleDiscResult, memcpy nfcid1 error: %{public}d", err);
            }
        }
    } else if (discoveryNtf.rf_disc_id == 2) {  // 2 represents the second uid
        if (discoveryNtf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) {
            errno_t err = memcpy_s(nfcID2, sizeof(nfcID2), discoveryNtf.rf_tech_param.param.pa.nfcid1,
                           discoveryNtf.rf_tech_param.param.pa.nfcid1_len);
            if (err != EOK) {
                ErrorLog("TagNciAdapterNtf::HandleDiscResult, memcpy nfcid2 error: %{public}d", err);
            }
        }
    }
    if (TagNciAdapterCommon::GetInstance().discNtfIndex_ >= MAX_NUM_TECHNOLOGY) {
        ErrorLog("TagNciAdapterNtf::HandleDiscResult, invalid discNtfIndex_: %{public}d",
            TagNciAdapterCommon::GetInstance().discNtfIndex_);
        return;
    }
    TagNciAdapterCommon::GetInstance().discNtfIndex_++;
    g_commonMultiTagDiscId.push_back(discoveryNtf.rf_disc_id);
    g_commonMultiTagDiscProtocol.push_back(discoveryNtf.protocol);
#if (NXP_EXTNS == TRUE)
    SetMultiTagData(discoveryNtf);
#endif
    if (discoveryNtf.more == NCI_DISCOVER_NTF_MORE) {
        return;
    }
    IsMultiTag(discoveryNtf, nfcID2, NCI_NFCID1_MAX_LEN);
}

uint32_t TagNciAdapterNtf::GetT1tMaxMessageSize(tNFA_ACTIVATED activated) const
{
    uint32_t t1tMaxMessageSize;
    DebugLog("GetT1tMaxMessageSize");
    if (activated.activate_ntf.protocol != NFC_PROTOCOL_T1T) {
        t1tMaxMessageSize = 0;
        return t1tMaxMessageSize;
    }
    // examine the first byte of header ROM bytes
    switch (activated.params.t1t.hr[0]) {
        case RW_T1T_IS_TOPAZ96:
            t1tMaxMessageSize = TOPAZ96_MAX_MESSAGE_SIZE;
            break;
        case RW_T1T_IS_TOPAZ512:
            t1tMaxMessageSize = TOPAZ512_MAX_MESSAGE_SIZE;
            break;
        default:
            ErrorLog("GetT1tMaxMessageSize: unknown T1T HR0=%u", activated.params.t1t.hr[0]);
            t1tMaxMessageSize = 0;
            break;
    }
    return t1tMaxMessageSize;
}

tNFA_INTF_TYPE TagNciAdapterNtf::GetRfInterface(uint32_t protocol) const
{
    tNFA_INTF_TYPE rfInterface;
    if (protocol == NFA_PROTOCOL_ISO_DEP) {
        rfInterface = NFA_INTERFACE_ISO_DEP;
    } else if (protocol == NFA_PROTOCOL_MIFARE) {
        rfInterface = NFA_INTERFACE_MIFARE;
    } else {
        rfInterface = NFA_INTERFACE_FRAME;
    }
    return rfInterface;
}

void TagNciAdapterNtf::SetIsMultiTag(bool isMultiTag)
{
    TagNciAdapterCommon::GetInstance().isMultiTag_ = isMultiTag &&
        TagNciAdapterCommon::GetInstance().isMultiTagSupported_;
}

bool TagNciAdapterNtf::GetIsMultiTag() const
{
    return TagNciAdapterCommon::GetInstance().isMultiTag_;
}

void TagNciAdapterNtf::SetDiscRstEvtNum(uint32_t num)
{
    if (num < MAX_NUM_TECHNOLOGY) {
        g_commonDiscRstEvtNum = num;
    }
}

uint32_t TagNciAdapterNtf::GetDiscRstEvtNum() const
{
    return g_commonDiscRstEvtNum;
}

void TagNciAdapterNtf::GetMultiTagTechsFromData(const tNFA_DISC_RESULT& discoveryData)
{
    uint32_t idx = g_commonDiscRstEvtNum;
    if (idx >= MAX_NUM_TECHNOLOGY || idx >= g_commonMultiTagDiscProtocol.size() ||
        idx >= g_commonMultiTagDiscId.size()) {
        ErrorLog("TagNciAdapterNtf::GetMultiTagTechsFromData: index error, index = %{public}d", idx);
        return;
    }
    g_commonMultiTagDiscId[idx] = discoveryData.discovery_ntf.rf_disc_id;
    g_commonMultiTagDiscProtocol[idx] = discoveryData.discovery_ntf.protocol;
    if (TagNciAdapterCommon::GetInstance().discNtfIndex_ < MAX_NUM_TECHNOLOGY) {
        TagNciAdapterCommon::GetInstance().discNtfIndex_++;
    }
    DebugLog("TagNciAdapterNtf::GetMultiTagTechsFromData: g_commonDiscRstEvtNum = %{public}d,"
        "discNtfIndex_ = %{public}d, discId = 0x%{public}X, protocol = 0x%{public}X",
        g_commonDiscRstEvtNum, TagNciAdapterCommon::GetInstance().discNtfIndex_,
        g_commonMultiTagDiscId[idx], g_commonMultiTagDiscProtocol[idx]);
}

tNFA_STATUS TagNciAdapterNtf::DoSelectForMultiTag(uint32_t currIdx)
{
    tNFA_STATUS result = NFA_STATUS_FAILED;
    if (currIdx == INVALID_TAG_INDEX) {
        ErrorLog("TagNciAdapterNtf::DoSelectForMultiTag: is NFC_DEP");
        return result;
    }
    InfoLog("TagNciAdapterNtf::DoSelectForMultiTag: protocol = 0x%{public}X", g_commonMultiTagDiscProtocol[currIdx]);

    if (g_commonMultiTagDiscProtocol[currIdx] == NFA_PROTOCOL_ISO_DEP) {
        result = NFA_Select(
            g_commonMultiTagDiscId[currIdx], g_commonMultiTagDiscProtocol[currIdx], NFA_INTERFACE_ISO_DEP);
    } else if (g_commonMultiTagDiscProtocol[currIdx] == NFA_PROTOCOL_MIFARE) {
        result = NFA_Select(
            g_commonMultiTagDiscId[currIdx], g_commonMultiTagDiscProtocol[currIdx], NFA_INTERFACE_MIFARE);
    } else {
        result = NFA_Select(
            g_commonMultiTagDiscId[currIdx], g_commonMultiTagDiscProtocol[currIdx], NFA_INTERFACE_FRAME);
    }
    return result;
}

bool TagNciAdapterNtf::SkipProtoActivateIfNeed(tNFC_PROTOCOL protocol)
{
    if ((protocol == NFA_PROTOCOL_ISO_DEP) && g_multiTagParams.isSkipIsoDepAct) {
        tNFA_CONN_EVT_DATA eventData;
        tNFC_ACTIVATE_DEVT& actNtf = eventData.activated.activate_ntf;
        tNFC_RESULT_DEVT& info = g_multiTagParams.discNtf;
        actNtf.rf_disc_id = info.rf_disc_id;
        actNtf.protocol = info.protocol;
        errno_t err = EOK;
        err = memcpy_s(&actNtf.rf_tech_param, sizeof(tNFC_RF_TECH_PARAMS),
                       &info.rf_tech_param, sizeof(tNFC_RF_TECH_PARAMS));
        if (err != EOK) {
            ErrorLog("TagNciAdapterNtf::SkipProtoActivateIfNeed, memcpy rf_tech_param error: %{public}d", err);
        }
        err = memcpy_s(&actNtf.intf_param, sizeof(tNFC_INTF_PARAMS),
                       &g_multiTagParams.intfParam, sizeof(tNFC_INTF_PARAMS));
        if (err != EOK) {
            ErrorLog("TagNciAdapterNtf::SkipProtoActivateIfNeed, memcpy intfParam error: %{public}d", err);
        }
        InfoLog("TagNciAdapterNtf::SkipProtoActivateIfNeed,(SAK28) discID:%{public}u is skipped", actNtf.rf_disc_id);
        NfccNciAdapter::GetInstance().SendActEvtForSak28Tag(NFA_ACTIVATED_EVT, &eventData);
        g_commonDiscRstEvtNum--;
        return true;
    }
    return false;
}

void TagNciAdapterNtf::SelectTheFirstTag()
{
    uint32_t currIdx = INVALID_TAG_INDEX;
    for (uint32_t i = 0; i < TagNciAdapterCommon::GetInstance().discNtfIndex_; i++) {
        InfoLog("TagNciAdapterNtf::SelectTheFirstTag index = %{public}d discId = 0x%{public}X protocol = 0x%{public}X",
            i, g_commonMultiTagDiscId[i], g_commonMultiTagDiscProtocol[i]);
        // logic for SAK28 issue
        if (isSkipMifareActive_) {
            if ((g_commonMultiTagDiscProtocol[i] == NFA_PROTOCOL_NFC_DEP) ||
                (g_commonMultiTagDiscProtocol[i] == NFA_PROTOCOL_MIFARE)) {
                continue;
            }
#if (NXP_EXTNS == TRUE)
            if (!SkipProtoActivateIfNeed(g_commonMultiTagDiscProtocol[i])) {
                g_selectedIdx = i;
#endif
                g_commonSelectedTagIdx = i;
                currIdx = i;
                break;
#if (NXP_EXTNS == TRUE)
            }
#endif
        } else if (g_commonMultiTagDiscProtocol[i] != NFA_PROTOCOL_NFC_DEP) {
#if (NXP_EXTNS == TRUE)
            if (!SkipProtoActivateIfNeed(g_commonMultiTagDiscProtocol[i])) {
                g_selectedIdx = i;
#endif
                g_commonSelectedTagIdx = i;
                currIdx = i;
                break;
#if (NXP_EXTNS == TRUE)
            }
#endif
        }
    }
    isSkipMifareActive_ = false;

    // logic for normal tag
    tNFA_STATUS result = DoSelectForMultiTag(currIdx);
    InfoLog("TagNciAdapterNtf::SelectTheFirstTag result = %{public}d", result);
}

void TagNciAdapterNtf::SelectTheNextTag()
{
    if (g_commonDiscRstEvtNum == 0) {
        InfoLog("TagNciAdapterNtf::SelectTheNextTag: next tag does not exist");
        return;
    }
    uint32_t currIdx = INVALID_TAG_INDEX;
    g_commonDiscRstEvtNum--;
    for (uint32_t i = 0; i < TagNciAdapterCommon::GetInstance().discNtfIndex_; i++) {
        InfoLog("TagNciAdapterNtf::SelectTheNextTag index = %{public}d discId = 0x%{public}X protocol = 0x%{public}X",
            i, g_commonMultiTagDiscId[i], g_commonMultiTagDiscProtocol[i]);
        if (g_commonMultiTagDiscId[i] != g_commonMultiTagDiscId[g_commonSelectedTagIdx] ||
            (g_commonMultiTagDiscProtocol[i] != g_commonMultiTagDiscProtocol[g_commonSelectedTagIdx] &&
            (g_commonMultiTagDiscProtocol[i] != NFA_PROTOCOL_NFC_DEP))) {
            g_commonSelectedTagIdx = i;
            currIdx = i;
            break;
        }
    }
    tNFA_STATUS result = DoSelectForMultiTag(currIdx);
    InfoLog("TagNciAdapterNtf::DoSelectForMultiTag result = %{public}d", result);
}

/* method for SAK28 issue */
void TagNciAdapterNtf::SetSkipMifareInterface()
{
    InfoLog("TagNciAdapterNtf::SetSkipMifareInterface");
    isSkipMifareActive_ = true;
    g_commonDiscRstEvtNum = 1;
}

bool TagNciAdapterNtf::IsMultiMFCTag()
{
    return TagNciAdapterRw::GetInstance().IsMultiMFCTag();
}

void TagNciAdapterNtf::SetTagActivated()
{
    TagNciAdapterRw::GetInstance().SetTagActivated();
}

void TagNciAdapterNtf::ResetTagFieldOnFlag()
{
    TagNciAdapterRw::GetInstance().ResetTagFieldOnFlag();
}

void TagNciAdapterNtf::SetTagDeactivated(bool isSleep)
{
    TagNciAdapterRw::GetInstance().SetTagDeactivated(isSleep);
}

void TagNciAdapterNtf::HandleDeactivatedResult(tNFA_DEACTIVATE_TYPE deactType)
{
    TagNciAdapterRw::GetInstance().HandleDeactivatedResult(deactType);
}

void TagNciAdapterNtf::AbortWait()
{
    TagNciAdapterRw::GetInstance().AbortWait();
}

bool TagNciAdapterNtf::IsTagDeactivating()
{
    return TagNciAdapterRw::GetInstance().IsTagDeactivating();
}

void TagNciAdapterNtf::HandleFieldCheckResult(uint8_t status)
{
    TagNciAdapterRw::GetInstance().HandleFieldCheckResult(status);
}

void TagNciAdapterNtf::HandleNdefCheckResult(uint8_t status, uint32_t currentSize, uint32_t flag, uint32_t maxSize)
{
    TagNciAdapterRw::GetInstance().HandleNdefCheckResult(status, currentSize, flag, maxSize);
}

void TagNciAdapterNtf::HandleTranceiveData(uint8_t status, uint8_t* data, uint32_t dataLen)
{
    TagNciAdapterRw::GetInstance().HandleTranceiveData(status, data, dataLen);
}

}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
