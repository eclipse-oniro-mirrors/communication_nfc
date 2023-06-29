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

#include "nfc_polling_params.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class NfcPollingParamsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TECH_MASK = 4;
};

void NfcPollingParamsTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcPollingParamsTest." << std::endl;
}

void NfcPollingParamsTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcPollingParamsTest." << std::endl;
}

void NfcPollingParamsTest::SetUp()
{
    std::cout << " SetUp NfcPollingParamsTest." << std::endl;
}

void NfcPollingParamsTest::TearDown()
{
    std::cout << " TearDown NfcPollingParamsTest." << std::endl;
}

/**
 * @tc.name: GetTechMask001
 * @tc.desc: Test NfcPollingParams GetTechMask.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingParamsTest, GetTechMask001, TestSize.Level1)
{
    NfcPollingParams nfcPollingParams;
    int getTechMask = nfcPollingParams.GetTechMask();
    ASSERT_TRUE(getTechMask == 0);
}
/**
 * @tc.name: ShouldEnablePolling001
 * @tc.desc: Test NfcPollingParams ShouldEnablePolling.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingParamsTest, ShouldEnablePolling001, TestSize.Level1)
{
    NfcPollingParams nfcPollingParams;
    bool shouldEnablePolling = nfcPollingParams.ShouldEnablePolling();
    ASSERT_TRUE(shouldEnablePolling == false);
}
/**
 * @tc.name: ShouldEnableLowPowerPolling001
 * @tc.desc: Test NfcPollingParams ShouldEnableLowPowerPolling.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingParamsTest, ShouldEnableLowPowerPolling001, TestSize.Level1)
{
    NfcPollingParams nfcPollingParams;
    bool shouldEnableLowPowerPolling = nfcPollingParams.ShouldEnableLowPowerPolling();
    ASSERT_TRUE(shouldEnableLowPowerPolling == true);
}
/**
 * @tc.name: ShouldEnableReaderMode001
 * @tc.desc: Test NfcPollingParams ShouldEnableReaderMode.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingParamsTest, ShouldEnableReaderMode001, TestSize.Level1)
{
    NfcPollingParams nfcPollingParams;
    bool shouldEnableReaderMode = nfcPollingParams.ShouldEnableReaderMode();
    ASSERT_TRUE(shouldEnableReaderMode == false);
}
/**
 * @tc.name: ShouldEnableHostRouting001
 * @tc.desc: Test NfcPollingParams ShouldEnableHostRouting.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingParamsTest, ShouldEnableHostRouting001, TestSize.Level1)
{
    NfcPollingParams nfcPollingParams;
    bool shouldEnableHostRouting = nfcPollingParams.ShouldEnableHostRouting();
    ASSERT_TRUE(shouldEnableHostRouting == false);
}
/**
 * @tc.name: SetTechMask001
 * @tc.desc: Test NfcPollingParams SetTechMask.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingParamsTest, SetTechMask001, TestSize.Level1)
{
    int techMask = TECH_MASK;
    NfcPollingParams nfcPollingParams;
    nfcPollingParams.SetTechMask(techMask);
    ASSERT_TRUE(techMask == TECH_MASK);
}
/**
 * @tc.name: ToString001
 * @tc.desc: Test NfcPollingParams ToString.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingParamsTest, ToString001, TestSize.Level1)
{
    NfcPollingParams nfcPollingParams;
    std::string toString = nfcPollingParams.ToString();
    ASSERT_TRUE(toString != "");
}
}
}
}