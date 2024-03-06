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
#include <unistd.h>
#include <securec.h>
#include "loghelper.h"
#include "nfc_config.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace NCI {
// default initialize values
static const uint16_t DEFAULT_SYS_CODE = 0xFEFE;
static const uint8_t AID_ROUTE_QUAL_PREFIX = 0x10;
static const uint8_t DEFAULT_OFF_HOST_ROUTE_DEST = 0x01;
static const uint8_t DEFAULT_FELICA_ROUTE_DEST = 0x02;
static const uint8_t DEFAULT_HOST_ROUTE_DEST = 0x00;
static const uint8_t DEFAULT_EE_ROUTE_DEST = 0x01; // ese
static const uint8_t DEFAULT_UICC1_ROUTE_DEST = 0x02; // sim1
static const uint8_t DEFAULT_UICC2_ROUTE_DEST = 0x03; // sim2
static const std::vector<uint8_t> DEFAULT_UICC_ROUTE_DEST = {0x02, 0x03};
static const tNFA_EE_PWR_STATE DEFAULT_SYS_CODE_PWR_STA = 0x00;
static const tNFA_HANDLE DEFAULT_SYS_CODE_ROUTE_DEST = 0xC0;
static const uint8_t MAX_NUM_OF_EE = 5;
static const int EE_INFO_WAITE_INTERVAL = 100 * 1000; // ms for usleep
static const int AID_DEFAULT_ROUTING_WAIT_TIME_MS = 2000;

// power state masks
static const uint8_t PWR_STA_SWTCH_ON_SCRN_UNLCK = 0x01;
static const uint8_t PWR_STA_SWTCH_OFF = 0x02;
static const uint8_t PWR_STA_BATT_OFF = 0x04;
static const uint8_t PWR_STA_SWTCH_ON_SCRN_LOCK = 0x10;
static const uint8_t PWR_STA_SWTCH_ON_SCRN_OFF = 0x08;
static const uint8_t PWR_STA_SWTCH_ON_SCRN_OFF_LOCK = 0x20;
static const uint8_t DEFAULT_PWR_STA_HOST = PWR_STA_SWTCH_ON_SCRN_UNLCK | PWR_STA_SWTCH_ON_SCRN_LOCK;
static const uint8_t DEFAULT_PWR_STA_FOR_TECH_A_B = PWR_STA_SWTCH_ON_SCRN_UNLCK | PWR_STA_SWTCH_OFF |
    PWR_STA_SWTCH_ON_SCRN_OFF | PWR_STA_SWTCH_ON_SCRN_LOCK | PWR_STA_SWTCH_ON_SCRN_OFF_LOCK;

// routing entries
static const uint8_t NFA_SET_TECH_ROUTING = 0x01;
static const uint8_t NFA_SET_PROTO_ROUTING = 0x02;
static const uint32_t ROUTE_LOC_HOST_ID = 0x400;
static const uint32_t ROUTE_LOC_ESE_ID = 0x4C0;
static const uint32_t ROUTE_UICC1_ID = 0x480;
static const uint32_t ROUTE_UICC2_ID = 0x481;
static const uint32_t DEFAULT_PROTO_ROUTE_AND_POWER_ESE = 0x013B;
static const uint32_t DEFAULT_PROTO_ROUTE_AND_POWER_SIM1 = 0x023B;
static const uint8_t ROUTE_LOC_MASK = 8;
static const uint8_t PWR_STA_MASK = 0x3F;
static const uint8_t DEFAULT_LISTEN_TECH_MASK = 0x07;

RoutingManager& RoutingManager::GetInstance()
{
    static RoutingManager manager;
    return manager;
}

