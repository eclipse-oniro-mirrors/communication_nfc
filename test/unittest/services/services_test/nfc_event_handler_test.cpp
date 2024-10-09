/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "nfc_event_handler.h"
#include "nci_native_selector.h"

namespace OHOS {
namespace NFC {
namespace TAG {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class NfcEventHandlerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NfcEventHandlerTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcEventHandlerTest." << std::endl;
}

void NfcEventHandlerTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcEventHandlerTest." << std::endl;
}

void NfcEventHandlerTest::SetUp()
{
    std::cout << " SetUp NfcEventHandlerTest." << std::endl;
}

void NfcEventHandlerTest::TearDown()
{
    std::cout << " TearDown NfcEventHandlerTest." << std::endl;
}

/**
 * @tc.name: ProcessEvent001
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent001, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::shared_ptr<TAG::TagDispatcher> tagDispatcher = std::make_shared<TAG::TagDispatcher>(service);
    std::weak_ptr<CeService> ceService;
    std::weak_ptr<NfcPollingManager> nfcPollingManager;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    std::weak_ptr<NfcRoutingManager> nfcRoutingManager;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_TAG_FOUND), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent002
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent002, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::shared_ptr<TAG::TagDispatcher> tagDispatcher = std::make_shared<TAG::TagDispatcher>(service);
    std::weak_ptr<CeService> ceService;
    std::weak_ptr<NfcPollingManager> nfcPollingManager;
    std::weak_ptr<NfcRoutingManager> nfcRoutingManager;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_TAG_DEBOUNCE), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent003
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent003, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::shared_ptr<TAG::TagDispatcher> tagDispatcher = std::make_shared<TAG::TagDispatcher>(service);
    std::weak_ptr<CeService> ceService;
    std::weak_ptr<NfcPollingManager> nfcPollingManager;
    std::weak_ptr<NfcRoutingManager> nfcRoutingManager;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_TAG_LOST), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent004
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent004, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::weak_ptr<TAG::TagDispatcher> tagDispatcher;
    std::weak_ptr<CeService> ceService;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    std::weak_ptr<NCI::INciTagInterface> nciTagProxy;
    std::shared_ptr<NfcPollingManager> nfcPollingManager =
        std::make_shared<NfcPollingManager>(service, nciNfccProxy, nciTagProxy);
    std::weak_ptr<NfcRoutingManager> nfcRoutingManager;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_SCREEN_CHANGED), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent006
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent006, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::weak_ptr<TAG::TagDispatcher> tagDispatcher;
    std::weak_ptr<CeService> ceService;
    std::weak_ptr<NfcPollingManager> nfcPollingManager;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_COMMIT_ROUTING), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    std::weak_ptr<NCI::INciCeInterface> nciCeProxy;
    std::shared_ptr<NfcRoutingManager> nfcRoutingManager =
        std::make_shared<NfcRoutingManager>(nfcEventHandler, nciNfccProxy, nciCeProxy, service);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent007
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent007, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::weak_ptr<TAG::TagDispatcher> tagDispatcher;
    std::weak_ptr<CeService> ceService;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    std::weak_ptr<NfcPollingManager> nfcPollingManager;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_COMPUTE_ROUTING_PARAMS), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = NCI::NciNativeSelector::GetInstance().GetNciCeInterface();
    std::shared_ptr<NfcRoutingManager> nfcRoutingManager =
        std::make_shared<NfcRoutingManager>(nfcEventHandler, nciNfccProxy, nciCeProxy, service);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent008
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent008, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::weak_ptr<TAG::TagDispatcher> tagDispatcher;
    std::weak_ptr<NfcRoutingManager> nfcRoutingManager;
    std::weak_ptr<NfcPollingManager> nfcPollingManager;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_FIELD_ACTIVATED), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = NCI::NciNativeSelector::GetInstance().GetNciCeInterface();
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(service, nciCeProxy);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent009
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent009, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::weak_ptr<TAG::TagDispatcher> tagDispatcher;
    std::weak_ptr<NfcRoutingManager> nfcRoutingManager;
    std::weak_ptr<NfcPollingManager> nfcPollingManager;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_FIELD_DEACTIVATED), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = NCI::NciNativeSelector::GetInstance().GetNciCeInterface();
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(service, nciCeProxy);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent010
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent010, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::weak_ptr<TAG::TagDispatcher> tagDispatcher;
    std::weak_ptr<NfcRoutingManager> nfcRoutingManager;
    std::weak_ptr<NfcPollingManager> nfcPollingManager;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_ON), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = NCI::NciNativeSelector::GetInstance().GetNciCeInterface();
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(service, nciCeProxy);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent011
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent011, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::weak_ptr<TAG::TagDispatcher> tagDispatcher;
    std::weak_ptr<NfcRoutingManager> nfcRoutingManager;
    std::weak_ptr<NfcPollingManager> nfcPollingManager;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = NCI::NciNativeSelector::GetInstance().GetNciCeInterface();
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(service, nciCeProxy);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent012
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent012, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::weak_ptr<TAG::TagDispatcher> tagDispatcher;
    std::weak_ptr<NfcRoutingManager> nfcRoutingManager;
    std::weak_ptr<NfcPollingManager> nfcPollingManager;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF_TIMEOUT), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = NCI::NciNativeSelector::GetInstance().GetNciCeInterface();
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(service, nciCeProxy);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent013
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent013, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    std::weak_ptr<TAG::TagDispatcher> tagDispatcher;
    std::weak_ptr<NfcRoutingManager> nfcRoutingManager;
    std::weak_ptr<NfcPollingManager> nfcPollingManager;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_SHUTDOWN), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = NCI::NciNativeSelector::GetInstance().GetNciCeInterface();
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(service, nciCeProxy);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent014
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent014, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::weak_ptr<TAG::TagDispatcher> tagDispatcher;
    std::weak_ptr<NfcRoutingManager> nfcRoutingManager;
    std::weak_ptr<NfcPollingManager> nfcPollingManager;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_DATA_SHARE_READY), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = NCI::NciNativeSelector::GetInstance().GetNciCeInterface();
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(service, nciCeProxy);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent015
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent015, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::weak_ptr<TAG::TagDispatcher> tagDispatcher;
    std::weak_ptr<NfcRoutingManager> nfcRoutingManager;
    std::weak_ptr<NfcPollingManager> nfcPollingManager;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_VENDOR_EVENT),
        static_cast<int64_t>(KITS::VENDOR_APP_INIT_DONE));
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = NCI::NciNativeSelector::GetInstance().GetNciCeInterface();
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(service, nciCeProxy);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent016
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent016, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::weak_ptr<TAG::TagDispatcher> tagDispatcher;
    std::weak_ptr<NfcRoutingManager> nfcRoutingManager;
    std::weak_ptr<NfcPollingManager> nfcPollingManager;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_VENDOR_EVENT),
        static_cast<int64_t>(KITS::VENDOR_APP_CHANGE));
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = NCI::NciNativeSelector::GetInstance().GetNciCeInterface();
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(service, nciCeProxy);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent017
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent017, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    std::weak_ptr<TAG::TagDispatcher> tagDispatcher;
    std::weak_ptr<NfcRoutingManager> nfcRoutingManager;
    std::weak_ptr<NfcPollingManager> nfcPollingManager;
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_VENDOR_EVENT), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = NCI::NciNativeSelector::GetInstance().GetNciCeInterface();
    std::shared_ptr<CeService> ceService = std::make_shared<CeService>(service, nciCeProxy);
    nfcEventHandler->Intialize(tagDispatcher, ceService, nfcPollingManager, nfcRoutingManager, nciNfccProxy);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent018
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent018, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_WIFI_ENABLE_TIMEOUT), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent019
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent019, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_WIFI_CONNECT_TIMEOUT), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent020
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent020, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_WIFI_ENABLED), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent021
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent021, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_WIFI_CONNECTED), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent022
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent022, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_WIFI_NTF_CLICKED), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent023
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent023, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_BT_ENABLE_TIMEOUT), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent024
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent024, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_BT_PAIR_TIMEOUT), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent025
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent025, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_BT_CONNECT_TIMEOUT), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent026
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent026, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_BT_ENABLED), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent027
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent027, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_BT_PAIR_STATUS_CHANGED), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent028
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent028, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_BT_CONNECT_STATUS_CHANGED), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent029
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent029, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(static_cast<uint32_t>(NfcCommonEvent::MSG_BT_NTF_CLICKED), 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->ProcessEvent(event);
}

/**
 * @tc.name: ProcessEvent030
 * @tc.desc: Test NfcEventHandlerTest ProcessEvent.
 * @tc.type: FUNC
 */
HWTEST_F(NfcEventHandlerTest, ProcessEvent030, TestSize.Level1)
{
    std::shared_ptr<AppExecFwk::EventRunner> runner = nullptr;
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    AppExecFwk::InnerEvent::Pointer event =
        AppExecFwk::InnerEvent::Get(-1, 0);
    std::shared_ptr<NfcEventHandler> nfcEventHandler = std::make_shared<NfcEventHandler>(runner, service);
    nfcEventHandler->ProcessEvent(event);
}
} // namespace TEST
} // namespace TAG
} // namespace NFC
} // namespace OHOS