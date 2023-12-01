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
#include "loghelper.h"
#include "nfc_config.h"
#include "nfc_sdk_common.h"
#include "routing_manager.h"
#include "tag_nci_adapter.h"

using namespace OHOS::NFC;
namespace OHOS {
namespace NFC {
namespace NCI {
// values for SAK28 issue
static unsigned int g_isoMifareBitMap = 0;
static bool g_isIsoMifareFlag = false;
static int isoMifareUid[NCI_NFCID1_MAX_LEN];
const uint8_t NCI_RF_DISCOVER_NTF_FIRST_ID = 0x01;
const uint8_t NCI_RF_DISCOVER_NTF_SECOND_ID = 0x02;
const unsigned int FLAG_MULTI_TAG_ISO_DEP = 0x01;
const unsigned int FLAG_MULTI_TAG_MIFARE = 0x02;

NfccNciAdapter::NfccNciAdapter() = default;

NfccNciAdapter::~NfccNciAdapter() = default;

NfccNciAdapter& NfccNciAdapter::GetInstance()
{
    static NfccNciAdapter nfccNciAdapter;
    return nfccNciAdapter;
}

bool NfccNciAdapter::IsNfcActive()
{
    bool isActive = (isNfcEnabled_ && !isDisabling_);
    return isActive;
}

bool NfccNciAdapter::IsTagActive() const
{
    return isTagActive_;
}

/**
 * @brief Set card emulation listener to receive filed on/off event.
 * @param listener The listener to receive filed on/off event.
 */
void NfccNciAdapter::SetCeHostListener(std::weak_ptr<INciCeInterface::ICeHostListener> listener)
{
    cardEmulationListener_ = listener;
}

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
        rfEnabled_ = isStart;
        nfcStartStopPollingEvent_.Wait();
    } else {
        DebugLog("NfccNciAdapter::StartRfDiscovery: Failed to start/stop RF discovery; error=0x%{public}X", status);
    }
}

tNFA_STATUS NfccNciAdapter::StartPolling(tNFA_TECHNOLOGY_MASK techMask)
{
    DebugLog("NfccNciAdapter::StartPolling, techMask = 0x%{public}02X", techMask);
    tNFA_STATUS status = NFA_EnablePolling(techMask);
    if (status == NFA_STATUS_OK) {
        DebugLog("StartPolling: wait for enable event");
        pollingEnabled_ = true;
        // wait for NFA_POLL_ENABLED_EVT
        nfcStartStopPollingEvent_.Wait();
    } else {
        DebugLog("NfccNciAdapter::StartPolling: fail enable polling; error = 0x%{public}X", status);
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
        nfcStartStopPollingEvent_.Wait();
    } else {
        DebugLog("NfccNciAdapter::StopPolling: fail disable polling; error = 0x%{public}X", status);
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
    if (eventData == nullptr) {
        WarnLog("DoNfaActivatedEvt, invalid arg.");
        return;
    }
    uint8_t actProto = (tNFA_INTF_TYPE)eventData->activated.activate_ntf.protocol;
    if (actProto == NFC_PROTOCOL_T5T && TagNciAdapter::GetInstance().GetDiscRstEvtNum()) {
        // protocol T5T only support single protocol detection
        DebugLog("DoNfaActivatedEvt, NFC_PROTOCOL_T5T not support multi tag.");
        TagNciAdapter::GetInstance().SetDiscRstEvtNum(0);
    }
#if (NXP_EXTNS == TRUE)
    TagNciAdapter::GetInstance().isIsoDepDhReqFailed_ = false;
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
        TagNciAdapter::GetInstance().SetCurrRfInterface(
            (tNFA_INTF_TYPE)eventData->activated.activate_ntf.intf_param.type);
        TagNciAdapter::GetInstance().SetCurrRfProtocol(actProto);
#if (NXP_EXTNS == TRUE)
        uint8_t mode = eventData->activated.activate_ntf.rf_tech_param.mode;
        TagNciAdapter::GetInstance().SetCurrRfMode(mode);
        if (mode == NFC_DISCOVERY_TYPE_POLL_B || mode == NFC_DISCOVERY_TYPE_POLL_B_PRIME) {
            TagNciAdapter::GetInstance().SetNfcID0ForTypeB(
                eventData->activated.activate_ntf.rf_tech_param.param.pb.nfcid0);
        }
#endif
    }

#if (NXP_EXTNS == TRUE)
    //clear MulitProto Mifare Tag state on single proto tag activation
    if (!TagNciAdapter::GetInstance().GetIsMultiTag() &&
        TagNciAdapter::GetInstance().IsMultiMFCTag()) {
        TagNciAdapter::GetInstance().ClearMultiMFCTagState();
    }
#endif

    // handle ActivatedResult for Mifare tag
    if (Extns::GetInstance().EXTNS_GetConnectFlag() == true) {
        TagNciAdapter::GetInstance().SetTagActivated();
        TagNciAdapter::GetInstance().SetConnectStatus(true);
        return;
    }

    // handle ActivationResult for normal tags
    if (isDisabling_ || !isNfcEnabled_) {
        return;
    }
    isTagActive_ = true;
#if (NXP_EXTNS != TRUE)
    TagNciAdapter::GetInstance().SetTagActivated();
#endif
    if (TagNciAdapter::GetInstance().IsSwitchingRfIface()) {
#if (NXP_EXTNS == TRUE)
        if (TagNciAdapter::GetInstance().IsExpectedActRfProtocol(actProto)) {
            TagNciAdapter::GetInstance().SetTagActivated();
        }
#endif
        TagNciAdapter::GetInstance().SetConnectStatus(true);
        return;
    }
#if (NXP_EXTNS == TRUE)
    TagNciAdapter::GetInstance().SetTagActivated();
#endif
    TagNciAdapter::GetInstance().ResetTagFieldOnFlag();

    if (actProto == NFA_PROTOCOL_NFC_DEP) {
        // we do not support peer to peer
    } else {
        TagNciAdapter::GetInstance().HandleActivatedResult(eventData);
        if (TagNciAdapter::GetInstance().GetDiscRstEvtNum()) {
            // do deactivate to sleep and wait for reselect for multi tag
            NFA_Deactivate(true);
        }
        // skipped notify secureelement
    }
}

void NfccNciAdapter::DoNfaDeactivatedEvt(tNFA_CONN_EVT_DATA* eventData)
{
    tNFA_DEACTIVATE_TYPE type = eventData->deactivated.type;
    TagNciAdapter::GetInstance().SetTagDeactivated((type == NFA_DEACTIVATE_TYPE_SLEEP));
    TagNciAdapter::GetInstance().SelectTheNextTag();
    if (eventData->deactivated.type != NFA_DEACTIVATE_TYPE_SLEEP) {
        isTagActive_ = false;
#if (NXP_EXTNS == TRUE)
        TagNciAdapter::GetInstance().SetDiscRstEvtNum(0);
#endif
        TagNciAdapter::GetInstance().ResetTagFieldOnFlag();
#if (NXP_EXTNS == TRUE)
        if (!TagNciAdapter::GetInstance().IsSwitchingRfIface()) {
            TagNciAdapter::GetInstance().HandleDeactivatedResult(type);
            TagNciAdapter::GetInstance().AbortWait();
        }
#else
        TagNciAdapter::GetInstance().HandleDeactivatedResult(type);
        TagNciAdapter::GetInstance().AbortWait();
#endif
        TagNciAdapter::GetInstance().SetIsMultiTag(false);
    } else if (TagNciAdapter::GetInstance().IsTagDeactivating()) {
        TagNciAdapter::GetInstance().SetDeactivatedStatus();
    } else if (Extns::GetInstance().EXTNS_GetDeactivateFlag()) {
        TagNciAdapter::GetInstance().SetDeactivatedStatus();
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
        TagNciAdapter::GetInstance().SetDiscRstEvtNum(0);
    } else {
        TagNciAdapter::GetInstance().HandleDiscResult(eventData);
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
        TagNciAdapter::GetInstance().SetSkipMifareInterface();
    }

    // logic for normal tag
    TagNciAdapter::GetInstance().SetDiscRstEvtNum(TagNciAdapter::GetInstance().GetDiscRstEvtNum() + 1);
    if (discNtf->more == NCI_DISCOVER_NTF_MORE) {
        // there is more discovery notification coming
        return;
    }
    if (TagNciAdapter::GetInstance().GetDiscRstEvtNum() > 1) {
        TagNciAdapter::GetInstance().SetIsMultiTag(true);
    }
    TagNciAdapter::GetInstance().SetDiscRstEvtNum(TagNciAdapter::GetInstance().GetDiscRstEvtNum() - 1);
    // select the first tag of multiple tags that is discovered
    TagNciAdapter::GetInstance().SelectTheFirstTag();
}

void NfccNciAdapter::DoNfaSelectResultEvt(uint8_t status)
{
    TagNciAdapter::GetInstance().HandleSelectResult(status);
}

void NfccNciAdapter::DoNfaPresenceEvt(tNFA_CONN_EVT_DATA* eventData)
{
    static tNFA_STATUS curStatus = NFA_STATUS_FAILED;
    if (eventData->status != curStatus) {
        curStatus = eventData->status;
    }
    TagNciAdapter::GetInstance().HandleFieldCheckResult(curStatus);

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
            TagNciAdapter::GetInstance().HandleTranceiveData(eventData->status, eventData->data.p_data,
                eventData->data.len);
            break;
        }
        case NFA_PRESENCE_CHECK_EVT: {
            NfccNciAdapter::GetInstance().DoNfaPresenceEvt(eventData);
            break;
        }
        case NFA_READ_CPLT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_READ_CPLT_EVT: status = 0x%{public}X", eventData->status);
            TagNciAdapter::GetInstance().HandleReadComplete(eventData->status);
            break;
        }
        case NFA_WRITE_CPLT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_WRITE_CPLT_EVT: status = 0x%{public}X", eventData->status);
            TagNciAdapter::GetInstance().HandleWriteComplete(eventData->status);
            break;
        }
        case NFA_FORMAT_CPLT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_FORMAT_CPLT_EVT: status = 0x%{public}X", eventData->status);
            TagNciAdapter::GetInstance().HandleFormatComplete(eventData->status);
            break;
        }
        case NFA_NDEF_DETECT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_NDEF_DETECT_EVT: status = 0x%{public}X, protocol = 0x%{public}X,"
                " max_size = %{public}u, cur_size = %{public}u, flags = 0x%{public}X",
                eventData->ndef_detect.status, eventData->ndef_detect.protocol,
                static_cast<uint32_t>(eventData->ndef_detect.max_size),
                static_cast<uint32_t>(eventData->ndef_detect.cur_size), eventData->ndef_detect.flags);
            TagNciAdapter::GetInstance().HandleNdefCheckResult(eventData->ndef_detect.status,
                                                               eventData->ndef_detect.cur_size,
                                                               eventData->ndef_detect.flags,
                                                               eventData->ndef_detect.max_size);
            break;
        }
        case NFA_SET_TAG_RO_EVT: {
            DebugLog("NfaDeviceManagementCallback: NFA_SET_TAG_RO_EVT; status = 0x%{public}X", eventData->status);
            TagNciAdapter::GetInstance().HandleSetReadOnlyResult(eventData->status);
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
    isNfcEnabled_ = false;
    isDisabling_ = false;
}

