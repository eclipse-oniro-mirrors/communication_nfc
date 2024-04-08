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
#ifndef NFC_WIFI_CONN_MGR_H
#define NFC_WIFI_CONN_MGR_H

#include <shared_mutex>
#include "ndef_wifi_data_parser.h"
#include "nfc_event_handler.h"
#include "wifi_device.h"

namespace OHOS {
namespace NFC {
namespace TAG {
class WifiConnectionManager {
public:
    static WifiConnectionManager& GetInstance();
    void Initialize(std::weak_ptr<NfcService> nfcService);

    void TryConnectWifi(std::shared_ptr<WifiData> data);
    void OnWifiNtfClicked();
    void HandleWifiEnableFailed();
    void HandleWifiConnectFailed();
    void OnWifiEnabled();
    void OnWifiConnected();
protected:
    // wifi common event receiver
    class WifiCommonEventReceiver;
private:
    WifiConnectionManager();
    ~WifiConnectionManager() {}
    // wifi common event
    void SubscribeWifiCommonEvents();
    void UnsubscribeWifiCommonEvents();
    // timeout event messages
    void SendMsgToEvtHandler(NfcCommonEvent evt, int64_t delay);
    void RemoveMsgFromEvtHandler(NfcCommonEvent evt);
    // step 1: wifi state check
    std::shared_ptr<Wifi::WifiDevice> GetWifiDevPtr();
    bool IsWifiEnabled();
    bool HandleEnableWifi();
    // step 2: wifi connect
    bool IsSameSsid();
    bool HandleConnectWifi();
    // clear function
    void OnFinish();

private:
    std::weak_ptr<NfcService> nfcService_ {};
    std::shared_mutex mutex_ {};
    bool isInitialized_ = false;
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif // NFC_WIFI_CONN_MGR_H