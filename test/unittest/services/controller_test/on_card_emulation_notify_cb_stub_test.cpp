/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nfc_sdk_common.h"
#include "on_card_emulation_notify_cb_stub.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class OnCardEmulationNotifyCbStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

bool MyCallback(uint32_t, std::string)
{
    return false;
}

void OnCardEmulationNotifyCbStubTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase OnCardEmulationNotifyCbStubTest." << std::endl;
}

void OnCardEmulationNotifyCbStubTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase OnCardEmulationNotifyCbStubTest." << std::endl;
}

void OnCardEmulationNotifyCbStubTest::SetUp() {}

void OnCardEmulationNotifyCbStubTest::TearDown() {}

/**
 * @tc.name: RegisterCallback001
 * @tc.desc: Test OnCardEmulationNotifyCbStubTest RegisterCallback.
 * @tc.type: FUNC
 */
HWTEST_F(OnCardEmulationNotifyCbStubTest, RegisterCallback001, TestSize.Level1)
{
    ASSERT_TRUE(OnCardEmulationNotifyCbStub::GetInstance().RegisterCallback(nullptr));
}

/**
 * @tc.name: RegisterCallback002
 * @tc.desc: Test OnCardEmulationNotifyCbStubTest RegisterCallback.
 * @tc.type: FUNC
 */
HWTEST_F(OnCardEmulationNotifyCbStubTest, RegisterCallback002, TestSize.Level1)
{
    OnCardEmulationNotifyCb callback = MyCallback;
    ASSERT_TRUE(!(OnCardEmulationNotifyCbStub::GetInstance().RegisterCallback(callback)));
}

/**
 * @tc.name: RegisterCallback003
 * @tc.desc: Test OnCardEmulationNotifyCbStubTest RegisterCallback.
 * @tc.type: FUNC
 */
HWTEST_F(OnCardEmulationNotifyCbStubTest, RegisterCallback003, TestSize.Level1)
{
    OnCardEmulationNotifyCb callback = MyCallback;
    OnCardEmulationNotifyCbStub::GetInstance().RegisterCallback(callback);
    ASSERT_TRUE(OnCardEmulationNotifyCbStub::GetInstance().RegisterCallback(callback));
}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Test OnCardEmulationNotifyCbStubTest OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(OnCardEmulationNotifyCbStubTest, OnRemoteRequest001, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int ret = OnCardEmulationNotifyCbStub::GetInstance().OnRemoteRequest(0, data, reply, option);
    ASSERT_TRUE(ret == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Test OnCardEmulationNotifyCbStubTest OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(OnCardEmulationNotifyCbStubTest, OnRemoteRequest002, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::u16string descriptor = u"ohos.nfc.IOnCardEmulationNotifyCb";
    data.WriteInterfaceToken(descriptor);
    data.WriteInt32(1);
    int ret = OnCardEmulationNotifyCbStub::GetInstance().OnRemoteRequest(0, data, reply, option);
    ASSERT_TRUE(ret == 1);
}

/**
 * @tc.name: OnRemoteRequest004
 * @tc.desc: Test OnCardEmulationNotifyCbStubTest OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(OnCardEmulationNotifyCbStubTest, OnRemoteRequest004, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::u16string descriptor = u"ohos.nfc.IOnCardEmulationNotifyCb";
    data.WriteInterfaceToken(descriptor);
    data.WriteInt32(0);
    int ret = OnCardEmulationNotifyCbStub::GetInstance().OnRemoteRequest(0, data, reply, option);
    ASSERT_TRUE(ret);
}
}
}
}
