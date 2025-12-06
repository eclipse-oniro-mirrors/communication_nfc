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
#define private public
#define protected public

#include <gtest/gtest.h>
#include <thread>

#include "bt_connection_manager.h"

namespace OHOS {
namespace NFC {
namespace TAG {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class BtConnectionManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void BtConnectionManagerTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase BtConnectionManagerTest." << std::endl;
}

void BtConnectionManagerTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase BtConnectionManagerTest." << std::endl;
}

void BtConnectionManagerTest::SetUp()
{
    std::cout << " SetUp BtConnectionManagerTest." << std::endl;
}

void BtConnectionManagerTest::TearDown()
{
    std::cout << " TearDown BtConnectionManagerTest." << std::endl;
}

/**
 * @tc.name: TryPairBt001
 * @tc.desc: Test BtConnectionManagerTest TryPairBt.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, TryPairBt001, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = false;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: TryPairBt002
 * @tc.desc: Test BtConnectionManagerTest TryPairBt.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, TryPairBt002, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: TryPairBt003
 * @tc.desc: Test BtConnectionManagerTest TryPairBt.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, TryPairBt003, TestSize.Level1)
{
    std::shared_ptr<BtData> data = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnBtNtfClicked001
 * @tc.desc: Test BtConnectionManagerTest OnBtNtfClicked.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnBtNtfClicked001, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);
    BtConnectionManager::GetInstance().OnBtNtfClicked();
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: HandleBtEnableFailed001
 * @tc.desc: Test BtConnectionManagerTest HandleBtEnableFailed.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, HandleBtEnableFailed001, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);
    BtConnectionManager::GetInstance().HandleBtEnableFailed();
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: HandleBtPairFailed001
 * @tc.desc: Test BtConnectionManagerTest HandleBtPairFailed.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, HandleBtPairFailed001, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);
    BtConnectionManager::GetInstance().HandleBtPairFailed();
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: HandleBtConnectFailed001
 * @tc.desc: Test BtConnectionManagerTest HandleBtConnectFailed.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, HandleBtConnectFailed001, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);
    BtConnectionManager::GetInstance().HandleBtConnectFailed();
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnBtEnabled001
 * @tc.desc: Test BtConnectionManagerTest OnBtEnabled.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnBtEnabled001, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);
    BtConnectionManager::GetInstance().OnBtEnabled();
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnPairStatusChanged001
 * @tc.desc: Test BtConnectionManagerTest OnPairStatusChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnPairStatusChanged001, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);

    std::shared_ptr<BtConnectionInfo> info = nullptr;
    BtConnectionManager::GetInstance().OnPairStatusChanged(info);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnPairStatusChanged002
 * @tc.desc: Test BtConnectionManagerTest OnPairStatusChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnPairStatusChanged002, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);

    std::shared_ptr<BtConnectionInfo> info = std::make_shared<BtConnectionInfo>();
    info->macAddr_ = "AA:BB:CC:DD:EE:FF";
    info->state_ = Bluetooth::PAIR_NONE;
    BtConnectionManager::GetInstance().OnPairStatusChanged(info);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnPairStatusChanged003
 * @tc.desc: Test BtConnectionManagerTest OnPairStatusChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnPairStatusChanged003, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);

    std::shared_ptr<BtConnectionInfo> info = std::make_shared<BtConnectionInfo>();
    info->macAddr_ = "AA:BB:CC:DD:EE:FF";
    info->state_ = Bluetooth::PAIR_PAIRED;
    BtConnectionManager::GetInstance().OnPairStatusChanged(info);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnPairStatusChanged004
 * @tc.desc: Test BtConnectionManagerTest OnPairStatusChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnPairStatusChanged004, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);

    std::shared_ptr<BtConnectionInfo> info = std::make_shared<BtConnectionInfo>();
    info->macAddr_ = "11:22:33:44:55:66";
    info->state_ = Bluetooth::PAIR_PAIRED;
    BtConnectionManager::GetInstance().OnPairStatusChanged(info);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnConnectionStateChanged001
 * @tc.desc: Test BtConnectionManagerTest OnConnectionStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnConnectionStateChanged001, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);

    std::shared_ptr<BtConnectionInfo> info = nullptr;
    BtConnectionManager::GetInstance().OnConnectionStateChanged(info);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnConnectionStateChanged002
 * @tc.desc: Test BtConnectionManagerTest OnConnectionStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnConnectionStateChanged002, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);

    std::shared_ptr<BtConnectionInfo> info = std::make_shared<BtConnectionInfo>();
    info->macAddr_ = "AA:BB:CC:DD:EE:FF";
    info->state_ = static_cast<int32_t>(Bluetooth::BTConnectState::CONNECTED);
    info->type_ = static_cast<uint8_t>(BtConnectionManager::BtProfileType::HFP_AG);
    BtConnectionManager::GetInstance().OnConnectionStateChanged(info);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnConnectionStateChanged003
 * @tc.desc: Test BtConnectionManagerTest OnConnectionStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnConnectionStateChanged003, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);

    std::shared_ptr<BtConnectionInfo> info = std::make_shared<BtConnectionInfo>();
    info->macAddr_ = "AA:BB:CC:DD:EE:FF";
    info->state_ = static_cast<int32_t>(Bluetooth::BTConnectState::DISCONNECTED);
    info->type_ = static_cast<uint8_t>(BtConnectionManager::BtProfileType::HFP_AG);
    BtConnectionManager::GetInstance().OnConnectionStateChanged(info);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnConnectionStateChanged004
 * @tc.desc: Test BtConnectionManagerTest OnConnectionStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnConnectionStateChanged004, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);

    std::shared_ptr<BtConnectionInfo> info = std::make_shared<BtConnectionInfo>();
    info->macAddr_ = "11:22:33:44:55:66";
    info->state_ = static_cast<int32_t>(Bluetooth::BTConnectState::DISCONNECTED);
    info->type_ = static_cast<uint8_t>(BtConnectionManager::BtProfileType::HFP_AG);
    BtConnectionManager::GetInstance().OnConnectionStateChanged(info);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnConnectionStateChanged005
 * @tc.desc: Test BtConnectionManagerTest OnConnectionStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnConnectionStateChanged005, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);

    std::shared_ptr<BtConnectionInfo> info = std::make_shared<BtConnectionInfo>();
    info->macAddr_ = "AA:BB:CC:DD:EE:FF";
    info->state_ = static_cast<int32_t>(Bluetooth::BTConnectState::CONNECTED);
    info->type_ = static_cast<uint8_t>(BtConnectionManager::BtProfileType::A2DP_SRC);
    BtConnectionManager::GetInstance().OnConnectionStateChanged(info);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnConnectionStateChanged006
 * @tc.desc: Test BtConnectionManagerTest OnConnectionStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnConnectionStateChanged006, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);

    std::shared_ptr<BtConnectionInfo> info = std::make_shared<BtConnectionInfo>();
    info->macAddr_ = "AA:BB:CC:DD:EE:FF";
    info->state_ = static_cast<int32_t>(Bluetooth::BTConnectState::CONNECTED);
    info->type_ = static_cast<uint8_t>(BtConnectionManager::BtProfileType::HID_HOST);
    BtConnectionManager::GetInstance().OnConnectionStateChanged(info);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnConnectionStateChanged007
 * @tc.desc: Test BtConnectionManagerTest OnConnectionStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnConnectionStateChanged007, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);

    std::shared_ptr<BtConnectionInfo> info = std::make_shared<BtConnectionInfo>();
    info->macAddr_ = "AA:BB:CC:DD:EE:FF";
    info->state_ = static_cast<int32_t>(Bluetooth::BTConnectState::DISCONNECTED);
    info->type_ = static_cast<uint8_t>(BtConnectionManager::BtProfileType::A2DP_SRC);
    BtConnectionManager::GetInstance().OnConnectionStateChanged(info);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnConnectionStateChanged008
 * @tc.desc: Test BtConnectionManagerTest OnConnectionStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnConnectionStateChanged008, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = true;
    data->name_ = "NFC";
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    data->macAddress_ = "AA:BB:CC:DD:EE:FF";
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);

    std::shared_ptr<BtConnectionInfo> info = std::make_shared<BtConnectionInfo>();
    info->macAddr_ = "AA:BB:CC:DD:EE:FF";
    info->state_ = static_cast<int32_t>(Bluetooth::BTConnectState::DISCONNECTED);
    info->type_ = static_cast<uint8_t>(BtConnectionManager::BtProfileType::HID_HOST);
    BtConnectionManager::GetInstance().OnConnectionStateChanged(info);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: NextActionInit
 * @tc.desc: Test BtConnectionManagerTest NextActionInit.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, NextActionInit, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = false;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);

    BtConnectionManager::GetInstance().NextActionInit();
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: PublishPairBtNtf
 * @tc.desc: Test BtConnectionManagerTest PublishPairBtNtf.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, PublishPairBtNtf, TestSize.Level1)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = false;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().TryPairBt(data);

    BtConnectionManager::GetInstance().PublishPairBtNtf();
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: OnStateChanged001
 * @tc.desc: Test BtConnectionManagerTest OnStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnStateChanged001, TestSize.Level1)
{
    auto instance = BtConnectionManager::BtStateObserver::GetInstance();
    instance->OnStateChanged(0, 1);
    ASSERT_TRUE(instance != nullptr);
}

/**
 * @tc.name: OnStateChanged002
 * @tc.desc: Test BtConnectionManagerTest OnStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnStateChanged002, TestSize.Level1)
{
    auto instance = BtConnectionManager::BtStateObserver::GetInstance();
    instance->OnStateChanged(0, 0);
    ASSERT_TRUE(instance != nullptr);
}

/**
 * @tc.name: OnStateChanged003
 * @tc.desc: Test BtConnectionManagerTest OnStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnStateChanged003, TestSize.Level1)
{
    auto instance = BtConnectionManager::BtStateObserver::GetInstance();
    instance->OnStateChanged(1, 1);
    ASSERT_TRUE(instance != nullptr);
}

/**
 * @tc.name: OnStateChanged004
 * @tc.desc: Test BtConnectionManagerTest OnStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, OnStateChanged004, TestSize.Level1)
{
    auto instance = BtConnectionManager::BtStateObserver::GetInstance();
    instance->OnStateChanged(1, 0);
    ASSERT_TRUE(instance != nullptr);
}

/**
 * @tc.name: SendMsgToEvtHandler001
 * @tc.desc: Test BtConnectionManagerTest SendMsgToEvtHandler.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, SendMsgToEvtHandler001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    NfcCommonEvent evt = NfcCommonEvent::MSG_TAG_FOUND;
    int64_t delay = 0;
    BtConnectionManager::GetInstance().SendMsgToEvtHandler(evt, delay);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: SendMsgToEvtHandler002
 * @tc.desc: Test BtConnectionManagerTest SendMsgToEvtHandler.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, SendMsgToEvtHandler002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    BtConnectionManager::GetInstance().Initialize(service);
    NfcCommonEvent evt = NfcCommonEvent::MSG_TAG_FOUND;
    int64_t delay = 0;
    BtConnectionManager::GetInstance().SendMsgToEvtHandler(evt, delay);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: SendConnMsgToEvtHandler001
 * @tc.desc: Test BtConnectionManagerTest SendConnMsgToEvtHandler.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, SendConnMsgToEvtHandler001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    NfcCommonEvent evt = NfcCommonEvent::MSG_TAG_FOUND;
    Bluetooth::BluetoothRemoteDevice device;
    int32_t state = 1;
    BtConnectionManager::BtProfileType type = BtConnectionManager::BtProfileType::A2DP_SRC;
    BtConnectionManager::GetInstance().SendConnMsgToEvtHandler(evt, device, state, type);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: SendConnMsgToEvtHandler002
 * @tc.desc: Test BtConnectionManagerTest SendConnMsgToEvtHandler.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, SendConnMsgToEvtHandler002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    BtConnectionManager::GetInstance().Initialize(service);
    NfcCommonEvent evt = NfcCommonEvent::MSG_TAG_FOUND;
    Bluetooth::BluetoothRemoteDevice device;
    int32_t state = 1;
    BtConnectionManager::BtProfileType type = BtConnectionManager::BtProfileType::A2DP_SRC;
    BtConnectionManager::GetInstance().SendConnMsgToEvtHandler(evt, device, state, type);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: RemoveMsgFromEvtHandler001
 * @tc.desc: Test BtConnectionManagerTest RemoveMsgFromEvtHandler.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, RemoveMsgFromEvtHandler001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    NfcCommonEvent evt = NfcCommonEvent::MSG_TAG_FOUND;
    BtConnectionManager::GetInstance().RemoveMsgFromEvtHandler(evt);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: HandleEnableBt001
 * @tc.desc: Test BtConnectionManagerTest HandleEnableBt.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, RemoveMsgFromEvtHandler001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().HandleEnableBt();
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: IsA2dpSupported001
 * @tc.desc: Test BtConnectionManagerTest IsA2dpSupported.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, IsA2dpSupported001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().IsA2dpSupported();
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: IsHfpSuported001
 * @tc.desc: Test BtConnectionManagerTest IsHfpSuported.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, IsHfpSuported001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().IsHfpSuported();
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: HandleBtInit001
 * @tc.desc: Test BtConnectionManagerTest HandleBtInit.
 * @tc.type: FUNC
 */
HWTEST_F(BtConnectionManagerTest, HandleBtInit001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    BtConnectionManager::GetInstance().Initialize(service);
    BtConnectionManager::GetInstance().HandleBtInit();
    ASSERT_TRUE(service != nullptr);
}
} // namespace TEST
} // namespace TAG
} // namespace NFC
} // namespace OHOS