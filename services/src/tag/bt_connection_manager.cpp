/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "bt_connection_manager.h"

#include "common_event_manager.h"
#include "common_event_support.h"
#include "external_deps_proxy.h"
#include "infc_service.h"
#include "loghelper.h"
#include "nfc_service.h"

namespace OHOS {
namespace NFC {
namespace TAG {
const uint64_t BT_ENABLE_TIMEOUT = 3000; // ms
const uint64_t BT_PAIR_TIMEOUT = 10000; // ms
const uint64_t BT_CONNECT_TIMEOUT = 5000; // ms
const uint8_t PROFILE_MAX_SIZE = 21;

enum BtConnAction {
    ACTION_INIT = 1,
    ACTION_DISCONNECT,
    ACTION_CONNECT
};

enum BtConnState {
    STATE_WAITING_FOR_BT_ENABLE = 1,
    STATE_INIT,
    STATE_INIT_COMPLETE,
    STATE_PAIRING,
    STATE_PAIR_COMPLETE,
    STATE_CONNECTING,
    STATE_DISCONNECTING,
    STATE_COMPLETE
};

enum BtConnResult {
    CONN_RES_WAITING = 1,
    CONN_RES_CONNECTED,
    CONN_RES_DISCONNECTED
};

enum BtToastType : int {
    UNABLE_TO_PAIR = 101,
    PAIRING_WITH_DEVICE,
    PAIRING_SUCCESS,
    PAIRING_FAILURE,
    PAIR_TIME_OUT,
};

static Bluetooth::A2dpSource *g_a2dp;
static Bluetooth::HandsFreeAudioGateway *g_hfp;
static Bluetooth::HidHost *g_hid;

Bluetooth::BluetoothRemoteDevice g_device;
std::shared_ptr<BtData> g_btData {};

bool g_isStateObserverRegistered = false;
bool g_isDevObserverRegistered = false;
bool g_isA2dpSupported = false;
bool g_isHfpSupported = false;
bool g_isOnBtNtClicked = false;

uint8_t g_a2dpConnState = 0;
uint8_t g_hfpConnState = 0;
uint8_t g_hidConnState = 0;

uint8_t g_state = 0;
uint8_t g_action = 0;

BtConnectionManager::BtConnectionManager()
{
}

BtConnectionManager& BtConnectionManager::GetInstance()
{
    static BtConnectionManager instance;
    return instance;
}

void BtConnectionManager::Initialize(std::weak_ptr<NfcService> nfcService)
{
    DebugLog("Init: isInitialized = %{public}d", isInitialized_);
    if (isInitialized_) {
        return;
    }
    nfcService_ = nfcService;
    isInitialized_ = true;
}

void BtConnectionManager::SendMsgToEvtHandler(NfcCommonEvent evt, int64_t delay)
{
    if (nfcService_.expired()) {
        ErrorLog("nfcService expired");
        return;
    }
    if (nfcService_.lock()->eventHandler_ == nullptr) {
        ErrorLog("event handler is null");
        return;
    }
    DebugLog("SendMsgToEvtHandler: event:%{public}d, delay:%{public}ld", evt, delay);
    nfcService_.lock()->eventHandler_->SendEvent(static_cast<uint32_t>(evt), delay);
}

void BtConnectionManager::SendConnMsgToEvtHandler(NfcCommonEvent evt, const Bluetooth::BluetoothRemoteDevice &device,
                                                  int32_t state, BtProfileType type)
{
    if (nfcService_.expired()) {
        ErrorLog("nfcService expired");
        return;
    }
    if (nfcService_.lock()->eventHandler_ == nullptr) {
        ErrorLog("event handler is null");
        return;
    }
    std::shared_ptr<BtConnectionInfo> info = std::make_shared<BtConnectionInfo>();
    info->macAddr_ = device.GetDeviceAddr();
    info->state_ = state;
    info->type_ = static_cast<uint8_t>(type);
    DebugLog("SendConnMsgToEvtHandler: event:%{public}d", evt);
    nfcService_.lock()->eventHandler_->SendEvent(static_cast<uint32_t>(evt), info, 0);
}

void BtConnectionManager::RemoveMsgFromEvtHandler(NfcCommonEvent evt)
{
    if (nfcService_.expired()) {
        ErrorLog("nfcService expired");
        return;
    }
    if (nfcService_.lock()->eventHandler_ == nullptr) {
        ErrorLog("event handler is null");
        return;
    }
    DebugLog("RemoveMsgFromEvtHandler: event:%{public}d", evt);
    nfcService_.lock()->eventHandler_->RemoveEvent(static_cast<uint32_t>(evt), static_cast<int64_t>(0));
}

void BtConnectionManager::RegisterBtObserver()
{
    DebugLog("RegisterBtStateObserver");
    if (btObserver_ == nullptr) {
        btObserver_ = BtConnectionManager::BtStateObserver::GetInstance();
    }
    Bluetooth::BluetoothHost::GetDefaultHost().RegisterObserver(btObserver_);
    g_isStateObserverRegistered = true;
}

void BtConnectionManager::UnregisterBtObserverAndProfile()
{
    DebugLog("UnregisterBtObserverAndProfile");
    if (btObserver_ != nullptr && g_isStateObserverRegistered) {
        Bluetooth::BluetoothHost::GetDefaultHost().DeregisterObserver(btObserver_);
        btObserver_ = nullptr;
        g_isStateObserverRegistered = false;
        DebugLog("UnregisterBtObserverAndProfile: bt state observer deregistered");
    }
    if (btRemoteDevObserver_ != nullptr && g_isDevObserverRegistered) {
        Bluetooth::BluetoothHost::GetDefaultHost().DeregisterRemoteDeviceObserver(btRemoteDevObserver_);
        btRemoteDevObserver_ = nullptr;
        g_isDevObserverRegistered = false;
        DebugLog("UnregisterBtObserverAndProfile: bt remote device observer deregistered");
    }
    if (g_a2dp != nullptr) {
        if (btA2dpObserver_) {
            g_a2dp->DeregisterObserver(btA2dpObserver_);
            btA2dpObserver_ = nullptr;
            DebugLog("UnregisterBtObserverAndProfile: bt a2dp observer deregistered");
        }
        g_a2dp = nullptr;
    }
    if (g_hfp != nullptr) {
        if (btHfpObserver_) {
            g_hfp->DeregisterObserver(btHfpObserver_);
            btHfpObserver_ = nullptr;
            DebugLog("UnregisterBtObserverAndProfile: bt state observer deregistered");
        }
        g_hfp = nullptr;
    }
    if (g_hid != nullptr) {
        if (btHidObserver_) {
            g_hid->DeregisterObserver(btHidObserver_);
            btHidObserver_ = nullptr;
            DebugLog("UnregisterBtObserverAndProfile: bt state observer deregistered");
        }
        g_hid = nullptr;
    }
}

// lock outside
void BtConnectionManager::OnFinish()
{
    DebugLog("OnFinish");
    RemoveMsgFromEvtHandler(NfcCommonEvent::MSG_BT_ENABLE_TIMEOUT);
    RemoveMsgFromEvtHandler(NfcCommonEvent::MSG_BT_PAIR_TIMEOUT);
    RemoveMsgFromEvtHandler(NfcCommonEvent::MSG_BT_CONNECT_TIMEOUT);
    UnregisterBtObserverAndProfile();
    g_isStateObserverRegistered = false;
    g_isDevObserverRegistered = false;
    g_isA2dpSupported = false;
    g_isHfpSupported = false;
    g_isOnBtNtClicked = false;
    g_a2dpConnState = 0;
    g_hfpConnState = 0;
    g_hidConnState = 0;
    g_state = 0;
    g_action = 0;
    g_btData = nullptr;
}

void BtConnectionManager::HandleBtEnableFailed()
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    ErrorLog("Bt Enable Failed");
    OnFinish();
}

void BtConnectionManager::HandleBtPairFailed()
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    ErrorLog("Bt Pair Failed");
    ShowBtConnectionToast(PAIR_TIME_OUT);
    OnFinish();
}

void BtConnectionManager::HandleBtConnectFailed()
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    ErrorLog("Bt Connect Failed");
    OnFinish();
}

