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
#include "nfcc_host.h"
#include "loghelper.h"
#include "nfc_config.h"
#include "nfc_sdk_common.h"
#include "nci_adaptations.h"
#include "tag_nci_adapter.h"

using namespace OHOS::NFC;
namespace OHOS {
namespace NFC {
namespace NCI {
static const int ISO_DEP_MAX_TRANSEIVE_LENGTH = 0xFEFF;

OHOS::NFC::SynchronizeEvent NfccNciAdapter::nfcEnableEvent_;
OHOS::NFC::SynchronizeEvent NfccNciAdapter::nfcDisableEvent_;

bool NfccNciAdapter::isNfcEnabled_ = false;
bool NfccNciAdapter::rfEnabled_ = false;
bool NfccNciAdapter::discoveryEnabled_ = false;  // is polling or listening
bool NfccNciAdapter::pollingEnabled_ = false;    // is polling for tag
bool NfccNciAdapter::isDisabling_ = false;
bool NfccNciAdapter::readerModeEnabled_ = false;
unsigned long NfccNciAdapter::discoveryDuration_;
bool NfccNciAdapter::isTagActive_ = false;
unsigned char NfccNciAdapter::curScreenState_ = NFA_SCREEN_STATE_OFF_LOCKED;
std::shared_ptr<INfcNci> NfccNciAdapter::nciAdaptation_ = std::make_shared<NciAdaptations>();

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

void NfccNciAdapter::SetNciAdaptation(std::shared_ptr<INfcNci> nciAdaptation)
{
    nciAdaptation_ = nciAdaptation;
}

void NfccNciAdapter::StartRfDiscovery(bool isStart) const
{
    DebugLog("NfccNciAdapter::StartRfDiscovery: isStart= %{public}d", isStart);
    tNFA_STATUS status;
    if (isStart) {
        status = nciAdaptation_->NfaStartRfDiscovery();
    } else {
        status = nciAdaptation_->NfaStopRfDiscovery();
    }
    if (status == NFA_STATUS_OK) {
        rfEnabled_ = isStart;
    } else {
        DebugLog("NfccNciAdapter::StartRfDiscovery: Failed to start/stop RF discovery; error=0x%{public}X", status);
    }
}

tNFA_STATUS NfccNciAdapter::StartPolling(tNFA_TECHNOLOGY_MASK techMask) const
{
    DebugLog("NfccNciAdapter::StartPolling, techMask = 0x%{public}02X", techMask);
    tNFA_STATUS status = nciAdaptation_->NfaEnablePolling(techMask);
    if (status == NFA_STATUS_OK) {
        DebugLog("StartPolling: wait for enable event");
        pollingEnabled_ = true;
    } else {
        DebugLog("NfccNciAdapter::StartPolling: fail enable polling; error = 0x%{public}X", status);
    }
    return status;
}

tNFA_STATUS NfccNciAdapter::StopPolling() const
{
    DebugLog("NfccNciAdapter::StopPolling");
    tNFA_STATUS status = nciAdaptation_->NfaDisablePolling();
    if (status == NFA_STATUS_OK) {
        pollingEnabled_ = false;
    } else {
        DebugLog("NfccNciAdapter::StopPolling: fail disable polling; error = 0x%{public}X", status);
    }
    return status;
}

void NfccNciAdapter::DoNfaActivatedEvt(tNFA_CONN_EVT_DATA* eventData)
{
    if (isDisabling_) {
        return;
    }
    if (eventData->activated.activate_ntf.protocol == NCI_PROTOCOL_NFC_DEP) {
        DebugLog("Is peer to peer");
        return;
    }

    if (eventData->activated.activate_ntf.rf_tech_param.mode < NCI_DISCOVERY_TYPE_LISTEN_A &&
        eventData->activated.activate_ntf.intf_param.type != NFC_INTERFACE_EE_DIRECT_RF) {
        isTagActive_ = true;
        /* Is polling and is not ee direct rf */
        if (TagNciAdapter::GetInstance().IsReconnecting()) {
            DebugLog("isReconnect, %{public}d", TagNciAdapter::GetInstance().IsReconnecting());
            TagNciAdapter::GetInstance().HandleActivatedResult();
            return;
        }
        TagNciAdapter::GetInstance().ResetTagFieldOnFlag();
        TagNciAdapter::GetInstance().BuildTagInfo(eventData);
    }
    if (TagNciAdapter::GetInstance().GetDiscRstEvtNum() > 0) {
        NFA_Deactivate(true);
    }
}

void NfccNciAdapter::DoNfaDeactivatedEvt(tNFA_CONN_EVT_DATA* eventData)
{
    TagNciAdapter::GetInstance().SelectTheNextTag();
    if (eventData->deactivated.type == NFA_DEACTIVATE_TYPE_SLEEP) {
        DebugLog("Enter sleep mode");
        return;
    }
    TagNciAdapter::GetInstance().HandleDeactivatedResult();
    isTagActive_ = false;
}

void NfccNciAdapter::DoNfaDiscResultEvt(tNFA_CONN_EVT_DATA* eventData)
{
    static tNFA_STATUS status = eventData->disc_result.status;
    DebugLog("DoNfaDiscResultEvt: status = 0x%{public}X", status);
    if (status != NFA_STATUS_OK) {
        TagNciAdapter::GetInstance().SetDiscRstEvtNum(0);
    } else {
        TagNciAdapter::GetInstance().GetMultiTagTechsFromData(eventData->disc_result);
        TagNciAdapter::GetInstance().SetDiscRstEvtNum(TagNciAdapter::GetInstance().GetDiscRstEvtNum() + 1);
        if (eventData->disc_result.discovery_ntf.more == NCI_DISCOVER_NTF_MORE) {
            return;
        }
        if (TagNciAdapter::GetInstance().GetDiscRstEvtNum() > 1) {
            TagNciAdapter::GetInstance().SetIsMultiTag(true);
        }
        TagNciAdapter::GetInstance().SetDiscRstEvtNum(TagNciAdapter::GetInstance().GetDiscRstEvtNum() - 1);
        // select the first tag of multiple tags that discovered
        TagNciAdapter::GetInstance().SelectTheFirstTag();
    }
}

void NfccNciAdapter::DoNfaPresenceEvt(tNFA_CONN_EVT_DATA* eventData)
{
    static tNFA_STATUS curStatus = NFA_STATUS_FAILED;
    if (eventData->status != curStatus) {
        curStatus = eventData->status;
    }
    TagNciAdapter::GetInstance().HandleFieldCheckResult(curStatus);
}

void NfccNciAdapter::NfcConnectionCallback(uint8_t connEvent, tNFA_CONN_EVT_DATA* eventData)
{
    switch (connEvent) {
        /* whether polling successfully started */
        case NFA_POLL_ENABLED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_POLL_ENABLED_EVT: status = %{public}u", eventData->status);
            break;
        }
        /* Listening/Polling stopped */
        case NFA_POLL_DISABLED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_POLL_DISABLED_EVT: status = %{public}u", eventData->status);
            break;
        }
        /* RF Discovery started event */
        case NFA_RF_DISCOVERY_STARTED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_RF_DISCOVERY_STARTED_EVT: status = %{public}u", eventData->status);
            break;
        }
        /* RF Discovery stopped event */
        case NFA_RF_DISCOVERY_STOPPED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_RF_DISCOVERY_STOPPED_EVT: status = %{public}u", eventData->status);
            break;
        }
        /* NFC link/protocol activated */
        case NFA_ACTIVATED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_ACTIVATED_EVT");
            DoNfaActivatedEvt(eventData);
            break;
        }
        /* NFC link/protocol deactivated */
        case NFA_DEACTIVATED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_DEACTIVATED_EVT");
            DoNfaDeactivatedEvt(eventData);
            break;
        }
        case NFA_DISC_RESULT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_DISC_RESULT_EVT");
            DoNfaDiscResultEvt(eventData);
            break;
        }
        case NFA_SELECT_RESULT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_SELECT_RESULT_EVT: status = 0x%{public}X", eventData->status);
            TagNciAdapter::GetInstance().HandleSelectResult();
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
            DoNfaPresenceEvt(eventData);
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
                static_cast<unsigned int>(eventData->ndef_detect.max_size),
                static_cast<unsigned int>(eventData->ndef_detect.cur_size), eventData->ndef_detect.flags);
            TagNciAdapter::GetInstance().HandleNdefCheckResult(eventData->ndef_detect.status,
                                                               eventData->ndef_detect.cur_size,
                                                               eventData->ndef_detect.flags,
                                                               eventData->ndef_detect.max_size);
            break;
        }
        default: {
            DebugLog("NfaConnectionCallback: unknown event %{public}u", connEvent);
            break;
        }
    }
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
        if (eventData->rf_field.rf_field_status == NFA_DM_RF_FIELD_ON) {
            NfccHost::RemoteFieldActivated();
        } else {
            NfccHost::RemoteFieldDeactivated();
        }
    }
}