bool RoutingManager::Initialize()
{
    mRxDataBuffer.clear();
    tNFA_STATUS status;
    {
        SynchronizeGuard guard(eeRegisterEvent_);
        InfoLog("Initialize: try ee register");
        status = NFA_EeRegister(NfaEeCallback);
        if (status != NFA_STATUS_OK) {
            ErrorLog("Initialize: fail ee register; error=0x%{public}X", status);
            return false;
        }
        eeRegisterEvent_.Wait(); // wait for NFA_EE_REGISTER_EVT
    }

    // NFA_EE_REGISTER_EVT and NFA_EE_DISCOVER_REQ_EVT may come at the same time
    // wait 100ms here to avoid timing issue in executing eeInfoEvent_
    usleep(EE_INFO_WAITE_INTERVAL);
    if ((defaultOffHostRoute_ != 0) || (defaultFelicaRoute_ != 0)) {
        SynchronizeGuard guard(eeInfoEvent_);
        if (!isEeInfoReceived_) {
            InfoLog("Initialize: Waiting for EE info");
            eeInfoEvent_.Wait(); // wait for NFA_EE_DISCOVER_REQ_EVT if eeinfo not received
        }
    }
    seTechMask_ = UpdateEeTechRouteSetting();

    // Set the host-routing Tech
    status = NFA_CeSetIsoDepListenTech(
        hostListenTechMask_ & (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B));
    if (status != NFA_STATUS_OK) {
        ErrorLog("Initialize: Failed to configure CE IsoDep technologies");
    }

    // Regrister AID routed to the host with an AID length of 0
    status = NFA_CeRegisterAidOnDH(NULL, 0, NfaCeStackCallback);
    if (status != NFA_STATUS_OK) {
        ErrorLog("Initialize: failed to register null AID to DH");
    }

    UpdateDefaultRoute();
    UpdateDefaultProtoRoute();
    SetOffHostNfceeTechMask();
    return true;
}

void RoutingManager::UpdateDefaultProtoRoute()
{
    // update default proto route for iso-dep
    tNFA_PROTOCOL_MASK protoMask = NFA_PROTOCOL_MASK_ISO_DEP;
    tNFA_STATUS status = NFA_STATUS_FAILED;
    if (defaultIsoDepRoute_ != NFC_DH_ID &&
        IsTypeABSupportedInEe(defaultIsoDepRoute_ | NFA_HANDLE_GROUP_EE)) {
        status = NFA_EeClearDefaultProtoRouting(defaultIsoDepRoute_, protoMask);
        status = NFA_EeSetDefaultProtoRouting(
            defaultIsoDepRoute_, protoMask, isSecureNfcEnabled_ ? 0 : protoMask, 0,
            isSecureNfcEnabled_ ? 0 : protoMask, isSecureNfcEnabled_ ? 0 : protoMask,
            isSecureNfcEnabled_ ? 0 : protoMask);
    } else {
        status = NFA_EeClearDefaultProtoRouting(NFC_DH_ID, protoMask);
        status = NFA_EeSetDefaultProtoRouting(
            NFC_DH_ID, protoMask, 0, 0, isSecureNfcEnabled_ ? 0 : protoMask, 0, 0);
    }
    if (status != NFA_STATUS_OK) {
        ErrorLog("UpdateDefaultProtoRoute: failed to register default ISO-DEP route");
    }
}

void RoutingManager::SetOffHostNfceeTechMask()
{
    tNFA_STATUS status = NFA_STATUS_FAILED;
    tNFA_HANDLE handle = ROUTE_LOC_ESE_ID;
    int uiccListenTechMask = 0x07;
    {
        status = NFA_CeConfigureUiccListenTech(handle, uiccListenTechMask);
        if (status != NFA_STATUS_OK) {
            ErrorLog("SetOffHostNfceeTechMask: failed to start uicc listen");
        }
    }
}

bool RoutingManager::ComputeRoutingParams(int defaultPaymentType)
{
    InfoLog("ComputeRoutingParams");
    uint8_t valueProtoIsoDep = 0x01;

    // route for protocol
    ClearRoutingEntry(NFA_SET_PROTO_ROUTING);
    uint32_t defaultRouteAndPower = GetDefaultProtoRouteAndPower(defaultPaymentType);
    SetRoutingEntry(NFA_SET_PROTO_ROUTING, valueProtoIsoDep,
                    ((defaultRouteAndPower >> ROUTE_LOC_MASK) & DEFAULT_LISTEN_TECH_MASK),
                    defaultRouteAndPower & PWR_STA_MASK);

    // route for technology
    // currently set tech F default to ese with power 0x3B
    uint8_t techSeId = DEFAULT_EE_ROUTE_DEST;
    uint8_t techFSeId = DEFAULT_EE_ROUTE_DEST;
    uint8_t techRouteForTypeAB = 0x03;
    uint8_t techRouteForTypeF = 0x04;
    ClearRoutingEntry(NFA_SET_TECH_ROUTING);
    SetRoutingEntry(NFA_SET_TECH_ROUTING, techRouteForTypeAB, techSeId, DEFAULT_PWR_STA_FOR_TECH_A_B);
    SetRoutingEntry(NFA_SET_TECH_ROUTING, techRouteForTypeF, techFSeId, DEFAULT_PWR_STA_FOR_TECH_A_B);

    SetDefaultAidRoute(defaultPaymentType);
    return true;
}

