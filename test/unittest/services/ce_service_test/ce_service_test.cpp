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

#include "ce_service.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class CeServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CeServiceTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase CeServiceTest." << std::endl;
}

void CeServiceTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase CeServiceTest." << std::endl;
}

void CeServiceTest::SetUp()
{
    std::cout << " SetUp CeServiceTest." << std::endl;
}

void CeServiceTest::TearDown()
{
    std::cout << " TearDown CeServiceTest." << std::endl;
}

/**
 * @tc.name: RegHceCmdCallback001
 * @tc.desc: Test CeServiceTest RegHceCmdCallback.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, RegHceCmdCallback001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    const sptr<KITS::IHceCmdCallback> callback = nullptr;
    const std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: SendHostApduData001
 * @tc.desc: Test CeServiceTest SendHostApduData.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, SendHostApduData001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    std::string hexCmdData = "";
    bool raw = false;
    std::string hexRespData = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool sendHostApduData = ceService->SendHostApduData(hexCmdData, raw, hexRespData, callerToken);
    ASSERT_TRUE(sendHostApduData == false);
}

/**
 * @tc.name: SendHostApduData002
 * @tc.desc: Test CeServiceTest SendHostApduData.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, SendHostApduData002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    std::string hexCmdData = "";
    bool raw = false;
    std::string hexRespData = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool sendHostApduData = ceService->SendHostApduData(hexCmdData, raw, hexRespData, callerToken);
    ASSERT_TRUE(sendHostApduData == false);
}

/**
 * @tc.name: InitConfigAidRouting001
 * @tc.desc: Test CeServiceTest InitConfigAidRouting.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, InitConfigAidRouting001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    bool ret = ceService->InitConfigAidRouting();
    ASSERT_TRUE(ret == false);
}

/**
 * @tc.name: OnDefaultPaymentServiceChange001
 * @tc.desc: Test CeServiceTest OnDefaultPaymentServiceChange.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnDefaultPaymentServiceChange001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    ceService->OnDefaultPaymentServiceChange();
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnAppAddOrChangeOrRemove001
 * @tc.desc: Test CeServiceTest OnAppAddOrChangeOrRemove.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnAppAddOrChangeOrRemove001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    std::shared_ptr<EventFwk::CommonEventData> data = nullptr;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    ceService->OnAppAddOrChangeOrRemove(data);
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnAppAddOrChangeOrRemove002
 * @tc.desc: Test CeServiceTest OnAppAddOrChangeOrRemove.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnAppAddOrChangeOrRemove002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    std::shared_ptr<EventFwk::CommonEventData> data = std::make_shared<EventFwk::CommonEventData>();
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    ceService->OnAppAddOrChangeOrRemove(data);
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnAppAddOrChangeOrRemove003
 * @tc.desc: Test CeServiceTest OnAppAddOrChangeOrRemove.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnAppAddOrChangeOrRemove003, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    std::shared_ptr<EventFwk::CommonEventData> data = std::make_shared<EventFwk::CommonEventData>();
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    data->SetWant(want);
    ceService->OnAppAddOrChangeOrRemove(data);
}

/**
 * @tc.name: OnAppAddOrChangeOrRemove004
 * @tc.desc: Test CeServiceTest OnAppAddOrChangeOrRemove.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnAppAddOrChangeOrRemove004, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    std::shared_ptr<EventFwk::CommonEventData> data = std::make_shared<EventFwk::CommonEventData>();
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    data->SetWant(want);
    ceService->OnAppAddOrChangeOrRemove(data);
}

/**
 * @tc.name: OnAppAddOrChangeOrRemove005
 * @tc.desc: Test CeServiceTest OnAppAddOrChangeOrRemove.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnAppAddOrChangeOrRemove005, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    std::shared_ptr<EventFwk::CommonEventData> data = std::make_shared<EventFwk::CommonEventData>();
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    data->SetWant(want);
    ceService->OnAppAddOrChangeOrRemove(data);
}

/**
 * @tc.name: OnAppAddOrChangeOrRemove006
 * @tc.desc: Test CeServiceTest OnAppAddOrChangeOrRemove.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnAppAddOrChangeOrRemove006, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    std::shared_ptr<EventFwk::CommonEventData> data = std::make_shared<EventFwk::CommonEventData>();
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_USER_PRESENT);
    data->SetWant(want);
    ceService->OnAppAddOrChangeOrRemove(data);
}

/**
 * @tc.name: OnAppAddOrChangeOrRemove007
 * @tc.desc: Test CeServiceTest OnAppAddOrChangeOrRemove.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnAppAddOrChangeOrRemove007, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    std::shared_ptr<EventFwk::CommonEventData> data = std::make_shared<EventFwk::CommonEventData>();
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    ElementName element("123", "456", "789");
    want.SetElement(element);
    data->SetWant(want);
    ceService->OnAppAddOrChangeOrRemove(data);
}

/**
 * @tc.name: OnAppAddOrChangeOrRemove008
 * @tc.desc: Test CeServiceTest OnAppAddOrChangeOrRemove.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnAppAddOrChangeOrRemove008, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    std::shared_ptr<EventFwk::CommonEventData> data = std::make_shared<EventFwk::CommonEventData>();
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    ElementName element("123", "456", "789");
    want.SetElement(element);
    data->SetWant(want);
    ceService->OnAppAddOrChangeOrRemove(data);
}

/**
 * @tc.name: ConfigRoutingAndCommit001
 * @tc.desc: Test CeServiceTest ConfigRoutingAndCommit.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, ConfigRoutingAndCommit001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    ceService->ConfigRoutingAndCommit();
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: ConfigRoutingAndCommit002
 * @tc.desc: Test CeServiceTest ConfigRoutingAndCommit.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, ConfigRoutingAndCommit002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    ceService->ConfigRoutingAndCommit();
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: HandleFieldDeactivated001
 * @tc.desc: Test CeServiceTest HandleFieldDeactivated.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, HandleFieldDeactivated001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    ceService->HandleFieldDeactivated();
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnCardEmulationData001
 * @tc.desc: Test CeServiceTest OnCardEmulationData.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnCardEmulationData001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    std::vector<uint8_t> data;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    ceService->OnCardEmulationData(data);
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnCardEmulationData002
 * @tc.desc: Test CeServiceTest OnCardEmulationData.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnCardEmulationData002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    std::vector<uint8_t> data;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    ceService->OnCardEmulationData(data);
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnCardEmulationActivated001
 * @tc.desc: Test CeServiceTest OnCardEmulationActivated.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnCardEmulationActivated001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    ceService->OnCardEmulationActivated();
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnCardEmulationActivated002
 * @tc.desc: Test CeServiceTest OnCardEmulationActivated.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnCardEmulationActivated002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    ceService->OnCardEmulationActivated();
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnCardEmulationDeactivated001
 * @tc.desc: Test CeServiceTest OnCardEmulationDeactivated.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnCardEmulationDeactivated001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    ceService->OnCardEmulationDeactivated();
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnCardEmulationDeactivated002
 * @tc.desc: Test CeServiceTest OnCardEmulationDeactivated.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, OnCardEmulationDeactivated002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    ceService->OnCardEmulationDeactivated();
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: PublishFieldOnOrOffCommonEvent001
 * @tc.desc: Test CeServiceTest PublishFieldOnOrOffCommonEvent.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, PublishFieldOnOrOffCommonEvent001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    bool isFieldOn = false;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    ceService->PublishFieldOnOrOffCommonEvent(isFieldOn);
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: PublishFieldOnOrOffCommonEvent002
 * @tc.desc: Test CeServiceTest PublishFieldOnOrOffCommonEvent.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, PublishFieldOnOrOffCommonEvent002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    bool isFieldOn = true;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    ceService->PublishFieldOnOrOffCommonEvent(isFieldOn);
    bool regHceCmdCallback = ceService->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: UnRegHceCmdCallback001
 * @tc.desc: Test CeServiceTest UnRegHceCmdCallback.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, UnRegHceCmdCallback001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    const std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool unRegHceCmdCallback = ceService->UnRegHceCmdCallback(type, callerToken);
    ASSERT_TRUE(unRegHceCmdCallback == false);
}

/**
 * @tc.name: UnRegAllCallback001
 * @tc.desc: Test CeServiceTest UnRegAllCallback.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, UnRegAllCallback001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool unRegAllCallback = ceService->UnRegAllCallback(callerToken);
    ASSERT_TRUE(unRegAllCallback == false);
}

/**
 * @tc.name: UnRegAllCallback002
 * @tc.desc: Test CeServiceTest UnRegAllCallback.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, UnRegAllCallback002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool unRegAllCallback = ceService->UnRegAllCallback(callerToken);
    ASSERT_TRUE(unRegAllCallback == false);
}

/**
 * @tc.name: HandleFieldActivated001
 * @tc.desc: Test CeServiceTest HandleFieldActivated.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, HandleFieldActivated001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    Security::AccessToken::AccessTokenID callerToken = 0;
    ceService->HandleFieldActivated();
    bool unRegAllCallback = ceService->UnRegAllCallback(callerToken);
    ASSERT_TRUE(unRegAllCallback == false);
}

/**
 * @tc.name: HandleWhenRemoteDie001
 * @tc.desc: Test CeServiceTest HandleWhenRemoteDie.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, HandleWhenRemoteDie001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool res = ceService->HandleWhenRemoteDie(callerToken);
    ASSERT_TRUE(res == false);
}

/**
 * @tc.name: SearchElementByAid001
 * @tc.desc: Test CeServiceTest SearchElementByAid.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, SearchElementByAid001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    std::string aid = "";
    ElementName aidElement;
    ceService->SearchElementByAid(aid, aidElement);
}

/**
 * @tc.name: SearchElementByAid002
 * @tc.desc: Test CeServiceTest SearchElementByAid.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, SearchElementByAid002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ceService->Initialize();
    std::string aid = "123";
    ElementName aidElement;
    ceService->SearchElementByAid(aid, aidElement);
}

/**
 * @tc.name: StartHce001
 * @tc.desc: Test CeServiceTest StartHce.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, StartHce001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ElementName element;
    std::vector<std::string> aids;
    bool res = ceService->StartHce(element, aids);
    ASSERT_TRUE(!res);
}

/**
 * @tc.name: StartHce002
 * @tc.desc: Test CeServiceTest StartHce.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, StartHce002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    ElementName element;
    std::vector<std::string> aids;
    bool res = ceService->StartHce(element, aids);
    ASSERT_TRUE(!res);
}

/**
 * @tc.name: GetDefaultPaymentType001
 * @tc.desc: Test CeServiceTest GetDefaultPaymentType.
 * @tc.type: FUNC
 */
HWTEST_F(CeServiceTest, GetDefaultPaymentType001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(nfcService, nciCeProxy);
    KITS::DefaultPaymentType res = ceService->GetDefaultPaymentType();
    ASSERT_TRUE(res == KITS::DefaultPaymentType::TYPE_EMPTY);
}
}
}
}