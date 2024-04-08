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
#ifndef I_NFC_SERVICE_H
#define I_NFC_SERVICE_H
#include "element_name.h"
#include "iforeground_callback.h"

namespace OHOS {
namespace NFC {
namespace TAG {
    class TagDispatcher;
#ifdef NDEF_WIFI_ENABLED
    class WifiConnectionManager;
#endif
#ifdef NDEF_BT_ENABLED
    class BtConnectionManager;
#endif
}
class NfcEventHandler;
class NfcPollingManager;
class NfcRoutingManager;
class CeService;
enum class NfcCommonEvent {
    // tags found
    MSG_TAG_FOUND = 0,
    MSG_TAG_DEBOUNCE,
    MSG_TAG_LOST,

    // screen changed
    MSG_SCREEN_CHANGED,

    // package updated
    MSG_PACKAGE_UPDATED,

    // card emulation
    MSG_ROUTE_AID,
    MSG_COMMIT_ROUTING,
    MSG_COMPUTE_ROUTING_PARAMS,

    // field activated
    MSG_FIELD_ACTIVATED,

    // field deactivated
    MSG_FIELD_DEACTIVATED,

    // notify field on
    MSG_NOTIFY_FIELD_ON,

    // notify field off
    MSG_NOTIFY_FIELD_OFF,
    MSG_NOTIFY_FIELD_OFF_TIMEOUT,

    // device shutdown
    MSG_SHUTDOWN,
#ifdef VENDOR_APPLICATIONS_ENABLED
    // vendor event
    MSG_VENDOR_EVENT,
#endif
#ifdef NDEF_WIFI_ENABLED
    // for connect wifi by ndef wifi tag
    MSG_WIFI_ENABLE_TIMEOUT,
    MSG_WIFI_CONNECT_TIMEOUT,
    MSG_WIFI_ENABLED,
    MSG_WIFI_CONNECTED,
    MSG_WIFI_NTF_CLICKED,
#endif
#ifdef NDEF_BT_ENABLED
    // for connect BT by ndef bt tag
    MSG_BT_ENABLE_TIMEOUT,
    MSG_BT_PAIR_TIMEOUT,
    MSG_BT_CONNECT_TIMEOUT,
    MSG_BT_ENABLED,
    MSG_BT_PAIR_STATUS_CHANGED,
    MSG_BT_CONNECT_STATUS_CHANGED,
    MSG_BT_NTF_CLICKED,
#endif
};

enum class ScreenState {
    SCREEN_STATE_UNKNOWN = 0x00,
    SCREEN_STATE_OFF_UNLOCKED = 0x01,
    SCREEN_STATE_OFF_LOCKED = 0x02,
    SCREEN_STATE_ON_LOCKED = 0x04,
    SCREEN_STATE_ON_UNLOCKED = 0x08,
    // Polling mask
    SCREEN_POLLING_TAG_MASK = 0x10,
    SCREEN_POLLING_READER_MASK = 0x40
};

class INfcService {
public:
    virtual ~INfcService() {}

    virtual bool IsNfcEnabled() = 0;
    virtual int GetNfcState() = 0;
    virtual int GetScreenState() = 0;
    virtual int GetNciVersion() = 0;
    virtual std::weak_ptr<TAG::TagDispatcher> GetTagDispatcher() = 0;
    virtual std::weak_ptr<NfcPollingManager> GetNfcPollingManager() = 0;
    virtual std::weak_ptr<NfcRoutingManager> GetNfcRoutingManager() = 0;
    virtual OHOS::sptr<IRemoteObject> GetTagServiceIface() = 0;
    virtual OHOS::sptr<IRemoteObject> GetHceServiceIface() = 0;
    virtual std::weak_ptr<CeService> GetCeService() = 0 ;
    virtual std::string GetSimVendorBundleName() = 0;
};
}  // namespace NFC
}  // namespace OHOS
#endif  // I_NFC_SERVICE_H
