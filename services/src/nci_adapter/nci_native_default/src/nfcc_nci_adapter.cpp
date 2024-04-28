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

#include "nfcc_nci_adapter.h"

#include <unistd.h>

#include "loghelper.h"
#include "nfc_config.h"
#include "nfc_sdk_common.h"
#include "routing_manager.h"
#include "securec.h"
#include "tag_nci_adapter_ntf.h"
#include "tag_nci_adapter_common.h"

using namespace OHOS::NFC;
namespace OHOS {
namespace NFC {
namespace NCI {
// values for SAK28 issue
static unsigned int g_isoMifareBitMap = 0;
static bool g_isIsoMifareFlag = false;
static uint8_t isoMifareUid[NCI_NFCID1_MAX_LEN] = { 0 };
const uint8_t NCI_RF_DISCOVER_NTF_FIRST_ID = 0x01;
const uint8_t NCI_RF_DISCOVER_NTF_SECOND_ID = 0x02;
const unsigned int FLAG_MULTI_TAG_ISO_DEP = 0x01;
const unsigned int FLAG_MULTI_TAG_MIFARE = 0x02;
// wait nci event 2000 ms
const unsigned int NCI_EVT_WAIT_TIMEOUT = 2000;

NfccNciAdapter::NfccNciAdapter() = default;
NfccNciAdapter::~NfccNciAdapter() = default;

NfccNciAdapter& NfccNciAdapter::GetInstance()
{
    static NfccNciAdapter nfccNciAdapter;
    return nfccNciAdapter;
}

/**
 * @brief whether nfc is enabled or disabled.
 * @return true/false - nfc is enabled/disabled.
 */
bool NfccNciAdapter::IsNfcActive()
{
    bool isActive = (isNfcEnabled_ && !isDisabling_);
    return isActive;
}

/**
 * @brief whether tag is active.
 * @return True/false tag is active/deactive.
 */
bool NfccNciAdapter::IsTagActive() const
{
    return isTagActive_;
}

/**
 * @brief Set card emulation listener to receive field on/off event.
 * @param listener The listener to receive field on/off event.
 */
void NfccNciAdapter::SetCeHostListener(std::weak_ptr<INciCeInterface::ICeHostListener> listener)
{
    cardEmulationListener_ = listener;
}

/**
 * @brief Start or stop rf discovery.
 * @param isStart True/false start/stop rf discovery.
 */
void NfccNciAdapter::StartRfDiscovery(bool isStart)
{
    DebugLog("NfccNciAdapter::StartRfDiscovery: isStart= %{public}d", isStart);
    tNFA_STATUS status = NFA_STATUS_FAILED;
    if (isStart) {
        status = NFA_StartRfDiscovery();
    } else {
        status = NFA_StopRfDiscovery();
    }
    if (status == NFA_STATUS_OK) {
        if (nfcStartStopPollingEvent_.Wait(NCI_EVT_WAIT_TIMEOUT) == false) {
            ErrorLog("NfccNciAdapter::StartRfDiscovery timeout. isStart = %{public}d", isStart);
            return;
        }
        rfEnabled_ = isStart;
    } else {
        ErrorLog("NfccNciAdapter::StartRfDiscovery: Failed to start/stop RF discovery; error=0x%{public}X", status);
    }
}

tNFA_STATUS NfccNciAdapter::StartPolling(tNFA_TECHNOLOGY_MASK techMask)
{
    DebugLog("NfccNciAdapter::StartPolling, techMask = 0x%{public}02X", techMask);
    tNFA_STATUS status = NFA_EnablePolling(techMask);
    if (status == NFA_STATUS_OK) {
        DebugLog("StartPolling: wait for enable event");
        // wait for NFA_POLL_ENABLED_EVT
        if (nfcStartStopPollingEvent_.Wait(NCI_EVT_WAIT_TIMEOUT) == false) {
            ErrorLog("NfccNciAdapter::StartPolling timeout.");
            return status;
        }
        pollingEnabled_ = true;
    } else {
        ErrorLog("NfccNciAdapter::StartPolling: fail enable polling; error = 0x%{public}X", status);
    }
    return status;
}

tNFA_STATUS NfccNciAdapter::StopPolling()
{
    DebugLog("NfccNciAdapter::StopPolling");
    tNFA_STATUS status = NFA_DisablePolling();
    if (status == NFA_STATUS_OK) {
        pollingEnabled_ = false;
        // wait for NFA_POLL_DISABLED_EVT
        if (nfcStartStopPollingEvent_.Wait(NCI_EVT_WAIT_TIMEOUT) == false) {
            ErrorLog("NfccNciAdapter::StopPolling timeout.");
        }
    } else {
        ErrorLog("NfccNciAdapter::StopPolling: fail disable polling; error = 0x%{public}X", status);
    }
    return status;
}

bool NfccNciAdapter::IsDiscTypeListen(tNFC_ACTIVATE_DEVT& actNtf)
{
    return ((actNtf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_LISTEN_A) ||
            (actNtf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_LISTEN_B) ||
            (actNtf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_LISTEN_F) ||
            (actNtf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_LISTEN_A_ACTIVE) ||
            (actNtf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_LISTEN_F_ACTIVE) ||
            (actNtf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_LISTEN_ISO15693) ||
            (actNtf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_LISTEN_B_PRIME) ||
            (actNtf.intf_param.type == NFC_INTERFACE_EE_DIRECT_RF));
}

void NfccNciAdapter::DoNfaActivatedEvt(tNFA_CONN_EVT_DATA* eventData)
{
    uint8_t actProto = (tNFA_INTF_TYPE)eventData->activated.activate_ntf.protocol;
    if (actProto == NFC_PROTOCOL_T5T && TagNciAdapterNtf::GetInstance().GetDiscRstEvtNum()) {
        // protocol T5T only support single protocol detection
        InfoLog("DoNfaActivatedEvt, NFC_PROTOCOL_T5T not support multi tag.");
        TagNciAdapterNtf::GetInstance().SetDiscRstEvtNum(0);
    }
#if (NXP_EXTNS == TRUE)
    TagNciAdapterCommon::GetInstance().isIsoDepDhReqFailed_ = false;
#endif
    // logic for SAK28 issue
    if (g_isIsoMifareFlag) {
        InfoLog("DoNfaActivatedEvt(SAK28) - ISOMIFARE data cleanup");
        g_isIsoMifareFlag = false;
        g_isoMifareBitMap = 0;
        (void)memset_s(isoMifareUid, sizeof(isoMifareUid), 0, sizeof(isoMifareUid));
    }

    // sync activated iface and proto
    if ((actProto != NFA_PROTOCOL_NFC_DEP) && !IsDiscTypeListen(eventData->activated.activate_ntf)) {
        TagNciAdapterNtf::GetInstance().SetCurrRfInterface(
            (tNFA_INTF_TYPE)eventData->activated.activate_ntf.intf_param.type);
        TagNciAdapterNtf::GetInstance().SetCurrRfProtocol(actProto);
#if (NXP_EXTNS == TRUE)
        uint8_t mode = eventData->activated.activate_ntf.rf_tech_param.mode;
        TagNciAdapterNtf::GetInstance().SetCurrRfMode(mode);
        if (mode == NFC_DISCOVERY_TYPE_POLL_B || mode == NFC_DISCOVERY_TYPE_POLL_B_PRIME) {
            TagNciAdapterNtf::GetInstance().SetNfcID0ForTypeB(
                eventData->activated.activate_ntf.rf_tech_param.param.pb.nfcid0);
        }
#endif
    }

#if (NXP_EXTNS == TRUE)
    //clear MulitProto Mifare Tag state on single proto tag activation
    if (!TagNciAdapterNtf::GetInstance().GetIsMultiTag() &&
        TagNciAdapterNtf::GetInstance().IsMultiMFCTag()) {
        TagNciAdapterNtf::GetInstance().ClearMultiMFCTagState();
    }
#endif

    // handle ActivatedResult for Mifare tag
    if (Extns::GetInstance().EXTNS_GetConnectFlag() == true) {
        TagNciAdapterNtf::GetInstance().SetTagActivated();
        TagNciAdapterNtf::GetInstance().SetConnectStatus(true);
        return;
    }

    // handle ActivationResult for normal tags
    if (isDisabling_ || !isNfcEnabled_) {
        return;
    }
    isTagActive_ = true;
    if (TagNciAdapterNtf::GetInstance().IsSwitchingRfIface()) {
#if (NXP_EXTNS == TRUE)
        if (TagNciAdapterNtf::GetInstance().IsExpectedActRfProtocol(actProto)) {
            TagNciAdapterNtf::GetInstance().SetTagActivated();
        }
#else
        TagNciAdapterNtf::GetInstance().SetTagActivated();
#endif
        TagNciAdapterNtf::GetInstance().SetConnectStatus(true);
        return;
    }
    TagNciAdapterNtf::GetInstance().SetTagActivated();
    TagNciAdapterNtf::GetInstance().ResetTagFieldOnFlag();

    if (actProto == NFA_PROTOCOL_NFC_DEP) {
        // we do not support peer to peer
    } else {
        TagNciAdapterNtf::GetInstance().HandleActivatedResult(eventData);
        if (TagNciAdapterNtf::GetInstance().GetDiscRstEvtNum()) {
            // do deactivate to sleep and wait for reselect for multi tag
            NFA_Deactivate(true);
        }
        // skipped notify secureelement
    }
}

void NfccNciAdapter::DoNfaDeactivatedEvt(tNFA_CONN_EVT_DATA* eventData)
{
    tNFA_DEACTIVATE_TYPE type = eventData->deactivated.type;
    TagNciAdapterNtf::GetInstance().SetTagDeactivated((type == NFA_DEACTIVATE_TYPE_SLEEP));
    TagNciAdapterNtf::GetInstance().SelectTheNextTag();
    if (eventData->deactivated.type != NFA_DEACTIVATE_TYPE_SLEEP) {
        isTagActive_ = false;
#if (NXP_EXTNS == TRUE)
        TagNciAdapterNtf::GetInstance().SetDiscRstEvtNum(0);
#endif
        TagNciAdapterNtf::GetInstance().ResetTagFieldOnFlag();
#if (NXP_EXTNS == TRUE)
        if (!TagNciAdapterNtf::GetInstance().IsSwitchingRfIface()) {
            TagNciAdapterNtf::GetInstance().HandleDeactivatedResult(type);
            TagNciAdapterNtf::GetInstance().AbortWait();
        }
#else
        TagNciAdapterNtf::GetInstance().HandleDeactivatedResult(type);
        TagNciAdapterNtf::GetInstance().AbortWait();
#endif
        TagNciAdapterNtf::GetInstance().SetIsMultiTag(false);
    } else if (TagNciAdapterNtf::GetInstance().IsTagDeactivating() ||
               Extns::GetInstance().EXTNS_GetDeactivateFlag()) {
        TagNciAdapterNtf::GetInstance().SetDeactivatedStatus();
    }
    // skipped special process for Secure Element transaction
}

void NfccNciAdapter::DoNfaDiscResultEvt(tNFA_CONN_EVT_DATA* eventData)
{
    static tNFA_STATUS status = eventData->disc_result.status;
    DebugLog("DoNfaDiscResultEvt: status = 0x%{public}X", status);
#if (NXP_EXTNS == TRUE)
    static uint8_t prevMoreVal = 0x00;
    uint8_t curMoreVal = eventData->disc_result.discovery_ntf.more;
    bool isMoreValid = true;
    // 01 means the last notification due to nfcc reaching resource limit
    // 02 means more notification
    if ((curMoreVal == 0x01) && (prevMoreVal != 0x02)) {
        ErrorLog("DoNfaDiscResultEvt: invalid more value");
        isMoreValid = false;
    } else {
        DebugLog("DoNfaDiscResultEvt: valid more value");
        isMoreValid = true;
        prevMoreVal = curMoreVal;
    }
#endif
    if (!isMoreValid) {
        TagNciAdapterNtf::GetInstance().SetDiscRstEvtNum(0);
    } else {
        TagNciAdapterNtf::GetInstance().HandleDiscResult(eventData);
        HandleDiscNtf(&eventData->disc_result.discovery_ntf);
    }
}

void NfccNciAdapter::HandleDiscNtf(tNFC_RESULT_DEVT* discNtf)
{
    // logic for SAK28 issue
    if (discNtf->rf_disc_id == NCI_RF_DISCOVER_NTF_FIRST_ID) {
        (void)memset_s(isoMifareUid, sizeof(isoMifareUid), 0, sizeof(isoMifareUid));
        g_isoMifareBitMap = 0;
        errno_t err = EOK;
        if (discNtf->rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) {
            err = memcpy_s(isoMifareUid, sizeof(isoMifareUid),
                           discNtf->rf_tech_param.param.pa.nfcid1,
                           discNtf->rf_tech_param.param.pa.nfcid1_len);
            if (err != EOK) {
                ErrorLog("HandleDiscNtf:(SAK28) memcpy_s first uid failed, err = %{public}d", err);
            }
            if (discNtf->protocol == NFC_PROTOCOL_ISO_DEP) {
                g_isoMifareBitMap |= FLAG_MULTI_TAG_ISO_DEP;
            } else if (discNtf->protocol == NFC_PROTOCOL_MIFARE) {
                g_isoMifareBitMap |= FLAG_MULTI_TAG_MIFARE;
            }
        }
    } else if (discNtf->rf_disc_id == NCI_RF_DISCOVER_NTF_SECOND_ID) {
        if (discNtf->rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) {
            if (memcmp(isoMifareUid, discNtf->rf_tech_param.param.pa.nfcid1,
                       discNtf->rf_tech_param.param.pa.nfcid1_len) == 0) {
                InfoLog("HandleDiscNtf:(SAK28) multicard with same uid");
                if (discNtf->protocol == NFC_PROTOCOL_ISO_DEP) {
                    g_isoMifareBitMap |= FLAG_MULTI_TAG_ISO_DEP;
                } else if (discNtf->protocol == NFC_PROTOCOL_MIFARE) {
                    g_isoMifareBitMap |= FLAG_MULTI_TAG_MIFARE;
                }
            }
        }
    }
    InfoLog("HandleDiscNtf:(SAK28) g_isoMifareBitMap = 0x%{public}02X, g_isIsoMifareFlag = %{public}d",
            g_isoMifareBitMap, g_isIsoMifareFlag);
    if ((g_isoMifareBitMap & FLAG_MULTI_TAG_ISO_DEP) && (g_isoMifareBitMap & FLAG_MULTI_TAG_MIFARE) &&
         g_isIsoMifareFlag && readerModeEnabled_ == false) {
        InfoLog("HandleDiscNtf:(SAK28) same tag discovered twice, skip Mifare detection");
        g_isoMifareBitMap = 0;
        TagNciAdapterNtf::GetInstance().SetSkipMifareInterface();
    }

    // logic for normal tag
    if (discNtf->more == NCI_DISCOVER_NTF_MORE) {
        // there is more discovery notification coming
        TagNciAdapterNtf::GetInstance().SetDiscRstEvtNum(TagNciAdapterNtf::GetInstance().GetDiscRstEvtNum() + 1);
        return;
    }
    if (TagNciAdapterNtf::GetInstance().GetDiscRstEvtNum() > 0) {
        TagNciAdapterNtf::GetInstance().SetIsMultiTag(true);
    }
    // select the first tag of multiple tags that is discovered
    TagNciAdapterNtf::GetInstance().SelectTheFirstTag();
}

void NfccNciAdapter::DoNfaSelectResultEvt(uint8_t status)
{
    TagNciAdapterNtf::GetInstance().HandleSelectResult(status);
}

void NfccNciAdapter::DoNfaPresenceEvt(tNFA_CONN_EVT_DATA* eventData)
{
    static tNFA_STATUS curStatus = NFA_STATUS_FAILED;
    if (eventData->status != curStatus) {
        curStatus = eventData->status;
    }
    TagNciAdapterNtf::GetInstance().HandleFieldCheckResult(curStatus);

    // logic for SAK28 issue
    if (curStatus != NFA_STATUS_OK) {
        if ((g_isoMifareBitMap & FLAG_MULTI_TAG_ISO_DEP) && (g_isoMifareBitMap & FLAG_MULTI_TAG_MIFARE)) {
            InfoLog("DoNfaPresenceEvt:(SAK28) set g_isIsoMifareFlag");
            g_isIsoMifareFlag = true;
        }
        g_isoMifareBitMap = 0;
    }
}

void NfccNciAdapter::NfcConnectionCallback(uint8_t connEvent, tNFA_CONN_EVT_DATA* eventData)
{
    if (eventData == nullptr) {
        ErrorLog("NfcConnectionCallback, eventData is null. connEvent = %{public}X", connEvent);
        return;
    }
    switch (connEvent) {
        /* whether polling successfully started */
        case NFA_POLL_ENABLED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_POLL_ENABLED_EVT: status = %{public}u", eventData->status);
            NfccNciAdapter::GetInstance().DoNfaPollEnabledDisabledEvt();
            break;
        }
        /* Listening/Polling stopped */
        case NFA_POLL_DISABLED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_POLL_DISABLED_EVT: status = %{public}u", eventData->status);
            NfccNciAdapter::GetInstance().DoNfaPollEnabledDisabledEvt();
            break;
        }
        /* RF Discovery started event */
        case NFA_RF_DISCOVERY_STARTED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_RF_DISCOVERY_STARTED_EVT: status = %{public}u", eventData->status);
            NfccNciAdapter::GetInstance().DoNfaPollEnabledDisabledEvt();
            break;
        }
        /* RF Discovery stopped event */
        case NFA_RF_DISCOVERY_STOPPED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_RF_DISCOVERY_STOPPED_EVT: status = %{public}u", eventData->status);
            NfccNciAdapter::GetInstance().DoNfaPollEnabledDisabledEvt();
            break;
        }
        /* NFC deactivate failed event */
        case NFA_DEACTIVATE_FAIL_EVT: {
            DebugLog("NfaConnectionCallback: NFA_DEACTIVATE_FAIL_EVT: status = %{public}u", eventData->status);
#if (NXP_EXTNS == TRUE)
            if (eventData->status == NFC_DEACTIVATE_REASON_DH_REQ_FAILED) {
                TagNciAdapterCommon::GetInstance().isIsoDepDhReqFailed_ = true;
            }
#endif
            break;
        }
        /* NFC link/protocol activated */
        case NFA_ACTIVATED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_ACTIVATED_EVT");
            NfccNciAdapter::GetInstance().DoNfaActivatedEvt(eventData);
            break;
        }
        /* NFC link/protocol deactivated */
        case NFA_DEACTIVATED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_DEACTIVATED_EVT");
            NfccNciAdapter::GetInstance().DoNfaDeactivatedEvt(eventData);
            break;
        }
        case NFA_DISC_RESULT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_DISC_RESULT_EVT");
            NfccNciAdapter::GetInstance().DoNfaDiscResultEvt(eventData);
            break;
        }
        case NFA_SELECT_RESULT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_SELECT_RESULT_EVT: status = 0x%{public}X", eventData->status);
            NfccNciAdapter::GetInstance().DoNfaSelectResultEvt(eventData->status);
            break;
        }
        /* Data message received (for non-NDEF reads) */
        case NFA_DATA_EVT: {
            DebugLog("NfaConnectionCallback: NFA_DATA_EVT: status = 0x%{public}X, len = %{public}d",
                eventData->status, eventData->data.len);
            TagNciAdapterNtf::GetInstance().HandleTranceiveData(eventData->status, eventData->data.p_data,
                eventData->data.len);
            break;
        }
        case NFA_PRESENCE_CHECK_EVT: {
            NfccNciAdapter::GetInstance().DoNfaPresenceEvt(eventData);
            break;
        }
        case NFA_READ_CPLT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_READ_CPLT_EVT: status = 0x%{public}X", eventData->status);
            TagNciAdapterNtf::GetInstance().HandleReadComplete(eventData->status);
            break;
        }
        case NFA_WRITE_CPLT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_WRITE_CPLT_EVT: status = 0x%{public}X", eventData->status);
            TagNciAdapterNtf::GetInstance().HandleWriteComplete(eventData->status);
            break;
        }
        case NFA_FORMAT_CPLT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_FORMAT_CPLT_EVT: status = 0x%{public}X", eventData->status);
            TagNciAdapterNtf::GetInstance().HandleFormatComplete(eventData->status);
            break;
        }
        case NFA_NDEF_DETECT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_NDEF_DETECT_EVT: status = 0x%{public}X, protocol = 0x%{public}X,"
                " max_size = %{public}u, cur_size = %{public}u, flags = 0x%{public}X",
                eventData->ndef_detect.status, eventData->ndef_detect.protocol,
                static_cast<uint32_t>(eventData->ndef_detect.max_size),
                static_cast<uint32_t>(eventData->ndef_detect.cur_size), eventData->ndef_detect.flags);
            TagNciAdapterNtf::GetInstance().HandleNdefCheckResult(eventData->ndef_detect.status,
                eventData->ndef_detect.cur_size, eventData->ndef_detect.flags, eventData->ndef_detect.max_size);
            break;
        }
        case NFA_SET_TAG_RO_EVT: {
            DebugLog("NfaDeviceManagementCallback: NFA_SET_TAG_RO_EVT; status = 0x%{public}X", eventData->status);
            TagNciAdapterNtf::GetInstance().HandleSetReadOnlyResult(eventData->status);
            break;
        }
        default: {
            DebugLog("NfaConnectionCallback: unknown event %{public}u", connEvent);
            break;
        }
    }
}

