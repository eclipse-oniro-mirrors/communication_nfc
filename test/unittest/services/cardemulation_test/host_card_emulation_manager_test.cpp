/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "host_card_emulation_manager.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class HostCardEmulationManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HostCardEmulationManagerTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase HostCardEmulationManagerTest." << std::endl;
}

void HostCardEmulationManagerTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase HostCardEmulationManagerTest." << std::endl;
}

void HostCardEmulationManagerTest::SetUp()
{
    std::cout << " SetUp HostCardEmulationManagerTest." << std::endl;
}

void HostCardEmulationManagerTest::TearDown()
{
    std::cout << " TearDown HostCardEmulationManagerTest." << std::endl;
}

/**
 * @tc.name: OnHostCardEmulationDataNfcA001
 * @tc.desc: Test HostCardEmulationManagerTest OnHostCardEmulationDataNfcA.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, OnHostCardEmulationDataNfcA001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::vector<uint8_t> data;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy);
    hostCardEmulationManager->OnHostCardEmulationDataNfcA(data);
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = hostCardEmulationManager->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnHostCardEmulationDataNfcA002
 * @tc.desc: Test HostCardEmulationManagerTest OnHostCardEmulationDataNfcA.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, OnHostCardEmulationDataNfcA002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::vector<uint8_t> data = {1, 2, 3, 4, 5, 6};
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy);
    hostCardEmulationManager->OnHostCardEmulationDataNfcA(data);
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = hostCardEmulationManager->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnCardEmulationActivated001
 * @tc.desc: Test HostCardEmulationManagerTest OnCardEmulationActivated.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, OnCardEmulationActivated001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy);
    hostCardEmulationManager->OnCardEmulationActivated();
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = hostCardEmulationManager->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnCardEmulationDeactivated001
 * @tc.desc: Test HostCardEmulationManagerTest OnCardEmulationDeactivated.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, OnCardEmulationDeactivated001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy);
    hostCardEmulationManager->OnCardEmulationDeactivated();
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = hostCardEmulationManager->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: SendHostApduData001
 * @tc.desc: Test HostCardEmulationManagerTest SendHostApduData.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, SendHostApduData001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::string hexCmdData = "";
    bool raw = false;
    std::string hexRespData = "";
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy);
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool sendHostApduData = hostCardEmulationManager->SendHostApduData(hexCmdData, raw, hexRespData, callerToken);
    ASSERT_TRUE(sendHostApduData == false);
}
}
}
}