bool BtConnectionManager::IsBtEnabled()
{
    bool isEnabled = Bluetooth::BluetoothHost::GetDefaultHost().IsBrEnabled();
    InfoLog("IsBtEnabled: %{public}d", isEnabled);
    return isEnabled;
}

bool BtConnectionManager::HandleEnableBt()
{
    DebugLog("HandleEnableBt");
    SendMsgToEvtHandler(NfcCommonEvent::MSG_BT_ENABLE_TIMEOUT, BT_ENABLE_TIMEOUT);
    if (Bluetooth::BluetoothHost::GetDefaultHost().EnableBle() != Bluetooth::RET_NO_ERROR) { // EnableBt() is deprecated
        ErrorLog("HandleEnableBt: failed");
        ShowBtConnectionToast(UNABLE_TO_PAIR);
        return false;
    }
    return true;
}

bool BtConnectionManager::HandleBtPair()
{
    DebugLog("HandleBtPair");
    bool isDiscovering = false;
    Bluetooth::BluetoothHost::GetDefaultHost().IsBtDiscovering(isDiscovering, g_btData->transport_);
    if (isDiscovering) {
        InfoLog("Cancel discovery");
        Bluetooth::BluetoothHost::GetDefaultHost().CancelBtDiscovery();
    }
    // oob pair mode currently not supported by bluetooth
    if (btRemoteDevObserver_ == nullptr) {
        btRemoteDevObserver_ = std::make_shared<BtRemoteDevObserver>();
    }
    Bluetooth::BluetoothHost::GetDefaultHost().RegisterRemoteDeviceObserver(btRemoteDevObserver_);
    g_isDevObserverRegistered = true;

    InfoLog("Handle bt pair start");
    ShowBtConnectionToast(PAIRING_WITH_DEVICE);
    g_device.StartCrediblePair();
    SendMsgToEvtHandler(NfcCommonEvent::MSG_BT_PAIR_TIMEOUT, BT_PAIR_TIMEOUT);
    return true;
}

