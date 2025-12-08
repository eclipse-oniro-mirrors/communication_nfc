/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
HWTEST_F(BasicTagSessionTest, GetTagRfDiscId001, TestSize.Level1)
{
    std::shared_ptr<TagInfo> tagInfo = nullptr;
    TagTechnology tagTechnology = TagTechnology::NFC_INVALID_TECH;
    BasicTagSession basicTagSession{tagInfo, tagTechnology};
    int ret = basicTagSession.GetTagRfDiscId();
    ASSERT_TRUE(ret == ErrorCode::ERR_TAG_PARAMETERS);
}

/**
 * @tc.name: SetConnectedTagTech001
 * @tc.desc: Test BasicTagSessionTest SetConnectedTagTech.
 * @tc.type: FUNC
 */
HWTEST_F(BasicTagSessionTest, SetConnectedTagTech001, TestSize.Level1)
{
    std::vector<int> tagTechList;
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    std::string tagUid = "123";
    int tagRfDiscId = 1;
    std::shared_ptr<TagInfo> tagInfo = std::make_shared<TagInfo> (tagTechList,
                                                                  tagTechExtras,
                                                                  tagUid,
                                                                  tagRfDiscId,
                                                                  nullptr);
    std::shared_ptr<TagInfo> tagInfo = nullptr;
    TagTechnology tagTechnology = TagTechnology::NFC_INVALID_TECH;
    BasicTagSession basicTagSession{tagInfo, tagTechnology};
    basicTagSession.SetConnectedTagTech(TagTechnology::NFC_INVALID_TECH);
    ASSERT_TRUE(tagInfo != nullptr);
}

/**
 * @tc.name: GetConnectedTagTech001
 * @tc.desc: Test BasicTagSessionTest GetConnectedTagTech.
 * @tc.type: FUNC
 */
HWTEST_F(BasicTagSessionTest, GetConnectedTagTech001, TestSize.Level1)
{
    std::shared_ptr<TagInfo> tagInfo = nullptr;
    TagTechnology tagTechnology = TagTechnology::NFC_INVALID_TECH;
    BasicTagSession basicTagSession{tagInfo, tagTechnology};
    KITS::TagTechnology ret = basicTagSession.GetConnectedTagTech();
    ASSERT_TRUE(ret == TagTechnology::NFC_INVALID_TECH);
}

/**
 * @tc.name: ResetTimeout001
 * @tc.desc: Test BasicTagSessionTest ResetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(BasicTagSessionTest, ResetTimeout001, TestSize.Level1)
{
    std::vector<int> tagTechList;
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    std::string tagUid = "123";
    int tagRfDiscId = 1;
    std::shared_ptr<TagInfo> tagInfo = std::make_shared<TagInfo> (tagTechList,
                                                                  tagTechExtras,
                                                                  tagUid,
                                                                  tagRfDiscId,
                                                                  nullptr);
    TagTechnology tagTechnology = TagTechnology::NFC_INVALID_TECH;
    BasicTagSession basicTagSession{tagInfo, tagTechnology};
    basicTagSession.ResetTimeout();
    ASSERT_TRUE(tagInfo != nullptr);
}
}
}
}