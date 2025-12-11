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

#include "nfc_controller.h"
#include "nfc_controller_impl.h"
#include "nfc_controller_stub.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include "nfc_service_tdd.h"
#include "nfc_permission_checker.h"
#include "tag_dispatcher.h"
#include "tag_session.h"
#include "tag_ability_dispatcher.h"

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
int g_connectedTechIndex = 0;
static const int g_testTagRfId = 1;

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
 * @tc.name: HandleTagFound001
 * @tc.desc: Test TagSession HandleTagFound.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, HandleTagFound001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    tagDispatcher->HandleTagFound(g_testTagRfId);
    ASSERT_TRUE(tagDispatcher != nullptr);
}

/**
 * @tc.name: HandleTagFound002
 * @tc.desc: Test TagSession HandleTagFound.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, HandleTagFound002, TestSize.Level1)
{
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(nullptr);
    tagDispatcher->HandleTagFound(g_testTagRfId);
    ASSERT_TRUE(tagDispatcher != nullptr);
}

/**
 * @tc.name: HandleTagFound003
 * @tc.desc: Test TagSession HandleTagFound.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, HandleTagFound003, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    tagDispatcher->HandleTagFound(g_testTagRfId);
    ASSERT_TRUE(tagDispatcher != nullptr);
}

/**
 * @tc.name: OnNotificationButtonClicked001
 * @tc.desc: Test TagSession OnNotificationButtonClicked.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, OnNotificationButtonClicked001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service;
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    tagDispatcher->OnNotificationButtonClicked(114003);
    ASSERT_TRUE(tagDispatcher != nullptr);
}

/**
 * @tc.name: OnNotificationButtonClicked002
 * @tc.desc: Test TagSession OnNotificationButtonClicked.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, OnNotificationButtonClicked002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    tagDispatcher->OnNotificationButtonClicked(114003);
    ASSERT_TRUE(tagDispatcher != nullptr);
}

/**
 * @tc.name: OnNotificationButtonClicked003
 * @tc.desc: Test TagSession OnNotificationButtonClicked.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, OnNotificationButtonClicked003, TestSize.Level1)
{
    std::shared_ptr<NfcService> service;
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    tagDispatcher->OnNotificationButtonClicked(114000);
    ASSERT_TRUE(tagDispatcher != nullptr);
}

/**
 * @tc.name: OnNotificationButtonClicked004
 * @tc.desc: Test TagSession OnNotificationButtonClicked.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, OnNotificationButtonClicked004, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    tagDispatcher->OnNotificationButtonClicked(114000);
    ASSERT_TRUE(tagDispatcher != nullptr);
}

/**
 * @tc.name: OnNotificationButtonClicked005
 * @tc.desc: Test TagSession OnNotificationButtonClicked.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, OnNotificationButtonClicked005, TestSize.Level1)
{
    std::shared_ptr<NfcService> service;
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    tagDispatcher->OnNotificationButtonClicked(114006);
    ASSERT_TRUE(tagDispatcher != nullptr);
}

/**
 * @tc.name: OnNotificationButtonClicked006
 * @tc.desc: Test TagSession OnNotificationButtonClicked.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, OnNotificationButtonClicked006, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    tagDispatcher->OnNotificationButtonClicked(114006);
    ASSERT_TRUE(tagDispatcher != nullptr);
}

/**
 * @tc.name: OnNotificationButtonClicked007
 * @tc.desc: Test TagSession OnNotificationButtonClicked.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, OnNotificationButtonClicked007, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    tagDispatcher->OnNotificationButtonClicked(114006);
    ASSERT_TRUE(tagDispatcher != nullptr);
}

/**
 * @tc.name: DispatchAbilitySingleApp001
 * @tc.desc: Test TagSession DispatchAbilitySingleApp.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, DispatchAbilitySingleApp001, TestSize.Level1)
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
    std::shared_ptr<NFC::TAG::TagAbilityDispatcher> tagAbilityDispatcher =
        std::make_shared<NFC::TAG::TagAbilityDispatcher>();
    AAFwk::Want want;
    want.SetAction(KITS::ACTION_TAG_FOUND);
    want.SetElement(element);
    tagAbilityDispatcher->DispatchTagAbility(tagInfo, nullptr);
    tagAbilityDispatcher->DispatchAbilitySingleApp(want);
    ASSERT_TRUE(element.GetBundleName() == "");
}
/**
 * @tc.name: DispatchAbilityMultiApp001
 * @tc.desc: Test TagSession DispatchAbilityMultiApp.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, DispatchAbilityMultiApp001, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    element.SetBundleName(KITS::ACTION_TAG_FOUND);
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap tagTechExtrasData;
    AppExecFwk::PacMap isoDepExtrasData;
    tagTechExtras.push_back(tagTechExtrasData);
    tagTechExtras.push_back(isoDepExtrasData);
    int tagRfDiscId = TEST_INDEX_2;
    std::shared_ptr<KITS::TagInfo> tagInfo = std::make_shared<KITS::TagInfo>(tagTechList, tagTechExtras, tagUid,
        tagRfDiscId, nullptr);
    std::shared_ptr<NFC::TAG::TagAbilityDispatcher> tagAbilityDispatcher =
        std::make_shared<NFC::TAG::TagAbilityDispatcher>();
    AAFwk::Want want;
    want.SetAction(KITS::ACTION_TAG_FOUND);
    want.SetElement(element);
    tagAbilityDispatcher->DispatchTagAbility(tagInfo, nullptr);
    tagAbilityDispatcher->DispatchAbilityMultiApp(tagInfo, want);
    ASSERT_TRUE(element.GetBundleName() == KITS::ACTION_TAG_FOUND);
}

/**
 * @tc.name: HandleNdefDispatch001
 * @tc.desc: Test TagSession HandleNdefDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, HandleNdefDispatch001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    uint32_t tagDiscId = 0;
    std::string msg = "";
    tagDispatcher->HandleNdefDispatch(tagDiscId, msg);
    ASSERT_TRUE(tagDispatcher != nullptr);
}

/**
 * @tc.name: HandleNdefDispatch002
 * @tc.desc: Test TagSession HandleNdefDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, HandleNdefDispatch002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    uint32_t tagDiscId = 0;
    std::string msg = "";
    tagDispatcher->HandleNdefDispatch(tagDiscId, msg);
    ASSERT_TRUE(tagDispatcher != nullptr);
}

/**
 * @tc.name: PublishTagNotification
 * @tc.desc: Test TagSession PublishTagNotification.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, PublishTagNotification, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    uint32_t tagDiscId = 0;
    bool isIsoDep = false;
    tagDispatcher->PublishTagNotification(tagDiscId, isIsoDep);
    ASSERT_TRUE(tagDispatcher != nullptr);
}

/**
 * @tc.name: GetTagInfoFromTag001
 * @tc.desc: Test TagSession GetTagInfoFromTag.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, GetTagInfoFromTag001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    uint32_t tagDiscId = 0;
    std::shared_ptr<KITS::TagInfo> tagInfo = tagDispatcher->GetTagInfoFromTag001(tagDiscId);
    ASSERT_TRUE(tagInfo != nullptr);
}

/**
 * @tc.name: GetTagInfoParcelableFromTag001
 * @tc.desc: Test TagSession GetTagInfoParcelableFromTag.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, GetTagInfoParcelableFromTag001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    uint32_t tagDiscId = 0;
    tagDispatcher->GetTagInfoParcelableFromTag(tagDiscId);
    ASSERT_TRUE(tagInfo != nullptr);
}

/**
 * @tc.name: GerTagInfoParcelableFromTag002
 * @tc.desc: Test TagSession GerTagInfoParcelableFromTag.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, GerTagInfoParcelableFromTag002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    uint32_t tagDiscId = 0;
    tagDispatcher->GetTagInfoParcelableFromTag(tagDiscId);
    ASSERT_TRUE(tagInfo != nullptr);
}

/**
 * @tc.name: ParseNdefInfo001
 * @tc.desc: Test TagSession ParseNdefInfo.
 * @tc.type: FUNC
 */
HWTEST_F(TagDispatcherTest, ParseNdefInfo001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
    std::string ndefMsg = "test";
    std::shared_ptr<KITS::NdefMessage> ndefMessage = KITS::NdefMessage::GetNdefMessage(ndefMsg);
    std::string ndefInfo = tagDispatcher->ParseNdefInfo(ndefMessage);
    ASSERT_TRUE(ndefInfo == "");
}
}
}
}