bool BtConnectionManager::IsA2dpSupported()
{
    if (g_btData == nullptr) {
        ErrorLog("IsA2dpSupported: g_btData error");
        return false;
    }
    for (Bluetooth::UUID uuid : g_btData->uuids_) {
        if ((uuid.ToString().compare(Bluetooth::BLUETOOTH_UUID_A2DP_SINK) == 0) ||
            (uuid.ToString().compare(Bluetooth::BLUETOOTH_UUID_AVRCP_CT) == 0)) {
            return true;
        }
    }
    return g_btData->btClass_.IsProfileSupported(Bluetooth::PROFILE_ID_A2DP_SRC);
}

bool BtConnectionManager::IsHfpSupported()
{
    if (g_btData == nullptr) {
        ErrorLog("IsHfpSupported: g_btData error");
        return false;
    }
    for (Bluetooth::UUID uuid : g_btData->uuids_) {
        if ((uuid.ToString().compare(Bluetooth::BLUETOOTH_UUID_HFP_AG) == 0) ||
            (uuid.ToString().compare(Bluetooth::BLUETOOTH_UUID_HFP_HF) == 0)) {
            return true;
        }
    }
    return g_btData->btClass_.IsProfileSupported(Bluetooth::PROFILE_ID_HFP_AG);
}

// false to OnFinish()
bool BtConnectionManager::HandleBtInit()
{
    if (g_btData == nullptr) {
        ErrorLog("HandleBtInit: g_btData error");
        ShowBtConnectionToast(UNABLE_TO_PAIR);
        return false;
    }
    if (g_device.GetDeviceAddr().compare(g_btData->macAddress_) != 0) {
        g_device = Bluetooth::BluetoothRemoteDevice(g_btData->macAddress_, g_btData->transport_);
    }
    if (g_btData->transport_ == Bluetooth::GATT_TRANSPORT_TYPE_LE) {
        InfoLog("Init: hid getprofile");
        if (g_hid == nullptr) {
            g_hid = Bluetooth::HidHost::GetProfile();
        }
    } else {
        if (g_a2dp == nullptr) {
            g_a2dp = Bluetooth::A2dpSource::GetProfile();
        }
        if (g_hfp == nullptr) {
            g_hfp = Bluetooth::HandsFreeAudioGateway::GetProfile();
        }
        g_isA2dpSupported = IsA2dpSupported();
        g_isHfpSupported = IsHfpSupported();
        InfoLog("Init:a2dp: %{public}d, hfp: %{public}d", g_isA2dpSupported, g_isHfpSupported);
        if (!g_isA2dpSupported && !g_isHfpSupported) {
            // if both not supported, maybe the info is empty in ndef
            // try both
            g_isA2dpSupported = true;
            g_isHfpSupported = true;
            InfoLog("Init:a2dp and hid not supported");
        }
    }
    return true;
}

