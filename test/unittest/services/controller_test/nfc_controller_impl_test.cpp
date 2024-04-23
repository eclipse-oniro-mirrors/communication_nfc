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

#include "nfc_controller_impl.h"
#include "nfc_service.h"

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
    int getState = nfcControllerImpl->GetState();
    ASSERT_TRUE(getState == KITS::ERR_NFC_PARAMETERS);
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
    int turnOn = nfcControllerImpl->TurnOn();
    ASSERT_TRUE(turnOn == KITS::ERR_NFC_PARAMETERS);
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
    int turnOff = nfcControllerImpl->TurnOff();
    ASSERT_TRUE(turnOff == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: IsNfcOpen001
 * @tc.desc: Test NfcControllerImplTest IsNfcOpen.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, IsNfcOpen001, TestSize.Level1)
{
    bool isOpen = false;
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    int isNfcOpen = nfcControllerImpl->IsNfcOpen(isOpen);
    ASSERT_TRUE(isNfcOpen == KITS::ERR_NFC_PARAMETERS);
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
    Security::AccessToken::AccessTokenID callerToken = 0;
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    KITS::ErrorCode error = nfcControllerImpl->RegisterCallBack(callback, type, callerToken);
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
    Security::AccessToken::AccessTokenID callerToken = 0;
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    KITS::ErrorCode error = nfcControllerImpl->UnRegisterCallBack(type, callerToken);
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
    Security::AccessToken::AccessTokenID callerToken = 0;
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    KITS::ErrorCode error = nfcControllerImpl->UnRegisterCallBack(type, callerToken);
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
    KITS::ErrorCode error = nfcControllerImpl->UnRegisterAllCallBack(callerToken);
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
    KITS::ErrorCode error = nfcControllerImpl->UnRegisterAllCallBack(callerToken);
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
    sptr<IRemoteObject> iRemoteObject = nfcControllerImpl->GetTagServiceIface();
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
    sptr<IRemoteObject> iRemoteObject = nfcControllerImpl->GetTagServiceIface();
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
    KITS::ErrorCode error = nfcControllerImpl->RegNdefMsgCallback(callback);
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
    KITS::ErrorCode error = nfcControllerImpl->RegQueryApplicationCb(callback);
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
    KITS::ErrorCode error = nfcControllerImpl->RegCardEmulationNotifyCb(callback);
    ASSERT_TRUE(error == KITS::ERR_NONE);
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
    sptr<IRemoteObject> iRemoteObject = nfcControllerImpl->GetHceServiceIface();
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
    sptr<IRemoteObject> iRemoteObject = nfcControllerImpl->GetHceServiceIface();
    ASSERT_TRUE(iRemoteObject == nullptr);
}

/**
 * @tc.name: Dump001
 * @tc.desc: Test NfcControllerImplTest Dump.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, Dump001, TestSize.Level1)
{
    int32_t fd = 0;
    std::vector<std::u16string> args;
    std::shared_ptr<NfcService> nfcService = nullptr;
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    int dump = nfcControllerImpl->Dump(fd, args);
    ASSERT_TRUE(dump == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: Dump002
 * @tc.desc: Test NfcControllerImplTest Dump.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerImplTest, Dump002, TestSize.Level1)
{
    int32_t fd = 0;
    std::vector<std::u16string> args;
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<NfcControllerImpl> nfcControllerImpl = std::make_shared<NfcControllerImpl>(nfcService);
    int dump = nfcControllerImpl->Dump(fd, args);
    ASSERT_TRUE(dump == KITS::ERR_NFC_PARAMETERS);
}
}
}
}