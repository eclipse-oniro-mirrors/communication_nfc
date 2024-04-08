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

#include "wifi_connection_manager.h"

#include "common_event_manager.h"
#include "common_event_support.h"
#include "external_deps_proxy.h"
#include "infc_service.h"
#include "loghelper.h"
#include "nfc_service.h"
#include "wifi_errcode.h"
#include "wifi_msg.h"

namespace OHOS {
namespace NFC {
namespace TAG {
const int64_t ENABLE_WIFI_TIMEOUT = 5000; // ms
const int64_t CONNECT_WIFI_TIMEOUT = 5000; // ms
std::shared_ptr<Wifi::WifiDevice> wifiDevPtr_ {};
Wifi::WifiDeviceConfig* config_ {};
std::shared_ptr<EventFwk::CommonEventSubscriber> wifiSubscriber_ {};
bool g_isWaitingForWifiEnable = false;
bool g_isWaitingForWifiConnect = false;

WifiConnectionManager::WifiConnectionManager()
{
}

WifiConnectionManager& WifiConnectionManager::GetInstance()
{
    static WifiConnectionManager instance;
    return instance;
}

void WifiConnectionManager::Initialize(std::weak_ptr<NfcService> nfcService)
{
    DebugLog("Init: isInitialized = %{public}d", isInitialized_);
    if (isInitialized_) {
        return;
    }
    nfcService_ = nfcService;
    isInitialized_ = true;
}

class WifiConnectionManager::WifiCommonEventReceiver : public EventFwk::CommonEventSubscriber {
public:
    explicit WifiCommonEventReceiver(WifiConnectionManager& nfcWifiConnMgr,
        const EventFwk::CommonEventSubscribeInfo& subscribeInfo);
    ~WifiCommonEventReceiver()
    {
    }
    void OnReceiveEvent(const EventFwk::CommonEventData& data) override;

private:
    WifiConnectionManager& nfcWifiConnMgr_;
};

WifiConnectionManager::WifiCommonEventReceiver::WifiCommonEventReceiver(WifiConnectionManager& nfcWifiConnMgr,
    const EventFwk::CommonEventSubscribeInfo& subscribeInfo)
    : EventFwk::CommonEventSubscriber(subscribeInfo),
    nfcWifiConnMgr_(nfcWifiConnMgr)
{
}

void WifiConnectionManager::SubscribeWifiCommonEvents()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_WIFI_POWER_STATE);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_WIFI_CONN_STATE);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    wifiSubscriber_ = std::make_shared<WifiCommonEventReceiver>(*this, subscribeInfo);
    if (wifiSubscriber_ == nullptr) {
        ErrorLog("Create wifi subscriber failed");
        return;
    }
    if (!EventFwk::CommonEventManager::SubscribeCommonEvent(wifiSubscriber_)) {
        ErrorLog("Subscribe wifi event fail");
    }
}

void WifiConnectionManager::UnsubscribeWifiCommonEvents()
{
    if (!wifiSubscriber_) {
        InfoLog("already unsubscribed");
        return;
    }
    DebugLog("UnsubscribeWifiCommonEvents");
    if (!EventFwk::CommonEventManager::UnSubscribeCommonEvent(wifiSubscriber_)) {
        ErrorLog("Unsubscribe wifi event fail");
        return;
    }
    wifiSubscriber_ = nullptr;
}

void WifiConnectionManager::SendMsgToEvtHandler(NfcCommonEvent evt, int64_t delay)
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

void WifiConnectionManager::RemoveMsgFromEvtHandler(NfcCommonEvent evt)
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

std::shared_ptr<Wifi::WifiDevice> WifiConnectionManager::GetWifiDevPtr()
{
    if (wifiDevPtr_ == nullptr) {
        wifiDevPtr_ = Wifi::WifiDevice::GetInstance(WIFI_DEVICE_ABILITY_ID);
    }
    return wifiDevPtr_;
}

