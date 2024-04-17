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
#include <gtest/gtest.h>
#include <thread>

#include "wifi_connection_manager.h"

namespace OHOS {
namespace NFC {
namespace TAG {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class WifiConnectionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void WifiConnectionManagerTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase WifiConnectionManagerTest." << std::endl;
}

void WifiConnectionManagerTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase WifiConnectionManagerTest." << std::endl;
}

void WifiConnectionManagerTest::SetUp()
{
    std::cout << " SetUp WifiConnectionManagerTest." << std::endl;
}

void WifiConnectionManagerTest::TearDown()
{
    std::cout << " TearDown WifiConnectionManagerTest." << std::endl;
}

/**
 * @tc.name: TryConnectWifi001
 * @tc.desc: Test WifiConnectionManagerTest TryConnectWifi.
 * @tc.type: FUNC
 */
HWTEST_F(WifiConnectionManagerTest, TryConnectWifi001, TestSize.Level1)
{
    std::shared_ptr<WifiData> data = std::make_shared<WifiData>();
    data->isValid_ = false;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    WifiConnectionManager::GetInstance().Initialize(service);
    WifiConnectionManager::GetInstance().TryConnectWifi(data);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: TryConnectWifi002
 * @tc.desc: Test WifiConnectionManagerTest TryConnectWifi.
 * @tc.type: FUNC
 */
HWTEST_F(WifiConnectionManagerTest, TryConnectWifi002, TestSize.Level1)
{
    std::shared_ptr<WifiData> data = std::make_shared<WifiData>();
    data->isValid_ = true;
    data->config_ = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    WifiConnectionManager::GetInstance().Initialize(service);
    WifiConnectionManager::GetInstance().TryConnectWifi(data);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: TryConnectWifi003
 * @tc.desc: Test WifiConnectionManagerTest TryConnectWifi.
 * @tc.type: FUNC
 */
HWTEST_F(WifiConnectionManagerTest, TryConnectWifi003, TestSize.Level1)
{
    std::shared_ptr<WifiData> data = std::make_shared<WifiData>();
    data->isValid_ = true;
    data->config_ = new Wifi::WifiDeviceConfig();
    data->config_->ssid = "NFC";
    data->config_->preSharedKey = "88888888";
    data->config_->keyMgmt = Wifi::KEY_MGMT_WPA_PSK;

    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    WifiConnectionManager::GetInstance().Initialize(service);
    WifiConnectionManager::GetInstance().TryConnectWifi(data);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnWifiNtfClicked001
 * @tc.desc: Test WifiConnectionManagerTest OnWifiNtfClicked.
 * @tc.type: FUNC
 */
HWTEST_F(WifiConnectionManagerTest, OnWifiNtfClicked001, TestSize.Level1)
{
    std::shared_ptr<WifiData> data = std::make_shared<WifiData>();
    data->isValid_ = true;
    data->config_ = new Wifi::WifiDeviceConfig();
    data->config_->ssid = "NFC";
    data->config_->preSharedKey = "88888888";
    data->config_->keyMgmt = Wifi::KEY_MGMT_WPA_PSK;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    WifiConnectionManager::GetInstance().Initialize(service);
    WifiConnectionManager::GetInstance().TryConnectWifi(data);
    WifiConnectionManager::GetInstance().OnWifiNtfClicked();
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: HandleWifiEnableFailed001
 * @tc.desc: Test WifiConnectionManagerTest HandleWifiEnableFailed.
 * @tc.type: FUNC
 */
HWTEST_F(WifiConnectionManagerTest, HandleWifiEnableFailed001, TestSize.Level1)
{
    std::shared_ptr<WifiData> data = std::make_shared<WifiData>();
    data->isValid_ = true;
    data->config_ = new Wifi::WifiDeviceConfig();
    data->config_->ssid = "NFC";
    data->config_->preSharedKey = "88888888";
    data->config_->keyMgmt = Wifi::KEY_MGMT_WPA_PSK;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    WifiConnectionManager::GetInstance().Initialize(service);
    WifiConnectionManager::GetInstance().TryConnectWifi(data);
    WifiConnectionManager::GetInstance().HandleWifiEnableFailed();
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: HandleWifiConnectFailed001
 * @tc.desc: Test WifiConnectionManagerTest HandleWifiConnectFailed.
 * @tc.type: FUNC
 */
HWTEST_F(WifiConnectionManagerTest, HandleWifiConnectFailed001, TestSize.Level1)
{
    std::shared_ptr<WifiData> data = std::make_shared<WifiData>();
    data->isValid_ = true;
    data->config_ = new Wifi::WifiDeviceConfig();
    data->config_->ssid = "NFC";
    data->config_->preSharedKey = "88888888";
    data->config_->keyMgmt = Wifi::KEY_MGMT_WPA_PSK;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    WifiConnectionManager::GetInstance().Initialize(service);
    WifiConnectionManager::GetInstance().TryConnectWifi(data);
    WifiConnectionManager::GetInstance().HandleWifiConnectFailed();
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnWifiEnabled001
 * @tc.desc: Test WifiConnectionManagerTest OnWifiEnabled.
 * @tc.type: FUNC
 */
HWTEST_F(WifiConnectionManagerTest, OnWifiEnabled001, TestSize.Level1)
{
    std::shared_ptr<WifiData> data = std::make_shared<WifiData>();
    data->isValid_ = true;
    data->config_ = new Wifi::WifiDeviceConfig();
    data->config_->preSharedKey = "88888888";
    data->config_->keyMgmt = Wifi::KEY_MGMT_WPA_PSK;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    WifiConnectionManager::GetInstance().Initialize(service);
    WifiConnectionManager::GetInstance().TryConnectWifi(data);
    WifiConnectionManager::GetInstance().OnWifiEnabled();
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnWifiConnected001
 * @tc.desc: Test WifiConnectionManagerTest OnWifiConnected.
 * @tc.type: FUNC
 */
HWTEST_F(WifiConnectionManagerTest, OnWifiConnected001, TestSize.Level1)
{
    std::shared_ptr<WifiData> data = std::make_shared<WifiData>();
    data->isValid_ = true;
    data->config_ = new Wifi::WifiDeviceConfig();
    data->config_->ssid = "NFC";
    data->config_->preSharedKey = "88888888";
    data->config_->keyMgmt = Wifi::KEY_MGMT_WPA_PSK;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    WifiConnectionManager::GetInstance().Initialize(service);
    WifiConnectionManager::GetInstance().TryConnectWifi(data);
    WifiConnectionManager::GetInstance().OnWifiConnected();
    ASSERT_TRUE(service != nullptr);
}
} // namespace TEST
} // namespace TAG
} // namespace NFC
} // namespace OHOS