tNFA_HANDLE RoutingManager::GetEeHandle(uint32_t route)
{
    switch (route) {
        case DEFAULT_HOST_ROUTE_DEST:
            return ROUTE_LOC_HOST_ID;
        case DEFAULT_EE_ROUTE_DEST:
            return ROUTE_LOC_ESE_ID;
        case DEFAULT_UICC1_ROUTE_DEST:
            return ROUTE_UICC1_ID;
        case DEFAULT_UICC2_ROUTE_DEST:
            return ROUTE_UICC2_ID;
        default:
            return ROUTE_LOC_HOST_ID;
    }
}

uint32_t RoutingManager::GetDefaultProtoRouteAndPower(int defaultPaymentType)
{
    if (defaultPaymentType ==static_cast<int>(KITS::DefaultPaymentType::TYPE_ESE)) {
        return DEFAULT_PROTO_ROUTE_AND_POWER_ESE;
    }

    return DEFAULT_PROTO_ROUTE_AND_POWER_SIM1;
}

bool RoutingManager::AddAidRouting(const std::string &aidStr, int route,
                                   int aidInfo, int power)
{
    std::vector<unsigned char> aidBytes;
    KITS::NfcSdkCommon::HexStringToBytes(aidStr, aidBytes);
    size_t aidLen = aidBytes.size();
    tNFA_STATUS status = NFA_EeAddAidRouting(route, aidLen, static_cast<uint8_t*>(aidBytes.data()), power, aidInfo);
    if (status == NFA_STATUS_OK) {
        InfoLog("AddAidRouting: Succeed ");
        return true;
    } else {
        ErrorLog("AddAidRouting: failed ");
        return false;
    }
    return false;
}

bool RoutingManager::ClearAidTable()
{
    tNFA_STATUS status = NFA_EeRemoveAidRouting(NFA_REMOVE_ALL_AID_LEN,
        reinterpret_cast<uint8_t *>(NFA_REMOVE_ALL_AID));
    if (status == NFA_STATUS_OK) {
        InfoLog("ClearAidTable: Succeed ");
        return true;
    } else {
        ErrorLog("ClearAidTable: failed ");
        return false;
    }
}

bool RoutingManager::SetRoutingEntry(uint32_t type, uint32_t value, uint32_t route, uint32_t power)
{
    InfoLog("SetRoutingEntry: type:0x%{public}X, value:0x%{public}X, route:0x%{public}X, power:0x%{public}X",
        type, value, route, power);
    uint8_t maxTechMask = 0x03; // 0x01 for type A, 0x02 for type B, 0x03 for both
    uint8_t last4BitsMask = 0xF0;
    tNFA_STATUS status = NFA_STATUS_FAILED;
    tNFA_HANDLE handle = GetEeHandle(route);
    uint8_t swtchOnMask = 0;
    uint8_t swtchOffMask = 0;
    uint8_t battOffMask = 0;
    uint8_t scrnLockMask = 0;
    uint8_t scrnOffMask = 0;
    uint8_t scrnOffLockMask = 0;
    uint8_t protoMask = 0;

    // validate power state value
    power &= PWR_STA_MASK;
    if ((handle == ROUTE_LOC_HOST_ID) && (type == NFA_SET_PROTO_ROUTING)) {
        power &= ~(PWR_STA_SWTCH_OFF | PWR_STA_BATT_OFF);
    }
    if (type == NFA_SET_TECH_ROUTING) {
        InfoLog("SetRoutingEntry: NFA_SET_TECH_ROUTING maxTechMask ");
        value &= maxTechMask;
        swtchOnMask = (power & PWR_STA_SWTCH_ON_SCRN_UNLCK) ? value : 0;
        swtchOffMask = (power & PWR_STA_SWTCH_OFF) ? value : 0;
        battOffMask = (power & PWR_STA_BATT_OFF) ? value : 0;
        scrnLockMask = (power & PWR_STA_SWTCH_ON_SCRN_LOCK) ? value : 0;
        scrnOffMask = (power & PWR_STA_SWTCH_ON_SCRN_OFF) ? value : 0;
        scrnOffLockMask = (power & PWR_STA_SWTCH_ON_SCRN_OFF_LOCK) ? value : 0;
        if (hostListenTechMask_) {
            RegisterTechRoutingEntry(handle, swtchOnMask, swtchOffMask, battOffMask, scrnLockMask, scrnOffMask,
                scrnOffLockMask);
        }
    } else if (type == NFA_SET_PROTO_ROUTING) {
        value &= ~last4BitsMask;
        while (value) {
            protoMask = GetProtoMaskFromTechMask(value);
            if ((protoMask & (NFA_PROTOCOL_MASK_ISO_DEP | NFC_PROTOCOL_MASK_ISO7816)) &&
                (handle != NFA_EE_HANDLE_DH) &&
                (maxTechMask & (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B)) == 0) {
                InfoLog("SetRoutingEntry: proto entry rejected, handle 0x%{public}x does not support"
                    "proto mask 0x%{public}x", handle, protoMask);
                return status;
            }
            swtchOnMask = (power & PWR_STA_SWTCH_ON_SCRN_UNLCK) ? protoMask : 0;
            swtchOffMask = (power & PWR_STA_SWTCH_OFF) ? protoMask : 0;
            battOffMask = (power & PWR_STA_BATT_OFF) ? protoMask : 0;
            scrnLockMask = (power & PWR_STA_SWTCH_ON_SCRN_LOCK) ? protoMask : 0;
            scrnOffMask = (power & PWR_STA_SWTCH_ON_SCRN_OFF) ? protoMask : 0;
            scrnOffLockMask = (power & PWR_STA_SWTCH_ON_SCRN_OFF_LOCK) ? protoMask : 0;
            RegisterProtoRoutingEntry(handle, swtchOnMask, swtchOffMask, battOffMask, scrnLockMask, scrnOffMask,
                scrnOffLockMask);
            protoMask = 0;
        }
    }
    return status;
}