void WifiConnectionManager::OnFinish()
{
    DebugLog("OnFinish");
    g_isWaitingForWifiEnable = false;
    g_isWaitingForWifiConnect = false;
    delete config_;
    config_ = nullptr;
    RemoveMsgFromEvtHandler(NfcCommonEvent::MSG_WIFI_ENABLE_TIMEOUT);
    RemoveMsgFromEvtHandler(NfcCommonEvent::MSG_WIFI_CONNECT_TIMEOUT);
    UnsubscribeWifiCommonEvents();
}

void WifiConnectionManager::HandleWifiEnableFailed()
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    ErrorLog("Wifi Enable Failed");
    OnFinish();
}

void WifiConnectionManager::HandleWifiConnectFailed()
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    ErrorLog("Wifi Connect Failed");
    OnFinish();
}

__attribute__((no_sanitize("cfi"))) bool WifiConnectionManager::IsWifiEnabled()
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    if (GetWifiDevPtr() == nullptr) {
        ErrorLog("wifi dev is null");
        return false;
    }
    bool isEnabled = false;
    ErrCode ret = wifiDevPtr_->IsWifiActive(isEnabled);
    if (ret != Wifi::WIFI_OPT_SUCCESS) {
        ErrorLog("get wifi active status failed ret = %{public}d", ret);
        return false;
    }
    InfoLog("get wifi active status = %{public}d", isEnabled);
    return isEnabled;
}

__attribute__((no_sanitize("cfi"))) bool WifiConnectionManager::HandleEnableWifi()
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    if (GetWifiDevPtr() == nullptr) {
        ErrorLog("wifi dev is null");
        return false;
    }
    ErrCode ret = wifiDevPtr_->EnableWifi();
    if (ret != Wifi::WIFI_OPT_SUCCESS) {
        ErrorLog("enable wifi failed ret = %{public}d", ret);
        return false;
    }
    g_isWaitingForWifiEnable = true;
    SendMsgToEvtHandler(NfcCommonEvent::MSG_WIFI_ENABLE_TIMEOUT, ENABLE_WIFI_TIMEOUT);
    return true;
}

__attribute__((no_sanitize("cfi"))) bool WifiConnectionManager::IsSameSsid()
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    if (GetWifiDevPtr() == nullptr) {
        ErrorLog("wifi dev is null");
        return false;
    }
    if (config_ == nullptr) {
        ErrorLog("config_ is null");
        return false;
    }
    Wifi::WifiLinkedInfo info;
    ErrCode ret = wifiDevPtr_->GetLinkedInfo(info);
    if (ret != Wifi::WIFI_OPT_SUCCESS) {
        ErrorLog("get linked info failed ret = %{public}d", ret);
        return false;
    }
    DebugLog("current ssid: %{private}s, target ssid: %{private}s", info.ssid.c_str(), config_->ssid.c_str());
    if (info.ssid.compare(config_->ssid.c_str()) == 0) {
        return true;
    }
    return false;
}

void WifiConnectionManager::TryConnectWifi(std::shared_ptr<WifiData> data)
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    if (!data || !data->isValid_) {
        ErrorLog("data invalid");
        return;
    }
    if (!data->config_) {
        ErrorLog("config is null");
        return;
    }
    RemoveMsgFromEvtHandler(NfcCommonEvent::MSG_WIFI_ENABLE_TIMEOUT);
    RemoveMsgFromEvtHandler(NfcCommonEvent::MSG_WIFI_CONNECT_TIMEOUT);
    config_ = data->config_;
    InfoLog("TryConnectWifi: Publish notification ssid: %{private}s", config_->ssid.c_str());
    ExternalDepsProxy::GetInstance().PublishNfcNotification(NFC_WIFI_NOTIFICATION_ID, config_->ssid, 0);
}

