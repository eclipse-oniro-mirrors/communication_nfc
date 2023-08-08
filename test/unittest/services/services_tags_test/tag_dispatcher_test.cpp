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

#include "nfc_controller.h"
#include "nfc_controller_impl.h"
#include "nfc_controller_stub.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include "nfc_service_tdd.h"
#include "permission_tools.h"
#include "tag_dispatcher.h"
#include "tag_host.h"
#include "tag_session.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;

std::vector<int> tagTechList = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
std::vector<int> tagRfDiscIdList = {0, 1, 2};
std::vector<int> tagActivatedProtocols = {0x04};
std::string tagUid = "5B7FCFA9";
std::vector<std::string> tagPollBytes = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B",
    "0C", "0D", "0E", "0F", "10", "11"};
std::vector<std::string> tagActivatedBytes = tagPollBytes;

class TagDispatcherTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TEST_INDEX_1 = 0;
    static constexpr const auto TEST_INDEX_2 = 1;
};

void TagDispatcherTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase TagDispatcherTest." << std::endl;
}

void TagDispatcherTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase TagDispatcherTest." << std::endl;
}

void TagDispatcherTest::SetUp()
{
    std::cout << " SetUp TagDispatcherTest." << std::endl;
}

void TagDispatcherTest::TearDown()
{
    std::cout << " TearDown TagDispatcherTest." << std::endl;
}

/**
 * @tc.name: FindTagHost001
 * @tc.desc: Test TagSession FindTagHost.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, FindTagHost001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    int rfDiscId = TEST_INDEX_2;
    tagDispatcher->HandleTagDebounce();
    std::weak_ptr<NCI::ITagHost> findTagHost = tagDispatcher->FindTagHost(rfDiscId);
    ASSERT_TRUE(findTagHost.use_count() == TEST_INDEX_1);
}
/**
 * @tc.name: FindTagHost002
 * @tc.desc: Test TagSession FindTagHost.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, FindTagHost002, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    int rfDiscId = TEST_INDEX_1;
    std::shared_ptr<NCI::ITagHost> tag = std::make_shared<NCI::TagHost>(
        tagTechList, tagRfDiscIdList, tagActivatedProtocols, tagUid, tagPollBytes, tagActivatedBytes);
    int handleTagFound = tagDispatcher->HandleTagFound(tag);
    std::weak_ptr<NCI::ITagHost> findTagHost = tagDispatcher->FindTagHost(rfDiscId);
    ASSERT_TRUE(findTagHost.use_count() != handleTagFound);
}
/**
 * @tc.name: HandleTagFound001
 * @tc.desc: Test TagSession HandleTagFound.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, HandleTagFound001, TestSize.Level1)
{
    std::shared_ptr<NCI::ITagHost> tag = nullptr;
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    int handleTagFound = tagDispatcher->HandleTagFound(tag);
    ASSERT_TRUE(handleTagFound == TEST_INDEX_1);
}
/**
 * @tc.name: HandleTagFound002
 * @tc.desc: Test TagSession HandleTagFound.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, HandleTagFound002, TestSize.Level1)
{
    std::shared_ptr<KITS::TagInfo> tagInfo = nullptr;
    std::shared_ptr<NCI::ITagHost> tag = std::make_shared<NCI::TagHost>(
        tagTechList, tagRfDiscIdList, tagActivatedProtocols, tagUid, tagPollBytes, tagActivatedBytes);
    std::shared_ptr<INfcService> service = nullptr;
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    tagDispatcher->DispatchAbilityMultiApp(tagInfo);
    int handleTagFound = tagDispatcher->HandleTagFound(tag);
    ASSERT_TRUE(handleTagFound == TEST_INDEX_1);
}
/**
 * @tc.name: HandleTagFound003
 * @tc.desc: Test TagSession HandleTagFound.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, HandleTagFound003, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap tagTechExtrasData;
    AppExecFwk::PacMap isoDepExtrasData;
    tagTechExtras.push_back(tagTechExtrasData);
    tagTechExtras.push_back(isoDepExtrasData);
    int tagRfDiscId = TEST_INDEX_2;
    std::shared_ptr<KITS::TagInfo> tagInfo = std::make_shared<KITS::TagInfo>(tagTechList, tagTechExtras, tagUid,
        tagRfDiscId, nullptr);
    std::shared_ptr<NCI::ITagHost> tag = std::make_shared<NCI::TagHost>(tagTechList, tagRfDiscIdList,
        tagActivatedProtocols, tagUid, tagPollBytes, tagActivatedBytes);
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    tagDispatcher->DispatchAbilityMultiApp(tagInfo);
    tagDispatcher->DispatchAbilitySingleApp(element, tagInfo);
    int handleTagFound = tagDispatcher->HandleTagFound(tag);
    ASSERT_TRUE(handleTagFound == TEST_INDEX_1);
}
}
}
}