void RoutingManager::SetDefaultAidRoute(int defaultPaymentType)
{
    tNFA_STATUS status = NFA_STATUS_FAILED;
    SynchronizeGuard guard(routingEvent_);
    uint32_t defaultRouteAndPower = GetDefaultProtoRouteAndPower(defaultPaymentType);
    uint32_t routeLoc = (defaultRouteAndPower >> ROUTE_LOC_MASK) & DEFAULT_LISTEN_TECH_MASK;
    uint32_t power = defaultRouteAndPower & PWR_STA_MASK;
    tNFA_HANDLE handle = GetEeHandle(routeLoc);
    if (handle == ROUTE_LOC_HOST_ID) {
        power = PWR_STA_SWTCH_ON_SCRN_UNLCK;
    }

    if (isSecureNfcEnabled_) {
        power = PWR_STA_SWTCH_ON_SCRN_UNLCK;
    }
    status = NFA_EeAddAidRouting(handle, 0, NULL, power, AID_ROUTE_QUAL_PREFIX);
    if (status == NFA_STATUS_OK) {
        if (routingEvent_.Wait(AID_DEFAULT_ROUTING_WAIT_TIME_MS) == false) {
            ErrorLog("SetDefaultAidRoute:  register zero length AID time out ");
        } else {
            InfoLog("SetDefaultAidRoute: Succeed to register zero length AID");
        }
    } else {
        ErrorLog("SetDefaultAidRoute: failed to register zero length AID");
    }
}

uint8_t RoutingManager::GetProtoMaskFromTechMask(uint32_t& value)
{
    if (value & NFA_TECHNOLOGY_MASK_A) {
        value &= ~NFA_TECHNOLOGY_MASK_A;
        return NFA_PROTOCOL_MASK_ISO_DEP;
    } else if (value & NFA_TECHNOLOGY_MASK_B) {
        value &= ~NFA_TECHNOLOGY_MASK_B;
        return NFA_PROTOCOL_MASK_NFC_DEP;
    } else if (value & NFA_TECHNOLOGY_MASK_F) {
        value &= ~NFA_TECHNOLOGY_MASK_F;
        return NFA_PROTOCOL_MASK_T3T;
    } else if (value & NFA_TECHNOLOGY_MASK_V) {
        value &= ~NFA_TECHNOLOGY_MASK_V;
        return NFC_PROTOCOL_MASK_ISO7816;
    }
    return 0;
}

