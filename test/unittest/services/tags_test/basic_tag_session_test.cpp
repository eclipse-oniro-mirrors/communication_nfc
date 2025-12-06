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

#define private publi
#define protected public

#include <gtest/gtest.h>
#include <thread>

#include "basic_tag_session.h"
#include "loghelper.h"
#include "nfc_controller.h"
#include "nfc_sdk_common.h"
#include "tag_session_proxy.h"
#include "taginfo.h"

namespace OHOS {
namespace NFC {
namespace TEST {
    using namespace testing::ext;
    using namespace OHOS::NFC;
    using namespace OHOS::NFC::KITS;

    class BasicTagSessionTest : public testing::Test {
    public:
        static void SetUpTestCase();
        static void TearDownTestCase();
        void SetUp();
        void TearDown();
    };

void BasicTagSessionTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase BasicTagSessionTest." << std::endl;
}

void BasicTagSessionTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase BasicTagSessionTest." << std::endl;
}

void BasicTagSessionTest::SetUp()
{
    std::cout << " SetUp BasicTagSessionTest." << std::endl;
}

void BasicTagSessionTest::TearDown()
{
    std::cout << " TearDown BasicTagSessionTest." << std::endl;
}

/**
 * @tc.name: GetTagRfDiscId001
 * @tc.desc: Test BasicTagSessionTest GetTagRfDiscId.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingParamsTest, GetTagRfDiscId001, TestSize.Level1)
{
    std::shared_ptr<TagInfo> tagInfo = nullptr;
    TagTechnology tagTechnology = TagTechnology::NFC_INVALID_TECH;
    BasicTagSession basicTagSession{tagInfo, tagTechnology};
    int ret = basicTagSession.GetTagRfDiscId();
    ASSERT_TRUE(ret == ErrorCode::ERR_TAG_PARAMETERS);
}

/**
 * @tc.name: SetVonnectedTagTech001
 * @tc.desc: Test BasicTagSessionTest SetVonnectedTagTech.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingParamsTest, SetVonnectedTagTech001, TestSize.Level1)
{
    std::shared_ptr<TagInfo> tagInfo = nullptr;
    TagTechnology tagTechnology = TagTechnology::NFC_INVALID_TECH;
    BasicTagSession basicTagSession{tagInfo, tagTechnology};
    basicTagSession.SetVonnectedTagTech();
}

/**
 * @tc.name: GetConnectedTagTech001
 * @tc.desc: Test BasicTagSessionTest GetConnectedTagTech.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingParamsTest, GetConnectedTagTech001, TestSize.Level1)
{
    std::shared_ptr<TagInfo> tagInfo = nullptr;
    TagTechnology tagTechnology = TagTechnology::NFC_INVALID_TECH;
    BasicTagSession basicTagSession{tagInfo, tagTechnology};
    KITS::TagTechnology ret = basicTagSession.GetConnectedTagTech();
    ASSERT_TRUE(ret == TagTechnology::NFC_INVALID_TECH);
}

/**
 * @tc.name: ResetTimout001
 * @tc.desc: Test BasicTagSessionTest ResetTimout.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingParamsTest, ResetTimout001, TestSize.Level1)
{
    std::shared_ptr<TagInfo> tagInfo = nullptr;
    TagTechnology tagTechnology = TagTechnology::NFC_INVALID_TECH;
    BasicTagSession basicTagSession{tagInfo, tagTechnology};
    basicTagSession.ResetTimout();
}

}
}
}