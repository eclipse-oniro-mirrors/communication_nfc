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

#include "external_deps_proxy.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class ExternalDepsProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ExternalDepsProxyTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase ExternalDepsProxyTest." << std::endl;
}

void ExternalDepsProxyTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase ExternalDepsProxyTest." << std::endl;
}

void ExternalDepsProxyTest::SetUp()
{
    std::cout << " SetUp ExternalDepsProxyTest." << std::endl;
}

void ExternalDepsProxyTest::TearDown()
{
    std::cout << " TearDown ExternalDepsProxyTest." << std::endl;
}

/**
 * @tc.name: HandleAppAddOrChangedEvent001
 * @tc.desc: Test ExternalDepsProxyTest HandleAppAddOrChangedEvent.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, HandleAppAddOrChangedEvent001, TestSize.Level1)
{
    std::shared_ptr<EventFwk::CommonEventData> data = nullptr;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->HandleAppAddOrChangedEvent(data);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: GetDispatchTagAppsByTech001
 * @tc.desc: Test ExternalDepsProxyTest GetDispatchTagAppsByTech.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, GetDispatchTagAppsByTech001, TestSize.Level1)
{
    std::vector<int> discTechList;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}
#ifdef VENDOR_APPLICATIONS_ENABLED
/**
 * @tc.name: GetVendorDispatchTagAppsByTech001
 * @tc.desc: Test ExternalDepsProxyTest GetVendorDispatchTagAppsByTech.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, GetVendorDispatchTagAppsByTech001, TestSize.Level1)
{
    std::vector<int> discTechList;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    std::vector<ElementName> getVendorDispatchTagAppsByTech =
        externalDepsProxy->GetVendorDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getVendorDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: RegQueryApplicationCb001
 * @tc.desc: Test ExternalDepsProxyTest RegQueryApplicationCb.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, RegQueryApplicationCb001, TestSize.Level1)
{
    sptr<IQueryAppInfoCallback> callback = nullptr;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->RegQueryApplicationCb(callback);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: RegCardEmulationNotifyCb001
 * @tc.desc: Test ExternalDepsProxyTest RegCardEmulationNotifyCb.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, RegCardEmulationNotifyCb001, TestSize.Level1)
{
    sptr<IOnCardEmulationNotifyCb> callback = nullptr;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->RegCardEmulationNotifyCb(callback);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: GetNotifyCardEmulationCallback001
 * @tc.desc: Test ExternalDepsProxyTest GetNotifyCardEmulationCallback.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, GetNotifyCardEmulationCallback001, TestSize.Level1)
{
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->GetNotifyCardEmulationCallback();
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}
#endif
/**
 * @tc.name: NfcDataGetValue001
 * @tc.desc: Test ExternalDepsProxyTest NfcDataGetValue.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, NfcDataGetValue001, TestSize.Level1)
{
    Uri nfcEnableUri(KITS::NFC_DATA_URI);
    std::string column = "";
    int32_t value = 0;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    KITS::ErrorCode errorCode = externalDepsProxy->NfcDataGetValue(nfcEnableUri, column, value);
    ASSERT_TRUE(errorCode == KITS::ERR_NFC_DATABASE_RW);
}

/**
 * @tc.name: NfcDataSetValue001
 * @tc.desc: Test ExternalDepsProxyTest NfcDataSetValue.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, NfcDataSetValue001, TestSize.Level1)
{
    Uri nfcEnableUri(KITS::NFC_DATA_URI);
    std::string column = "";
    int32_t value = 0;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    KITS::ErrorCode errorCode = externalDepsProxy->NfcDataSetValue(nfcEnableUri, column, value);
    ASSERT_TRUE(errorCode == KITS::ERR_NFC_DATABASE_RW);
}

/**
 * @tc.name: NfcDataSetString001
 * @tc.desc: Test ExternalDepsProxyTest NfcDataSetString.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, NfcDataSetString001, TestSize.Level1)
{
    std::string key = "";
    std::string value = "";
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->NfcDataSetString(key, value);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: NfcDataGetString001
 * @tc.desc: Test ExternalDepsProxyTest NfcDataGetString.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, NfcDataGetString001, TestSize.Level1)
{
    std::string key = "";
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->NfcDataGetString(key);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: PublishNfcStateChanged001
 * @tc.desc: Test ExternalDepsProxyTest PublishNfcStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, PublishNfcStateChanged001, TestSize.Level1)
{
    int newState = 0;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->PublishNfcStateChanged(newState);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: WriteOpenAndCloseHiSysEvent001
 * @tc.desc: Test ExternalDepsProxyTest WriteOpenAndCloseHiSysEvent.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, WriteOpenAndCloseHiSysEvent001, TestSize.Level1)
{
    int openRequestCnt = 0;
    int openFailCnt = 0;
    int closeRequestCnt = 0;
    int closeFailCnt = 0;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->WriteOpenAndCloseHiSysEvent(openRequestCnt, openFailCnt, closeRequestCnt, closeFailCnt);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: WriteHceSwipeResultHiSysEvent001
 * @tc.desc: Test ExternalDepsProxyTest WriteHceSwipeResultHiSysEvent.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, WriteHceSwipeResultHiSysEvent001, TestSize.Level1)
{
    std::string appPackageName = "";
    int hceSwipeCnt = 0;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->WriteHceSwipeResultHiSysEvent(appPackageName, hceSwipeCnt);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: WriteDefaultPaymentAppChangeHiSysEvent001
 * @tc.desc: Test ExternalDepsProxyTest WriteDefaultPaymentAppChangeHiSysEvent.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, WriteDefaultPaymentAppChangeHiSysEvent001, TestSize.Level1)
{
    std::string oldAppPackageName = "";
    std::string newAppPackageName = "";
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->WriteDefaultPaymentAppChangeHiSysEvent(oldAppPackageName, newAppPackageName);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: WriteTagFoundHiSysEvent001
 * @tc.desc: Test ExternalDepsProxyTest WriteTagFoundHiSysEvent.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, WriteTagFoundHiSysEvent001, TestSize.Level1)
{
    std::vector<int> techList;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->WriteTagFoundHiSysEvent(techList);
    techList = {1, 2};
    externalDepsProxy->WriteTagFoundHiSysEvent(techList);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: WritePassiveListenHiSysEvent001
 * @tc.desc: Test ExternalDepsProxyTest WritePassiveListenHiSysEvent.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, WritePassiveListenHiSysEvent001, TestSize.Level1)
{
    int requestCnt = 0;
    int failCnt = 0;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->WritePassiveListenHiSysEvent(requestCnt, failCnt);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: WriteFirmwareUpdateHiSysEvent001
 * @tc.desc: Test ExternalDepsProxyTest WriteFirmwareUpdateHiSysEvent.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, WriteFirmwareUpdateHiSysEvent001, TestSize.Level1)
{
    int requestCnt = 0;
    int failCnt = 0;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->WriteFirmwareUpdateHiSysEvent(requestCnt, failCnt);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: BuildFailedParams001
 * @tc.desc: Test ExternalDepsProxyTest BuildFailedParams.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, BuildFailedParams001, TestSize.Level1)
{
    NfcFailedParams nfcFailedParams;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->BuildFailedParams(nfcFailedParams, MainErrorCode::INIT_SA_FAILED,
        SubErrorCode::DEFAULT_ERR_DEF);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: IsGranted001
 * @tc.desc: Test ExternalDepsProxyTest IsGranted.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, IsGranted001, TestSize.Level1)
{
    std::string permission = "";
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    bool isGranted = externalDepsProxy->IsGranted(permission);
    ASSERT_TRUE(isGranted);
}

/**
 * @tc.name: DispatchTagAbility001
 * @tc.desc: Test ExternalDepsProxyTest DispatchTagAbility.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, DispatchTagAbility001, TestSize.Level1)
{
    std::shared_ptr<KITS::TagInfo> tagInfo = nullptr;
    OHOS::sptr<IRemoteObject> tagServiceIface = nullptr;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->DispatchTagAbility(tagInfo, tagServiceIface);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: StartVibratorOnce001
 * @tc.desc: Test ExternalDepsProxyTest StartVibratorOnce.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, StartVibratorOnce001, TestSize.Level1)
{
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->StartVibratorOnce();
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: GetPaymentAbilityInfos001
 * @tc.desc: Test ExternalDepsProxyTest GetPaymentAbilityInfos.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, GetPaymentAbilityInfos001, TestSize.Level1)
{
    std::vector<AbilityInfo> paymentAbilityInfos;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->GetPaymentAbilityInfos(paymentAbilityInfos);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: GetHceAppsByAid001
 * @tc.desc: Test ExternalDepsProxyTest GetHceAppsByAid.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, GetHceAppsByAid001, TestSize.Level1)
{
    std::string aid = "";
    std::vector<AppDataParser::HceAppAidInfo> hceApps;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->GetHceAppsByAid(aid, hceApps);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}

/**
 * @tc.name: GetHceApps001
 * @tc.desc: Test ExternalDepsProxyTest GetHceApps.
 * @tc.type: FUNC
 */
HWTEST_F(ExternalDepsProxyTest, GetHceApps001, TestSize.Level1)
{
    std::vector<AppDataParser::HceAppAidInfo> hceApps;
    std::shared_ptr<ExternalDepsProxy> externalDepsProxy = std::make_shared<ExternalDepsProxy>();
    externalDepsProxy->GetHceApps(hceApps);
    std::vector<int> discTechList;
    std::vector<ElementName> getDispatchTagAppsByTech = externalDepsProxy->GetDispatchTagAppsByTech(discTechList);
    ASSERT_TRUE(getDispatchTagAppsByTech.size() == 0);
}
}
}
}