void NfccNciAdapter::DoNfaDmRfFieldEvt(tNFA_DM_CBACK_DATA* eventData)
{
    if (eventData->rf_field.status == NFA_STATUS_OK) {
        // notify field on/off event to nfc service.
        if (!cardEmulationListener_.expired()) {
            if (eventData->rf_field.rf_field_status == NFA_DM_RF_FIELD_ON) {
                cardEmulationListener_.lock()->FieldActivated();
            } else {
                cardEmulationListener_.lock()->FieldDeactivated();
            }
        }
    }
}

void NfccNciAdapter::DoNfaDmNfccTimeoutEvt(tNFA_DM_CBACK_DATA* eventData)
{
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
            DebugLog("NfaDeviceManagementCallback: NFA_DM_SET_CONFIG_EVT");
            break;
        }

        case NFA_DM_SET_POWER_SUB_STATE_EVT: {
            DebugLog("NfaDeviceManagementCallback: NFA_DM_SET_POWER_SUB_STATE_EVT; status=0x%{public}X",
                     eventData->power_mode.status);
            break;
        }

        case NFA_SET_TAG_RO_EVT: {
            DebugLog("NfaDeviceManagementCallback: NFA_SET_TAG_RO_EVT; status = 0x%{public}X", eventData->status);
            TagNciAdapter::GetInstance().HandleSetReadOnlyResult(eventData->status);
            break;
        }
        default: {
            DebugLog("NfaDeviceManagementCallback: unknown event %{public}d", dmEvent);
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

bool NfccNciAdapter::Initialize()
{
    DebugLog("NfccNciAdapter::Initialize");
    tNFA_STATUS status = NFA_STATUS_FAILED;
    if (isNfcEnabled_) {
        DebugLog("NfccNciAdapter::Initialize: already enabled");
        return isNfcEnabled_;
    }

    NfcAdaptation::GetInstance().Initialize();  // start GKI, NCI task, NFC task
    {
        SynchronizeGuard guard(nfcEnableEvent_);
        tHAL_NFC_ENTRY* halFuncEntries = NfcAdaptation::GetInstance().GetHalEntryFuncs();

        NFA_Init(halFuncEntries);
        status = NFA_Enable(NfcDeviceManagementCallback, NfcConnectionCallback);
        if (status == NFA_STATUS_OK) {
            nfcEnableEvent_.Wait();
        }
        Extns::GetInstance().EXTNS_Init(NfcDeviceManagementCallback, NfcConnectionCallback);
    }

    NfaRegVSCback(true, PrivateNciCallback);

    if (status == NFA_STATUS_OK) {
        // sIsNfaEnabled indicates whether stack started successfully
        if (isNfcEnabled_) {
#ifdef _NFC_SERVICE_HCE_
            NciBalCe::GetInstance().InitializeCe();
            HciManager::GetInstance().Initialize();
#endif
            isRoutingInited_ = RoutingManager::GetInstance().Initialize();
            TagNciAdapter::GetInstance().RegisterNdefHandler();
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

bool NfccNciAdapter::Deinitialize()
{
    DebugLog("NfccNciAdapter::Deinitialize");
    if (!IsNfcActive()) {
        DebugLog("NfccNciAdapter::Deinitialize: Nfc not initialized");
        return NFA_STATUS_OK;
    }

    tNFA_STATUS status = NFA_STATUS_OK;
    isDisabling_ = true;

#ifdef _NFC_SERVICE_HCE_
    NciBalCe::GetInstance().Deinitialize();
#endif
    RoutingManager::GetInstance().Deinitialize();

    if (isNfcEnabled_) {
        Extns::GetInstance().EXTNS_Close();
        status = NFA_Disable(true);
        if (status == NFA_STATUS_OK) {
            DebugLog("NfccNciAdapter::Deinitialize: wait for completion");
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

void NfccNciAdapter::EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart)
{
    DebugLog("NfccNciAdapter::EnableDiscovery");
    if (!IsNfcActive()) {
        ErrorLog("NfccNciAdapter::EnableDiscovery: Nfc not initialized.");
        return;
    }

    if (discoveryEnabled_ && !restart) {
        DebugLog("NfccNciAdapter::EnableDiscovery: already discovering");
        return;
    }

    if (rfEnabled_) {
        // Stop RF discovery to reconfigure
        StartRfDiscovery(false);
    }

    tNFA_TECHNOLOGY_MASK technologyMask = DEFAULT_TECH_MASK;
    technologyMask = techMask & DEFAULT_TECH_MASK;

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

void NfccNciAdapter::DisableDiscovery()
{
    DebugLog("NfccNciAdapter::DisableDiscovery");
    if (!IsNfcActive()) {
        ErrorLog("NfccNciAdapter::DisableDiscovery: Nfc not initialized.");
        return;
    }
    if (!discoveryEnabled_) {
        DebugLog("NfccNciAdapter::DisableDiscovery: already disabled");
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

bool NfccNciAdapter::SendRawFrame(std::string& rawData)
{
    uint16_t length = KITS::NfcSdkCommon::GetHexStrBytesLen(rawData);
    uint8_t data[length];
    for (uint32_t i = 0; i < length; i++) {
        data[i] = KITS::NfcSdkCommon::GetByteFromHexStr(rawData, i);
    }
    tNFA_STATUS status = NFA_SendRawFrame(data, length, 0);
    InfoLog("SendRawFrame status = %{public}d", status);
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

void NfccNciAdapter::SetScreenStatus(unsigned char screenStateMask)
{
    DebugLog("NfccNciAdapter::SetScreenStatus");
    if (!IsNfcActive()) {
        DebugLog("Do not handle Screen state change when NFC is not active");
        return;
    }
    unsigned char screenState = screenStateMask & NFA_SCREEN_STATE_MASK;
    if (curScreenState_ == screenState) {
        DebugLog("Screen state not changed");
        return;
    }
    if (GetNciVersion() != NCI_VERSION_2_0) {
        DebugLog("only update curScreenState when NCI version under 2.0");
        curScreenState_ = screenState;
        return;
    }

    // set power state for screen state.
    tNFA_STATUS status = NFA_STATUS_FAILED;
    if (curScreenState_ == NFA_SCREEN_STATE_OFF_LOCKED || curScreenState_ == NFA_SCREEN_STATE_OFF_UNLOCKED ||
        curScreenState_ == NFA_SCREEN_STATE_ON_LOCKED || curScreenState_ == NFA_SCREEN_STATE_UNKNOWN) {
        status = NFA_SetPowerSubStateForScreenState(screenState);
        if (status != NFA_STATUS_OK) {
            ErrorLog("NFA_SetPowerSubStateForScreenState fail, error=0x%{public}X", status);
            return;
        }
    }

    uint8_t discParam = GetDiscovryParam(screenState, screenStateMask);
    status = NFA_SetConfig(NCI_PARAM_ID_CON_DISCOVERY_PARAM,
        NCI_PARAM_LEN_CON_DISCOVERY_PARAM, &discParam);
    if (status != NFA_STATUS_OK) {
        ErrorLog("NFA_SetConfig fail, error=0x%{public}X", status);
        return;
    }

    if (curScreenState_ == NFA_SCREEN_STATE_ON_UNLOCKED) {
        status = NFA_SetPowerSubStateForScreenState(screenState);
        if (status != NFA_STATUS_OK) {
            ErrorLog("NFA_SetPowerSubStateForScreenState fail, error=0x%{public}X", status);
            return;
        }
    }
    curScreenState_ = screenState;
    return;
}

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
    DebugLog("NfccNciAdapter::Abort");
    abort();
}

bool NfccNciAdapter::CheckFirmware()
{
    DebugLog("NfccNciAdapter::CheckFirmware");
    NfcAdaptation::GetInstance().Initialize();
    NfcAdaptation::GetInstance().DownloadFirmware(nullptr, true);
    NfcAdaptation::GetInstance().Finalize();
    return true;
}

void NfccNciAdapter::Dump(uint32_t fd) const
{
    DebugLog("NfccNciAdapter::Dump, fd=%{public}d", fd);
    NfcAdaptation::GetInstance().Dump(fd);
}

void NfccNciAdapter::FactoryReset() const
{
    DebugLog("NfccNciAdapter::FactoryReset");
    NfcAdaptation::GetInstance().FactoryReset();
}

void NfccNciAdapter::Shutdown() const
{
    DebugLog("NfccNciAdapter::Shutdown");
    NfcAdaptation::GetInstance().DeviceShutdown();
}

bool NfccNciAdapter::IsRfEbabled()
{
    return rfEnabled_;
}

bool NfccNciAdapter::CommitRouting()
{
    return RoutingManager::GetInstance().CommitRouting();
}

bool NfccNciAdapter::ComputeRoutingParams()
{
    return RoutingManager::GetInstance().ComputeRoutingParams();
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