// false to OnFinish()
bool BtConnectionManager::DecideInitNextAction()
{
    if (g_btData == nullptr) {
        ErrorLog("HandleBtInit: g_btData error");
        ShowBtConnectionToast(UNABLE_TO_PAIR);
        return false;
    }
    int32_t state = 0;
    if (g_btData->transport_ == Bluetooth::GATT_TRANSPORT_TYPE_LE) {
        if (!g_hid) {
            ErrorLog("DecideInitNextAction: hid not supported");
            ShowBtConnectionToast(UNABLE_TO_PAIR);
            return false;
        }
        g_hid->GetDeviceState(g_device, state);
        InfoLog("DecideInitNextAction: hid device state: %{public}d", state);
        if (state == static_cast<int32_t>(Bluetooth::BTConnectState::CONNECTED)) {
            g_action = ACTION_DISCONNECT;
        } else {
            g_action = ACTION_CONNECT;
        }
    } else {
        if (g_a2dp && g_isA2dpSupported) {
            g_a2dp->GetDeviceState(g_device, state);
        }
        
        InfoLog("DecideInitNextAction: a2dp device state: %{public}d", state);
        if (state == static_cast<int32_t>(Bluetooth::BTConnectState::CONNECTED)) {
            g_action = ACTION_DISCONNECT;
        } else {
            // unconnected a2dp and all hfp go to connect action
            g_action = ACTION_CONNECT;
        }
    }
    return true;
}

bool BtConnectionManager::GetProfileList()
{
    std::vector<uint32_t> profileList = Bluetooth::BluetoothHost::GetDefaultHost().GetProfileList();
    if (profileList.size() == 0 && profileList.size() > PROFILE_MAX_SIZE) {
        ErrorLog("profile list size error");
        return false;
    }
    for (uint32_t i = 0; i < profileList.size(); i++) {
        if (g_btData->transport_ == Bluetooth::GATT_TRANSPORT_TYPE_LE &&
            profileList[i] == Bluetooth::PROFILE_ID_HID_HOST) {
            InfoLog("PROFILE_ID_HID_HOST");
            return true;
        } else {
            if (profileList[i] == Bluetooth::PROFILE_ID_A2DP_SRC ||
                profileList[i] == Bluetooth::PROFILE_ID_HFP_AG) {
                InfoLog("PROFILE_ID_A2DP_SRC OR PROFILE_ID_HFP_AG");
                return true;
            }
        }
    }
    return false;
}

void BtConnectionManager::NextActionInit()
{
    InfoLog("NextActionInit: g_state: %{public}d", g_state);
    if (g_state == STATE_WAITING_FOR_BT_ENABLE) {
        return;
    }
    if (g_state == STATE_INIT) {
        if (!GetProfileList()) {
            ErrorLog("NextActionInit: GetProfileList error");
            return OnFinish();
        }
        if (!HandleBtInit()) {
            ErrorLog("NextActionInit: HandleBtInit error");
            return OnFinish();
        }
        if (!DecideInitNextAction()) {
            ErrorLog("NextActionInit: DecideInitNextAction error");
            return OnFinish();
        }
        g_state = STATE_INIT_COMPLETE;
    }
    NextAction();
}

void BtConnectionManager::RegisterProfileObserver(BtProfileType type)
{
    InfoLog("RegisterProfileObserver type: %{public}d", type);
    switch (type) {
        case A2DP_SRC: {
            if (!g_a2dp || !g_isA2dpSupported) {
                ErrorLog("RegisterProfileObserver: a2dp error");
                break;
            }
            if (!btA2dpObserver_) {
                btA2dpObserver_ = std::make_shared<BtA2dpObserver>();
                g_a2dp->RegisterObserver(btA2dpObserver_);
            }
            break;
        }
        case HFP_AG: {
            if (!g_hfp || !g_isHfpSupported) {
                ErrorLog("RegisterProfileObserver: hfp error");
                break;
            }
            if (!btHfpObserver_) {
                btHfpObserver_ = std::make_shared<BtHfpObserver>();
                g_hfp->RegisterObserver(btHfpObserver_);
            }
            break;
        }
        case HID_HOST: {
            if (!g_hid) {
                ErrorLog("RegisterProfileObserver: hid error");
                break;
            }
            if (!btHidObserver_) {
                btHidObserver_ = std::make_shared<BtHidObserver>();
                g_hid->RegisterObserver(btHidObserver_);
            }
            break;
        }
        default:
            break;
    }
}

