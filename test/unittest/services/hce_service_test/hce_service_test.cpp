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

#include "hce_service.h"
#include "nfc_controller.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class HceServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HceServiceTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase HceServiceTest." << std::endl;
}

void HceServiceTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase HceServiceTest." << std::endl;
}

void HceServiceTest::SetUp()
{
    std::cout << " SetUp HceServiceTest." << std::endl;
}

void HceServiceTest::TearDown()
{
    std::cout << " TearDown HceServiceTest." << std::endl;
}

class HceCmdListener : public IHceCmdCallback {
public:
    HceCmdListener() {}

    virtual ~HceCmdListener() {}

public:
    void OnCeApduData(const std::vector<uint8_t>& data) override
    {
        std::cout << "OnCeApduData" << std::endl;
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

/**
 * @tc.name: RegHceCmdCallback001
 * @tc.desc: Test HceServiceTest RegHceCmdCallback.
 * @tc.type: FUNC
 */
HWTEST_F(HceServiceTest, RegHceCmdCallback001, TestSize.Level1)
{
    sptr<HceCmdListener> callback = sptr<HceCmdListener>(new (std::nothrow) HceCmdListener());
    int ret = HceService::GetInstance().RegHceCmdCallback(callback, "hceCmd");
    ASSERT_TRUE(ret == NFC::KITS::ErrorCode::ERR_HCE_STATE_UNBIND);
}

/**
 * @tc.name: SendRawFrame001
 * @tc.desc: Test HceServiceTest SendRawFrame001.
 * @tc.type: FUNC
 */
HWTEST_F(HceServiceTest, SendRawFrame001, TestSize.Level1)
{
    std::string hexCmdData = "010203";
    std::string hexRespData = "";
    int ret = HceService::GetInstance().SendRawFrame(hexCmdData, true, hexRespData);
    ASSERT_TRUE(ret == NFC::KITS::ErrorCode::ERR_HCE_STATE_UNBIND);
}

/**
 * @tc.name: GetPaymentServices001
 * @tc.desc: Test HceServiceTest GetPaymentServices001.
 * @tc.type: FUNC
 */
HWTEST_F(HceServiceTest, GetPaymentServices001, TestSize.Level1)
{
    std::vector<AbilityInfo> abilityInfos;
    int ret = HceService::GetInstance().GetPaymentServices(abilityInfos);
    ASSERT_TRUE(ret == NFC::KITS::ErrorCode::ERR_HCE_STATE_UNBIND);
}

/**
 * @tc.name: IsDefaultService001
 * @tc.desc: Test HceServiceTest IsDefaultService001.
 * @tc.type: FUNC
 */
HWTEST_F(HceServiceTest, IsDefaultService001, TestSize.Level1)
{
    ElementName element;
    const std::string type = "";
    bool isDefaultService = true;
    int ret = HceService::GetInstance().IsDefaultService(element, type, isDefaultService);
    ASSERT_TRUE(ret == NFC::KITS::ErrorCode::ERR_HCE_STATE_UNBIND);
}

/**
 * @tc.name: StopHce001
 * @tc.desc: Test HceServiceTest StopHce001.
 * @tc.type: FUNC
 */
HWTEST_F(HceServiceTest, StopHce001, TestSize.Level1)
{
    ElementName element;
    int ret = HceService::GetInstance().StopHce(element);
    ASSERT_TRUE(ret == NFC::KITS::ErrorCode::ERR_HCE_STATE_UNBIND);
}
}
}
}