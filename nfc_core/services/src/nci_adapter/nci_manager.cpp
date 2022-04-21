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
#include "nci_manager.h"

#include "nfcc_host.h"
#include "loghelper.h"
#include "nfc_config.h"
#include "nci_adaptations.h"

#ifdef _NFC_SERVICE_HCE_
#include "hci_manager.h"
#include "nci_bal_ce.h"
#endif

using namespace OHOS::NFC;

namespace OHOS {
namespace NFC {
namespace NCI {
bool NciManager::isNfcEnabled_ = false;
bool NciManager::rfEnabled_ = false;
bool NciManager::discoveryEnabled_ = false;  // is polling or listening
bool NciManager::pollingEnabled_ = false;    // is polling for tag
bool NciManager::isDisabling_ = false;
bool NciManager::readerModeEnabled_ = false;
unsigned long NciManager::discoveryDuration_;
bool NciManager::isReconnect_ = false;
bool NciManager::isTagActive_ = false;
unsigned char NciManager::curScreenState_ = NFA_SCREEN_STATE_OFF_LOCKED;
std::shared_ptr<ILibNfcNci> NciManager::nciAdaptation_ = std::make_shared<NciAdaptations>();

NciManager::NciManager() = default;

NciManager::~NciManager() = default;

NciManager& NciManager::GetInstance()
{
    static NciManager mNciManager;
    return mNciManager;
}

bool NciManager::IsNfcActive()
{
    bool isActive = (isNfcEnabled_ && !isDisabling_);
    return isActive;
}

bool NciManager::IsTagActive() const
{
    return isTagActive_;
}

void NciManager::SetNciAdaptation(std::shared_ptr<ILibNfcNci> nciAdaptation)
{
    nciAdaptation_ = nciAdaptation;
}

void NciManager::StartRfDiscovery(bool isStart) const
{
    DebugLog("NciManager::StartRfDiscovery: isStart= %d", isStart);
    tNFA_STATUS status;
    if (isStart) {
        status = nciAdaptation_->NfaStartRfDiscovery();
    } else {
        status = nciAdaptation_->NfaStopRfDiscovery();
    }
    if (status == NFA_STATUS_OK) {
        rfEnabled_ = isStart;
    } else {
        DebugLog("NciManager::StartRfDiscovery: Failed to start/stop RF discovery; error=0x%X", status);
    }
}

tNFA_STATUS NciManager::StartPolling(tNFA_TECHNOLOGY_MASK techMask) const
{
    DebugLog("NciManager::StartPolling, techMask = 0x%02X", techMask);
    tNFA_STATUS status = nciAdaptation_->NfaEnablePolling(techMask);
    if (status == NFA_STATUS_OK) {
        DebugLog("StartPolling: wait for enable event");
        pollingEnabled_ = true;
    } else {
        DebugLog("NciManager::StartPolling: fail enable polling; error = 0x%X", status);
    }
    return status;
}

tNFA_STATUS NciManager::StopPolling() const
{
    DebugLog("NciManager::StopPolling");
    tNFA_STATUS status = nciAdaptation_->NfaDisablePolling();
    if (status == NFA_STATUS_OK) {
        pollingEnabled_ = false;
    } else {
        DebugLog("NciManager::StopPolling: fail disable polling; error = 0x%X", status);
    }
    return status;
}

void NciManager::DoNfaActivatedEvt(tNFA_CONN_EVT_DATA* eventData)
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
        if (isReconnect_) {
            DebugLog("sIsReconnect, %d", isReconnect_);
            return;
        }
    }
}

void NciManager::DoNfaDeactivatedEvt(tNFA_CONN_EVT_DATA* eventData)
{
    if (eventData->deactivated.type == NFA_DEACTIVATE_TYPE_SLEEP) {
        DebugLog("Enter sleep mode");
        isReconnect_ = true;
        return;
    }
    isTagActive_ = false;
    isReconnect_ = false;
}

void NciManager::DoNfaDiscResultEvt(tNFA_CONN_EVT_DATA* eventData)
{
    static tNFA_STATUS status = eventData->disc_result.status;
    DebugLog("DoNfaDiscResultEvt: status = 0x%X", status);
    if (status == NFA_STATUS_OK) {
        // do something
    }
}

void NciManager::DoNfaPresenceEvt(tNFA_CONN_EVT_DATA* eventData)
{
    static tNFA_STATUS curStatus = NFA_STATUS_FAILED;
    if (eventData->status != curStatus) {
        DebugLog("DoNfaPresenceEvt: status = 0x%X", eventData->status);
        curStatus = eventData->status;
    }
    return;
}

void NciManager::NfcConnectionCallback(uint8_t connEvent, tNFA_CONN_EVT_DATA* eventData)
{
    switch (connEvent) {
        /* whether polling successfully started */
        case NFA_POLL_ENABLED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_POLL_ENABLED_EVT: status = %u", eventData->status);
            break;
        }
        /* Listening/Polling stopped */
        case NFA_POLL_DISABLED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_POLL_DISABLED_EVT: status = %u", eventData->status);
            break;
        }
        /* RF Discovery started event */
        case NFA_RF_DISCOVERY_STARTED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_RF_DISCOVERY_STARTED_EVT: status = %u", eventData->status);
            break;
        }
        /* RF Discovery stopped event */
        case NFA_RF_DISCOVERY_STOPPED_EVT: {
            DebugLog("NfaConnectionCallback: NFA_RF_DISCOVERY_STOPPED_EVT: status = %u", eventData->status);
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
            DebugLog("NfaConnectionCallback: NFA_SELECT_RESULT_EVT: status = 0x%X", eventData->status);
            break;
        }
        /* Data message received (for non-NDEF reads) */
        case NFA_DATA_EVT: {
            DebugLog("NfaConnectionCallback: NFA_DATA_EVT: status = 0x%X, len = %d",
                eventData->status, eventData->data.len);
            break;
        }
        case NFA_PRESENCE_CHECK_EVT: {
            DebugLog("NfaConnectionCallback: NFA_PRESENCE_CHECK_EVT");
            DoNfaPresenceEvt(eventData);
            break;
        }
        case NFA_READ_CPLT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_READ_CPLT_EVT: status = 0x%X", eventData->status);
            break;
        }
        case NFA_WRITE_CPLT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_WRITE_CPLT_EVT: status = 0x%X", eventData->status);
            break;
        }
        case NFA_FORMAT_CPLT_EVT: {
            DebugLog("NfaConnectionCallback: NFA_FORMAT_CPLT_EVT: status = 0x%X", eventData->status);
            break;
        }
        case NFA_NDEF_DETECT_EVT: {
            DebugLog(
                "NfaConnectionCallback: NFA_NDEF_DETECT_EVT: status = 0x%X, protocol = 0x%X, max_size = %u, cur_size "
                "= %u, flags = 0x%X",
                eventData->ndef_detect.status,
                eventData->ndef_detect.protocol,
                static_cast<unsigned int>(eventData->ndef_detect.max_size),
                static_cast<unsigned int>(eventData->ndef_detect.cur_size),
                eventData->ndef_detect.flags);
            break;
        }
        default: {
            DebugLog("NfaConnectionCallback: unknown event %u", connEvent);
            break;
        }
    }
}