// true for need wait, false for go to NextStep
bool BtConnectionManager::HandleBtConnect()
{
    DebugLog("HandleBtConnect");
    int32_t state = 0;
    if (g_hfp) {
        g_hfp->GetDeviceState(g_device, state);
        InfoLog("HandleBtConnect: hfp state = %{public}d", state);
        if (state != static_cast<int32_t>(Bluetooth::BTConnectState::CONNECTED)) {
            if (g_isHfpSupported) {
                g_hfpConnState = BtConnResult::CONN_RES_WAITING;
                RegisterProfileObserver(HFP_AG);
                InfoLog("HandleBtConnect: hfp connect start");
                g_hfp->Connect(g_device);
            } else {
                g_hfpConnState = BtConnResult::CONN_RES_DISCONNECTED;
                InfoLog("HandleBtConnect: hfp disconnected");
            }
        } else {
            g_hfpConnState = BtConnResult::CONN_RES_CONNECTED;
            InfoLog("HandleBtConnect: hfp connected");
        }
    }
    if (g_a2dp) {
        g_a2dp->GetDeviceState(g_device, state);
        InfoLog("HandleBtConnect: a2dp state = %{public}d", state);
        if (state != static_cast<int32_t>(Bluetooth::BTConnectState::CONNECTED)) {
            if (g_isA2dpSupported) {
                g_a2dpConnState = BtConnResult::CONN_RES_WAITING;
                RegisterProfileObserver(A2DP_SRC);
                InfoLog("HandleBtConnect: a2dp connect start");
                g_a2dp->Connect(g_device);
            } else {
                g_a2dpConnState = BtConnResult::CONN_RES_DISCONNECTED;
                InfoLog("HandleBtConnect: a2dp disconnected");
            }
        } else {
            g_a2dpConnState = BtConnResult::CONN_RES_CONNECTED;
            InfoLog("HandleBtConnect: a2dp connected");
        }
    }
    if (g_a2dpConnState == BtConnResult::CONN_RES_WAITING ||
        g_hfpConnState == BtConnResult::CONN_RES_WAITING) {
        InfoLog("HandleBtConnect: waiting for connect result");
        SendMsgToEvtHandler(NfcCommonEvent::MSG_BT_CONNECT_TIMEOUT, BT_CONNECT_TIMEOUT);
        return true;
    }
    return false;
}

// true for need wait, false for go to OnFinish()
bool BtConnectionManager::HandleBtConnectWaiting()
{
    if (g_a2dpConnState == BtConnResult::CONN_RES_WAITING ||
        g_hfpConnState == BtConnResult::CONN_RES_WAITING) {
        InfoLog("HandleBtConnectWaiting: waiting for connect result");
        return true;
    }
    if (g_a2dpConnState == BtConnResult::CONN_RES_CONNECTED ||
        g_hfpConnState == BtConnResult::CONN_RES_CONNECTED) {
        InfoLog("HandleBtConnectWaiting: connect success");
        g_device.SetDeviceAlias(g_btData->name_);
        return false;
    } else {
        // connect failed
        return false;
    }
}

void BtConnectionManager::NextActionConnect()
{
    int pairState = 0;
    g_device.GetPairState(pairState);
    InfoLog("NextActionConnect: state: %{public}d, pairState: %{public}d", g_state, pairState);
    switch (g_state) {
        case STATE_INIT_COMPLETE: {
            if (pairState != Bluetooth::PAIR_PAIRED) {
                g_state = STATE_PAIRING;
                PublishPairBtNtf();
                break;
            }
            // fall-through
            // when already paired
        }
        case STATE_PAIR_COMPLETE: {
            g_state = STATE_CONNECTING;
            if (g_btData->transport_ != Bluetooth::GATT_TRANSPORT_TYPE_LE) {
                if (HandleBtConnect()) {
                    InfoLog("connecting, need wait");
                    break;
                }
            }
            // fall-through
            // when transport not LE or connect success
        }
        case STATE_CONNECTING: {
            if (g_btData->transport_ != Bluetooth::GATT_TRANSPORT_TYPE_LE) {
                if (!HandleBtConnectWaiting()) {
                    OnFinish();
                }
            }
            break;
        }
        default:
            break;
    }
}