/* method for SAK28 issue */
void NfccNciAdapter::SendActEvtForSak28Tag(uint8_t connEvent, tNFA_CONN_EVT_DATA* eventData)
{
    NfcConnectionCallback(connEvent, eventData);
}

void NfccNciAdapter::DoNfaPollEnabledDisabledEvt()
{
    SynchronizeGuard guard(nfcStartStopPollingEvent_);
    nfcStartStopPollingEvent_.NotifyOne();
}

void NfccNciAdapter::DoNfaDmEnableEvt(tNFA_DM_CBACK_DATA* eventData)
{
    SynchronizeGuard guard(nfcEnableEvent_);
    isNfcEnabled_ = (eventData->status == NFA_STATUS_OK);
    isDisabling_ = false;
    nfcEnableEvent_.NotifyOne();
}

void NfccNciAdapter::DoNfaDmDisableEvt(tNFA_DM_CBACK_DATA* eventData)
{
    SynchronizeGuard guard(nfcDisableEvent_);
    isNfcEnabled_ = false;
    isDisabling_ = false;
    nfcDisableEvent_.NotifyOne();
}

/**
 * @brief Whether rf field is on or off.
 * @return True/false to be field on/off.
 */
bool NfccNciAdapter::isRfFieldOn()
{
    if (isRfFieldOn_) {
        return true;
    }
    uint64_t currTime = KITS::NfcSdkCommon::GetCurrentTime();
    // If it is less than 50ms before fieldoff, then it is considered field on;
    if ((currTime - lastRfFieldTime) < 50) {
        return true;
    }
    return false;
}

