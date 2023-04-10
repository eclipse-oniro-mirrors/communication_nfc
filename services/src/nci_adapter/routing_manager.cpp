/*
 * Copyright (C) 2023 - 2023 Huawei Device Co., Ltd.
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
#include "routing_manager.h"

#include "nfa_api.h"
#include "nfc_config.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace NCI {
// Every routing table entry is matches exact
static const int AID_MATCHING_EXACT_ONLY = 0x00;

// Every routing table entry matches exact or prefix
static const int AID_MATCHING_EXACT_OR_PREFIX = 0x01;

// Every routing table entry matches a prefix
static const int AID_MATCHING_PREFIX_ONLY = 0x02;

static const uint16_t DEFAULT_SYS_CODE = 0xFEFE;
static const uint8_t AID_ROUTE_QUAL_PREFIX = 0x10;
static const int DEFAULT_OFF_HOST_ROUTE_DEST = 0x01;
static const int DEFAULT_FELICA_ROUTE_DEST = 0x02;
static const int DEFAULT_EE_ROUTE_DEST = 0x01; // ese
static const std::vector<uint8_t> DEFAULT_UICC_ROUTE_DEST = {0x02, 0x03};
static const tNFA_EE_PWR_STATE DEFAULT_SYS_CODE_PWR_STA = 0x00;
static const tNFA_HANDLE DEFAULT_SYS_CODE_ROUTE_DEST = 0xC0;
static const int PWR_STA_SWITCH_ON = 0x01;
static const int PWR_STA_CREAN_ON_LOCK = 0x10;
static const int DEFAULT_PWR_STA_HOST = PWR_STA_SWITCH_ON | PWR_STA_CREAN_ON_LOCK;

RoutingManager& RoutingManager::GetInstance()
{
    static RoutingManager manager;
    return manager;
}

bool RoutingManager::Initialize()
{
    tNFA_STATUS status;
    {
        SynchronizeEvent guard(eeRegisterEvent_);
        InfoLog("Initialize: try ee register");
        status = NFA_EeRegister(NfaEeCallback);
        if (status != NFA_STATUS_OK) {
            ErrorLog("Initialize: fail ee register; error=0x%{public}X", status);
            return false;
        }
        eeRegisterEvent_.Wait();
    }
    if ((defaultOffHostRoute_ != 0) || (defaultFelicaRoute_ != 0)) {
        // Wait for EE info if needed
        SynchronizeEvent guard(eeInfoEvent_);
        if (!isEeInfoReceived_) {
            InfoLog("Initialize: Waiting for EE info");
            eeInfoEvent_.Wait();
        }
    }
    seTechMask_ = UpdateEeTechRouteSetting();

    // Set the host-routing Tech
    status = NFA_CeSetIsoDepListenTech(hostListenTechMask_ & (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B));
    if (status != NFA_STATUS_OK) {
        ErrorLog("Initialize: Failed to configure CE IsoDep technologies");
    }

    UpdateDefaultRoute();
    return true;
}

void RoutingManager::UpdateDefaultRoute()
{
    if (NFC_GetNCIVersion() != NCI_VERSION_2_0) {
        return;
    }
    tNFA_STATUS status;

    // Register System Code for routing
    SynchronizeEvent guard(routingEvent_);
    status = NFA_EeAddSystemCodeRouting(
        defaultSysCode_, defaultSysCodeRoute_,
        isSecureNfcEnabled_ ? PWR_STA_SWITCH_ON : defaultSysCodePowerstate_);
    if (status == NFA_STATUS_NOT_SUPPORTED) {
        ErrorLog("UpdateDefaultRoute: SCBR not supported");
    } else if (status == NFA_STATUS_OK) {
        routingEvent_.Wait();
        DebugLog("UpdateDefaultRoute: Succeed to register system code");
    } else {
        ErrorLog("UpdateDefaultRoute: Fail to register system code");
    }

    // Register zero lengthy Aid for default Aid Routing
    if (defaultEe_ != defaultIsoDepRoute_) {
        if (defaultEe_ != NFC_DH_ID) {
            InfoLog("UpdateDefaultRoute: defaultEe_is not NFC_DH_ID Returning...");
            return;
        }
        uint8_t powerState = PWR_STA_SWITCH_ON;
        if (!isSecureNfcEnabled_) {
            powerState = (defaultEe_ != 0x00) ? offHostAidRoutingPowerState_ : DEFAULT_PWR_STA_HOST;
        }
        status = NFA_EeAddAidRouting(defaultEe_, 0, NULL, powerState, AID_ROUTE_QUAL_PREFIX);
        if (status == NFA_STATUS_OK) {
            InfoLog("UpdateDefaultRoute: Succeed to register zero length AID");
        } else {
            ErrorLog("UpdateDefaultRoute: failed to register zero length AID");
        }
    }
}

void RoutingManager::Deinitialize()
{
}

tNFA_TECHNOLOGY_MASK RoutingManager::UpdateEeTechRouteSetting()
{
    const tNFA_TECHNOLOGY_MASK noSeTechMask = 0x00;
    tNFA_TECHNOLOGY_MASK allSeTechMask = noSeTechMask;
    InfoLog("UpdateEeTechRouteSetting: eeInfo_.num_ee = 0x%{public}02x", (int)eeInfo_.num_ee);
    tNFA_STATUS status;
    for (uint8_t i = 0; i < eeInfo_.num_ee; i++) {
        tNFA_HANDLE eeHandle = eeInfo_.ee_disc_info[i].ee_handle;
        tNFA_TECHNOLOGY_MASK seTechMask = 0;
        InfoLog("UpdateEeTechRouteSetting: EE[%{public}u] Handle: 0x%{public}04x  techA: 0x%{public}02x  techB: "
            "0x%{public}02x  techF: 0x%{public}02x  techBprime: 0x%{public}02x",
            i, eeHandle, eeInfo_.ee_disc_info[i].la_protocol, eeInfo_.ee_disc_info[i].lb_protocol,
            eeInfo_.ee_disc_info[i].lf_protocol, eeInfo_.ee_disc_info[i].lbp_protocol);

        if ((defaultOffHostRoute_ != 0) && (eeHandle == (defaultOffHostRoute_ | NFA_HANDLE_GROUP_EE))) {
            if (eeInfo_.ee_disc_info[i].la_protocol != 0) {
                seTechMask |= NFA_TECHNOLOGY_MASK_A;
            }
            if (eeInfo_.ee_disc_info[i].lb_protocol != 0) {
                seTechMask |= NFA_TECHNOLOGY_MASK_B;
            }
        }
        if ((defaultFelicaRoute_ != 0) && (eeHandle == (defaultFelicaRoute_ | NFA_HANDLE_GROUP_EE))) {
            if (eeInfo_.ee_disc_info[i].lf_protocol != 0) {
                seTechMask |= NFA_TECHNOLOGY_MASK_F;
            }
        }

        InfoLog("UpdateEeTechRouteSetting: seTechMask[%{public}u]=0x%{public}02x", i, seTechMask);
        if (seTechMask != noSeTechMask) {
            InfoLog("UpdateEeTechRouteSetting: Configuring tech mask 0x%{public}02x on EE 0x%{public}04x",
                seTechMask, eeHandle);

            status = NFA_CeConfigureUiccListenTech(eeHandle, seTechMask);
            if (status != NFA_STATUS_OK) {
                ErrorLog("UpdateEeTechRouteSetting: NFA_CeConfigureUiccListenTech failed.");
            }

            // clear default tech routing before setting new power state
            status = NFA_EeClearDefaultTechRouting(eeHandle, seTechMask);
            if (status != NFA_STATUS_OK) {
                ErrorLog("UpdateEeTechRouteSetting: NFA_EeClearDefaultTechRouting failed.");
            }
              
            status = NFA_EeSetDefaultTechRouting(eeHandle, seTechMask, isSecureNfcEnabled_ ? 0 : seTechMask, 0,
                isSecureNfcEnabled_ ? 0 : seTechMask, isSecureNfcEnabled_ ? 0 : seTechMask,
                isSecureNfcEnabled_ ? 0 : seTechMask);
            if (status != NFA_STATUS_OK) {
                ErrorLog("UpdateEeTechRouteSetting: NFA_EeSetDefaultTechRouting failed.");
            }
            allSeTechMask |= seTechMask;
        }
    }
    return allSeTechMask;
}

bool RoutingManager::CommitRouting()
{
    tNFA_STATUS status = 0;
    if (isEeInfoChanged_) {
        seTechMask_ = UpdateEeTechRouteSetting();
        isEeInfoChanged_ = false;
    }
    {
        SynchronizeEvent guard(eeUpdateEvent_);
        status = NFA_EeUpdateNow();
        if (status == NFA_STATUS_OK) {
            eeUpdateEvent_.Wait();  // wait for NFA_EE_UPDATED_EVT
        }
    }
    return (status == NFA_STATUS_OK);
}

void RoutingManager::NfaEeCallback(tNFA_EE_EVT event, tNFA_EE_CBACK_DATA* eventData)
{
    if (!eventData) {
        ErrorLog("NfaEeCallback: eventData is null");
        return;
    }
    InfoLog("NfaEeCallback: event = %{public}d, status=0x%{public}X", event, eventData->status);
    switch (event) {
        case NFA_EE_REGISTER_EVT: {
            DoNfaEeRegisterEvent();
            break;
        }
        case NFA_EE_DEREGISTER_EVT: {
            DoNfaEeDeregisterEvent(eventData);
            break;
        }
        case NFA_EE_MODE_SET_EVT:
            break;
        case NFA_EE_SET_TECH_CFG_EVT: {
            NotifyRoutingEvent();
            break;
        }
        case NFA_EE_CLEAR_TECH_CFG_EVT: {
            NotifyRoutingEvent();
            break;
        }
        case NFA_EE_SET_PROTO_CFG_EVT: {
            NotifyRoutingEvent();
            break;
        }
        case NFA_EE_CLEAR_PROTO_CFG_EVT: {
            NotifyRoutingEvent();
            break;
        }
        case NFA_EE_ACTION_EVT:
            break;
        case NFA_EE_DISCOVER_REQ_EVT: {
            DoNfaEeDiscoverReqEvent(eventData);
            break;
        }
        case NFA_EE_NO_CB_ERR_EVT:
            break;
        case NFA_EE_ADD_AID_EVT: {
            DoNfaEeAddOrRemoveAidEvent(eventData);
            break;
        }
        case NFA_EE_ADD_SYSCODE_EVT: {
            NotifyRoutingEvent();
            break;
        }
        case NFA_EE_REMOVE_SYSCODE_EVT: {
            NotifyRoutingEvent();
            break;
        }
        case NFA_EE_REMOVE_AID_EVT: {
            DoNfaEeAddOrRemoveAidEvent(eventData);
            break;
        }
        case NFA_EE_NEW_EE_EVT: {
            InfoLog("NfaEeCallback: NFA_EE_NEW_EE_EVT h=0x%{public}X; status=%{public}u",
                eventData->new_ee.ee_handle, eventData->new_ee.ee_status);
            break;
        }
        case NFA_EE_UPDATED_EVT: {
            DoNfaEeUpdateEvent();
            break;
        }
        case NFA_EE_PWR_AND_LINK_CTRL_EVT: {
            break;
        }
        default:
            InfoLog("NfaEeCallback: unknown event=%{public}u ????", event);
    }
}

void RoutingManager::DoNfaEeRegisterEvent()
{
    RoutingManager& rm = RoutingManager::GetInstance();
    SynchronizeEvent guard(rm.eeRegisterEvent_);
    InfoLog("NfaEeCallback: NFA_EE_REGISTER_EVT");
    rm.eeRegisterEvent_.NotifyOne();
}

void RoutingManager::DoNfaEeDeregisterEvent(tNFA_EE_CBACK_DATA* eventData)
{
    RoutingManager& rm = RoutingManager::GetInstance();
    InfoLog("NfaEeCallback: NFA_EE_DEREGISTER_EVT status=0x%{public}X", eventData->status);
    rm.isEeInfoReceived_ = false;
    rm.isDeinitializing_ = false;
}

void RoutingManager::NotifyRoutingEvent()
{
    RoutingManager& rm = RoutingManager::GetInstance();
    SynchronizeEvent guard(rm.routingEvent_);
    rm.routingEvent_.NotifyOne();
}

void RoutingManager::DoNfaEeAddOrRemoveAidEvent(tNFA_EE_CBACK_DATA* eventData)
{
    InfoLog("NfaEeCallback: NFA_EE_ADD_AID_EVT  status=%{public}u", eventData->status);
    RoutingManager& rm = RoutingManager::GetInstance();
    SynchronizeEvent guard(rm.routingEvent_);
    rm.isAidRoutingConfigured_ = (eventData->status == NFA_STATUS_OK);
    rm.routingEvent_.NotifyOne();
}

void RoutingManager::DoNfaEeDiscoverReqEvent(tNFA_EE_CBACK_DATA* eventData)
{
    InfoLog("NfaEeCallback: NFA_EE_DISCOVER_REQ_EVT; status=0x%{public}X; num ee=%{public}u",
        eventData->discover_req.status, eventData->discover_req.num_ee);
    RoutingManager& rm = RoutingManager::GetInstance();
    SynchronizeEvent guard(rm.eeInfoEvent_);
    int status = memcpy_s(&rm.eeInfo_, sizeof(rm.eeInfo_), &eventData->discover_req, sizeof(rm.eeInfo_));
    if (status != 0) {
        return;
    }
    if (rm.isEeInfoReceived_ && !rm.isDeinitializing_) {
        rm.isEeInfoChanged_ = true;
    }
    rm.isEeInfoReceived_ = true;
    rm.eeInfoEvent_.NotifyOne();
}

void RoutingManager::DoNfaEeUpdateEvent()
{
    InfoLog("NfaEeCallback: NFA_EE_UPDATED_EVT");
    RoutingManager& rm = RoutingManager::GetInstance();
    SynchronizeEvent guard(rm.eeUpdateEvent_);
    rm.eeUpdateEvent_.NotifyOne();
}

RoutingManager::RoutingManager() : isSecureNfcEnabled_(false),
    isAidRoutingConfigured_(false) {
    // read default route params
    defaultOffHostRoute_ = NfcConfig::getUnsigned(NAME_DEFAULT_OFFHOST_ROUTE, DEFAULT_OFF_HOST_ROUTE_DEST);
    defaultFelicaRoute_ = NfcConfig::getUnsigned(NAME_DEFAULT_NFCF_ROUTE, DEFAULT_FELICA_ROUTE_DEST);
    defaultEe_ = NfcConfig::getUnsigned(NAME_DEFAULT_ROUTE, DEFAULT_EE_ROUTE_DEST);
    aidMatchingMode_ = NfcConfig::getUnsigned(NAME_AID_MATCHING_MODE, AID_MATCHING_EXACT_ONLY);
    if (NfcConfig::hasKey(NAME_OFFHOST_ROUTE_UICC)) {
        offHostRouteUicc_ = NfcConfig::getBytes(NAME_OFFHOST_ROUTE_UICC);
    } else {
        offHostRouteUicc_ = DEFAULT_UICC_ROUTE_DEST;
    }
    if (NfcConfig::hasKey(NAME_OFFHOST_ROUTE_ESE)) {
        offHostRouteEse_ = NfcConfig::getBytes(NAME_OFFHOST_ROUTE_ESE);
    } else {
        offHostRouteEse_ = {DEFAULT_EE_ROUTE_DEST};
    }
    InfoLog("RoutingManager: defaultEe_ is 0x%{public}02x, defaultFelicaRoute_ is 0x%{public}02x",
        defaultEe_, defaultFelicaRoute_);

    // read syscode params from config
    defaultSysCodeRoute_ = NfcConfig::getUnsigned(NAME_DEFAULT_SYS_CODE_ROUTE, DEFAULT_SYS_CODE_ROUTE_DEST);
    defaultSysCodePowerstate_ = NfcConfig::getUnsigned(NAME_DEFAULT_SYS_CODE_PWR_STATE, DEFAULT_SYS_CODE_PWR_STA);
    defaultSysCode_ = DEFAULT_SYS_CODE;

    isDeinitializing_ = false;
    isEeInfoChanged_ = false;
}

RoutingManager::~RoutingManager() {}
} // NCI
} // NFC
} // OHOS