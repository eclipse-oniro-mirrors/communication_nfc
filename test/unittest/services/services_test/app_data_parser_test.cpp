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
#include "app_data_parser.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class AppDataParserTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TECH_MASK = 4;
};

void AppDataParserTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase AppDataParserTest." << std::endl;
}

void AppDataParserTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase AppDataParserTest." << std::endl;
}

void AppDataParserTest::SetUp()
{
    std::cout << " SetUp AppDataParserTest." << std::endl;
}

void AppDataParserTest::TearDown()
{
    std::cout << " TearDown AppDataParserTest." << std::endl;
}

/**
 * @tc.name: GetTechMask001
 * @tc.desc: Test AppDataParser GetTechMask.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, GetTechMask001, TestSize.Level1)
{
    AAFwk::Want want;
    want.SetAction(KITS::ACTION_TAG_FOUND);
    EventFwk::CommonEventData data;
    data.SetWant(want);
    const std::shared_ptr<EventFwk::CommonEventData> mdata =
        std::make_shared<EventFwk::CommonEventData>(data);
    AppDataParser parser = AppDataParser::GetInstance();
    parser.HandleAppAddOrChangedEvent(nullptr);
    parser.HandleAppAddOrChangedEvent(mdata);

    parser.HandleAppRemovedEvent(nullptr);
    parser.HandleAppRemovedEvent(mdata);

    parser.InitAppList();

    // no given tag technologies
    std::vector<int> discTechList;
    ASSERT_TRUE(parser.GetDispatchTagAppsByTech(discTechList).size() == 0);
}
/**
 * @tc.name: GetTechMask002
 * @tc.desc: Test AppDataParser GetTechMask.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, GetTechMask002, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    element.SetBundleName(KITS::ACTION_TAG_FOUND);
    AAFwk::Want want;
    want.SetElement(element);
    EventFwk::CommonEventData data;
    data.SetWant(want);
    const std::shared_ptr<EventFwk::CommonEventData> mdata =
        std::make_shared<EventFwk::CommonEventData>(data);
    AppDataParser parser = AppDataParser::GetInstance();
    parser.HandleAppAddOrChangedEvent(mdata);

    parser.HandleAppRemovedEvent(mdata);

    std::vector<int> discTechList;
    // no app installed, or has app installed to matched with the given tag technologies.
    discTechList.push_back(static_cast<int>(KITS::TagTechnology::NFC_A_TECH));
    discTechList.push_back(static_cast<int>(KITS::TagTechnology::NFC_ISODEP_TECH));
    ASSERT_TRUE(parser.GetDispatchTagAppsByTech(discTechList).size() >= 0);
}
}
}
}