void NfccNciAdapter::DoNfaDmRfFieldEvt(tNFA_DM_CBACK_DATA* eventData)
{
    lastRfFieldTime = 0;
    isRfFieldOn_ = false;
    if (cardEmulationListener_.expired()) {
        DebugLog("DoNfaDmRfFieldEvt: cardEmulationListener_ is null");
        return;
    }
    if (eventData->rf_field.status == NFA_STATUS_OK) {
        lastRfFieldTime = KITS::NfcSdkCommon::GetCurrentTime();
        // notify field on/off event to nfc service.
        if (eventData->rf_field.rf_field_status == NFA_DM_RF_FIELD_ON) {
            isRfFieldOn_ = true;
            cardEmulationListener_.lock()->FieldActivated();
        } else {
            isRfFieldOn_ = false;
            cardEmulationListener_.lock()->FieldDeactivated();
        }
    }
}

void NfccNciAdapter::DoNfaDmSetConfig()
{
    SynchronizeGuard guard(nfcSetConfigEvent_);
    DebugLog("NfaDeviceManagementCallback: NFA_DM_SET_CONFIG_EVT");
    nfcSetConfigEvent_.NotifyOne();
}
void NfccNciAdapter::DoNfaSetPowerSubState()
{
    SynchronizeGuard guard(nfcSetPowerSubStateEvent_);
    nfcSetPowerSubStateEvent_.NotifyOne();
}

