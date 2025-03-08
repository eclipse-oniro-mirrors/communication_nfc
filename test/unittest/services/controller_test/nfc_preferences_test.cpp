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

#include "nfc_preferences.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class NfcPreferencesTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NfcPreferencesTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcPreferencesTest." << std::endl;
}

void NfcPreferencesTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcPreferencesTest." << std::endl;
}

void NfcPreferencesTest::SetUp()
{
    std::cout << " SetUp NfcPreferencesTest." << std::endl;
}

void NfcPreferencesTest::TearDown()
{
    std::cout << " TearDown NfcPreferencesTest." << std::endl;
}

/**
 * @tc.name: SetInt001
 * @tc.desc: Test NfcPreferencesTest SetInt.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPreferencesTest, SetInt001, TestSize.Level1)
{
    std::string key = "123";
    NfcPreferences::GetInstance().SetInt(key, 456);
    int value = NfcPreferences::GetInstance().GetInt(key);
    ASSERT_TRUE(value == 456);
}
}
}
}