void RoutingManager::RegisterProtoRoutingEntry(tNFA_HANDLE eeHandle,
    tNFA_PROTOCOL_MASK protoSwitchOn, tNFA_PROTOCOL_MASK protoSwitchOff,
    tNFA_PROTOCOL_MASK protoBatteryOn, tNFA_PROTOCOL_MASK protoScreenLock,
    tNFA_PROTOCOL_MASK protoScreenOff, tNFA_PROTOCOL_MASK protoSwitchOffLock)
{
    tNFA_STATUS status = NFA_STATUS_FAILED;
    {
        SynchronizeGuard guard(routingEvent_);
        status = NFA_EeSetDefaultProtoRouting(eeHandle, protoSwitchOn,
            isSecureNfcEnabled_ ? 0 : protoSwitchOff,
            isSecureNfcEnabled_ ? 0 : protoBatteryOn,
            isSecureNfcEnabled_ ? 0 : protoScreenLock,
            isSecureNfcEnabled_ ? 0 : protoSwitchOff,
            isSecureNfcEnabled_ ? 0 : protoSwitchOffLock);
        if (status == NFA_STATUS_OK) {
            routingEvent_.Wait();
            InfoLog("RegisterProtoRoutingEntry: Register Proto Routing Entry SUCCESS");
        } else {
            ErrorLog("RegisterProtoRoutingEntry: Register Proto Routing Entry Failed");
        }
    }
}

void RoutingManager::RegisterTechRoutingEntry(tNFA_HANDLE eeHandle,
    tNFA_PROTOCOL_MASK protoSwitchOn, tNFA_PROTOCOL_MASK protoSwitchOff,
    tNFA_PROTOCOL_MASK protoBatteryOn, tNFA_PROTOCOL_MASK protoScreenLock,
    tNFA_PROTOCOL_MASK protoScreenOff, tNFA_PROTOCOL_MASK protoSwitchOffLock)
{
    tNFA_STATUS status = NFA_STATUS_FAILED;
    {
        SynchronizeGuard guard(routingEvent_);
        status = NFA_EeSetDefaultTechRouting(eeHandle, protoSwitchOn,
            isSecureNfcEnabled_ ? 0 : protoSwitchOff,
            isSecureNfcEnabled_ ? 0 : protoBatteryOn,
            isSecureNfcEnabled_ ? 0 : protoScreenLock,
            isSecureNfcEnabled_ ? 0 : protoSwitchOff,
            isSecureNfcEnabled_ ? 0 : protoSwitchOffLock);
        if (status == NFA_STATUS_OK) {
            routingEvent_.Wait();
            InfoLog("RegisterTechRoutingEntry: Register Tech Routing Entry SUCCESS");
        } else {
            ErrorLog("RegisterTechRoutingEntry: Register Tech Routing Entry Failed");
        }
    }
}

bool RoutingManager::ClearRoutingEntry(uint32_t type)
{
    InfoLog("ClearRoutingEntry: type = %{public}d", type);
    tNFA_STATUS status = NFA_STATUS_FAILED;
    SynchronizeGuard guard(routingEvent_);
    if (type & NFA_SET_TECH_ROUTING) {
        status = NFA_EeClearDefaultTechRouting(NFA_EE_HANDLE_DH,
            (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B | NFA_TECHNOLOGY_MASK_F));
        if (status == NFA_STATUS_OK) {
            routingEvent_.Wait();
        }
    }
    if (type & NFA_SET_PROTO_ROUTING) {
        {
            status = NFA_EeClearDefaultProtoRouting(ROUTE_LOC_ESE_ID,
                (NFA_PROTOCOL_MASK_ISO_DEP | NFC_PROTOCOL_MASK_ISO7816));
            if (status == NFA_STATUS_OK) {
                routingEvent_.Wait();
            }
        }
        {
            status = NFA_EeClearDefaultProtoRouting(NFA_EE_HANDLE_DH,
                (NFA_PROTOCOL_MASK_ISO_DEP | NFC_PROTOCOL_MASK_ISO7816));
            if (status == NFA_STATUS_OK) {
                routingEvent_.Wait();
            }
        }
    }
    return (status == NFA_STATUS_OK);
}

bool RoutingManager::IsTypeABSupportedInEe(tNFA_HANDLE eeHandle)
{
    bool rst = false;
    uint8_t numEe = MAX_NUM_OF_EE;
    tNFA_EE_INFO eeInfo[numEe];
    if (!memset_s(&eeInfo, numEe * sizeof(tNFA_EE_INFO), 0, numEe * sizeof(tNFA_EE_INFO))) {
        ErrorLog("IsTypeABSupportedInEe, memset_s error");
        return rst;
    }
    tNFA_STATUS status = NFA_EeGetInfo(&numEe, eeInfo);
    InfoLog("IsTypeABSupportedInEe, NFA_EeGetInfo status = %{public}d", status);
    if (status != NFA_STATUS_OK) {
        return rst;
    }
    for (auto i = 0; i < numEe; i++) {
        if (eeHandle == eeInfo[i].ee_handle) {
            if (eeInfo[i].la_protocol || eeInfo[i].lb_protocol) {
                rst = true;
                break;
            }
        }
    }
    return rst;
}