// true for need wait, false for go to NextStep
bool BtConnectionManager::HandleBtDisconnect()
{
    int32_t devState = 0;
    if (g_btData->transport_ == Bluetooth::GATT_TRANSPORT_TYPE_LE) {
        if (!g_hid) {
            return false;
        }
        g_hid->GetDeviceState(g_device, devState);
        if (devState != static_cast<int32_t>(Bluetooth::BTConnectState::DISCONNECTED)) {
            g_hidConnState = BtConnResult::CONN_RES_WAITING;
            RegisterProfileObserver(HID_HOST);
            InfoLog("HandleBtDisconnect: hid disconnect start");
            g_hid->Disconnect(g_device);
            return true;
        } else {
            g_hidConnState = BtConnResult::CONN_RES_DISCONNECTED;
            InfoLog("HandleBtDisconnect: hfp disconnect");
        }
    } else {
        if (g_hfp && g_isHfpSupported) {
            g_hfp->GetDeviceState(g_device, devState);
            if (devState != static_cast<int32_t>(Bluetooth::BTConnectState::DISCONNECTED)) {
                g_hfpConnState = BtConnResult::CONN_RES_WAITING;
                RegisterProfileObserver(HFP_AG);
                InfoLog("HandleBtDisconnect: hfp disconnect start");
            } else {
                g_hfpConnState = BtConnResult::CONN_RES_DISCONNECTED;
                InfoLog("HandleBtDisconnect: hfp disconnected");
            }
        }
        if (g_a2dp && g_isA2dpSupported) {
            g_a2dp->GetDeviceState(g_device, devState);
            if (devState != static_cast<int32_t>(Bluetooth::BTConnectState::DISCONNECTED)) {
                g_a2dpConnState = BtConnResult::CONN_RES_WAITING;
                RegisterProfileObserver(A2DP_SRC);
                InfoLog("HandleBtDisconnect: a2dp disconnect start");
            } else {
                g_a2dpConnState = BtConnResult::CONN_RES_DISCONNECTED;
                InfoLog("HandleBtDisconnect: a2dp disconnected");
            }
        }
        if (g_a2dpConnState == BtConnResult::CONN_RES_WAITING ||
            g_hfpConnState == BtConnResult::CONN_RES_WAITING) {
            Bluetooth::BluetoothHost::GetDefaultHost().DisconnectAllowedProfiles(g_btData->macAddress_);
            InfoLog("HandleBtDisconnect: waiting for disconnect result");
            return true;
        }
    }
    return false;
}

// true for need wait, false for go to OnFinish()
bool BtConnectionManager::HandleBtDisconnectWaiting()
{
    if (g_btData->transport_ == Bluetooth::GATT_TRANSPORT_TYPE_LE) {
        if (g_hidConnState == BtConnResult::CONN_RES_DISCONNECTED) {
            InfoLog("HandleBtDisconnectWaiting:hid disconnected");
            return false;
        }
    } else {
        if (g_a2dpConnState == BtConnResult::CONN_RES_WAITING ||
            g_hfpConnState == BtConnResult::CONN_RES_WAITING) {
            InfoLog("HandleBtDisconnectWaiting: waiting for disconnect result");
            return true;
        }
        if (g_a2dpConnState == BtConnResult::CONN_RES_DISCONNECTED &&
            g_hfpConnState == BtConnResult::CONN_RES_DISCONNECTED) {
            InfoLog("HandleBtDisconnectWaiting: disconnect success");
            return false;
        }
    }
    return false;
}

void BtConnectionManager::NextActionDisconnect()
{
    int pairState = 0;
    g_device.GetPairState(pairState);
    InfoLog("NextActionConnect: state: %{public}d, pairState: %{public}d", g_state, pairState);
    switch (g_state) {
        case STATE_INIT_COMPLETE: {
            g_state = STATE_DISCONNECTING;
            if (HandleBtDisconnect()) {
                InfoLog("NextActionConnect: disconnecting need wait");
                break;
            }
            // fall-through
            // when already disconnected
        }
        case STATE_DISCONNECTING: {
            if (!HandleBtDisconnectWaiting()) {
                OnFinish();
            }
            break;
        }
        default:
            break;
    }
}

void BtConnectionManager::NextAction()
{
    InfoLog("NextAction: state: %{public}d action: %{public}d", g_state, g_action);
    switch (g_action) {
        case ACTION_INIT:
            NextActionInit();
            break;
        case ACTION_CONNECT:
            NextActionConnect();
            break;
        case ACTION_DISCONNECT:
            NextActionDisconnect();
            break;
        default:
            break;
    }
}

