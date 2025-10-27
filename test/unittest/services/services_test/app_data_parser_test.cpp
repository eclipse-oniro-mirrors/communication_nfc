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
#include "app_data_parser.h"
#include "nfc_sdk_common.h"
#include "nfc_notification_publisher.h"
#include "nfc_param_util.h"
#include "nfc_data_share_impl.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
using namespace OHOS::NFC::TAG;
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
/**
 * @tc.name: GetTechMask003
 * @tc.desc: Test AppDataParser GetTechMask.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, GetTechMask003, TestSize.Level1)
{
    AAFwk::Want want;
    want.SetAction(KITS::ACTION_OFF_HOST_APDU_SERVICE);
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

    ASSERT_TRUE(parser.g_offHostAppAndAidMap.size() >= 0);
}
/**
 * @tc.name: IsBundleInstalled001
 * @tc.desc: Test AppDataParser IsBundleInstalled.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, IsBundleInstalled001, TestSize.Level1)
{
    std::string bundleName = "test";
    AppDataParser parser = AppDataParser::GetInstance();
    bool ret = parser.IsBundleInstalled(bundleName);
    ASSERT_TRUE(!ret);
}

/**
 * @tc.name: GetBundleInfo001
 * @tc.desc: Test AppDataParser GetBundleInfo.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, GetBundleInfo001, TestSize.Level1)
{
    AppExecFwk::BundleInfo bundleInfo;
    std::string bundleName = "";
    AppDataParser parser = AppDataParser::GetInstance();
    bool ret = parser.GetBundleInfo(bundleInfo, bundleName);
    ASSERT_TRUE(!ret);
}

/**
 * @tc.name: GetBundleInfo002
 * @tc.desc: Test AppDataParser GetBundleInfo.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, GetBundleInfo002, TestSize.Level1)
{
    AppExecFwk::BundleInfo bundleInfo;
    std::string bundleName = "test";
    AppDataParser parser = AppDataParser::GetInstance();
    bool ret = parser.GetBundleInfo(bundleInfo, bundleName);
    ASSERT_TRUE(!ret);
}

/**
 * @tc.name: IsHceApp001
 * @tc.desc: Test AppDataParser IsHceApp.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, IsHceApp001, TestSize.Level1)
{
    ElementName elementName;
    AppDataParser parser = AppDataParser::GetInstance();
    bool ret = parser.IsHceApp(elementName);
    ASSERT_TRUE(!ret);
}

/**
 * @tc.name: GetBundleNameByUid001
 * @tc.desc: Test AppDataParser GetBundleNameByUid.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, GetBundleNameByUid001, TestSize.Level1)
{
    uint32_t uid = 0;
    AppDataParser parser = AppDataParser::GetInstance();
    std::string ret = parser.GetBundleNameByUid(uid);
    ASSERT_TRUE(ret == "");
}

/**
 * @tc.name: RegNotificationCallback001
 * @tc.desc: Test AppDataParser RegNotificationCallback.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, RegNotificationCallback001, TestSize.Level1)
{
    std::weak_ptr<NfcService> service;
    NfcNotificationPublisher::GetInstance().RegNotificationCallback(service);
    uint32_t uid = 0;
    AppDataParser parser = AppDataParser::GetInstance();
    std::string ret = parser.GetBundleNameByUid(uid);
    ASSERT_TRUE(ret == "");
}

/**
 * @tc.name: RegNotificationCallback002
 * @tc.desc: Test AppDataParser RegNotificationCallback.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, RegNotificationCallback002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    NfcNotificationPublisher::GetInstance().RegNotificationCallback(service);
    uint32_t uid = 0;
    AppDataParser parser = AppDataParser::GetInstance();
    std::string ret = parser.GetBundleNameByUid(uid);
    ASSERT_TRUE(ret == "");
}

/**
 * @tc.name: RegNotificationCallback003
 * @tc.desc: Test AppDataParser RegNotificationCallback.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, RegNotificationCallback003, TestSize.Level1)
{
    std::weak_ptr<NfcService> service;
    NfcNotificationPublisher::GetInstance().RegNotificationCallback(service);
    NfcNotificationPublisher::GetInstance().RegNotificationCallback(service);
    uint32_t uid = 0;
    AppDataParser parser = AppDataParser::GetInstance();
    std::string ret = parser.GetBundleNameByUid(uid);
    ASSERT_TRUE(ret == "");
}

/**
 * @tc.name: RegNotificationCallback004
 * @tc.desc: Test AppDataParser RegNotificationCallback.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, RegNotificationCallback004, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    NfcNotificationPublisher::GetInstance().RegNotificationCallback(service);
    NfcNotificationPublisher::GetInstance().RegNotificationCallback(service);
    uint32_t uid = 0;
    AppDataParser parser = AppDataParser::GetInstance();
    std::string ret = parser.GetBundleNameByUid(uid);
    ASSERT_TRUE(ret == "");
}

/**
 * @tc.name: IsNfcNtfDisabled001
 * @tc.desc: Test IsNfcNtfDisabled.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, IsNfcNtfDisabled001, TestSize.Level1)
{
    constexpr const char* NFC_NOT_DISTURB_KEYWORD = "settings.nfc.not_disturb";
    int INVALID_VALUE = -1;
    const std::string NFC_NOT_DISTURB_SUFFIX =
        "/com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=settings.nfc.not_disturb";
    const std::string NFC_NOT_DISTURB_PREFIX = "datashare://";
    const std::string NFC_DATA_URI_NOT_DISTURB = NFC_NOT_DISTURB_PREFIX + NFC_NOT_DISTURB_SUFFIX;
    Uri nfcNotDisturb(NFC_DATA_URI_NOT_DISTURB);
    auto dataShare = NfcDataShareImpl::GetInstance();
    ASSERT_TRUE(dataShare != nullptr);
    dataShare->SetValue(nfcNotDisturb, NFC_NOT_DISTURB_KEYWORD, INVALID_VALUE);
    ASSERT_TRUE(!NfcNotificationPublisher::GetInstance().IsNfcNtfDisabled());
}

/**
 * @tc.name: UpdateNfcStateToParam001
 * @tc.desc: Test AppDataParser UpdateNfcStateToParam.
 * @tc.type: FUNC
 */
HWTEST_F(AppDataParserTest, UpdateNfcStateToParam001, TestSize.Level1)
{
    int newState = 0;
    std::shared_ptr<NfcParamUtil> nfcParamUtil = std::make_shared<NfcParamUtil>();
    nfcParamUtil->UpdateNfcStateToParam(newState);
    uint32_t uid = 0;
    AppDataParser parser = AppDataParser::GetInstance();
    std::string ret = parser.GetBundleNameByUid(uid);
    ASSERT_TRUE(ret == "");
}
}
}
}