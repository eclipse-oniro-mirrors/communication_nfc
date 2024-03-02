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

#include "nci_ce_proxy.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::NCI;
class NciCeProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NciCeProxyTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NciCeProxyTest." << std::endl;
}

void NciCeProxyTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NciCeProxyTest." << std::endl;
}

void NciCeProxyTest::SetUp()
{
    std::cout << " SetUp NciCeProxyTest." << std::endl;
}

void NciCeProxyTest::TearDown()
{
    std::cout << " TearDown NciCeProxyTest." << std::endl;
}

/**
 * @tc.name: SetCeHostListener001
 * @tc.desc: Test NciCeProxyTest SetCeHostListener.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, SetCeHostListener001, TestSize.Level1)
{
    std::shared_ptr<NCI::INciCeInterface::ICeHostListener> listener = nullptr;
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    nciCeProxy->SetCeHostListener(listener);
    bool computeRoutingParams = nciCeProxy->ComputeRoutingParams(static_cast<int>(KITS::DefaultPaymentType::TYPE_ESE));
    ASSERT_TRUE(computeRoutingParams == true);
}

/**
 * @tc.name: ComputeRoutingParams001
 * @tc.desc: Test NciCeProxyTest ComputeRoutingParams.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, ComputeRoutingParams001, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    bool computeRoutingParams = nciCeProxy->ComputeRoutingParams(static_cast<int>(KITS::DefaultPaymentType::TYPE_ESE));
    ASSERT_TRUE(computeRoutingParams == true);
}

/**
 * @tc.name: CommitRouting001
 * @tc.desc: Test NciCeProxyTest CommitRouting.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, CommitRouting001, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    bool commitRouting = nciCeProxy->CommitRouting();
    ASSERT_TRUE(commitRouting == false);
}

/**
 * @tc.name: SendRawFrame001
 * @tc.desc: Test NciCeProxyTest SendRawFrame.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, SendRawFrame001, TestSize.Level1)
{
    std::string hexCmdData = "";
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    bool sendRawFrame = nciCeProxy->SendRawFrame(hexCmdData);
    ASSERT_TRUE(sendRawFrame == false);
}

/**
 * @tc.name: AddAidRouting001
 * @tc.desc: Test NciCeProxyTest AddAidRouting.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, AddAidRouting001, TestSize.Level1)
{
    std::string aidStr = "";
    int route = 0;
    int aidInfo = 0;
    int power = 0;
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    bool addAidRouting = nciCeProxy->AddAidRouting(aidStr, route, aidInfo, power);
    ASSERT_TRUE(addAidRouting == false);
}
}
}
}