void RoutingManager::UpdateDefaultRoute()
{
    if (NFA_GetNCIVersion() != NCI_VERSION_2_0) {
        return;
    }
    tNFA_STATUS status;

    // Register System Code for routing
    SynchronizeGuard guard(routingEvent_);
    status = NFA_EeAddSystemCodeRouting(
        defaultSysCode_, defaultSysCodeRoute_,
        isSecureNfcEnabled_ ? PWR_STA_SWTCH_ON_SCRN_UNLCK : defaultSysCodePowerstate_);
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
        uint8_t powerState = PWR_STA_SWTCH_ON_SCRN_UNLCK;
        if (!isSecureNfcEnabled_) {
            powerState = (defaultEe_ != 0x00) ? offHostAidRoutingPowerState_ : DEFAULT_PWR_STA_HOST;
        }
        status = NFA_EeAddAidRouting(
            defaultEe_, 0, NULL, powerState, AID_ROUTE_QUAL_PREFIX);
        if (status == NFA_STATUS_OK) {
            InfoLog("UpdateDefaultRoute: Succeed to register zero length AID");
        } else {
            ErrorLog("UpdateDefaultRoute: failed to register zero length AID");
        }
    }
}

void RoutingManager::OnNfcDeinit()
{
    if (defaultOffHostRoute_ == DEFAULT_HOST_ROUTE_DEST &&
        defaultFelicaRoute_ == DEFAULT_HOST_ROUTE_DEST) {
        return;
    }
    tNFA_STATUS status = NFA_STATUS_FAILED;
    isDeinitializing_ = true;
    uint8_t numEe = MAX_NUM_OF_EE;
    tNFA_EE_INFO eeInfo[numEe];
    if (memset_s(&eeInfo, numEe * sizeof(tNFA_EE_INFO), 0, numEe * sizeof(tNFA_EE_INFO))) {
        ErrorLog("OnNfcDeinit, memset_s error");
        return;
    }
    status = NFA_EeGetInfo(&numEe, eeInfo);
    if (status != NFA_STATUS_OK) {
        ErrorLog("OnNfcDeinit: fail get info; error=0x%{public}X", status);
        return;
    }
    if (numEe != 0) {
        for (uint8_t i = 0; i < numEe; i++) {
            // only do set ee mode to deactive when ee is active
            // on NCI VER ower than 2.0, the active state is NCI_NFCEE_INTERFACE_HCI_ACCESS
            bool isOffHostEEPresent = (NFA_GetNCIVersion() < NCI_VERSION_2_0)
                ? (eeInfo[i].num_interface != 0) : (eeInfo[i].ee_interface[0] !=
                NCI_NFCEE_INTERFACE_HCI_ACCESS) && (eeInfo[i].ee_status == NFA_EE_STATUS_ACTIVE);
            if (isOffHostEEPresent)  {
                InfoLog("OnNfcDeinit: Handle: 0x%{public}04x Change Status Active to Inactive",
                    eeInfo[i].ee_handle);
                SynchronizeGuard guard(eeSetModeEvent_);
                status = NFA_EeModeSet(eeInfo[i].ee_handle, NFA_EE_MD_DEACTIVATE);
                if (status == NFA_STATUS_OK) {
                    eeSetModeEvent_.Wait();
                } else {
                    ErrorLog("OnNfcDeinit: Failed to set EE inactive");
                }
            }
        }
    } else {
        InfoLog("OnNfcDeinit: No active EEs found");
    }
}

void RoutingManager::ClearAllEvents()
{
    InfoLog("ClearAllEvents");
    {
        SynchronizeGuard guard(eeUpdateEvent_);
        eeUpdateEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(eeRegisterEvent_);
        eeRegisterEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(eeInfoEvent_);
        eeInfoEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(routingEvent_);
        routingEvent_.NotifyOne();
    }
    {
        SynchronizeGuard guard(eeSetModeEvent_);
        eeSetModeEvent_.NotifyOne();
    }
}

void RoutingManager::Deinitialize()
{
    InfoLog("Deinitialize");
    ClearAllEvents();
    OnNfcDeinit();
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

            status = NFA_EeSetDefaultTechRouting(
                eeHandle, seTechMask, isSecureNfcEnabled_ ? 0 : seTechMask, 0,
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
        SynchronizeGuard guard(eeUpdateEvent_);
        status = NFA_EeUpdateNow();
        if (status == NFA_STATUS_OK) {
            eeUpdateEvent_.Wait();  // wait for NFA_EE_UPDATED_EVT
        }
    }
    return (status == NFA_STATUS_OK);
}

