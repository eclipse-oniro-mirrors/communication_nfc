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
#include "ndef_utils.h"
#include "nfa_api.h"
#include "nfa_ce_api.h"
#include "nfa_ee_api.h"
#include "nfa_hci_api.h"
#include "nfa_rw_api.h"
#include "nfc_hal_api.h"
#include "synchronize_event.h"
#include "nfcc_nci_adapter.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class RoutingManager final {
public:
    static RoutingManager& GetInstance();
    bool Initialize();
    void Deinitialize();
    bool CommitRouting();
    bool ComputeRoutingParams(int defaultPaymentType);
    bool AddAidRouting(const std::string& aidStr, int route, int aidInfo, int power);
    bool ClearAidTable();

private:
    RoutingManager();
    ~RoutingManager();
    uint32_t GetDefaultProtoRouteAndPower(int defaultPaymentType);

    // update route settings
    tNFA_TECHNOLOGY_MASK UpdateEeTechRouteSetting();
    void UpdateDefaultRoute();
    void UpdateDefaultProtoRoute();
    void SetOffHostNfceeTechMask();

    // routing entries
    bool ClearRoutingEntry(uint32_t type);
    bool SetRoutingEntry(uint32_t type, uint32_t value, uint32_t route, uint32_t power);
    void SetDefaultAidRoute(int defaultPaymentType);
    void RegisterProtoRoutingEntry(tNFA_HANDLE eeHandle, tNFA_PROTOCOL_MASK protoSwitchOn,
                                   tNFA_PROTOCOL_MASK protoSwitchOff, tNFA_PROTOCOL_MASK protoBatteryOn,
                                   tNFA_PROTOCOL_MASK protoScreenLock, tNFA_PROTOCOL_MASK protoScreenOff,
                                   tNFA_PROTOCOL_MASK protoSwitchOffLock);
    void RegisterTechRoutingEntry(tNFA_HANDLE eeHandle,
        tNFA_PROTOCOL_MASK protoSwitchOn, tNFA_PROTOCOL_MASK protoSwitchOff,
        tNFA_PROTOCOL_MASK protoBatteryOn, tNFA_PROTOCOL_MASK protoScreenLock,
        tNFA_PROTOCOL_MASK protoScreenOff, tNFA_PROTOCOL_MASK protoSwitchOffLock);
    bool IsTypeABSupportedInEe(tNFA_HANDLE eeHandle);
    uint8_t GetProtoMaskFromTechMask(uint32_t& value);
    tNFA_HANDLE GetEeHandle(uint32_t route);

    void DoNfaEeRegisterEvent();
    void DoNfaEeModeSetEvent(tNFA_EE_CBACK_DATA* eventData);
    void DoNfaEeDeregisterEvent(tNFA_EE_CBACK_DATA* eventData);
    void NotifyRoutingEvent();
    void DoNfaEeDiscoverReqEvent(tNFA_EE_CBACK_DATA* eventData);
    void DoNfaEeAddOrRemoveAidEvent(tNFA_EE_CBACK_DATA* eventData);
    void DoNfaEeUpdateEvent();
    void ClearAllEvents();
    void OnNfcDeinit();
    void DoNfaEeRegisterEvt(); // NFA_EE_REGISTER_EVT

    // static callback functions regiter to nci stack.
    static void NfaEeCallback(tNFA_EE_EVT event, tNFA_EE_CBACK_DATA* eventData);
    static void NfaCeStackCallback(uint8_t event, tNFA_CONN_EVT_DATA* eventData);

    void DoNfaCeDataEvt(const tNFA_CE_DATA& ce_data);

private:
    // default routes
    uint32_t defaultOffHostRoute_ = 0;
    uint32_t defaultFelicaRoute_ = 0;
    uint32_t defaultIsoDepRoute_ = 0;
    int defaultEe_ = 0;

    // system code params
    int defaultSysCode_ = 0;
    tNFA_EE_PWR_STATE defaultSysCodePowerstate_;
    tNFA_HANDLE defaultSysCodeRoute_;

    std::vector<uint8_t> offHostRouteUicc_ {};
    std::vector<uint8_t> offHostRouteEse_ {};
    std::vector<uint8_t> mRxDataBuffer;

    tNFA_TECHNOLOGY_MASK seTechMask_;
    tNFA_EE_DISCOVER_REQ eeInfo_;

    SynchronizeEvent eeUpdateEvent_;
    SynchronizeEvent eeRegisterEvent_;
    SynchronizeEvent eeInfoEvent_;
    SynchronizeEvent routingEvent_;
    SynchronizeEvent eeSetModeEvent_;

    bool isEeInfoChanged_ = false;
    bool isEeInfoReceived_ = false;
    bool isSecureNfcEnabled_ = false;
    bool isDeinitializing_ = false;
    bool isAidRoutingConfigured_ = false;
    uint8_t hostListenTechMask_ = 0;
    uint32_t offHostAidRoutingPowerState_ = 0;
};
}
}
}
#endif  // NCI_MANAGER_H