void NfccNciAdapter::DoNfaDmNfccTimeoutEvt(tNFA_DM_CBACK_DATA* eventData)
{
    {
        SynchronizeGuard guard(nfcEnableEvent_);
        nfcEnableEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(nfcDisableEvent_);
        nfcDisableEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(nfcStartStopPollingEvent_);
        nfcStartStopPollingEvent_.NotifyOne();
    }
    discoveryEnabled_ = false;
    pollingEnabled_ = false;

    if (IsNfcActive()) {
        NFA_Disable(FALSE);
        isDisabling_ = true;
    } else {
        isNfcEnabled_ = false;
        isDisabling_ = false;
    }
}

void NfccNciAdapter::NfcDeviceManagementCallback(uint8_t dmEvent, tNFA_DM_CBACK_DATA* eventData)
{
    if (eventData == nullptr) {
        ErrorLog("NfcDeviceManagementCallback, eventData is null. dmEvent = %{public}X", dmEvent);
        return;
    }
    DebugLog("NfaDeviceManagementCallback: event= %{public}u", dmEvent);

    switch (dmEvent) {
        /* Result of NFA_Enable */
        case NFA_DM_ENABLE_EVT: {
            DebugLog("NfaDeviceManagementCallback: NFA_DM_ENABLE_EVT; status = 0x%{public}X", eventData->status);
            NfccNciAdapter::GetInstance().DoNfaDmEnableEvt(eventData);
            break;
        }
        /* Result of NFA_Disable */
        case NFA_DM_DISABLE_EVT: {
            DebugLog("NfaDeviceManagementCallback: NFA_DM_DISABLE_EVT");
            NfccNciAdapter::GetInstance().DoNfaDmDisableEvt(eventData);
            break;
        }

        case NFA_DM_RF_FIELD_EVT: {
            DebugLog("NfaDeviceManagementCallback: NFA_DM_RF_FIELD_EVT; status = 0x%{public}X; field status = "
                "%{public}u", eventData->rf_field.status, eventData->rf_field.rf_field_status);
            NfccNciAdapter::GetInstance().DoNfaDmRfFieldEvt(eventData);
            break;
        }

        case NFA_DM_NFCC_TRANSPORT_ERR_EVT:
        case NFA_DM_NFCC_TIMEOUT_EVT: {
            NfccNciAdapter::GetInstance().DoNfaDmNfccTimeoutEvt(eventData);
            break;
        }

        case NFA_DM_SET_CONFIG_EVT: {
            NfccNciAdapter::GetInstance().DoNfaDmSetConfig();
            break;
        }

        case NFA_DM_SET_POWER_SUB_STATE_EVT: {
            DebugLog("NfaDeviceManagementCallback: NFA_DM_SET_POWER_SUB_STATE_EVT; status=0x%{public}X",
                     eventData->power_mode.status);
            NfccNciAdapter::GetInstance().DoNfaSetPowerSubState();
            break;
        }

        case NFA_SET_TAG_RO_EVT: {
            DebugLog("NfaDeviceManagementCallback: NFA_SET_TAG_RO_EVT; status = 0x%{public}X", eventData->status);
            TagNciAdapterNtf::GetInstance().HandleSetReadOnlyResult(eventData->status);
            break;
        }
        default: {
            ErrorLog("NfaDeviceManagementCallback: unknown event %{public}d", dmEvent);
            break;
        }
    }
}