void NfccNciAdapter::DoNfaDmNfccTimeoutEvt(tNFA_DM_CBACK_DATA* eventData)
{
    discoveryEnabled_ = false;
    pollingEnabled_ = false;

    if (IsNfcActive()) {
        nciAdaptation_->NfaDisable(FALSE);
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
            DoNfaDmEnableEvt(eventData);
            break;
        }
        /* Result of NFA_Disable */
        case NFA_DM_DISABLE_EVT: {
            DebugLog("NfaDeviceManagementCallback: NFA_DM_DISABLE_EVT");
            DoNfaDmDisableEvt(eventData);
            break;
        }

        case NFA_DM_RF_FIELD_EVT: {
            DebugLog("NfaDeviceManagementCallback: NFA_DM_RF_FIELD_EVT; status = 0x%{public}X; field status = "
                "%{public}u", eventData->rf_field.status, eventData->rf_field.rf_field_status);
            DoNfaDmRfFieldEvt(eventData);
            break;
        }

        case NFA_DM_NFCC_TRANSPORT_ERR_EVT:
        case NFA_DM_NFCC_TIMEOUT_EVT: {
            DoNfaDmNfccTimeoutEvt(eventData);
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
        
        default: {
            DebugLog("NfaDeviceManagementCallback: unknown event %{public}d", dmEvent);
            break;
        }
    }
}