void NciManager::DoNfaDmEnableEvt(tNFA_DM_CBACK_DATA* eventData)
{
    isNfcEnabled_ = (eventData->status == NFA_STATUS_OK);
    isDisabling_ = false;
}

void NciManager::DoNfaDmDisableEvt(tNFA_DM_CBACK_DATA* eventData)
{
    isNfcEnabled_ = false;
    isDisabling_ = false;
}

void NciManager::DoNfaDmRfFieldEvt(tNFA_DM_CBACK_DATA* eventData)
{
    if (eventData->rf_field.status == NFA_STATUS_OK) {
        if (eventData->rf_field.rf_field_status == NFA_DM_RF_FIELD_ON) {
            NfccHost::RemoteFieldActivated();
        } else {
            NfccHost::RemoteFieldDeactivated();
        }
    }
}

void NciManager::DoNfaDmNfccTimeoutEvt(tNFA_DM_CBACK_DATA* eventData)
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

void NciManager::NfcDeviceManagementCallback(uint8_t dmEvent, tNFA_DM_CBACK_DATA* eventData)
{
    DebugLog("NfaDeviceManagementCallback: event= %u", dmEvent);

    switch (dmEvent) {
        /* Result of NFA_Enable */
        case NFA_DM_ENABLE_EVT: {
            DebugLog("NfaDeviceManagementCallback: NFA_DM_ENABLE_EVT; status = 0x%X", eventData->status);
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
            DebugLog("NfaDeviceManagementCallback: NFA_DM_RF_FIELD_EVT; status = 0x%X; field status = %u",
                     eventData->rf_field.status, eventData->rf_field.rf_field_status);
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
            DebugLog("NfaDeviceManagementCallback: NFA_DM_SET_POWER_SUB_STATE_EVT; status=0x%X",
                     eventData->power_mode.status);
            break;
        }
        
        default: {
            DebugLog("NfaDeviceManagementCallback: unknown event %d", dmEvent);
            break;
        }
    }
}

