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

#include <gtest/gtest.h>
#include <thread>

#include "loghelper.h"
#include "nfc_param_util.h"
#include "nfc_service.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class NfcServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NfcServiceTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcServiceTest." << std::endl;
}

void NfcServiceTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcServiceTest." << std::endl;
}

void NfcServiceTest::SetUp()
{
    std::cout << " SetUp NfcServiceTest." << std::endl;
}

void NfcServiceTest::TearDown()
{
    std::cout << " TearDown NfcServiceTest." << std::endl;
}

/**
 * @tc.name: ShouldTurnOnNfc001
 * @tc.desc: Test NfcServiceTest ShouldTurnOnNfc.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, ShouldTurnOnNfc001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    NfcParamUtil::UpdateNfcStateToParam(KITS::STATE_TURNING_ON);
    NfcParamUtil::UpdateNfcStateToParam(KITS::STATE_TURNING_OFF);
    NfcParamUtil::UpdateNfcStateToParam(KITS::STATE_ON);
    service->nfcState_ = KITS::STATE_OFF;
    InfoLog("ShouldTurnOnNfc 1 %{public}d", service->ShouldTurnOnNfc());

    service->nfcState_ = KITS::STATE_ON;
    NfcParamUtil::SetNfcParamStr(IS_FIRST_TIME_ENABLE_PARAM_NAME, "false");
    InfoLog("ShouldTurnOnNfc 2 %{public}d", service->ShouldTurnOnNfc());

    NfcParamUtil::SetNfcParamStr(IS_FIRST_TIME_ENABLE_PARAM_NAME, "true");
    InfoLog("ShouldTurnOnNfc 3 %{public}d", service->ShouldTurnOnNfc());

    NfcParamUtil::UpdateNfcStateToParam(KITS::STATE_OFF);
    InfoLog("ShouldTurnOnNfc 4 %{public}d", service->ShouldTurnOnNfc());

    NfcParamUtil::SetNfcParamStr(IS_FIRST_TIME_ENABLE_PARAM_NAME, "false");
    InfoLog("ShouldTurnOnNfc 5 %{public}d", service->ShouldTurnOnNfc());

    service->nfcState_ = KITS::STATE_OFF;
    InfoLog("ShouldTurnOnNfc 6 %{public}d", service->ShouldTurnOnNfc());

    NfcParamUtil::SetNfcParamStr(IS_FIRST_TIME_ENABLE_PARAM_NAME, "true");
    InfoLog("ShouldTurnOnNfc 7 %{public}d", service->ShouldTurnOnNfc());
    ASSERT_TRUE(NfcParamUtil::GetNfcStateFromParam() >= 0);
}

/**
 * @tc.name: GetInstance001
 * @tc.desc: Test NfcServiceTest GetInstance.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, GetInstance001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr nfcService = service->GetInstance();
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: UnloadNfcSa001
 * @tc.desc: Test NfcServiceTest UnloadNfcSa.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, UnloadNfcSa001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    nfcService->UnloadNfcSa();
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: OnTagDiscovered001
 * @tc.desc: Test NfcServiceTest OnTagDiscovered.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, OnTagDiscovered001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    uint32_t tagDiscId = 1;
    nfcService->OnTagDiscovered(tagDiscId);
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: GetTagDispatcher001
 * @tc.desc: Test NfcServiceTest GetTagDispatcher.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, GetTagDispatcher001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    std::weak_ptr<TAG::TagDispatcher> tagDispatcher = nfcService->GetTagDispatcher();
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: OnTagLost001
 * @tc.desc: Test NfcServiceTest OnTagLost.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, OnTagLost001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    uint32_t tagDiscId = 1;
    nfcService->OnTagLost(tagDiscId);
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: FieldActivated001
 * @tc.desc: Test NfcServiceTest FieldActivated.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, FieldActivated001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    nfcService->FieldActivated();
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: FieldDeactivated001
 * @tc.desc: Test NfcServiceTest FieldDeactivated.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, FieldDeactivated001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    nfcService->FieldDeactivated();
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: OnVendorEvent001
 * @tc.desc: Test NfcServiceTest OnVendorEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, OnVendorEvent001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    int eventType = 0;
    int arg1 = 1;
    std::string arg2 = "test";
    nfcService->OnVendorEvent(eventType, arg1, arg2);
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: OnCardEmulationData001
 * @tc.desc: Test NfcServiceTest OnCardEmulationData.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, OnCardEmulationData001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    std::vector<uint8_t> data;
    nfcService->OnCardEmulationData(data);
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: OnCardEmulationActivated001
 * @tc.desc: Test NfcServiceTest OnCardEmulationActivated.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, OnCardEmulationActivated001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    nfcService->OnCardEmulationActivated();
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: OnCardEmulationDeactivated001
 * @tc.desc: Test NfcServiceTest OnCardEmulationDeactivated.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, OnCardEmulationDeactivated001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    nfcService->OnCardEmulationDeactivated();
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: CheckNfcState001
 * @tc.desc: Test NfcServiceTest CheckNfcState.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, CheckNfcState001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService::NfcSwitchEventHandler> nfcSwitchEventHandler =
        std::make_shared<NfcService::NfcSwitchEventHandler>(runner, nfcService);
    int param = 1;
    bool ret = nfcSwitchEventHandler->CheckNfcState(param);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: CheckNfcState002
 * @tc.desc: Test NfcServiceTest CheckNfcState.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, CheckNfcState002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService::NfcSwitchEventHandler> nfcSwitchEventHandler =
        std::make_shared<NfcService::NfcSwitchEventHandler>(runner, nfcService);
    int param = 1;
    bool ret = nfcSwitchEventHandler->CheckNfcState(param);
    ASSERT_TRUE(!ret);
}

/**
 * @tc.name: ProcessEvent001
 * @tc.desc: Test NfcServiceTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, ProcessEvent001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService::NfcSwitchEventHandler> nfcSwitchEventHandler =
        std::make_shared<NfcService::NfcSwitchEventHandler>(runner, nfcService);
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_TAG_FOUND), 0);
    nfcSwitchEventHandler->ProcessEvent(event);
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: ProcessEvent002
 * @tc.desc: Test NfcServiceTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, ProcessEvent002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService::NfcSwitchEventHandler> nfcSwitchEventHandler =
        std::make_shared<NfcService::NfcSwitchEventHandler>(runner, nfcService);
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_TAG_FOUND), 0);
    nfcSwitchEventHandler->ProcessEvent(event);
    ASSERT_TRUE(nfcService == nullptr);
}

/**
 * @tc.name: IsMaxSwitchRetryTime001
 * @tc.desc: Test NfcServiceTest IsMaxSwitchRetryTime.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, IsMaxSwitchRetryTime001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    int ret = nfcService->IsMaxSwitchRetryTime();
    ASSERT_TRUE(!ret);
}

/**
 * @tc.name: SetRegisterCallBack001
 * @tc.desc: Test NfcServiceTest SetRegisterCallBack.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, SetRegisterCallBack001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    sptr<INfcControllerCallback> callBack = nullptr;
    std::string type = "test";
    Security::AccessToken::AccessTokenID callerToken = static_cast<Security::AccessToken::AccessTokenID>(0);
    nfcService->SetRegisterCallBack(callBack, type, callerToken);
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: UpdateNfcState001
 * @tc.desc: Test NfcServiceTest UpdateNfcState.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, UpdateNfcState001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    int newState = 1;
    nfcService->UpdateNfcState(newState);
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: GetScreenState001
 * @tc.desc: Test NfcServiceTest GetScreenState.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, GetScreenState001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    nfcService->GetScreenState();
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: GetNciVersion001
 * @tc.desc: Test NfcServiceTest GetNciVersion.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, GetNciVersion001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    int ret = nfcService->GetNciVersion();
    ASSERT_TRUE(ret == 0);
}

/**
 * @tc.name: SetupUnloadNfcSaTimer001
 * @tc.desc: Test NfcServiceTest SetupUnloadNfcSaTimer.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, SetupUnloadNfcSaTimer001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    bool shouldRestartTimer = true;
    nfcService->SetupUnloadNfcSaTimer(shouldRestartTimer);
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: SetupUnloadNfcSaTimer002
 * @tc.desc: Test NfcServiceTest SetupUnloadNfcSaTimer.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, SetupUnloadNfcSaTimer002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    bool shouldRestartTimer = false;
    nfcService->SetupUnloadNfcSaTimer(shouldRestartTimer);
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: CancelUnloadNfcSaTimer001
 * @tc.desc: Test NfcServiceTest CancelUnloadNfcSaTimer.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, CancelUnloadNfcSaTimer001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    nfcService->CancelUnloadNfcSaTimer();
    ASSERT_TRUE(nfcService != nullptr);
}

/**
 * @tc.name: NotifyMessageToVendor
 * @tc.desc: Test NfcServiceTest NotifyMessageToVendor.
 * @tc.type: FUNC
*/
HWTEST_F(NfcServiceTest, NotifyMessageToVendor, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->NotifyMessageToVendor(KITS::NOTIFY_TAG_DISCONNECT, "");
    ASSERT_TRUE(nfcService != nullptr);
}
}
}
}