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
#include "card_emulation/ce_service.h"

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
 * @tc.name: IsSupported001
 * @tc.desc: Test CardemulationTest IsSupported.
 * @tc.type: FUNC
 */
HWTEST_F(CardemulationTest, IsSupported001, TestSize.Level1)
{
    bool isSupport = false;
    CardEmulation cardemulation = CardEmulation::GetInstance();

    // Supports FeatureType { HCE = 0, UICC = 1, ESE = 2 } type card emulation
    isSupport = cardemulation.IsSupported(FeatureType::UICC);
    ASSERT_TRUE(isSupport == true);
}
/**
 * @tc.name: IsSupported002
 * @tc.desc: Test CardemulationTest IsSupported.
 * @tc.type: FUNC
 */
HWTEST_F(CardemulationTest, IsSupported002, TestSize.Level1)
{
    bool isSupport = false;
    CardEmulation cardemulation = CardEmulation::GetInstance();

    // Supports FeatureType { HCE = 0, UICC = 1, ESE = 2 } type card emulation
    isSupport = cardemulation.IsSupported(FeatureType::HCE);
    ASSERT_TRUE(isSupport == true);
}
/**
 * @tc.name: IsSupported003
 * @tc.desc: Test CardemulationTest IsSupported.
 * @tc.type: FUNC
 */
HWTEST_F(CardemulationTest, IsSupported003, TestSize.Level1)
{
    bool isSupport = false;
    CardEmulation cardemulation = CardEmulation::GetInstance();

    // Supports FeatureType { HCE = 0, UICC = 1, ESE = 2 } type card emulation
    isSupport = cardemulation.IsSupported(FeatureType::ESE);
    ASSERT_TRUE(isSupport == true);
}
/**
 * @tc.name: IsSupported004
 * @tc.desc: Test CardemulationTest IsSupported.
 * @tc.type: FUNC
 */
HWTEST_F(CardemulationTest, IsSupported004, TestSize.Level1)
{
    bool isSupport = true;
    CardEmulation cardemulation = CardEmulation::GetInstance();

    // card emulation is not supported
    isSupport = cardemulation.IsSupported(static_cast<FeatureType>(ErrorCode::ERR_NFC_BASE));
    ASSERT_TRUE(isSupport == false);
}

/**
 * @tc.name: CeService001
 * @tc.desc: Test CeService001.
 * @tc.type: FUNC
 */
HWTEST_F(CardemulationTest, CeService001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->PublishFieldOnOrOffCommonEvent(true);
    ceService->PublishFieldOnOrOffCommonEvent(false);
    ceService->HandleFieldActivated();
    ceService->HandleFieldDeactivated();
}

/**
 * @tc.name: CeService002
 * @tc.desc: Test CeService002.
 * @tc.type: FUNC
 */
HWTEST_F(CardemulationTest, CeService002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->HandleFieldActivated();
    ceService->HandleFieldDeactivated();
}
}
}
}