bool NciManager::Initialize()
{
    DebugLog("NciManager::Initialize");
    tNFA_STATUS status;
    std::lock_guard<std::mutex> lock(mutex_);
    if (isNfcEnabled_) {
        DebugLog("NciManager::Initialize: already enabled");
        return isNfcEnabled_;
    }

    nciAdaptation_->NfcAdaptationInitialize();  // start GKI, NCI task, NFC task
    {
        tHAL_NFC_ENTRY* halFuncEntries = nciAdaptation_->NfcAdaptationGetHalEntryFuncs();

        nciAdaptation_->NfaInit(halFuncEntries);
        status = nciAdaptation_->NfaEnable(NfcDeviceManagementCallback, NfcConnectionCallback);
        if (status == NFA_STATUS_OK) {
        }
    }

    if (status == NFA_STATUS_OK) {
        // sIsNfaEnabled indicates whether stack started successfully
        if (isNfcEnabled_) {
#ifdef _NFC_SERVICE_HCE_
            NciBalCe::GetInstance().InitializeCe();
            HciManager::GetInstance().Initialize();
#endif
            discoveryDuration_ = DEFAULT_DISCOVERY_DURATION;
            nciAdaptation_->NfaSetRfDiscoveryDuration((uint16_t)discoveryDuration_);
            DebugLog("NciManager::Initialize: nfc enabled = %x", isNfcEnabled_);
            return isNfcEnabled_;
        }
    }
    ErrorLog("NciManager::Initialize: fail nfa enable; error = 0x%X", status);
    if (isNfcEnabled_) {
        /* ungraceful */
        status = nciAdaptation_->NfaDisable(false);
    }
    nciAdaptation_->NfcAdaptationFinalize();
    DebugLog("NciManager::Initialize: nfc enabled = %x", isNfcEnabled_);
    return isNfcEnabled_;
}

bool NciManager::Deinitialize()
{
    DebugLog("NciManager::Deinitialize");
    if (!IsNfcActive()) {
        DebugLog("NciManager::Deinitialize: Nfc not initialized");
        return NFA_STATUS_OK;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    tNFA_STATUS status = NFA_STATUS_OK;
    isDisabling_ = true;

#ifdef _NFC_SERVICE_HCE_
    NciBalCe::GetInstance().Deinitialize();
#endif

    // Stop the discovery before calling NFA_Disable.
    if (rfEnabled_) {
        StartRfDiscovery(false);
    }

    if (isNfcEnabled_) {
        /* graceful */
        status = nciAdaptation_->NfaDisable(true);
        if (status == NFA_STATUS_OK) {
            DebugLog("NciManager::Deinitialize: wait for completion");
        } else {
            DebugLog("NciManager::Deinitialize: fail disable; error = 0x%X", status);
        }
    }
    isNfcEnabled_ = false;
    discoveryEnabled_ = false;
    isDisabling_ = false;
    pollingEnabled_ = false;

    nciAdaptation_->NfcAdaptationFinalize();
    DebugLog("NciManager::Deinitialize: exit");
    return (status == NFA_STATUS_OK);
}

void NciManager::EnableDiscovery(int techMask, bool enableReaderMode, bool enableHostRouting, bool restart)
{
    DebugLog("NciManager::EnableDiscovery");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsNfcActive()) {
        ErrorLog("NciManager::EnableDiscovery: Nfc not initialized.");
        return;
    }

    if (discoveryEnabled_ && !restart) {
        DebugLog("NciManager::EnableDiscovery: already discovering");
        return;
    }

    if (rfEnabled_) {
        // Stop RF discovery to reconfigure
        StartRfDiscovery(false);
    }

    tNFA_TECHNOLOGY_MASK technologyMask = DEFAULT_TECH_MASK;
    if (techMask != -1) {
        technologyMask = techMask & DEFAULT_TECH_MASK;
    }

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
    isReconnect_ = false;
    DebugLog("NciManager::EnableDiscovery: exit");
}

void NciManager::DisableDiscovery()
{
    DebugLog("NciManager::DisableDiscovery");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsNfcActive()) {
        ErrorLog("NciManager::DisableDiscovery: Nfc not initialized.");
        return;
    }
    if (!discoveryEnabled_) {
        DebugLog("NciManager::DisableDiscovery: already disabled");
        return;
    }
    // Stop RF Discovery.
    StartRfDiscovery(false);
    if (pollingEnabled_) {
        StopPolling();
    }
    discoveryEnabled_ = false;
    readerModeEnabled_ = false;
    DebugLog("NciManager::DisableDiscovery: exit");
}

bool NciManager::SendRawFrame(std::string& rawData)
{
    DebugLog("NciManager::SendRawFrame");
    std::lock_guard<std::mutex> lock(mutex_);
    nciAdaptation_->NfaSendRawFrame((uint8_t*)rawData.c_str(), (uint16_t)rawData.length(), 0);
    return true;
}