tNFA_STATUS NfccNciAdapter::NfaRegVSCback(bool isRegster, tNFA_VSC_CBACK* vscCback)
{
    return NFA_STATUS_OK;
}

void NfccNciAdapter::PrivateNciCallback(uint8_t event, uint16_t paramLen, uint8_t *param)
{
}

/**
 * @brief Initialize nfc.
 * @return true/false - initialize is successful or not successful.
 */
bool NfccNciAdapter::Initialize()
{
    DebugLog("NfccNciAdapter::Initialize");
    tNFA_STATUS status = NFA_STATUS_FAILED;
    if (isNfcEnabled_) {
        WarnLog("NfccNciAdapter::Initialize: already enabled");
        return isNfcEnabled_;
    }

    NfcAdaptation::GetInstance().Initialize();  // start GKI, NCI task, NFC task
    SynchronizeGuard guard(nfcEnableEvent_);
    tHAL_NFC_ENTRY* halFuncEntries = NfcAdaptation::GetInstance().GetHalEntryFuncs();

    NFA_Init(halFuncEntries);
    status = NFA_Enable(NfcDeviceManagementCallback, NfcConnectionCallback);
    if (status == NFA_STATUS_OK) {
        if (nfcEnableEvent_.Wait(NCI_EVT_WAIT_TIMEOUT) == false) {
            ErrorLog("NfccNciAdapter::Initialize : Enable nfc timeout");
        }
    }
    Extns::GetInstance().EXTNS_Init(NfcDeviceManagementCallback, NfcConnectionCallback);
    NfaRegVSCback(true, PrivateNciCallback);

    if (status == NFA_STATUS_OK) {
        // sIsNfaEnabled indicates whether stack started successfully
        if (isNfcEnabled_) {
#ifdef _NFC_SERVICE_HCE_
            NciBalCe::GetInstance().InitializeCe();
            HciManager::GetInstance().Initialize();
#endif
            isRoutingInited_ = RoutingManager::GetInstance().Initialize();
            TagNciAdapterNtf::GetInstance().RegisterNdefHandler();
            discoveryDuration_ = DEFAULT_DISCOVERY_DURATION;
            NFA_SetRfDiscoveryDuration(static_cast<uint16_t>(discoveryDuration_));
            DebugLog("NfccNciAdapter::Initialize: nfc enabled = %{public}d", isNfcEnabled_);
            return isNfcEnabled_;
        }
    }
    ErrorLog("NfccNciAdapter::Initialize: fail nfa enable; error = %{public}d", status);
    if (isNfcEnabled_) {
        Extns::GetInstance().EXTNS_Close();
        status = NFA_Disable(false);
        DebugLog("NfccNciAdapter::Initialize: status = %{public}d", status);
    }
    NfcAdaptation::GetInstance().Finalize();
    DebugLog("NfccNciAdapter::Initialize: nfc enabled = %{public}d", isNfcEnabled_);
    return isNfcEnabled_;
}