void BtConnectionManager::TryPairBt(std::shared_ptr<BtData> data)
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    if (!data || !data->isValid_) {
        ErrorLog("TryPairBt: data error");
        ShowBtConnectionToast(UNABLE_TO_PAIR);
        return;
    }
    RemoveMsgFromEvtHandler(NfcCommonEvent::MSG_BT_ENABLE_TIMEOUT);
    RemoveMsgFromEvtHandler(NfcCommonEvent::MSG_BT_CONNECT_TIMEOUT);
    g_btData = data;
    g_action = ACTION_INIT;
    RegisterBtObserver();
    if (IsBtEnabled()) {
        g_state = STATE_INIT;
        NextAction();
    } else {
        g_state = STATE_WAITING_FOR_BT_ENABLE;
        if (!HandleEnableBt()) {
            ErrorLog("TryPairBt enable bt failed");
            OnFinish();
        }
    }
}

void BtConnectionManager::PublishPairBtNtf()
{
    RemoveMsgFromEvtHandler(NfcCommonEvent::MSG_BT_PAIR_TIMEOUT);
    if (g_btData == nullptr) {
        ErrorLog("PublishPairBtNtf: g_btData nullptr");
        return;
    }
    InfoLog("PublishPairBtNtf: Publish notification name: %{private}s", g_btData->name_.c_str());
    ExternalDepsProxy::GetInstance().PublishNfcNotification(NFC_BT_NOTIFICATION_ID, g_btData->name_, 0);
}

void BtConnectionManager::OnBtNtfClicked()
{
    InfoLog("OnBtNtfClicked");
    std::unique_lock<std::shared_mutex> guard(mutex_);
    g_isOnBtNtClicked = true;
    if (g_btData == nullptr) {
        ErrorLog("OnBtNtfClicked: g_btData error");
        return;
    }
    g_action = ACTION_CONNECT;
    if (IsBtEnabled()) {
        g_state = STATE_PAIRING;
        HandleBtPair();
        NextAction();
    } else {
        g_state = STATE_WAITING_FOR_BT_ENABLE;
        if (!HandleEnableBt()) {
            ErrorLog("OnBtNtfClicked enable bt failed");
            OnFinish();
        }
    }
}

void BtConnectionManager::OnBtEnabled()
{
    DebugLog("OnBtEnabled");
    RemoveMsgFromEvtHandler(NfcCommonEvent::MSG_BT_ENABLE_TIMEOUT);
    if (g_state == STATE_WAITING_FOR_BT_ENABLE) {
        g_state = STATE_INIT;
        NextAction();
    }
}

void BtConnectionManager::ShowBtConnectionToast(int type)
{
    if (nfcService_.expired()) {
        ErrorLog("nfcService expired");
        return;
    }
    if (!g_isOnBtNtClicked) {
        ErrorLog("g_isOnBtNtClicked is false, not shuow toast");
        return;
    }
    std::string btToastValue = std::to_string(type);
    if (type == PAIRING_WITH_DEVICE && g_btData != nullptr) {
        btToastValue.append(g_btData->name_);
    }
    InfoLog("NotifyMessageToVendor btToastValue: %{public}s", btToastValue.c_str());
    nfcService_.lock()->NotifyMessageToVendor(KITS::TOAST_TYPE_KEY, btToastValue);
}

void BtConnectionManager::BtStateObserver::OnStateChanged(const int transport, const int status)
{
    InfoLog("OnStateChanged transport: %{public}d, status: %{public}d", transport, status);
    if (transport == static_cast<int>(Bluetooth::BTTransport::ADAPTER_BREDR) &&
        status == static_cast<int>(Bluetooth::STATE_TURN_ON)) {
        BtConnectionManager::GetInstance().OnBtEnabled();
    }
}

void BtConnectionManager::OnPairStatusChanged(std::shared_ptr<BtConnectionInfo> info)
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    if (info == nullptr) {
        ErrorLog("OnPairStatusChanged: info is null");
        ShowBtConnectionToast(UNABLE_TO_PAIR);
        return;
    }
    if (g_btData == nullptr) {
        ErrorLog("OnPairStatusChanged: g_btData error");
        ShowBtConnectionToast(UNABLE_TO_PAIR);
        return;
    }
    if (info->macAddr_.compare(g_btData->macAddress_) != 0) {
        ErrorLog("OnPairStatusChanged not same device");
        ShowBtConnectionToast(UNABLE_TO_PAIR);
        return;
    }
    if (g_state == STATE_PAIRING) {
        if (info->state_ == Bluetooth::PAIR_PAIRED) {
            RemoveMsgFromEvtHandler(NfcCommonEvent::MSG_BT_PAIR_TIMEOUT);
            ShowBtConnectionToast(PAIRING_SUCCESS);
            g_state = STATE_PAIR_COMPLETE;
            NextAction();
        } else if (info->state_ == Bluetooth::PAIR_NONE) {
            // timeout msg removed in OnFinish()
            ErrorLog("OnPairStatusChanged: pair failed");
            ShowBtConnectionToast(PAIRING_FAILURE);
            OnFinish();
        }
    }
}

