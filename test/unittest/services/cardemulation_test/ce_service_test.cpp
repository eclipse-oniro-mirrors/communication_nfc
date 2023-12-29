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

#include "ce_service.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class CeServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CeServiceTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase CeServiceTest." << std::endl;
}

void CeServiceTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase CeServiceTest." << std::endl;
}

void CeServiceTest::SetUp()
{
    std::cout << " SetUp CeServiceTest." << std::endl;
}

void CeServiceTest::TearDown()
{
    std::cout << " TearDown CeServiceTest." << std::endl;
}

/**
 * @tc.name: RegHceCmdCallback001
 * @tc.desc: Test CeServiceTest RegHceCmdCallback.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, RegHceCmdCallback001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: SendHostApduData001
 * @tc.desc: Test CeServiceTest SendHostApduData.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, SendHostApduData001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::string hexCmdData = "";
    bool raw = false;
    std::string hexRespData = "";
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool sendHostApduData = ceService->SendHostApduData(hexCmdData, raw, hexRespData, callerToken);
    ASSERT_TRUE(sendHostApduData == false);
}

/**
 * @tc.name: HandleFieldDeactivated001
 * @tc.desc: Test CeServiceTest HandleFieldDeactivated.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, HandleFieldDeactivated001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->HandleFieldDeactivated();
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnCardEmulationData001
 * @tc.desc: Test CeServiceTest OnCardEmulationData.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnCardEmulationData001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::vector<uint8_t> data;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->OnCardEmulationData(data);
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnCardEmulationActivated001
 * @tc.desc: Test CeServiceTest OnCardEmulationActivated.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnCardEmulationActivated001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->OnCardEmulationActivated();
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnCardEmulationDeactivated001
 * @tc.desc: Test CeServiceTest OnCardEmulationDeactivated.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnCardEmulationDeactivated001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->OnCardEmulationDeactivated();
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}
}
}
}