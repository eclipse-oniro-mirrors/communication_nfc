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

#include "host_card_emulation_manager.h"
#include "ce_service.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class HostCardEmulationManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HostCardEmulationManagerTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase HostCardEmulationManagerTest." << std::endl;
}

void HostCardEmulationManagerTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase HostCardEmulationManagerTest." << std::endl;
}

void HostCardEmulationManagerTest::SetUp()
{
    std::cout << " SetUp HostCardEmulationManagerTest." << std::endl;
}

void HostCardEmulationManagerTest::TearDown()
{
    std::cout << " TearDown HostCardEmulationManagerTest." << std::endl;
}

/**
 * @tc.name: OnHostCardEmulationDataNfcA001
 * @tc.desc: Test HostCardEmulationManagerTest OnHostCardEmulationDataNfcA.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, OnHostCardEmulationDataNfcA001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::vector<uint8_t> data;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    hostCardEmulationManager->OnHostCardEmulationDataNfcA(data);
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = hostCardEmulationManager->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnHostCardEmulationDataNfcA002
 * @tc.desc: Test HostCardEmulationManagerTest OnHostCardEmulationDataNfcA.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, OnHostCardEmulationDataNfcA002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    std::vector<uint8_t> data = {1, 2, 3, 4, 5, 6};
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    hostCardEmulationManager->OnHostCardEmulationDataNfcA(data);
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = hostCardEmulationManager->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnCardEmulationActivated001
 * @tc.desc: Test HostCardEmulationManagerTest OnCardEmulationActivated.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, OnCardEmulationActivated001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    hostCardEmulationManager->OnCardEmulationActivated();
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = hostCardEmulationManager->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: OnCardEmulationDeactivated001
 * @tc.desc: Test HostCardEmulationManagerTest OnCardEmulationDeactivated.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, OnCardEmulationDeactivated001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    hostCardEmulationManager->OnCardEmulationDeactivated();
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = hostCardEmulationManager->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: SendHostApduData001
 * @tc.desc: Test HostCardEmulationManagerTest SendHostApduData.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, SendHostApduData001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    std::string hexCmdData = "";
    bool raw = false;
    std::string hexRespData = "";
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool sendHostApduData = hostCardEmulationManager->SendHostApduData(hexCmdData, raw, hexRespData, callerToken);
    ASSERT_TRUE(sendHostApduData == false);
}

/**
 * @tc.name: SendHostApduData002
 * @tc.desc: Test HostCardEmulationManagerTest SendHostApduData.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, SendHostApduData002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    std::string hexCmdData = "";
    bool raw = false;
    std::string hexRespData = "";
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool sendHostApduData = hostCardEmulationManager->SendHostApduData(hexCmdData, raw, hexRespData, callerToken);
    ASSERT_TRUE(sendHostApduData == false);
}

/**
 * @tc.name: HandleQueueData001
 * @tc.desc: Test HostCardEmulationManagerTest HandleQueueData.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, HandleQueueData001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    hostCardEmulationManager->HandleQueueData();
    ASSERT_TRUE(hostCardEmulationManager != nullptr);
}

/**
 * @tc.name: UnRegHceCmdCallback001
 * @tc.desc: Test HostCardEmulationManagerTest UnRegHceCmdCallback.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, UnRegHceCmdCallback001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    std::string type = "";
    Security::AccessToken::AccessTokenID callerToken = 0;
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    bool res = hostCardEmulationManager->UnRegHceCmdCallback(type, callerToken);
    ASSERT_TRUE(!res);
}

/**
 * @tc.name: NfcGetBundleMgrProxy001
 * @tc.desc: Test HostCardEmulationManagerTest NfcGetBundleMgrProxy.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, NfcGetBundleMgrProxy001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    hostCardEmulationManager->NfcGetBundleMgrProxy();
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = hostCardEmulationManager->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: IsFaModeApplication001
 * @tc.desc: Test HostCardEmulationManagerTest IsFaModeApplication.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, IsFaModeApplication001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    std::string bundleName = "";
    std::string abilityName = "";
    ElementName aidElement;
    aidElement.SetBundleName(bundleName);
    aidElement.SetAbilityName(abilityName);
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    bool isFaModeApplication = hostCardEmulationManager->IsFaModeApplication(aidElement);
    ASSERT_TRUE(isFaModeApplication == false);
}

/**
 * @tc.name: HandleDataForFaApplication001
 * @tc.desc: Test HostCardEmulationManagerTest HandleDataForFaApplication.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, HandleDataForFaApplication001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::vector<uint8_t> data = {1, 2, 3, 4, 5, 6};
    const std::string aid = "";
    std::string bundleName = "";
    std::string abilityName = "";
    ElementName aidElement;
    aidElement.SetBundleName(bundleName);
    aidElement.SetAbilityName(abilityName);
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    hostCardEmulationManager->HandleDataForFaApplication(aid, aidElement, data);
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = hostCardEmulationManager->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: HandleDataForStageApplication001
 * @tc.desc: Test HostCardEmulationManagerTest HandleDataForStageApplication.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, HandleDataForStageApplication001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::vector<uint8_t> data = {1, 2, 3, 4, 5, 6};
    const std::string aid = "";
    std::string bundleName = "";
    std::string abilityName = "";
    ElementName aidElement;
    aidElement.SetBundleName(bundleName);
    aidElement.SetAbilityName(abilityName);
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    hostCardEmulationManager->HandleDataForStageApplication(aid, aidElement, data);
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = hostCardEmulationManager->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: IsFaServiceConnected001
 * @tc.desc: Test HostCardEmulationManagerTest IsFaServiceConnected.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, IsFaServiceConnected001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::string bundleName = "";
    std::string abilityName = "";
    ElementName aidElement;
    aidElement.SetBundleName(bundleName);
    aidElement.SetAbilityName(abilityName);
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    hostCardEmulationManager->IsFaServiceConnected(aidElement);
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = hostCardEmulationManager->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: HandleQueueDataForFa001
 * @tc.desc: Test HostCardEmulationManagerTest HandleQueueDataForFa.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, HandleQueueDataForFa001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::string bundleName = "";
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    hostCardEmulationManager->HandleQueueDataForFa(bundleName);
    Security::AccessToken::AccessTokenID callerToken = 0;
    bool regHceCmdCallback = hostCardEmulationManager->RegHceCmdCallback(callback, type, callerToken);
    ASSERT_TRUE(regHceCmdCallback == false);
}

/**
 * @tc.name: HandleDataForFaApplication002
 * @tc.desc: Test HostCardEmulationManagerTest HandleDataForFaApplication.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, HandleDataForFaApplication002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    ElementName elementName;
    hostCardEmulationManager->hceState_ = HostCardEmulationManager::INITIAL_STATE;
    hostCardEmulationManager->HandleDataForFaApplication("", elementName, std::vector<uint8_t>());
    hostCardEmulationManager->hceState_ = HostCardEmulationManager::WAIT_FOR_SELECT;
    hostCardEmulationManager->HandleDataForFaApplication("", elementName, std::vector<uint8_t>());
    hostCardEmulationManager->hceState_ = HostCardEmulationManager::WAIT_FOR_SERVICE;
    hostCardEmulationManager->HandleDataForFaApplication("", elementName, std::vector<uint8_t>());
    hostCardEmulationManager->hceState_ = HostCardEmulationManager::DATA_TRANSFER;
    hostCardEmulationManager->HandleDataForFaApplication("", elementName, std::vector<uint8_t>());
    hostCardEmulationManager->hceState_ = HostCardEmulationManager::WAIT_FOR_DEACTIVATE;
    hostCardEmulationManager->HandleDataForFaApplication("", elementName, std::vector<uint8_t>());
    ASSERT_TRUE(hostCardEmulationManager != nullptr);
}

/**
 * @tc.name: HandleDataForStageApplication002
 * @tc.desc: Test HostCardEmulationManagerTest HandleDataForStageApplication.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, HandleDataForStageApplication002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    ElementName elementName;
    hostCardEmulationManager->hceState_ = HostCardEmulationManager::INITIAL_STATE;
    hostCardEmulationManager->HandleDataForStageApplication("", elementName, std::vector<uint8_t>());
    hostCardEmulationManager->hceState_ = HostCardEmulationManager::WAIT_FOR_SELECT;
    hostCardEmulationManager->HandleDataForStageApplication("", elementName, std::vector<uint8_t>());
    hostCardEmulationManager->hceState_ = HostCardEmulationManager::WAIT_FOR_SERVICE;
    hostCardEmulationManager->HandleDataForStageApplication("", elementName, std::vector<uint8_t>());
    hostCardEmulationManager->hceState_ = HostCardEmulationManager::DATA_TRANSFER;
    hostCardEmulationManager->HandleDataForStageApplication("", elementName, std::vector<uint8_t>());
    hostCardEmulationManager->hceState_ = HostCardEmulationManager::WAIT_FOR_DEACTIVATE;
    hostCardEmulationManager->HandleDataForStageApplication("", elementName, std::vector<uint8_t>());
    ASSERT_TRUE(hostCardEmulationManager != nullptr);
}

/**
 * @tc.name: SendDataToService
 * @tc.desc: Test HostCardEmulationManagerTest SendDataToService.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, SendDataToService, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    std::vector<uint8_t> data = {0};
    hostCardEmulationManager->SendDataToService(data);
    ASSERT_TRUE(hostCardEmulationManager != nullptr);
}

/**
 * @tc.name: SendDataToFaService
 * @tc.desc: Test HostCardEmulationManagerTest SendDataToFaService.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, SendDataToFaService, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    std::vector<uint8_t> data = {0};
    hostCardEmulationManager->SendDataToFaService(data, "nfc_service");
    ASSERT_TRUE(hostCardEmulationManager != nullptr);
}

/**
 * @tc.name: DispatchAbilitySingleApp
 * @tc.desc: Test HostCardEmulationManagerTest DispatchAbilitySingleApp.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, DispatchAbilitySingleApp, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    ElementName elementName;
    hostCardEmulationManager->DispatchAbilitySingleApp(elementName);
    hostCardEmulationManager->DispatchAbilitySingleAppForFaModel(elementName);
    ASSERT_TRUE(hostCardEmulationManager != nullptr);
}


/**
 * @tc.name: IsCorrespondentService
 * @tc.desc: Test HostCardEmulationManagerTest IsCorrespondentService.
 * @tc.type: FUNC
 */
HWTEST_F(HostCardEmulationManagerTest, IsCorrespondentService, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NCI::INciCeInterface> nciCeProxy = nullptr;
    std::shared_ptr<CeService> ceService = nullptr;
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy, ceService);
    Security::AccessToken::AccessTokenID callerToken = 0;
    hostCardEmulationManager->IsCorrespondentService(callerToken);
    ASSERT_TRUE(hostCardEmulationManager != nullptr);
}

} // namespace TEST
} // namespace NFC
} // namespace OHOS