bool NfccNciAdapter::Initialize()
{
    DebugLog("NfccNciAdapter::Initialize");
    tNFA_STATUS status;
    std::lock_guard<std::mutex> lock(mutex_);
    if (isNfcEnabled_) {
        DebugLog("NfccNciAdapter::Initialize: already enabled");
        return isNfcEnabled_;
    }

    nciAdaptation_->NfcAdaptationInitialize();  // start GKI, NCI task, NFC task
    {
        SynchronizeGuard guard(nfcEnableEvent_);
        tHAL_NFC_ENTRY* halFuncEntries = nciAdaptation_->NfcAdaptationGetHalEntryFuncs();

        nciAdaptation_->NfaInit(halFuncEntries);
        status = nciAdaptation_->NfaEnable(NfcDeviceManagementCallback, NfcConnectionCallback);
        if (status == NFA_STATUS_OK) {
            nfcEnableEvent_.Wait();
        }
    }

    if (status == NFA_STATUS_OK) {
        // sIsNfaEnabled indicates whether stack started successfully
        if (isNfcEnabled_) {
#ifdef _NFC_SERVICE_HCE_
            NciBalCe::GetInstance().InitializeCe();
            HciManager::GetInstance().Initialize();
#endif
            TagNciAdapter::GetInstance().RegisterNdefHandler();
            discoveryDuration_ = DEFAULT_DISCOVERY_DURATION;
            nciAdaptation_->NfaSetRfDiscoveryDuration((uint16_t)discoveryDuration_);
            DebugLog("NfccNciAdapter::Initialize: nfc enabled = %{public}d", isNfcEnabled_);
            return isNfcEnabled_;
        }
    }
    ErrorLog("NfccNciAdapter::Initialize: fail nfa enable; error = %{public}d", status);
    if (isNfcEnabled_) {
        /* ungraceful */
        status = nciAdaptation_->NfaDisable(false);
        DebugLog("NfccNciAdapter::Initialize: status = %{public}d", status);
    }
    nciAdaptation_->NfcAdaptationFinalize();
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

    std::lock_guard<std::mutex> lock(mutex_);
    tNFA_STATUS status = NFA_STATUS_OK;
    isDisabling_ = true;

#ifdef _NFC_SERVICE_HCE_
    NciBalCe::GetInstance().Deinitialize();
#endif

    if (isNfcEnabled_) {
        /* graceful */
        status = nciAdaptation_->NfaDisable(true);
        if (status == NFA_STATUS_OK) {
            DebugLog("NfccNciAdapter::Deinitialize: wait for completion");
        } else {
            ErrorLog("NfccNciAdapter::Deinitialize: fail disable; error = 0x%{public}X", status);
        }
    }
    isNfcEnabled_ = false;
    discoveryEnabled_ = false;
    isDisabling_ = false;
    pollingEnabled_ = false;

    nciAdaptation_->NfcAdaptationFinalize();
    DebugLog("NfccNciAdapter::Deinitialize: exit");
    return (status == NFA_STATUS_OK);
}