void NciManager::SetScreenStatus(unsigned char screenStateMask) const
{
    DebugLog("NciManager::SetScreenStatus");
    unsigned char screenState = screenStateMask & NFA_SCREEN_STATE_MASK;
    if (curScreenState_ == screenState) {
        DebugLog("Screen state not changed");
        return;
    }
    if (!IsNfcActive() || GetNciVersion() != NCI_VERSION_2_0) {
        curScreenState_ = screenState;
        return;
    }

    // NCI_VERSION_2_0
    tNFA_STATUS status;
    if (curScreenState_ == NFA_SCREEN_STATE_OFF_LOCKED || curScreenState_ == NFA_SCREEN_STATE_OFF_UNLOCKED ||
        curScreenState_ == NFA_SCREEN_STATE_ON_LOCKED || curScreenState_ == NFA_SCREEN_STATE_UNKNOWN) {
        status = nciAdaptation_->NfcSetPowerSubStateForScreenState(screenState);
        if (status != NFA_STATUS_OK) {
            DebugLog("NFA_SetPowerSubStateForScreenState fail, error=0x%X", status);
            return;
        }
    }

    uint8_t discovryParam = NCI_POLLING_DH_ENABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK;
    if (screenState == NFA_SCREEN_STATE_OFF_LOCKED || screenState == NFA_SCREEN_STATE_OFF_UNLOCKED) {
        discovryParam = NCI_POLLING_DH_DISABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK;
    }
    if (screenState == NFA_SCREEN_STATE_ON_LOCKED) {
        discovryParam = (screenStateMask & NFA_SCREEN_POLLING_TAG_MASK)
                            ? (NCI_POLLING_DH_ENABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK)
                            : (NCI_POLLING_DH_DISABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK);
    }
    if (screenState == NFA_SCREEN_STATE_ON_UNLOCKED) {
        discovryParam = NCI_POLLING_DH_ENABLE_MASK | NCI_LISTEN_DH_NFCEE_ENABLE_MASK;
    }
    status = nciAdaptation_->NfcSetConfig(NCI_PARAM_ID_CON_DISCOVERY_PARAM,
        NCI_PARAM_LEN_CON_DISCOVERY_PARAM, &discovryParam);
    if (status != NFA_STATUS_OK) {
        DebugLog("NFA_SetConfig fail, error=0x%X", status);
        return;
    }

    if (curScreenState_ == NFA_SCREEN_STATE_ON_UNLOCKED) {
        status = nciAdaptation_->NfcSetPowerSubStateForScreenState(screenState);
        if (status != NFA_STATUS_OK) {
            DebugLog("NFA_SetPowerSubStateForScreenState fail, error=0x%X", status);
            return;
        }
    }
    curScreenState_ = screenState;
    return;
}

int NciManager::GetNciVersion() const
{
    DebugLog("NciManager::GetNciVersion");
    unsigned char version = nciAdaptation_->NfcGetNciVersion();
    return version;
}

int NciManager::GetIsoDepMaxTransceiveLength()
{
    DebugLog("NciManager::GetIsoDepMaxTransceiveLength");
    return NfcConfig::getUnsigned(NAME_ISO_DEP_MAX_TRANSCEIVE, ISO_DEP_FRAME_MAX_LEN);
}

bool NciManager::RegisterT3tIdentifier(const std::string& t3tIdentifier) const
{
    DebugLog("NciManager::RegisterT3tIdentifier");
    if (t3tIdentifier.empty()) {
    }
    return false;
}

void NciManager::DeregisterT3tIdentifier(int handle) const
{
    DebugLog("NciManager::DeregisterT3tIdentifier");
    if (handle < 0) {
    }
}

void NciManager::ClearT3tIdentifiersCache()
{
    DebugLog("NciManager::ClearT3tIdentifiersCache");
}

int NciManager::GetLfT3tMax()
{
    DebugLog("NciManager::GetLfT3tMax");
    return 0;
}

int NciManager::GetLastError()
{
    DebugLog("NciManager::GetLastError");
    return 0;
}

void NciManager::Abort()
{
    DebugLog("NciManager::Abort");
}

bool NciManager::CheckFirmware()
{
    DebugLog("NciManager::CheckFirmware");
    std::lock_guard<std::mutex> lock(mutex_);
    nciAdaptation_->NfcAdaptationInitialize();
    nciAdaptation_->NfcAdaptationDownloadFirmware();
    nciAdaptation_->NfcAdaptationFinalize();
    return true;
}

void NciManager::Dump(int fd) const
{
    DebugLog("NciManager::Dump, fd=%d", fd);
    nciAdaptation_->NfcAdaptationDump(fd);
}

void NciManager::FactoryReset() const
{
    DebugLog("NciManager::FactoryReset");
    nciAdaptation_->NfcAdaptationFactoryReset();
}

void NciManager::Shutdown() const
{
    DebugLog("NciManager::Shutdown");
    nciAdaptation_->NfcAdaptationDeviceShutdown();
}

bool NciManager::IsRfEbabled()
{
    return rfEnabled_;
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
