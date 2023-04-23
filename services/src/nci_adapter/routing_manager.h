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
#ifndef ROUTING_MANAGER_H
#define ROUTING_MANAGER_H

#include <memory>
#include <string>

#include "nfa_api.h"
#include "infc_nci.h"
#include "infcc_host.h"
#include "synchronize_event.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class RoutingManager final {
public:
    static RoutingManager& GetInstance();
    bool Initialize();
    void Deinitialize();
    bool CommitRouting();
    bool ComputeRoutingParams();

private:
    RoutingManager();
    ~RoutingManager();

    // update route settings
    tNFA_TECHNOLOGY_MASK UpdateEeTechRouteSetting();
    void UpdateDefaultRoute();
    void UpdateDefaultProtoRoute();
    void SetOffHostNfceeTechMask();

    // routing entries
    bool ClearRoutingEntry(int type);
    bool SetRoutingEntry(int type, int value, int route, int power);
    void RegisterProtoRoutingEntry(tNFA_HANDLE eeHandle,
        tNFA_PROTOCOL_MASK protoSwitchOn, tNFA_PROTOCOL_MASK protoSwitchOff,
        tNFA_PROTOCOL_MASK protoBatteryOn, tNFA_PROTOCOL_MASK protoScreenLock,
        tNFA_PROTOCOL_MASK protoScreenOff, tNFA_PROTOCOL_MASK protoSwitchOffLock);
    void RegisterTechRoutingEntry(tNFA_HANDLE eeHandle,
        tNFA_PROTOCOL_MASK protoSwitchOn, tNFA_PROTOCOL_MASK protoSwitchOff,
        tNFA_PROTOCOL_MASK protoBatteryOn, tNFA_PROTOCOL_MASK protoScreenLock,
        tNFA_PROTOCOL_MASK protoScreenOff, tNFA_PROTOCOL_MASK protoSwitchOffLock);
    bool IsTypeABSupportedInEe(tNFA_HANDLE eeHandle);
    uint8_t GetProtoMaskFromTechMask(int& value);

    // callbacks and event handlers
    static void NfaCeStackCallback(uint8_t event, tNFA_CONN_EVT_DATA* eventData);
    static void NfaEeCallback(tNFA_EE_EVT event, tNFA_EE_CBACK_DATA* eventData);
    static void DoNfaEeRegisterEvent();
    static void DoNfaEeModeSetEvent(tNFA_EE_CBACK_DATA* eventData);
    static void DoNfaEeDeregisterEvent(tNFA_EE_CBACK_DATA* eventData);
    static void NotifyRoutingEvent();
    static void DoNfaEeDiscoverReqEvent(tNFA_EE_CBACK_DATA* eventData);
    static void DoNfaEeAddOrRemoveAidEvent(tNFA_EE_CBACK_DATA* eventData);
    static void DoNfaEeUpdateEvent();
    void ClearAllEvents();
    void OnNfcDeinit();

private:
    // default routes
    int defaultOffHostRoute_;
    int defaultFelicaRoute_;
    int defaultIsoDepRoute_;
    int defaultEe_;

    // system code params
    int defaultSysCode_;
    tNFA_EE_PWR_STATE defaultSysCodePowerstate_;
    tNFA_HANDLE defaultSysCodeRoute_;

    std::vector<uint8_t> offHostRouteUicc_;
    std::vector<uint8_t> offHostRouteEse_;

    tNFA_TECHNOLOGY_MASK seTechMask_;
    tNFA_EE_DISCOVER_REQ eeInfo_;

    SynchronizeEvent eeUpdateEvent_;
    SynchronizeEvent eeRegisterEvent_;
    SynchronizeEvent eeInfoEvent_;
    SynchronizeEvent routingEvent_;
    SynchronizeEvent eeSetModeEvent_;

    bool isEeInfoChanged_;
    bool isEeInfoReceived_;
    bool isSecureNfcEnabled_;
    bool isDeinitializing_;
    bool isAidRoutingConfigured_;

    uint8_t hostListenTechMask_;

    int offHostAidRoutingPowerState_;
};
}
}
}
#endif  // NCI_MANAGER_H