void RoutingManager::DoNfaEeRegisterEvt()
{
    InfoLog("DoNfaEeRegisterEvt: NFA_EE_REGISTER_EVT notified");
    SynchronizeGuard guard(routingEvent_);
    routingEvent_.NotifyOne();
}

void RoutingManager::NfaCeStackCallback(uint8_t event,
                                        tNFA_CONN_EVT_DATA* eventData)
{
    if (!eventData) {
        ErrorLog("NfaCeStackCallback: eventData is null");
        return;
    }
    InfoLog("NfaCeStackCallback: event = %{public}d", event);
    switch (event) {
        case NFA_EE_REGISTER_EVT: {
            RoutingManager::GetInstance().DoNfaEeRegisterEvt();
            break;
        }
        case NFA_CE_DATA_EVT: {
            tNFA_CE_DATA& ce_data = eventData->ce_data;
            InfoLog("NFA_CE_DATA_EVT: stat=0x%{public}X;h=0x%{public}X;data "
                    "len=%{public}u",
                    ce_data.status, ce_data.handle, ce_data.len);
            RoutingManager::GetInstance().DoNfaCeDataEvt(ce_data);
            break;
        }
        case NFA_CE_ACTIVATED_EVT: {
            InfoLog("tNFA_CE_ACTIVATED come");
            NfccNciAdapter::GetInstance().OnCardEmulationActivated();
            break;
        }
        case NFA_DEACTIVATED_EVT:
        case NFA_CE_DEACTIVATED_EVT: {
            InfoLog("tNFA_CE_ACTIVATED come");
            NfccNciAdapter::GetInstance().OnCardEmulationDeactivated();
            break;
        }
        default: break;
    }
}

void RoutingManager::DoNfaCeDataEvt(const tNFA_CE_DATA& ce_data)
{
    tNFA_STATUS status = ce_data.status;
    uint32_t dataLen = ce_data.len;
    const uint8_t* data = ce_data.p_data;
    if (status == NFC_STATUS_CONTINUE) {
        if (dataLen > 0) {
            mRxDataBuffer.insert(mRxDataBuffer.end(), &data[0], &data[dataLen]);
        }
        return;
    }
    if (status == NFA_STATUS_OK) {
        if (dataLen > 0) {
            mRxDataBuffer.insert(mRxDataBuffer.end(), &data[0], &data[dataLen]);
        }
    }
    if (status == NFA_STATUS_FAILED) {
        InfoLog("NFA_CE_DATA_EVT: stat=0x%{public}X;h=0x%{public}X;data "
                "len=%{public}u",
                ce_data.status, ce_data.handle, ce_data.len);
        mRxDataBuffer.clear();
    }

    std::vector<uint8_t> hostCardData = mRxDataBuffer;
    NfccNciAdapter::GetInstance().OnCardEmulationData(hostCardData);
    mRxDataBuffer.clear();
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
            RoutingManager::GetInstance().DoNfaEeRegisterEvent();
            break;
        }
        case NFA_EE_DEREGISTER_EVT: {
            RoutingManager::GetInstance().DoNfaEeDeregisterEvent(eventData);
            break;
        }
        case NFA_EE_MODE_SET_EVT: {
            RoutingManager::GetInstance().DoNfaEeModeSetEvent(eventData);
            break;
        }
        case NFA_EE_SET_TECH_CFG_EVT: {
            RoutingManager::GetInstance().NotifyRoutingEvent();
            break;
        }
        case NFA_EE_CLEAR_TECH_CFG_EVT: {
            RoutingManager::GetInstance().NotifyRoutingEvent();
            break;
        }
        case NFA_EE_SET_PROTO_CFG_EVT: {
            RoutingManager::GetInstance().NotifyRoutingEvent();
            break;
        }
        case NFA_EE_CLEAR_PROTO_CFG_EVT: {
            RoutingManager::GetInstance().NotifyRoutingEvent();
            break;
        }
        case NFA_EE_ACTION_EVT:
            break;
        case NFA_EE_DISCOVER_REQ_EVT: {
            RoutingManager::GetInstance().DoNfaEeDiscoverReqEvent(eventData);
            break;
        }
        case NFA_EE_NO_CB_ERR_EVT:
            break;
        case NFA_EE_ADD_AID_EVT: {
            RoutingManager::GetInstance().DoNfaEeAddOrRemoveAidEvent(eventData);
            break;
        }
        case NFA_EE_ADD_SYSCODE_EVT: {
            RoutingManager::GetInstance().NotifyRoutingEvent();
            break;
        }
        case NFA_EE_REMOVE_SYSCODE_EVT: {
            RoutingManager::GetInstance().NotifyRoutingEvent();
            break;
        }
        case NFA_EE_REMOVE_AID_EVT: {
            RoutingManager::GetInstance().DoNfaEeAddOrRemoveAidEvent(eventData);
            break;
        }
        case NFA_EE_NEW_EE_EVT: {
            InfoLog("NfaEeCallback: NFA_EE_NEW_EE_EVT h=0x%{public}X; status=%{public}u",
                eventData->new_ee.ee_handle, eventData->new_ee.ee_status);
            break;
        }
        case NFA_EE_UPDATED_EVT: {
            RoutingManager::GetInstance().DoNfaEeUpdateEvent();
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
    SynchronizeGuard guard(rm.eeRegisterEvent_);
    InfoLog("NfaEeCallback: NFA_EE_REGISTER_EVT");
    rm.eeRegisterEvent_.NotifyOne();
}

