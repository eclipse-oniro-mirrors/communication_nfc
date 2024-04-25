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

#include "nfc_service.h"
#include <unistd.h>
#include "app_data_parser.h"
#include "nfc_event_handler.h"
#include "nfc_controller.h"
#include "nfc_polling_params.h"
#include "nfc_sdk_common.h"
#include "nfc_watch_dog.h"
#include "want.h"
#include "nfc_preferences.h"
#include "tag_session.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class NfcPollingManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    std::shared_ptr<NfcService> nfcService_ {};
};

void NfcPollingManagerTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcPollingManagerTest." << std::endl;
}

void NfcPollingManagerTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcPollingManagerTest." << std::endl;
}

void NfcPollingManagerTest::SetUp()
{
    std::cout << " SetUp NfcPollingManagerTest." << std::endl;
}

void NfcPollingManagerTest::TearDown()
{
    std::cout << " TearDown NfcPollingManagerTest." << std::endl;
}

/**
 * @tc.name: IsForegroundEnabled001
 * @tc.desc: Test NfcPollingManager IsForegroundEnabled.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, IsForegroundEnabled001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr<NFC::NfcPollingManager> nfcPollingManager = service->GetNfcPollingManager();
    bool enable = nfcPollingManager.lock()->IsForegroundEnabled();
    ASSERT_TRUE(enable == false);
}
/**
 * @tc.name: DisableForegroundByDeathRcpt001
 * @tc.desc: Test NfcPollingManager DisableForegroundByDeathRcpt.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, DisableForegroundByDeathRcpt001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr<NFC::NfcPollingManager> nfcPollingManager = service->GetNfcPollingManager();
    bool disable = nfcPollingManager.lock()->DisableForegroundByDeathRcpt();
    ASSERT_TRUE(disable == true);
}
/**
 * @tc.name: DisableForegroundDispatch001
 * @tc.desc: Test NfcPollingManager DisableForegroundDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, DisableForegroundDispatch001, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr<NFC::NfcPollingManager> nfcPollingManager = service->GetNfcPollingManager();
    bool disable = nfcPollingManager.lock()->DisableForegroundDispatch(element);
    ASSERT_TRUE(disable == true);
}
/**
 * @tc.name: EnableForegroundDispatch001
 * @tc.desc: Test NfcPollingManager EnableForegroundDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, EnableForegroundDispatch001, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    std::vector<uint32_t> discTech = {1, 2, 4, 5, 10};
    const sptr<KITS::IForegroundCallback> callback = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr<NFC::NfcPollingManager> nfcPollingManager = service->GetNfcPollingManager();
    bool enable = nfcPollingManager.lock()->EnableForegroundDispatch(element, discTech, callback);
    ASSERT_TRUE(enable == false);
}
/**
 * @tc.name: GetForegroundData001
 * @tc.desc: Test NfcPollingManager GetForegroundData.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, GetForegroundData001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr<NFC::NfcPollingManager> nfcPollingManager = service->GetNfcPollingManager();
    std::shared_ptr<NfcPollingManager::ForegroundRegistryData> data = nfcPollingManager.lock()->GetForegroundData();
    ASSERT_TRUE(data != nullptr);
}

/**
 * @tc.name: GetPollingParameters001
 * @tc.desc: Test NfcPollingManager GetPollingParameters.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, GetPollingParameters001, TestSize.Level1)
{
    int screenState = 0;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr<NFC::NfcPollingManager> nfcPollingManager = service->GetNfcPollingManager();
    std::shared_ptr<NfcPollingParams> nfcPollingParams = nfcPollingManager.lock()->GetPollingParameters(screenState);
    ASSERT_TRUE(nfcPollingParams != nullptr);
}

/**
 * @tc.name: HandleScreenChanged001
 * @tc.desc: Test NfcPollingManager HandleScreenChanged.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, HandleScreenChanged001, TestSize.Level1)
{
    int screenState = 1;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr<NFC::NfcPollingManager> nfcPollingManager = service->GetNfcPollingManager();
    nfcPollingManager.lock()->HandleScreenChanged(screenState);
    std::shared_ptr<NfcPollingParams> nfcPollingParams = nfcPollingManager.lock()->GetPollingParameters(screenState);
    ASSERT_TRUE(nfcPollingParams != nullptr);
}

/**
 * @tc.name: HandlePackageUpdated001
 * @tc.desc: Test NfcPollingManager HandlePackageUpdated.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, HandlePackageUpdated001, TestSize.Level1)
{
    int screenState = 1;
    std::shared_ptr<EventFwk::CommonEventData> data = std::make_shared<EventFwk::CommonEventData>();
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr<NFC::NfcPollingManager> nfcPollingManager = service->GetNfcPollingManager();
    nfcPollingManager.lock()->HandlePackageUpdated(data);
    std::shared_ptr<NfcPollingParams> nfcPollingParams = nfcPollingManager.lock()->GetPollingParameters(screenState);
    ASSERT_TRUE(nfcPollingParams != nullptr);
}

/**
 * @tc.name: HandlePackageUpdated002
 * @tc.desc: Test NfcPollingManager HandlePackageUpdated.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, HandlePackageUpdated002, TestSize.Level1)
{
    std::shared_ptr<EventFwk::CommonEventData> data = std::make_shared<EventFwk::CommonEventData>();
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    data->SetWant(want);
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr<NFC::NfcPollingManager> nfcPollingManager = service->GetNfcPollingManager();
    bool res = nfcPollingManager.lock()->HandlePackageUpdated(data);
    ASSERT_TRUE(!res);
}

/**
 * @tc.name: HandlePackageUpdated003
 * @tc.desc: Test NfcPollingManager HandlePackageUpdated.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, HandlePackageUpdated003, TestSize.Level1)
{
    std::shared_ptr<EventFwk::CommonEventData> data = std::make_shared<EventFwk::CommonEventData>();
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    data->SetWant(want);
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr<NFC::NfcPollingManager> nfcPollingManager = service->GetNfcPollingManager();
    bool res = nfcPollingManager.lock()->HandlePackageUpdated(data);
    ASSERT_TRUE(!res);
}

/**
 * @tc.name: HandlePackageUpdated004
 * @tc.desc: Test NfcPollingManager HandlePackageUpdated.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, HandlePackageUpdated004, TestSize.Level1)
{
    std::shared_ptr<EventFwk::CommonEventData> data = std::make_shared<EventFwk::CommonEventData>();
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    data->SetWant(want);
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr<NFC::NfcPollingManager> nfcPollingManager = service->GetNfcPollingManager();
    bool res = nfcPollingManager.lock()->HandlePackageUpdated(data);
    ASSERT_TRUE(!res);
}

/**
 * @tc.name: HandlePackageUpdated005
 * @tc.desc: Test NfcPollingManager HandlePackageUpdated.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, HandlePackageUpdated005, TestSize.Level1)
{
    std::shared_ptr<EventFwk::CommonEventData> data = std::make_shared<EventFwk::CommonEventData>();
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_USER_PRESENT);
    data->SetWant(want);
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr<NFC::NfcPollingManager> nfcPollingManager = service->GetNfcPollingManager();
    bool res = nfcPollingManager.lock()->HandlePackageUpdated(data);
    ASSERT_TRUE(!res);
}

/**
 * @tc.name: SendTagToForeground001
 * @tc.desc: Test NfcPollingManager SendTagToForeground.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, SendTagToForeground001, TestSize.Level1)
{
    KITS::TagInfoParcelable* tagInfo = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr<NFC::NfcPollingManager> nfcPollingManager = service->GetNfcPollingManager();
    nfcPollingManager.lock()->SendTagToForeground(tagInfo);
    ASSERT_TRUE(service != nullptr);
}

/**
 * @tc.name: EnableReaderMode001
 * @tc.desc: Test NfcPollingManager EnableReaderMode.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, EnableReaderMode001, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    std::vector<uint32_t> discTech;
    sptr<KITS::IReaderModeCallback> callback = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr<NFC::NfcPollingManager> nfcPollingManager = service->GetNfcPollingManager();
    bool res = nfcPollingManager.lock()->EnableReaderMode(element, discTech, callback);
    ASSERT_TRUE(!res);
}
}
}
}