/**
 * @brief Deinitialize nfc.
 * @return true/false - deinitialize is successful or not successful.
 */
bool NfccNciAdapter::Deinitialize()
{
    DebugLog("NfccNciAdapter::Deinitialize");
    if (!IsNfcActive()) {
        WarnLog("NfccNciAdapter::Deinitialize: Nfc not initialized");
        return NFA_STATUS_OK;
    }

    tNFA_STATUS status = NFA_STATUS_OK;
    isDisabling_ = true;

#ifdef _NFC_SERVICE_HCE_
    NciBalCe::GetInstance().Deinitialize();
#endif
    RoutingManager::GetInstance().Deinitialize();

    if (isNfcEnabled_) {
        SynchronizeGuard guard(nfcDisableEvent_);
        Extns::GetInstance().EXTNS_Close();
        status = NFA_Disable(true);
        if (status == NFA_STATUS_OK) {
            if (nfcDisableEvent_.Wait(NCI_EVT_WAIT_TIMEOUT) == false) {
                ErrorLog("NfccNciAdapter::Deinitialize : disable nfc timeout");
            } else {
                DebugLog("NfccNciAdapter::Deinitialize: wait for completion");
            }
        } else {
            ErrorLog("NfccNciAdapter::Deinitialize: fail disable; error = 0x%{public}X", status);
        }
    }
    isNfcEnabled_ = false;
    isRoutingInited_ = false;
    discoveryEnabled_ = false;
    isDisabling_ = false;
    pollingEnabled_ = false;

    NfcAdaptation::GetInstance().Finalize();
    NfaRegVSCback(false, PrivateNciCallback);
    DebugLog("NfccNciAdapter::Deinitialize: exit");
    return (status == NFA_STATUS_OK);
}

/**
 * @brief whether to enable discovery for nfc.
 * @param techMask Supported rf technology for nfc.
 * @param enableReaderMode True/false to enable/disable reader mode
 * @param enableHostRouting True/false to enable/disable host routing
 * @param restart True/false to restart or not restart
 */
