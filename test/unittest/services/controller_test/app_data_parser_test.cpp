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
#ifdef VENDOR_APPLICATIONS_ENABLED
/**
 * @tc.name: GetVendorDispatchTagAppsByTech001
 * @tc.desc: Test AppDataParserTest GetVendorDispatchTagAppsByTech.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, GetVendorDispatchTagAppsByTech001, TestSize.Level1)
{
    std::vector<int> discTechList;
    AppDataParser appDataParser = AppDataParser::GetInstance();
    std::vector<ElementName> elementName = appDataParser.GetVendorDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(elementName.size() == 0);
}

/**
 * @tc.name: RegQueryApplicationCb001
 * @tc.desc: Test AppDataParserTest RegQueryApplicationCb.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, RegQueryApplicationCb001, TestSize.Level1)
{
    sptr<IQueryAppInfoCallback> callback = nullptr;
    std::vector<int> discTechList;
    AppDataParser appDataParser = AppDataParser::GetInstance();
    appDataParser.RegQueryApplicationCb(callback);
    std::vector<ElementName> elementName = appDataParser.GetVendorDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(elementName.size() == 0);
}

/**
 * @tc.name: RegCardEmulationNotifyCb001
 * @tc.desc: Test AppDataParserTest RegCardEmulationNotifyCb.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, RegCardEmulationNotifyCb001, TestSize.Level1)
{
    sptr<IOnCardEmulationNotifyCb> callback = nullptr;
    std::vector<int> discTechList;
    AppDataParser appDataParser = AppDataParser::GetInstance();
    appDataParser.RegCardEmulationNotifyCb(callback);
    std::vector<ElementName> elementName = appDataParser.GetVendorDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(elementName.size() == 0);
}

/**
 * @tc.name: GetNotifyCardEmulationCallback001
 * @tc.desc: Test AppDataParserTest GetNotifyCardEmulationCallback.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, GetNotifyCardEmulationCallback001, TestSize.Level1)
{
    AppDataParser appDataParser = AppDataParser::GetInstance();
    sptr<IOnCardEmulationNotifyCb> iOnCardEmulationNotifyCb = appDataParser.GetNotifyCardEmulationCallback();
    ASSERT_TRUE(iOnCardEmulationNotifyCb == nullptr);
}

/**
 * @tc.name: GetHceApps001
 * @tc.desc: Test AppDataParserTest GetHceApps.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, GetHceApps001, TestSize.Level1)
{
    std::vector<AppDataParser::HceAppAidInfo> hceApps;
    AppDataParser appDataParser = AppDataParser::GetInstance();
    appDataParser.GetHceApps(hceApps);
    std::vector<int> discTechList;
    std::vector<ElementName> elementName = appDataParser.GetVendorDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(elementName.size() == 0);
}
#endif
}
}
}