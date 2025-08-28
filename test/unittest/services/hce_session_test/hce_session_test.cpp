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

#include "hce_cmd_callback_stub.h"
#include "hce_session.h"
#include "hce_session_stub.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class HceSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HceSessionTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase HceSessionTest." << std::endl;
}

void HceSessionTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase HceSessionTest." << std::endl;
}

void HceSessionTest::SetUp()
{
    std::cout << " SetUp HceSessionTest." << std::endl;
}

void HceSessionTest::TearDown()
{
    std::cout << " TearDown HceSessionTest." << std::endl;
}

/**
 * @tc.name: HceSession001
 * @tc.desc: Test HceSessionTest HceSession.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, HceSession001, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = nullptr;
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ASSERT_TRUE(hceSession != nullptr);
}

/**
 * @tc.name: RegHceCmdCallback001
 * @tc.desc: Test HceSessionTest RegHceCmdCallback.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, RegHceCmdCallback001, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = nullptr;
    sptr<KITS::IHceCmdCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode regHceCmdCallback = hceSession->RegHceCmdCallback(callback, type);
    ASSERT_TRUE(regHceCmdCallback == NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS);
}

/**
 * @tc.name: RegHceCmdCallback002
 * @tc.desc: Test HceSessionTest RegHceCmdCallback.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, RegHceCmdCallback002, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = nullptr;
    sptr<KITS::IHceCmdCallback> callback = sptr<HCE::HceCmdCallbackStub>(new HCE::HceCmdCallbackStub);
    std::string type = "";
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode regHceCmdCallback = hceSession->RegHceCmdCallback(callback, type);
    ASSERT_TRUE(regHceCmdCallback == NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS);
}

/**
 * @tc.name: SendRawFrame001
 * @tc.desc: Test HceSessionTest SendRawFrame.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, SendRawFrame001, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    std::string hexCmdData(MAX_APDU_DATA_HEX_STR + 1, 'a');
    bool raw = false;
    std::string hexRespData = "";
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode sendRawFrame = hceSession->SendRawFrame(hexCmdData, raw, hexRespData);
    ASSERT_TRUE(sendRawFrame == NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS);
}

/**
 * @tc.name: SendRawFrame002
 * @tc.desc: Test HceSessionTest SendRawFrame.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, SendRawFrame002, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    std::string hexCmdData = "";
    bool raw = false;
    std::string hexRespData = "";
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode sendRawFrame = hceSession->SendRawFrame(hexCmdData, raw, hexRespData);
    ASSERT_TRUE(sendRawFrame == NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS);
}

/**
 * @tc.name: SendRawFrame003
 * @tc.desc: Test HceSessionTest SendRawFrame.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, SendRawFrame003, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    nfcService->Initialize();
    std::string hexCmdData = "";
    bool raw = false;
    std::string hexRespData = "";
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode sendRawFrame = hceSession->SendRawFrame(hexCmdData, raw, hexRespData);
    ASSERT_TRUE(sendRawFrame == NFC::KITS::ErrorCode::ERR_HCE_STATE_IO_FAILED);
}

/**
 * @tc.name: UnRegHceCmdCallback001
 * @tc.desc: Test HceSessionTest UnRegHceCmdCallback.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, UnRegHceCmdCallback001, TestSize.Level1)
{
    sptr<IHceCmdCallback> cb = nullptr;
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    const std::string type = "";
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode errorCode = hceSession->UnregHceCmdCallback(cb, type);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS);
}

/**
 * @tc.name: UnRegHceCmdCallback002
 * @tc.desc: Test HceSessionTest UnRegHceCmdCallback.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, UnRegHceCmdCallback002, TestSize.Level1)
{
    sptr<HCE::HceCmdCallbackStub> cb = sptr<HCE::HceCmdCallbackStub>(new HCE::HceCmdCallbackStub);
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    const std::string type = "";
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode errorCode = hceSession->UnregHceCmdCallback(cb, type);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS);
}

/**
 * @tc.name: UnRegHceCmdCallback003
 * @tc.desc: Test HceSessionTest UnRegHceCmdCallback.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, UnRegHceCmdCallback003, TestSize.Level1)
{
    sptr<HCE::HceCmdCallbackStub> cb = sptr<HCE::HceCmdCallbackStub>(new HCE::HceCmdCallbackStub);
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    nfcService->Initialize();
    const std::string type = "";
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode errorCode = hceSession->UnregHceCmdCallback(cb, type);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_NONE);
}

/**
 * @tc.name: UnRegAllCallback001
 * @tc.desc: Test HceSessionTest UnRegAllCallback.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, UnRegAllCallback001, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    Security::AccessToken::AccessTokenID callerToken = 0;
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode errorCode = hceSession->UnRegAllCallback(callerToken);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS);
}

/**
 * @tc.name: UnRegAllCallback002
 * @tc.desc: Test HceSessionTest UnRegAllCallback.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, UnRegAllCallback002, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    nfcService->Initialize();
    Security::AccessToken::AccessTokenID callerToken = 0;
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode errorCode = hceSession->UnRegAllCallback(callerToken);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS);
}

/**
 * @tc.name: IsDefaultService001
 * @tc.desc: Test HceSessionTest IsDefaultService.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, IsDefaultService001, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    ElementName element;
    const std::string type = "";
    bool isDefaultService = false;
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode errorCode = hceSession->IsDefaultService(element, type, isDefaultService);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS);
}

/**
 * @tc.name: IsDefaultService002
 * @tc.desc: Test HceSessionTest IsDefaultService.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, IsDefaultService002, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    nfcService->Initialize();
    ElementName element;
    const std::string type = "";
    bool isDefaultService = false;
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode errorCode = hceSession->IsDefaultService(element, type, isDefaultService);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_NONE);
}

/**
 * @tc.name: HandleWhenRemoteDie001
 * @tc.desc: Test HceSessionTest HandleWhenRemoteDie.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, HandleWhenRemoteDie001, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    Security::AccessToken::AccessTokenID callerToken = 0;
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode errorCode = hceSession->HandleWhenRemoteDie(callerToken);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS);
}

/**
 * @tc.name: HandleWhenRemoteDie002
 * @tc.desc: Test HceSessionTest HandleWhenRemoteDie.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, HandleWhenRemoteDie002, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    nfcService->Initialize();
    Security::AccessToken::AccessTokenID callerToken = 0;
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode errorCode = hceSession->HandleWhenRemoteDie(callerToken);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS);
}

/**
 * @tc.name: StartHce001
 * @tc.desc: Test HceSessionTest StartHce.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, StartHce001, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    ElementName element;
    std::vector<std::string> aids;
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode errorCode = hceSession->StartHce(element, aids);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS);
}

/**
 * @tc.name: StartHce002
 * @tc.desc: Test HceSessionTest StartHce.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, StartHce002, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    nfcService->Initialize();
    ElementName element;
    std::vector<std::string> aids;
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode errorCode = hceSession->StartHce(element, aids);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS);
}

/**
 * @tc.name: StopHce001
 * @tc.desc: Test HceSessionTest StopHce.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, StopHce001, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    ElementName element;
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode errorCode = hceSession->StopHce(element);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS);
}

/**
 * @tc.name: StopHce002
 * @tc.desc: Test HceSessionTest StopHce.
 * @tc.type: FUNC
 */
HWTEST_F(HceSessionTest, StopHce002, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
    nfcService->Initialize();
    std::weak_ptr<NFC::CeService> ceService = nfcService->GetCeService();
    ceService.lock()->Initialize();
    ElementName element;
    std::shared_ptr<HCE::HceSession> hceSession = std::make_shared<HCE::HceSession>(nfcService);
    ErrCode errorCode = hceSession->StopHce(element);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS);
}
}
}
}