void NfccNciAdapter::EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart)
{
    DebugLog("NfccNciAdapter::EnableDiscovery");
    if (!IsNfcActive()) {
        ErrorLog("NfccNciAdapter::EnableDiscovery: Nfc not initialized.");
        return;
    }

    if (discoveryEnabled_ && !restart) {
        WarnLog("NfccNciAdapter::EnableDiscovery: already discovering");
        return;
    }

    if (rfEnabled_) {
        // Stop RF discovery to reconfigure
        StartRfDiscovery(false);
    }

    tNFA_TECHNOLOGY_MASK technologyMask = techMask & DEFAULT_TECH_MASK;
    if (technologyMask != 0) {
        StopPolling();
        StartPolling(technologyMask);
        if (pollingEnabled_) {
            if (enableReaderMode && !readerModeEnabled_) {
                readerModeEnabled_ = true;
                NFA_DisableListening();
                NFA_SetRfDiscoveryDuration(DISCOVERY_DURATION);
            } else if (!enableReaderMode && readerModeEnabled_) {
                readerModeEnabled_ = false;
                NFA_EnableListening();
                NFA_SetRfDiscoveryDuration(DISCOVERY_DURATION);
            }
        }
    } else {
        StopPolling();
    }
#ifdef _NFC_SERVICE_HCE_
    NciBalCe::GetInstance().EnableHostRouting(enableHostRouting);
    NciBalCe::GetInstance().CommitRouting();
#endif

    StartRfDiscovery(true);
    discoveryEnabled_ = true;
    DebugLog("NfccNciAdapter::EnableDiscovery: exit");
}

/**
 * @brief Disable discovery for nfc.
 */
void NfccNciAdapter::DisableDiscovery()
{
    DebugLog("NfccNciAdapter::DisableDiscovery");
    if (!IsNfcActive()) {
        ErrorLog("NfccNciAdapter::DisableDiscovery: Nfc not initialized.");
        return;
    }
    if (!discoveryEnabled_) {
        WarnLog("NfccNciAdapter::DisableDiscovery: already disabled");
        return;
    }
    // Stop RF Discovery.
    StartRfDiscovery(false);
    if (pollingEnabled_) {
        StopPolling();
    }
    discoveryEnabled_ = false;
    readerModeEnabled_ = false;
    DebugLog("NfccNciAdapter::DisableDiscovery: exit");
}

/**
 * @brief Send raw data.
 * @param rawData Data needed to send
 * @return True/false to successful/failed to send
 */
bool NfccNciAdapter::SendRawFrame(std::string& rawData)
{
    uint16_t length = KITS::NfcSdkCommon::GetHexStrBytesLen(rawData);
    uint8_t data[length];
    for (uint32_t i = 0; i < length; i++) {
        data[i] = KITS::NfcSdkCommon::GetByteFromHexStr(rawData, i);
    }
    tNFA_STATUS status = NFA_SendRawFrame(data, length, 0);
    InfoLog("SendRawFrame status = %{public}d", status);
    if (status != NFA_STATUS_OK) {
        ErrorLog("NfccNciAdapter::SendRawFrame failed. status = %{public}X", status);
    }
    return status == NFA_STATUS_OK;
}

uint8_t NfccNciAdapter::GetDiscovryParam(unsigned char screenState, unsigned char screenStateMask)
{
    // discocery parameters for SCREEN OFF_LOCKED or OFF_UNLOCKED
    if (screenState == NFA_SCREEN_STATE_OFF_LOCKED || screenState == NFA_SCREEN_STATE_OFF_UNLOCKED) {
        return (NCI_POLLING_DH_DISABLE_MASK | NCI_LISTEN_DH_NFCEE_DISABLE_MASK);
    }

    // discocery parameters for SCREEN ON_LOCKED
    if (screenState == NFA_SCREEN_STATE_ON_LOCKED) {
        return (screenStateMask & NFA_SCREEN_POLLING_TAG_MASK)
                ? (NCI_POLLING_DH_ENABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK)
                : (NCI_POLLING_DH_DISABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK);
    }

    // discocery parameters for SCREEN ON_UNLOCKED
    if (screenState == NFA_SCREEN_STATE_ON_UNLOCKED) {
        return (NCI_POLLING_DH_ENABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK);
    }

    // default discocery parameters
    return (NCI_POLLING_DH_ENABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK);
}

/**
 * @brief Send the status of screen.
 * @param screenStateMask The state of screen
 */