void BtConnectionManager::BtRemoteDevObserver::OnPairStatusChanged(const Bluetooth::BluetoothRemoteDevice &device,
                                                                   int status, int cause)
{
    (void)cause; // Unused parameter
    InfoLog("OnPairStatusChanged status: %{public}d", status);
    BtConnectionManager::GetInstance().SendConnMsgToEvtHandler(NfcCommonEvent::MSG_BT_PAIR_STATUS_CHANGED,
        device, status, BtConnectionManager::BtProfileType::A2DP_SRC);
}

void BtConnectionManager::OnConnectionStateChanged(std::shared_ptr<BtConnectionInfo> info)
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    if (info == nullptr) {
        ErrorLog("OnConnectionStateChanged: info is null");
        return;
    }
    if (g_btData == nullptr) {
        ErrorLog("OnConnectionStateChanged: g_btData error");
        return;
    }
    if (info->macAddr_.compare(g_btData->macAddress_) != 0) {
        ErrorLog("OnConnectionStateChanged not same device");
        return;
    }
    if (info->state_ == static_cast<int32_t>(Bluetooth::BTConnectState::CONNECTED)) {
        {
            if (info->type_ == static_cast<uint8_t>(HFP_AG)) {
                g_hfpConnState = CONN_RES_CONNECTED;
            } else if (info->type_ == static_cast<uint8_t>(A2DP_SRC)) {
                g_a2dpConnState = CONN_RES_CONNECTED;
            } else if (info->type_ == static_cast<uint8_t>(HID_HOST)) {
                g_hidConnState = CONN_RES_CONNECTED;
            }
        }
        NextAction();
    } else if (info->state_ == static_cast<int32_t>(Bluetooth::BTConnectState::DISCONNECTED)) {
        {
            if (g_action == ACTION_CONNECT) {
                // need retry
                return;
            }
            if (info->type_ == static_cast<uint8_t>(HFP_AG)) {
                g_hfpConnState = CONN_RES_DISCONNECTED;
            } else if (info->type_ == static_cast<uint8_t>(A2DP_SRC)) {
                g_a2dpConnState = CONN_RES_DISCONNECTED;
            } else if (info->type_ == static_cast<uint8_t>(HID_HOST)) {
                g_hidConnState = CONN_RES_DISCONNECTED;
            }
        }
        NextAction();
    }
}

void BtConnectionManager::BtA2dpObserver::OnConnectionStateChanged(const Bluetooth::BluetoothRemoteDevice &device,
                                                                   int32_t state, int32_t cause)
{
    (void)cause; // Unused param
    InfoLog("BtA2dpObserver::OnConnectionStateChanged state: %{public}d", state);
    BtConnectionManager::GetInstance().SendConnMsgToEvtHandler(NfcCommonEvent::MSG_BT_CONNECT_STATUS_CHANGED,
        device, state, BtConnectionManager::BtProfileType::A2DP_SRC);
}

void BtConnectionManager::BtHfpObserver::OnConnectionStateChanged(const Bluetooth::BluetoothRemoteDevice &device,
                                                                  int32_t state, int32_t cause)
{
    (void)cause; // Unused param
    InfoLog("BtHfpObserver::OnConnectionStateChanged state: %{public}d", state);
    BtConnectionManager::GetInstance().SendConnMsgToEvtHandler(NfcCommonEvent::MSG_BT_CONNECT_STATUS_CHANGED,
        device, state, BtConnectionManager::BtProfileType::HFP_AG);
}

void BtConnectionManager::BtHidObserver::OnConnectionStateChanged(const Bluetooth::BluetoothRemoteDevice &device,
                                                                  int state, int cause)
{
    (void)cause; // Unused param
    InfoLog("BtHidObserver::OnConnectionStateChanged state: %{public}d", state);
    BtConnectionManager::GetInstance().SendConnMsgToEvtHandler(NfcCommonEvent::MSG_BT_CONNECT_STATUS_CHANGED,
        device, state, BtConnectionManager::BtProfileType::HID_HOST);
}
} // namespace TAG
} // namespace NFC
} // namespace OHOS
