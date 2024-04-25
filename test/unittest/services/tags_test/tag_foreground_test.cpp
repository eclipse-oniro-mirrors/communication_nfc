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

#include "tag_foreground.h"

#include "iforeground_callback.h"
#include "nfc_controller.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class TagForegroundTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void TagForegroundTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase TagForegroundTest." << std::endl;
}

void TagForegroundTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase TagForegroundTest." << std::endl;
}

void TagForegroundTest::SetUp()
{
    std::cout << " SetUp TagForegroundTest." << std::endl;
}

void TagForegroundTest::TearDown()
{
    std::cout << " TearDown TagForegroundTest." << std::endl;
}

/**
 * @tc.name: RegForeground001
 * @tc.desc: Test TagForeground RegForeground.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, RegForeground001, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    std::vector<uint32_t> discTech = {1, 2, 3, 4, 5};
    const sptr<KITS::IForegroundCallback> callback = nullptr;
    TagForeground instance = TagForeground::GetInstance();
    ErrorCode result = instance.RegForeground(element, discTech, callback);
    ASSERT_TRUE(result == ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}
/**
 * @tc.name: UnregForeground001
 * @tc.desc: Test TagForeground UnregForeground.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, UnregForeground001, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    TagForeground instance = TagForeground::GetInstance();
    ErrorCode result = instance.UnregForeground(element);
    ASSERT_TRUE(result == ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}

/**
 * @tc.name: RegReaderMode001
 * @tc.desc: Test TagForeground RegReaderMode.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, RegReaderMode001, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    std::vector<uint32_t> discTech;
    sptr<KITS::IReaderModeCallback> callback = nullptr;
    TagForeground instance = TagForeground::GetInstance();
    ErrorCode result = instance.RegReaderMode(element, discTech, callback);
    ASSERT_TRUE(result == ErrorCode::ERR_NO_PERMISSION);
}

/**
 * @tc.name: UnregReaderMode001
 * @tc.desc: Test TagForeground UnregReaderMode.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, UnregReaderMode001, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    TagForeground instance = TagForeground::GetInstance();
    ErrorCode result = instance.UnregReaderMode(element);
    ASSERT_TRUE(result == ErrorCode::ERR_NO_PERMISSION);
}
}
}
}
