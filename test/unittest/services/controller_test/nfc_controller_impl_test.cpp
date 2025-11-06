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

#include "ndef_msg_callback_stub.h"
#include "nfc_controller_callback_stub.h"
#include "nfc_controller_impl.h"
#include "nfc_service.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class NfcControllerImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

class INfcControllerCallbackImpl : public INfcControllerCallback {
public:
    INfcControllerCallbackImpl() {}

    virtual ~INfcControllerCallbackImpl() {}

public:
    void OnNfcStateChanged(int nfcState) override
    {
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

void NfcControllerImplTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcControllerImplTest." << std::endl;
}

void NfcControllerImplTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcControllerImplTest." << std::endl;
}

void NfcControllerImplTest::SetUp()
{
    std::cout << " SetUp NfcControllerImplTest." << std::endl;
}

void NfcControllerImplTest::TearDown()
{
    std::cout << " TearDown NfcControllerImplTest." << std::endl;
}

/**
 * @tc.name: GetState001
 * @tc.desc: Test NfcControllerImplTest GetState.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, GetState001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    int nfcState = 1;
    ErrCode errCode = nfcControllerImpl->GetState(nfcState);
    ASSERT_TRUE(errCode == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: GetState002
 * @tc.desc: Test NfcControllerImplTest GetState.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, GetState002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    int nfcState = 1;
    ErrCode errCode = nfcControllerImpl->GetState(nfcState);
    ASSERT_TRUE(errCode == KITS::ERR_NONE);
}

/**
 * @tc.name: TurnOn001
 * @tc.desc: Test NfcControllerImplTest TurnOn.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, TurnOn001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode turnOn = nfcControllerImpl->TurnOn();
    ASSERT_TRUE(turnOn == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: TurnOn002
 * @tc.desc: Test NfcControllerImplTest TurnOn.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, TurnOn002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode turnOn = nfcControllerImpl->TurnOn();
    InfoLog("TurnOn002, turnOn = %{public}d", turnOn);
    ASSERT_TRUE(turnOn == KITS::ERR_NFC_STATE_INVALID);
}

/**
 * @tc.name: TurnOff001
 * @tc.desc: Test NfcControllerImplTest TurnOff.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, TurnOff001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode turnOff = nfcControllerImpl->TurnOff();
    ASSERT_TRUE(turnOff == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: TurnOff002
 * @tc.desc: Test NfcControllerImplTest TurnOff.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, TurnOff002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode turnOff = nfcControllerImpl->TurnOff();
    ASSERT_TRUE(turnOff == KITS::ERR_NFC_STATE_INVALID);
}

/**
 * @tc.name: RegisterCallBack001
 * @tc.desc: Test NfcControllerImplTest RegisterCallBack.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, RegisterCallBack001, TestSize.Level1)
{
    sptr<INfcControllerCallback> callback = nullptr;
    std::string type = "";
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    nfcControllerImpl->CallbackEnter(101);
    nfcControllerImpl->CallbackExit(101, 0);
    ErrCode error = nfcControllerImpl->RegisterNfcStatusCallBack(callback, type);
    ASSERT_TRUE(error == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: RegisterCallBack002
 * @tc.desc: Test NfcControllerImplTest RegisterCallBack.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, RegisterCallBack002, TestSize.Level1)
{
    sptr<NfcControllerCallBackStub> callback =
        sptr<NfcControllerCallBackStub>(new NfcControllerCallBackStub());
    std::string type = "";
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode error = nfcControllerImpl->RegisterNfcStatusCallBack(callback, type);
    sleep(1);
    ASSERT_TRUE(error == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: UnRegisterCallBack001
 * @tc.desc: Test NfcControllerImplTest UnRegisterCallBack.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, UnRegisterCallBack001, TestSize.Level1)
{
    std::string type = "";
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode error = nfcControllerImpl->UnregisterNfcStatusCallBack(type);
    ASSERT_TRUE(error == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: UnRegisterCallBack002
 * @tc.desc: Test NfcControllerImplTest UnRegisterCallBack.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, UnRegisterCallBack002, TestSize.Level1)
{
    std::string type = "";
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode error = nfcControllerImpl->UnregisterNfcStatusCallBack(type);
    ASSERT_TRUE(error == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: UnRegisterAllCallBack001
 * @tc.desc: Test NfcControllerImplTest UnRegisterAllCallBack.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, UnRegisterAllCallBack001, TestSize.Level1)
{
    Security::AccessToken::AccessTokenID callerToken = 0;
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode error = nfcControllerImpl->UnRegisterAllCallBack(callerToken);
    ASSERT_TRUE(error == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: UnRegisterAllCallBack002
 * @tc.desc: Test NfcControllerImplTest UnRegisterAllCallBack.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, UnRegisterAllCallBack002, TestSize.Level1)
{
    Security::AccessToken::AccessTokenID callerToken = 0;
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode error = nfcControllerImpl->UnRegisterAllCallBack(callerToken);
    ASSERT_TRUE(error == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: GetTagServiceIface001
 * @tc.desc: Test NfcControllerImplTest GetTagServiceIface.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, GetTagServiceIface001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    sptr<IRemoteObject> iRemoteObject = nullptr;
    nfcControllerImpl->GetTagServiceIface(iRemoteObject);
    ASSERT_TRUE(iRemoteObject == nullptr);
}

/**
 * @tc.name: GetTagServiceIface002
 * @tc.desc: Test NfcControllerImplTest GetTagServiceIface.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, GetTagServiceIface002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    sptr<IRemoteObject> iRemoteObject = nullptr;
    nfcControllerImpl->GetTagServiceIface(iRemoteObject);
    ASSERT_TRUE(iRemoteObject == nullptr);
}

/**
 * @tc.name: RegNdefMsgCallback001
 * @tc.desc: Test NfcControllerImplTest RegNdefMsgCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, RegNdefMsgCallback001, TestSize.Level1)
{
    sptr<INdefMsgCallback> callback = nullptr;
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode error = nfcControllerImpl->RegNdefMsgCb(callback);
    ASSERT_TRUE(error == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: RegNdefMsgCallback002
 * @tc.desc: Test NfcControllerImplTest RegNdefMsgCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, RegNdefMsgCallback002, TestSize.Level1)
{
    sptr<NdefMsgCallbackStub> callback = sptr<NdefMsgCallbackStub>(new NdefMsgCallbackStub());
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode error = nfcControllerImpl->RegNdefMsgCb(callback);
    ASSERT_TRUE(error == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: RegNdefMsgCallback003
 * @tc.desc: Test NfcControllerImplTest RegNdefMsgCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, RegNdefMsgCallback003, TestSize.Level1)
{
    sptr<NdefMsgCallbackStub> callback = sptr<NdefMsgCallbackStub>(new NdefMsgCallbackStub());
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode error = nfcControllerImpl->RegNdefMsgCb(callback);
    ASSERT_TRUE(error == KITS::ERR_NFC_PARAMETERS);
}

#ifdef VENDOR_APPLICATIONS_ENABLED
/**
 * @tc.name: RegQueryApplicationCb001
 * @tc.desc: Test NfcControllerImplTest RegQueryApplicationCb.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, RegQueryApplicationCb001, TestSize.Level1)
{
    sptr<IQueryAppInfoCallback> callback = nullptr;
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode error = nfcControllerImpl->RegQueryApplicationCb(callback);
    ASSERT_TRUE(error == KITS::ERR_NONE);
}

/**
 * @tc.name: RegCardEmulationNotifyCb001
 * @tc.desc: Test NfcControllerImplTest RegCardEmulationNotifyCb.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, RegCardEmulationNotifyCb001, TestSize.Level1)
{
    sptr<IOnCardEmulationNotifyCb> callback = nullptr;
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode error = nfcControllerImpl->RegCardEmulationNotifyCb(callback);
    ASSERT_TRUE(error == KITS::ERR_NONE);
}

/**
 * @tc.name: NotifyEventStatus001
 * @tc.desc: Test NfcControllerImplTest NotifyEventStatus.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, NotifyEventStatus001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode error = nfcControllerImpl->NotifyEventStatus(0, 0, "");
    ASSERT_TRUE(error == KITS::ERR_NFC_PARAMETERS);
}
#endif

/**
 * @tc.name: GetHceServiceIface001
 * @tc.desc: Test NfcControllerImplTest GetHceServiceIface.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, GetHceServiceIface001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    sptr<IRemoteObject> iRemoteObject = nullptr;
    nfcControllerImpl->GetHceServiceIface(iRemoteObject);
    ASSERT_TRUE(iRemoteObject == nullptr);
}

/**
 * @tc.name: GetHceServiceIface002
 * @tc.desc: Test NfcControllerImplTest GetHceServiceIface.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, GetHceServiceIface002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    sptr<IRemoteObject> iRemoteObject = nullptr;
    nfcControllerImpl->GetHceServiceIface(iRemoteObject);
    ASSERT_TRUE(iRemoteObject == nullptr);
}

/**
 * @tc.name: RestartNfc
 * @tc.desc: Test RestartNfc.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, RestartNfc, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    ErrCode restart = nfcControllerImpl->RestartNfc();
    ASSERT_TRUE(restart > 0);
}

}
}
}