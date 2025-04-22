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

#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class NfcSdkCommonTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    std::shared_ptr<NfcSdkCommon> common = std::make_shared<NfcSdkCommon>();
};

void NfcSdkCommonTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcSdkCommonTest." << std::endl;
}

void NfcSdkCommonTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcSdkCommonTest." << std::endl;
}

void NfcSdkCommonTest::SetUp()
{
    std::cout << " SetUp NfcSdkCommonTest." << std::endl;
}

void NfcSdkCommonTest::TearDown()
{
    std::cout << " TearDown NfcSdkCommonTest." << std::endl;
}

/**
 * @tc.name: SecureStringToInt001
 * @tc.desc: Test NfcSdkCommonTest SecureStringToInt.
 * @tc.type: FUNC
 */
HWTEST_F(NfcSdkCommonTest, SecureStringToInt001, TestSize.Level1)
{
    int32_t value = 0;
    common->SecureStringToInt("", value, 10);
    value = 0;
    common->SecureStringToInt("abc", value, 10);
    value = 0;
    common->SecureStringToInt("123", value, 10);
    value = 0;
    common->SecureStringToInt("2147483648", value, 10);
    value = 0;
    common->SecureStringToInt("-100", value, 10);
    ASSERT_TRUE(common != nullptr);
}

/**
 * @tc.name: GetConfigFromJson001
 * @tc.desc: Test NfcSdkCommonTest GetConfigFromJson.
 * @tc.type: FUNC
 */
HWTEST_F(NfcSdkCommonTest, GetConfigFromJson001, TestSize.Level1)
{
    std::string key = "";
    std::string value = "";
    common->GetConfigFromJson(key, value);
    ASSERT_TRUE(common != nullptr);
}

/**
 * @tc.name: GetConfigFromJson002
 * @tc.desc: Test NfcSdkCommonTest GetConfigFromJson.
 * @tc.type: FUNC
 */
HWTEST_F(NfcSdkCommonTest, GetConfigFromJson002, TestSize.Level1)
{
    std::string key = "test";
    std::string value = "";
    common->GetConfigFromJson(key, value);
    ASSERT_TRUE(common != nullptr);
}

/**
 * @tc.name: GetConfigFromJson003
 * @tc.desc: Test NfcSdkCommonTest GetConfigFromJson.
 * @tc.type: FUNC
 */
HWTEST_F(NfcSdkCommonTest, GetConfigFromJson003, TestSize.Level1)
{
    std::string key = "report_appId";
    std::string value = "";
    common->GetConfigFromJson(key, value);
    ASSERT_TRUE(common != nullptr);
}
} // namespace TEST
} // namespace NFC
} // namespace OHOS