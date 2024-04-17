/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "wificonnectionmanager_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "wifi_connection_manager.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;
    using namespace OHOS::NFC::TAG;

    constexpr const auto FUZZER_THRESHOLD = 4;
    constexpr const auto INT_TO_BOOL_DIVISOR = 2;

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    void FuzzTryConnectWifi(const uint8_t* data, size_t size)
    {
        std::shared_ptr<WifiData> wifiData = std::make_shared<WifiData>();
        wifiData->isValid_ = data[0] % INT_TO_BOOL_DIVISOR;
        wifiData->config_ = new Wifi::WifiDeviceConfig();
        wifiData->config_->ssid = NfcSdkCommon::BytesVecToHexString(data, size);
        wifiData->config_->preSharedKey = NfcSdkCommon::BytesVecToHexString(data, size);
        wifiData->config_->keyMgmt = Wifi::KEY_MGMT_WPA_PSK;
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        WifiConnectionManager::GetInstance().Initialize(service);
        WifiConnectionManager::GetInstance().TryConnectWifi(wifiData);
    }

    void FuzzOnWifiNtfClicked(const uint8_t* data, size_t size)
    {
        std::shared_ptr<WifiData> wifiData = std::make_shared<WifiData>();
        wifiData->isValid_ = data[0] % INT_TO_BOOL_DIVISOR;
        wifiData->config_ = new Wifi::WifiDeviceConfig();
        wifiData->config_->ssid = NfcSdkCommon::BytesVecToHexString(data, size);
        wifiData->config_->preSharedKey = NfcSdkCommon::BytesVecToHexString(data, size);
        wifiData->config_->keyMgmt = Wifi::KEY_MGMT_WPA_PSK;
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        WifiConnectionManager::GetInstance().Initialize(service);
        WifiConnectionManager::GetInstance().TryConnectWifi(wifiData);
        WifiConnectionManager::GetInstance().OnWifiNtfClicked();
    }

    void FuzzHandleWifiEnableFailed(const uint8_t* data, size_t size)
    {
        std::shared_ptr<WifiData> wifiData = std::make_shared<WifiData>();
        wifiData->isValid_ = data[0] % INT_TO_BOOL_DIVISOR;
        wifiData->config_ = new Wifi::WifiDeviceConfig();
        wifiData->config_->ssid = NfcSdkCommon::BytesVecToHexString(data, size);
        wifiData->config_->preSharedKey = NfcSdkCommon::BytesVecToHexString(data, size);
        wifiData->config_->keyMgmt = Wifi::KEY_MGMT_WPA_PSK;
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        WifiConnectionManager::GetInstance().Initialize(service);
        WifiConnectionManager::GetInstance().TryConnectWifi(wifiData);
        WifiConnectionManager::GetInstance().HandleWifiEnableFailed();
    }

    void FuzzHandleWifiConnectFailed(const uint8_t* data, size_t size)
    {
        std::shared_ptr<WifiData> wifiData = std::make_shared<WifiData>();
        wifiData->isValid_ = data[0] % INT_TO_BOOL_DIVISOR;
        wifiData->config_ = new Wifi::WifiDeviceConfig();
        wifiData->config_->ssid = NfcSdkCommon::BytesVecToHexString(data, size);
        wifiData->config_->preSharedKey = NfcSdkCommon::BytesVecToHexString(data, size);
        wifiData->config_->keyMgmt = Wifi::KEY_MGMT_WPA_PSK;
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        WifiConnectionManager::GetInstance().Initialize(service);
        WifiConnectionManager::GetInstance().TryConnectWifi(wifiData);
        WifiConnectionManager::GetInstance().HandleWifiConnectFailed();
    }

    void FuzzOnWifiEnabled(const uint8_t* data, size_t size)
    {
        std::shared_ptr<WifiData> wifiData = std::make_shared<WifiData>();
        wifiData->isValid_ = data[0] % INT_TO_BOOL_DIVISOR;
        wifiData->config_ = new Wifi::WifiDeviceConfig();
        wifiData->config_->ssid = NfcSdkCommon::BytesVecToHexString(data, size);
        wifiData->config_->preSharedKey = NfcSdkCommon::BytesVecToHexString(data, size);
        wifiData->config_->keyMgmt = Wifi::KEY_MGMT_WPA_PSK;
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        WifiConnectionManager::GetInstance().Initialize(service);
        WifiConnectionManager::GetInstance().TryConnectWifi(wifiData);
        WifiConnectionManager::GetInstance().OnWifiEnabled();
    }

    void FuzzOnWifiConnected(const uint8_t* data, size_t size)
    {
        std::shared_ptr<WifiData> wifiData = std::make_shared<WifiData>();
        wifiData->isValid_ = data[0] % INT_TO_BOOL_DIVISOR;
        wifiData->config_ = new Wifi::WifiDeviceConfig();
        wifiData->config_->ssid = NfcSdkCommon::BytesVecToHexString(data, size);
        wifiData->config_->preSharedKey = NfcSdkCommon::BytesVecToHexString(data, size);
        wifiData->config_->keyMgmt = Wifi::KEY_MGMT_WPA_PSK;
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        WifiConnectionManager::GetInstance().Initialize(service);
        WifiConnectionManager::GetInstance().TryConnectWifi(wifiData);
        WifiConnectionManager::GetInstance().OnWifiConnected();
    }

}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzTryConnectWifi(data, size);
    OHOS::FuzzOnWifiNtfClicked(data, size);
    OHOS::FuzzHandleWifiEnableFailed(data, size);
    OHOS::FuzzHandleWifiConnectFailed(data, size);
    OHOS::FuzzOnWifiEnabled(data, size);
    OHOS::FuzzOnWifiConnected(data, size);
    return 0;
}