void NfccNciAdapter::EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart)
{
    DebugLog("NfccNciAdapter::EnableDiscovery");
    std::lock_guard<std::mutex> lock(mutex_);
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
                nciAdaptation_->NfaDisableListening();
                nciAdaptation_->NfaSetRfDiscoveryDuration(DISCOVERY_DURATION);
            } else if (!enableReaderMode && readerModeEnabled_) {
                readerModeEnabled_ = false;
                nciAdaptation_->NfaEnableListening();
                nciAdaptation_->NfaSetRfDiscoveryDuration(DISCOVERY_DURATION);
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
    std::lock_guard<std::mutex> lock(mutex_);
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
    DebugLog("NfccNciAdapter::SendRawFrame");
    std::lock_guard<std::mutex> lock(mutex_);
    uint16_t length = KITS::NfcSdkCommon::GetHexStrBytesLen(rawData);
    uint8_t data[length];
    for (uint32_t i = 0; i < length; i++) {
        data[i] = KITS::NfcSdkCommon::GetByteFromHexStr(rawData, i);
    }
    nciAdaptation_->NfaSendRawFrame(data, length, 0);
    return true;
}

uint8_t NfccNciAdapter::GetDiscovryParam(unsigned char screenState, unsigned char screenStateMask)
{
    // discocery parameters for SCREEN OFF_LOCKED or OFF_UNLOCKED
    if (screenState == NFA_SCREEN_STATE_OFF_LOCKED || screenState == NFA_SCREEN_STATE_OFF_UNLOCKED) {
        return (NCI_POLLING_DH_DISABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK);
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

void NfccNciAdapter::SetScreenStatus(unsigned char screenStateMask) const
{
    DebugLog("NfccNciAdapter::SetScreenStatus");
    unsigned char screenState = screenStateMask & NFA_SCREEN_STATE_MASK;
    if (curScreenState_ == screenState) {
        DebugLog("Screen state not changed");
        return;
    }
    if (!IsNfcActive() || GetNciVersion() != NCI_VERSION_2_0) {
        curScreenState_ = screenState;
        return;
    }

    // set power state for screen state.
    tNFA_STATUS status;
    if (curScreenState_ == NFA_SCREEN_STATE_OFF_LOCKED || curScreenState_ == NFA_SCREEN_STATE_OFF_UNLOCKED ||
        curScreenState_ == NFA_SCREEN_STATE_ON_LOCKED || curScreenState_ == NFA_SCREEN_STATE_UNKNOWN) {
        status = nciAdaptation_->NfcSetPowerSubStateForScreenState(screenState);
        if (status != NFA_STATUS_OK) {
            ErrorLog("NFA_SetPowerSubStateForScreenState fail, error=0x%{public}X", status);
            return;
        }
    }

    uint8_t discParam = GetDiscovryParam(screenState, screenStateMask);
    status = nciAdaptation_->NfcSetConfig(NCI_PARAM_ID_CON_DISCOVERY_PARAM,
        NCI_PARAM_LEN_CON_DISCOVERY_PARAM, &discParam);
    if (status != NFA_STATUS_OK) {
        ErrorLog("NFA_SetConfig fail, error=0x%{public}X", status);
        return;
    }

    if (curScreenState_ == NFA_SCREEN_STATE_ON_UNLOCKED) {
        status = nciAdaptation_->NfcSetPowerSubStateForScreenState(screenState);
        if (status != NFA_STATUS_OK) {
            ErrorLog("NFA_SetPowerSubStateForScreenState fail, error=0x%{public}X", status);
            return;
        }
    }
    curScreenState_ = screenState;
    return;
}

int NfccNciAdapter::GetNciVersion() const
{
    DebugLog("NfccNciAdapter::GetNciVersion");
    unsigned char version = nciAdaptation_->NfcGetNciVersion();
    return version;
}

int NfccNciAdapter::GetIsoDepMaxTransceiveLength()
{
    DebugLog("NfccNciAdapter::GetIsoDepMaxTransceiveLength");
    if (NfcConfig::hasKey(NAME_ISO_DEP_MAX_TRANSCEIVE)) {
        return NfcConfig::getUnsigned(NAME_ISO_DEP_MAX_TRANSCEIVE);
    } else {
        return ISO_DEP_MAX_TRANSEIVE_LENGTH;
    }
}

bool NfccNciAdapter::RegisterT3tIdentifier(const std::string& t3tIdentifier) const
{
    DebugLog("NfccNciAdapter::RegisterT3tIdentifier");
    if (t3tIdentifier.empty()) {
    }
    return false;
}

void NfccNciAdapter::DeregisterT3tIdentifier(int handle) const
{
    DebugLog("NfccNciAdapter::DeregisterT3tIdentifier");
    if (handle < 0) {
    }
}

void NfccNciAdapter::ClearT3tIdentifiersCache()
{
    DebugLog("NfccNciAdapter::ClearT3tIdentifiersCache");
}

int NfccNciAdapter::GetLfT3tMax()
{
    DebugLog("NfccNciAdapter::GetLfT3tMax");
    return 0;
}

int NfccNciAdapter::GetLastError()
{
    DebugLog("NfccNciAdapter::GetLastError");
    return 0;
}

void NfccNciAdapter::Abort()
{
    DebugLog("NfccNciAdapter::Abort");
}

bool NfccNciAdapter::CheckFirmware()
{
    DebugLog("NfccNciAdapter::CheckFirmware");
    std::lock_guard<std::mutex> lock(mutex_);
    nciAdaptation_->NfcAdaptationInitialize();
    nciAdaptation_->NfcAdaptationDownloadFirmware();
    nciAdaptation_->NfcAdaptationFinalize();
    return true;
}

void NfccNciAdapter::Dump(int fd) const
{
    DebugLog("NfccNciAdapter::Dump, fd=%{public}d", fd);
    nciAdaptation_->NfcAdaptationDump(fd);
}

void NfccNciAdapter::FactoryReset() const
{
    DebugLog("NfccNciAdapter::FactoryReset");
    nciAdaptation_->NfcAdaptationFactoryReset();
}

void NfccNciAdapter::Shutdown() const
{
    DebugLog("NfccNciAdapter::Shutdown");
    nciAdaptation_->NfcAdaptationDeviceShutdown();
}

bool NfccNciAdapter::IsRfEbabled()
{
    return rfEnabled_;
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