__attribute__((no_sanitize("cfi"))) bool WifiConnectionManager::HandleConnectWifi()
{
    if (IsSameSsid()) {
        InfoLog("already connected to target");
        OnFinish();
        return true;
    }

    std::unique_lock<std::shared_mutex> guard(mutex_);
    InfoLog("HandleConnectWifi");
    if (GetWifiDevPtr() == nullptr) {
        ErrorLog("wifi dev is null");
        HandleWifiConnectFailed();
        return false;
    }
    if (config_ == nullptr) {
        ErrorLog("config_ is null");
        HandleWifiConnectFailed();
        return false;
    }
    int result;
    ErrCode err = wifiDevPtr_->AddDeviceConfig(*(config_), result, false);
    InfoLog("AddDeviceConfig result: %{public}d, err: %{public}d", result, err);
    if (err != Wifi::WIFI_OPT_SUCCESS || result < 0) {
        ErrorLog("AddDeviceConfig failed result: %{public}d, err: %{public}d", result, err);
        HandleWifiConnectFailed();
        return false;
    }
    err = wifiDevPtr_->ConnectToDevice(*(config_));
    InfoLog("ConnectToDevice err: %{public}d", err);
    if (err != Wifi::WIFI_OPT_SUCCESS) {
        ErrorLog("ConnectToDevice failed err: %{public}d", err);
        HandleWifiConnectFailed();
        return false;
    }
    SendMsgToEvtHandler(NfcCommonEvent::MSG_WIFI_CONNECT_TIMEOUT, CONNECT_WIFI_TIMEOUT);
    g_isWaitingForWifiConnect = true;
    return true;
}

void WifiConnectionManager::OnWifiNtfClicked()
{
    InfoLog("OnWifiNtfClicked");
    SubscribeWifiCommonEvents();
    if (IsWifiEnabled()) {
        HandleConnectWifi();
    } else if (!HandleEnableWifi()) {
        HandleWifiEnableFailed();
    }
}

void WifiConnectionManager::OnWifiEnabled()
{
    DebugLog("OnWifiEnabled");
    {
        std::unique_lock<std::shared_mutex> guard(mutex_);
        if (!g_isWaitingForWifiEnable) {
            ErrorLog("not waiting for wifi enable, exit");
            return;
        }
        RemoveMsgFromEvtHandler(NfcCommonEvent::MSG_WIFI_ENABLE_TIMEOUT);
        g_isWaitingForWifiEnable = false;
    }
    HandleConnectWifi();
}

void WifiConnectionManager::OnWifiConnected()
{
    DebugLog("OnWifiConnected");
    {
        std::unique_lock<std::shared_mutex> guard(mutex_);
        if (!g_isWaitingForWifiConnect) {
            ErrorLog("not waiting for wifi connect, exit");
            return;
        }
    }
    if (!IsSameSsid()) {
        HandleWifiConnectFailed();
    } else {
        InfoLog("connected to target config");
        OnFinish();
    }
}

void WifiConnectionManager::WifiCommonEventReceiver::OnReceiveEvent(const EventFwk::CommonEventData& data)
{
    std::string action = data.GetWant().GetAction();
    if (action.empty()) {
        ErrorLog("action is empty");
        return;
    }
    InfoLog("OnReceiveEvent: action: %{public}s, code: %{public}d", action.c_str(), data.GetCode());
    if (action.compare(EventFwk::CommonEventSupport::COMMON_EVENT_WIFI_POWER_STATE) == 0) {
        if (data.GetCode() != static_cast<uint32_t>(Wifi::WifiState::ENABLED)) {
            return;
        }
        nfcWifiConnMgr_.SendMsgToEvtHandler(NfcCommonEvent::MSG_WIFI_ENABLED, 0);
    } else if (action.compare(EventFwk::CommonEventSupport::COMMON_EVENT_WIFI_CONN_STATE) == 0) {
        if (data.GetCode() != static_cast<uint32_t>(Wifi::ConnState::CONNECTED)) {
            return;
        }
        nfcWifiConnMgr_.SendMsgToEvtHandler(NfcCommonEvent::MSG_WIFI_CONNECTED, 0);
    }
}
} // namespace TAG
} // namespace NFC
} // namespace OHOS
