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
#define private public

#include <gtest/gtest.h>
#include <thread>

#include "nci_ce_proxy.h"
#include "nfc_service.h"

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
 * @tc.desc: Test NciCeProxy SetCeHostListener with nciCeInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, SetCeHostListener001, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    std::shared_ptr<NCI::INciCeInterface::ICeHostListener> listener = nullptr;
    nciCeProxy->SetCeHostListener(listener);
    ASSERT_TRUE(nciCeProxy != nullptr);
}

/**
 * @tc.name: SetCeHostListener002
 * @tc.desc: Test NciCeProxy SetCeHostListener with nciCeInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, SetCeHostListener002, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    nciCeProxy->nciCeInterface_ = nullptr;
    std::shared_ptr<NCI::INciCeInterface::ICeHostListener> listener = nullptr;
    nciCeProxy->SetCeHostListener(listener);
    ASSERT_TRUE(nciCeProxy != nullptr);
}

/**
 * @tc.name: SetCeHostListener003
 * @tc.desc: Test NciCeProxy SetCeHostListener with expired listener.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, SetCeHostListener003, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    std::shared_ptr<NfcService> listener = std::make_shared<NfcService>();
    nciCeProxy->SetCeHostListener(listener);
    ASSERT_TRUE(nciCeProxy != nullptr);
}

/**
 * @tc.name: ComputeRoutingParams001
 * @tc.desc: Test NciCeProxy ComputeRoutingParams with nciCeInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, ComputeRoutingParams001, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    int defaultPaymentType = 0;
    bool result = nciCeProxy->ComputeRoutingParams(defaultPaymentType);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: ComputeRoutingParams002
 * @tc.desc: Test NciCeProxy ComputeRoutingParams with nciCeInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, ComputeRoutingParams002, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    nciCeProxy->nciCeInterface_ = nullptr;
    int defaultPaymentType = 0;
    bool result = nciCeProxy->ComputeRoutingParams(defaultPaymentType);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: ComputeRoutingParams003
 * @tc.desc: Test NciCeProxy ComputeRoutingParams with different defaultPaymentType.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, ComputeRoutingParams003, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    for (int i = 0; i < 5; i++) {
        nciCeProxy->ComputeRoutingParams(i);
    }
    ASSERT_TRUE(nciCeProxy != nullptr);
}

/**
 * @tc.name: CommitRouting001
 * @tc.desc: Test NciCeProxy CommitRouting with nciCeInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, CommitRouting001, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    bool result = nciCeProxy->CommitRouting();
    ASSERT_TRUE(!result);
}

/**
 * @tc.name: CommitRouting002
 * @tc.desc: Test NciCeProxy CommitRouting with nciCeInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, CommitRouting002, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    nciCeProxy->nciCeInterface_ = nullptr;
    bool result = nciCeProxy->CommitRouting();
    ASSERT_TRUE(result);
}

/**
 * @tc.name: CommitRouting003
 * @tc.desc: Test NciCeProxy CommitRouting multiple times.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, CommitRouting003, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    for (int i = 0; i < 5; i++) {
        nciCeProxy->CommitRouting();
    }
    ASSERT_TRUE(nciCeProxy != nullptr);
}

/**
 * @tc.name: SendRawFrame001
 * @tc.desc: Test NciCeProxy SendRawFrame with nciCeInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, SendRawFrame001, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    std::string hexCmdData = "ABCDEF";
    bool result = nciCeProxy->SendRawFrame(hexCmdData);
    ASSERT_TRUE(!result);
}

/**
 * @tc.name: SendRawFrame002
 * @tc.desc: Test NciCeProxy SendRawFrame with nciCeInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, SendRawFrame002, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    nciCeProxy->nciCeInterface_ = nullptr;
    std::string hexCmdData = "ABCDEF";
    bool result = nciCeProxy->SendRawFrame(hexCmdData);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: SendRawFrame003
 * @tc.desc: Test NciCeProxy SendRawFrame with empty string.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, SendRawFrame003, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    std::string hexCmdData = "";
    bool result = nciCeProxy->SendRawFrame(hexCmdData);
    ASSERT_TRUE(!result);
}

/**
 * @tc.name: SendRawFrame004
 * @tc.desc: Test NciCeProxy SendRawFrame with different hex strings.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, SendRawFrame004, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    std::vector<std::string> testData = {"", "A", "AB", "ABC", "123456", "FEDCBA"};
    for (const auto& data : testData) {
        std::string hexCmdData = data;
        nciCeProxy->SendRawFrame(hexCmdData);
    }
    ASSERT_TRUE(nciCeProxy != nullptr);
}

/**
 * @tc.name: AddAidRouting001
 * @tc.desc: Test NciCeProxy AddAidRouting with nciCeInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, AddAidRouting001, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    std::string aidStr = "A0000000031010";
    int route = 0;
    int aidInfo = 0;
    int power = 0;
    bool result = nciCeProxy->AddAidRouting(aidStr, route, aidInfo, power);
    ASSERT_TRUE(!result);
}

/**
 * @tc.name: AddAidRouting002
 * @tc.desc: Test NciCeProxy AddAidRouting with nciCeInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, AddAidRouting002, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    nciCeProxy->nciCeInterface_ = nullptr;
    std::string aidStr = "A0000000031010";
    int route = 0;
    int aidInfo = 0;
    int power = 0;
    bool result = nciCeProxy->AddAidRouting(aidStr, route, aidInfo, power);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: AddAidRouting003
 * @tc.desc: Test NciCeProxy AddAidRouting with empty aidStr.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, AddAidRouting003, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    std::string aidStr = "";
    int route = 0;
    int aidInfo = 0;
    int power = 0;
    bool result = nciCeProxy->AddAidRouting(aidStr, route, aidInfo, power);
    ASSERT_TRUE(!result);
}

/**
 * @tc.name: ClearAidTable001
 * @tc.desc: Test NciCeProxy ClearAidTable with nciCeInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, ClearAidTable001, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    bool result = nciCeProxy->ClearAidTable();
    ASSERT_TRUE(!result);
}

/**
 * @tc.name: ClearAidTable002
 * @tc.desc: Test NciCeProxy ClearAidTable with nciCeInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, ClearAidTable002, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    nciCeProxy->nciCeInterface_ = nullptr;
    bool result = nciCeProxy->ClearAidTable();
    ASSERT_FALSE(result);
}

/**
 * @tc.name: ClearAidTable003
 * @tc.desc: Test NciCeProxy ClearAidTable multiple times.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, ClearAidTable003, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    for (int i = 0; i < 5; i++) {
        nciCeProxy->ClearAidTable();
    }
    ASSERT_TRUE(nciCeProxy != nullptr);
}

/**
 * @tc.name: GetSimVendorBundleName001
 * @tc.desc: Test NciCeProxy GetSimVendorBundleName with nciCeInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, GetSimVendorBundleName001, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    std::string result = nciCeProxy->GetSimVendorBundleName();
    ASSERT_TRUE(!result.empty());
}

/**
 * @tc.name: GetSimVendorBundleName002
 * @tc.desc: Test NciCeProxy GetSimVendorBundleName with nciCeInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, GetSimVendorBundleName002, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    nciCeProxy->nciCeInterface_ = nullptr;
    std::string result = nciCeProxy->GetSimVendorBundleName();
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: GetSimVendorBundleName003
 * @tc.desc: Test NciCeProxy GetSimVendorBundleName multiple times.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, GetSimVendorBundleName003, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    for (int i = 0; i < 5; i++) {
        nciCeProxy->GetSimVendorBundleName();
    }
    ASSERT_TRUE(nciCeProxy != nullptr);
}

/**
 * @tc.name: NotifyDefaultPaymentType001
 * @tc.desc: Test NciCeProxy NotifyDefaultPaymentType with nciCeInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, NotifyDefaultPaymentType001, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    int paymentType = 0;
    nciCeProxy->NotifyDefaultPaymentType(paymentType);
    ASSERT_TRUE(nciCeProxy != nullptr);
}

/**
 * @tc.name: NotifyDefaultPaymentType002
 * @tc.desc: Test NciCeProxy NotifyDefaultPaymentType with nciCeInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, NotifyDefaultPaymentType002, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    nciCeProxy->nciCeInterface_ = nullptr;
    int paymentType = 0;
    nciCeProxy->NotifyDefaultPaymentType(paymentType);
    ASSERT_TRUE(nciCeProxy != nullptr);
}

/**
 * @tc.name: NotifyDefaultPaymentType003
 * @tc.desc: Test NciCeProxy NotifyDefaultPaymentType with different payment types.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, NotifyDefaultPaymentType003, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    for (int i = 0; i < 5; i++) {
        nciCeProxy->NotifyDefaultPaymentType(i);
    }
    ASSERT_TRUE(nciCeProxy != nullptr);
}

/**
 * @tc.name: MixedOperations001
 * @tc.desc: Test NciCeProxy with mixed operations.
 * @tc.type: FUNC
 */
HWTEST_F(NciCeProxyTest, MixedOperations001, TestSize.Level1)
{
    std::shared_ptr<NciCeProxy> nciCeProxy = std::make_shared<NciCeProxy>();
    std::string hexCmdData = "ABCDEF";
    std::string aidStr = "A0000000031010";

    nciCeProxy->ComputeRoutingParams(0);
    nciCeProxy->CommitRouting();
    nciCeProxy->SendRawFrame(hexCmdData);
    nciCeProxy->AddAidRouting(aidStr, 0, 0, 0);
    nciCeProxy->ClearAidTable();
    nciCeProxy->GetSimVendorBundleName();
    nciCeProxy->NotifyDefaultPaymentType(0);
    ASSERT_TRUE(nciCeProxy != nullptr);
}
}
}
}