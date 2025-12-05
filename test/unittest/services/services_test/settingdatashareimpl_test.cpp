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

#define protected public

#include <gtest/gtest.h>
#include <thread>

namespace OHOS {
namespace NFC {
namespace TEST {
    using namespace testing::ext;
    using namespace OHOS::NFC;
    using namespace OHOS::NFC::KITS;

    class SettingDataShareImplTest : public testing::Test {
    public:
        static void SetUpTestCase();
        static void TearDownTestCase();
        void SetUp();
        void TearDown();
    };

void SettingDataShareImplTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase SettingDataShareImplTest." << std::endl;
}

void SettingDataShareImplTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase SettingDataShareImplTest." << std::endl;
}

void SettingDataShareImplTest::SetUp()
{
    std::cout << " SetUp SettingDataShareImplTest." << std::endl;
}

void SettingDataShareImplTest::TearDown()
{
    std::cout << " TearDown SettingDataShareImplTest." << std::endl;
}
ReleaseDataObserver
/**
 * @tc.name: ReleaseDataObserver001
 * @tc.desc: Test SettingDataShareImplTest ReleaseDataObserver.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingParamsTest, ReleaseDataObserver001, TestSize.Level1)
{
    std::shared_ptr<SettingDataShareImpl> settingDataShareImpl = std::make_shared<SettingDataShareImpl>();
    sptr<AAFwk::IDataAbilityObserver> dataObserver;
    settingDataShareImpl->dataShareHelper_ = nullptr;
    Uri uri(KITS::NFC_DATA_URI);
    ErrorCode errorCode = settingDataShareImpl->ReleaseDataObserver(uri, dataObserver);
    ASSERT_TRUE(errorCode == ERR_NONE);
}

}
}
}