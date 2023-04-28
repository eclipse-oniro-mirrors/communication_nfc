/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd.
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

#include "nfc_controller_callback_stub.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class NfcControllerCallBackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NfcControllerCallBackStubTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcControllerCallBackStubTest." << std::endl;
}

void NfcControllerCallBackStubTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcControllerCallBackStubTest." << std::endl;
}

void NfcControllerCallBackStubTest::SetUp()
{
    std::cout << " SetUp NfcControllerCallBackStubTest." << std::endl;
}

void NfcControllerCallBackStubTest::TearDown()
{
    std::cout << " TearDown NfcControllerCallBackStubTest." << std::endl;
}

/**
 * @tc.name: RegisterCallBack001
 * @tc.desc: Test NfcControllerCallBackStub RegisterCallBack.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerCallBackStubTest, RegisterCallBack001, TestSize.Level1)
{
    const sptr<INfcControllerCallback> callBack = nullptr;
    NfcControllerCallBackStub& ctrl = NfcControllerCallBackStub::GetInstance();
    int registerCallBack = ctrl.RegisterCallBack(callBack);
    ASSERT_TRUE(registerCallBack == KITS::ERR_NFC_PARAMETERS);
}
/**
 * @tc.name: RegisterCallBack002
 * @tc.desc: Test NfcControllerCallBackStub RegisterCallBack.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerCallBackStubTest, RegisterCallBack002, TestSize.Level1)
{
    NfcControllerCallBackStub nfcControllerCallBackStub;
    NfcControllerCallBackStub& ctrl = NfcControllerCallBackStub::GetInstance();
    int registerCallBack = ctrl.RegisterCallBack(&nfcControllerCallBackStub);
    ASSERT_TRUE(registerCallBack == KITS::ERR_NONE);
}
/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Test NfcControllerCallBackStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerCallBackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    uint32_t code = KITS::COMMAND_ON_NOTIFY;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    NfcControllerCallBackStub& ctrl = NfcControllerCallBackStub::GetInstance();
    int onRemoteRequest = ctrl.OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(onRemoteRequest == KITS::ERR_NFC_PARAMETERS);
}
/**
 * @tc.name: OnNfcStateChanged001
 * @tc.desc: Test NfcControllerCallBackStub OnNfcStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerCallBackStubTest, OnNfcStateChanged001, TestSize.Level1)
{
    int nfcRfState = 0;
    NfcControllerCallBackStub& ctrl = NfcControllerCallBackStub::GetInstance();
    ctrl.OnNfcStateChanged(nfcRfState);
    ASSERT_TRUE(nfcRfState == KITS::ErrorCode::ERR_NONE);
}
}
}
}
