/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nfc_controller.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class NfcControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NfcControllerTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcControllerTest." << std::endl;
}

void NfcControllerTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcControllerTest." << std::endl;
}

void NfcControllerTest::SetUp() {}

void NfcControllerTest::TearDown() {}

/**
 * @tc.name: GetNfcState001
 * @tc.desc: Test NfcController GetNfcState.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerTest, GetNfcState001, TestSize.Level1)
{
    NfcController ctrl = NfcController::GetInstance();
    int state = ctrl.GetNfcState();
    ASSERT_TRUE(state == NfcState::STATE_OFF ||
        state == NfcState::STATE_ON ||
        state == NfcState::STATE_TURNING_ON ||
        state == NfcState::STATE_TURNING_OFF);
}

/**
 * @tc.name: TurnOn001
 * @tc.desc: Test NfcController TurnOn.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerTest, TurnOn001, TestSize.Level1)
{
    NfcController ctrl = NfcController::GetInstance();
    ASSERT_TRUE(ctrl.TurnOn() == 0);

    // wait for turn on finished.
    std::this_thread::sleep_for(std::chrono::seconds(3));
    int state = ctrl.GetNfcState();
    ASSERT_TRUE(state == NfcState::STATE_ON);
}

/**
 * @tc.name: TurnOff001
 * @tc.desc: Test NfcController TurnOff.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerTest, TurnOff001, TestSize.Level1)
{
    NfcController ctrl = NfcController::GetInstance();
    ASSERT_TRUE(ctrl.TurnOff() == 0);

    // wait for turn off finished.
    std::this_thread::sleep_for(std::chrono::seconds(3));
    int state = ctrl.GetNfcState();
    ASSERT_TRUE(state == NfcState::STATE_OFF);
}
}
}
}