void NfccNciAdapter::SetScreenStatus(unsigned char screenStateMask)
{
    DebugLog("NfccNciAdapter::SetScreenStatus");
    if (!IsNfcActive()) {
        DebugLog("Do not handle Screen state change when NFC is not active");
        return;
    }
    unsigned char screenState = screenStateMask & NFA_SCREEN_STATE_MASK;
    if (curScreenState_ == screenState) {
        WarnLog("Screen state not changed");
        return;
    }
    if (GetNciVersion() != NCI_VERSION_2_0) {
        WarnLog("only update curScreenState when NCI version under 2.0");
        curScreenState_ = screenState;
        return;
    }

    // set power state for screen state.
    tNFA_STATUS status = NFA_STATUS_FAILED;
    unsigned char curScreenState = NFA_SCREEN_STATE_OFF_LOCKED | NFA_SCREEN_STATE_OFF_UNLOCKED |
        NFA_SCREEN_STATE_ON_LOCKED | NFA_SCREEN_STATE_UNKNOWN;
    if ((curScreenState_ & curScreenState) != 0) {
        SynchronizeGuard guard(nfcSetPowerSubStateEvent_);
        status = NFA_SetPowerSubStateForScreenState(screenState);
        if (status != NFA_STATUS_OK) {
            ErrorLog("NFA_SetPowerSubStateForScreenState fail, error=0x%{public}X, screenState = %{public}X,\
                curScreenState_ = %{public}X", status, screenState, curScreenState_);
            return;
        }
        if (nfcSetPowerSubStateEvent_.Wait(NCI_EVT_WAIT_TIMEOUT) == false) {
            ErrorLog("NfccNciAdapter::SetScreenStatus : SetScreenStatus nfc timeout");
        }
    }

    uint8_t discParam = GetDiscovryParam(screenState, screenStateMask);
    SynchronizeGuard guard(nfcSetConfigEvent_);
    status = NFA_SetConfig(NCI_PARAM_ID_CON_DISCOVERY_PARAM,
        NCI_PARAM_LEN_CON_DISCOVERY_PARAM, &discParam);
    if (status != NFA_STATUS_OK) {
        ErrorLog("NFA_SetConfig fail, error=0x%{public}X", status);
        return;
    }
    if (nfcSetConfigEvent_.Wait(NCI_EVT_WAIT_TIMEOUT) == false) {
        ErrorLog("NfccNciAdapter::SetScreenStatus : nfcSetConfigEvent_ nfc timeout");
    }

    if (curScreenState_ == NFA_SCREEN_STATE_ON_UNLOCKED) {
        SynchronizeGuard guard(nfcSetPowerSubStateEvent_);
        status = NFA_SetPowerSubStateForScreenState(screenState);
        if (status != NFA_STATUS_OK) {
            ErrorLog("NFA_SetPowerSubStateForScreenState fail, error=0x%{public}X", status);
            return;
        }
        if (nfcSetPowerSubStateEvent_.Wait(NCI_EVT_WAIT_TIMEOUT) == false) {
            ErrorLog("NfccNciAdapter::SetScreenStatus : SetScreenStatus nfc timeout");
        }
    }
    curScreenState_ = screenState;
    return;
}

/**
 * @brief Get nci version.
 * @return Nci version
 */
uint32_t NfccNciAdapter::GetNciVersion() const
{
    DebugLog("NfccNciAdapter::GetNciVersion");
    uint8_t version = NFC_GetNCIVersion();
    return version;
}

bool NfccNciAdapter::RegisterT3tIdentifier(const std::string& t3tIdentifier) const
{
    DebugLog("NfccNciAdapter::RegisterT3tIdentifier");
    return true;
}

void NfccNciAdapter::DeregisterT3tIdentifier(uint32_t handle) const
{
    DebugLog("NfccNciAdapter::DeregisterT3tIdentifier");
}

void NfccNciAdapter::ClearT3tIdentifiersCache()
{
    DebugLog("NfccNciAdapter::ClearT3tIdentifiersCache");
}

uint32_t NfccNciAdapter::GetLfT3tMax()
{
    DebugLog("NfccNciAdapter::GetLfT3tMax");
    return 0;
}

uint32_t NfccNciAdapter::GetLastError()
{
    DebugLog("NfccNciAdapter::GetLastError");
    return 0;
}

void NfccNciAdapter::Abort()
{
    InfoLog("NfccNciAdapter::Abort");
    _exit(0);
}

/**
 * @brief Check whether to load firmware.
 * @return True/false to success/fail to load firmware.
 */
bool NfccNciAdapter::CheckFirmware()
{
    DebugLog("NfccNciAdapter::CheckFirmware");
    NfcAdaptation::GetInstance().Initialize();
    NfcAdaptation::GetInstance().DownloadFirmware(nullptr, true);
    NfcAdaptation::GetInstance().Finalize();
    return true;
}

/**
 * @brief Dump debug info for nfc.
 * @param fd File descriptor to store debug info.
 */
void NfccNciAdapter::Dump(uint32_t fd) const
{
    DebugLog("NfccNciAdapter::Dump, fd=%{public}d", fd);
    NfcAdaptation::GetInstance().Dump(fd);
}

/**
 * @brief Reset nfc chip.
 */
void NfccNciAdapter::FactoryReset() const
{
    DebugLog("NfccNciAdapter::FactoryReset");
    NfcAdaptation::GetInstance().FactoryReset();
}

/**
 * @brief Close nfc.
 */
void NfccNciAdapter::Shutdown() const
{
    DebugLog("NfccNciAdapter::Shutdown");
    NfcAdaptation::GetInstance().DeviceShutdown();
}

/**
 * @brief Query whether to start rf discovery.
 * @return True/false to start/stop rf discovery.
 */
bool NfccNciAdapter::IsRfEbabled()
{
    return rfEnabled_;
}

/**
 * @brief Config commit routing table for nfc.
 * @return True/false to be successful/failed to config routing table.
 */
bool NfccNciAdapter::CommitRouting()
{
    return RoutingManager::GetInstance().CommitRouting();
}

/**
 * @brief Computer routing params.
 * @return True/false to be successful/failed to computer params.
 */
bool NfccNciAdapter::ComputeRoutingParams(int defaultPaymentType)
{
    return RoutingManager::GetInstance().ComputeRoutingParams(defaultPaymentType);
}

void NfccNciAdapter::OnCardEmulationData(const std::vector<uint8_t> &data)
{
    DebugLog("NfccNciAdapter::OnCardEmulationData");
    cardEmulationListener_.lock()->OnCardEmulationData(data);
}

void NfccNciAdapter::OnCardEmulationActivated()
{
    DebugLog("NfccNciAdapter::OnCardEmulationActivated");
    cardEmulationListener_.lock()->OnCardEmulationActivated();
}
void  NfccNciAdapter::OnCardEmulationDeactivated()
{
    DebugLog("NfccNciAdapter::OnCardEmulationDeactivated");
    cardEmulationListener_.lock()->OnCardEmulationDeactivated();
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
