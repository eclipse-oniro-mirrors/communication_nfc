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

#include "cardEmulation.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class CardemulationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CardemulationTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase CardemulationTest." << std::endl;
}

void CardemulationTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase CardemulationTest." << std::endl;
}

void CardemulationTest::SetUp() {}

void CardemulationTest::TearDown() {}

/**
 * @tc.name: IsSupporte001
 * @tc.desc: Test CardemulationTest IsSupporte.
 * @tc.type: FUNC
 */
HWTEST_F(CardemulationTest, IsSupported001, TestSize.Level1)
{
    bool issupport = false;
    CardEmulation cardemulation = CardEmulation::GetInstance();

    // Supports FeatureType { HCE = 0, UICC = 1, ESE = 2 } type card emulation
    issupport = cardemulation.IsSupported(FeatureType::UICC);
    ASSERT_TRUE(issupport == true);
}
}
}
}
