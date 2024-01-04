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

#include "tag_ability_dispatcher.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::TAG;
class TagAbilityDispatcherTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void TagAbilityDispatcherTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase TagAbilityDispatcherTest." << std::endl;
}

void TagAbilityDispatcherTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase TagAbilityDispatcherTest." << std::endl;
}

void TagAbilityDispatcherTest::SetUp()
{
    std::cout << " SetUp TagAbilityDispatcherTest." << std::endl;
}

void TagAbilityDispatcherTest::TearDown()
{
    std::cout << " TearDown TagAbilityDispatcherTest." << std::endl;
}

/**
 * @tc.name: DispatchTagAbility001
 * @tc.desc: Test TagAbilityDispatcherTest DispatchTagAbility.
 * @tc.type: FUNC
 */
HWTEST_F(TagAbilityDispatcherTest, DispatchTagAbility001, TestSize.Level1)
{
    std::shared_ptr<KITS::TagInfo> tagInfo = nullptr;
    OHOS::sptr<IRemoteObject> tagServiceIface = nullptr;
    std::shared_ptr<TagAbilityDispatcher> tagAbilityDispatcher = std::make_shared<TagAbilityDispatcher>();
    tagAbilityDispatcher->DispatchTagAbility(tagInfo, tagServiceIface);
}

/**
 * @tc.name: DispatchAbilityMultiApp001
 * @tc.desc: Test TagAbilityDispatcherTest DispatchAbilityMultiApp.
 * @tc.type: FUNC
 */
HWTEST_F(TagAbilityDispatcherTest, DispatchAbilityMultiApp001, TestSize.Level1)
{
    std::shared_ptr<KITS::TagInfo> tagInfo = nullptr;
    AAFwk::Want want;
    std::shared_ptr<TagAbilityDispatcher> tagAbilityDispatcher = std::make_shared<TagAbilityDispatcher>();
    tagAbilityDispatcher->DispatchAbilityMultiApp(tagInfo, want);
}

/**
 * @tc.name: DispatchAbilitySingleApp001
 * @tc.desc: Test TagAbilityDispatcherTest DispatchAbilitySingleApp.
 * @tc.type: FUNC
 */
HWTEST_F(TagAbilityDispatcherTest, DispatchAbilitySingleApp001, TestSize.Level1)
{
    AAFwk::Want want;
    std::shared_ptr<TagAbilityDispatcher> tagAbilityDispatcher = std::make_shared<TagAbilityDispatcher>();
    tagAbilityDispatcher->DispatchAbilitySingleApp(want);
}
}
}
}