void RoutingManager::DoNfaEeModeSetEvent(tNFA_EE_CBACK_DATA* eventData)
{
    RoutingManager& rm = RoutingManager::GetInstance();
    SynchronizeGuard guard(rm.eeSetModeEvent_);
    InfoLog("NfaEeCallback: NFA_EE_MODE_SET_EVT, status = 0x%{public}04X, handle = 0x%{public}04X",
        eventData->mode_set.status, eventData->mode_set.ee_handle);
    rm.eeSetModeEvent_.NotifyOne();
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
    SynchronizeGuard guard(rm.routingEvent_);
    rm.routingEvent_.NotifyOne();
}

void RoutingManager::DoNfaEeAddOrRemoveAidEvent(tNFA_EE_CBACK_DATA* eventData)
{
    InfoLog("NfaEeCallback: NFA_EE_ADD_AID_EVT  status=%{public}u", eventData->status);
    RoutingManager& rm = RoutingManager::GetInstance();
    SynchronizeGuard guard(rm.routingEvent_);
    rm.isAidRoutingConfigured_ = (eventData->status == NFA_STATUS_OK);
    rm.routingEvent_.NotifyOne();
}

void RoutingManager::DoNfaEeDiscoverReqEvent(tNFA_EE_CBACK_DATA* eventData)
{
    InfoLog("NfaEeCallback: NFA_EE_DISCOVER_REQ_EVT; status=0x%{public}X; num ee=%{public}u",
        eventData->discover_req.status, eventData->discover_req.num_ee);
    RoutingManager& rm = RoutingManager::GetInstance();
    SynchronizeGuard guard(rm.eeInfoEvent_);
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
    SynchronizeGuard guard(rm.eeUpdateEvent_);
    rm.eeUpdateEvent_.NotifyOne();
}

RoutingManager::RoutingManager() : isSecureNfcEnabled_(false),
    isAidRoutingConfigured_(false) {
    mRxDataBuffer.clear();
    // read default route params
    defaultOffHostRoute_ = NfcConfig::getUnsigned(
        NAME_DEFAULT_OFFHOST_ROUTE, DEFAULT_OFF_HOST_ROUTE_DEST);
    defaultFelicaRoute_ = NfcConfig::getUnsigned(
        NAME_DEFAULT_NFCF_ROUTE, DEFAULT_FELICA_ROUTE_DEST);
    defaultEe_ = NfcConfig::getUnsigned(
        NAME_DEFAULT_ROUTE, DEFAULT_EE_ROUTE_DEST);
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
    defaultSysCodeRoute_ = NfcConfig::getUnsigned(
        NAME_DEFAULT_SYS_CODE_ROUTE, DEFAULT_SYS_CODE_ROUTE_DEST);
    defaultSysCodePowerstate_ = NfcConfig::getUnsigned(
        NAME_DEFAULT_SYS_CODE_PWR_STATE, DEFAULT_SYS_CODE_PWR_STA);
    defaultSysCode_ = DEFAULT_SYS_CODE;
    seTechMask_ = 0x00;
    isDeinitializing_ = false;
    isEeInfoChanged_ = false;

    hostListenTechMask_ = NfcConfig::getUnsigned(
        NAME_HOST_LISTEN_TECH_MASK, NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B);
}

RoutingManager::~RoutingManager() {}
} // NCI
} // NFC
} // OHOS