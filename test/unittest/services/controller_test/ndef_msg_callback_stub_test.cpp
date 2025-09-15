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

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class NdefMsgCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

class INdefMsgCallbackImpl : public INdefMsgCallback {
public:
    INdefMsgCallbackImpl() {}

    virtual ~INdefMsgCallbackImpl() {}

public:
    bool OnNdefMsgDiscovered(const std::string &tagUid, const std::string &ndef, const std::string &payload,
        int ndefMsgType) override
    {
        return false;
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

void NdefMsgCallbackStubTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NdefMsgCallbackStubTest." << std::endl;
}

void NdefMsgCallbackStubTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NdefMsgCallbackStubTest." << std::endl;
}

void NdefMsgCallbackStubTest::SetUp()
{
    std::cout << " SetUp NdefMsgCallbackStubTest." << std::endl;
}

void NdefMsgCallbackStubTest::TearDown()
{
    std::cout << " TearDown NdefMsgCallbackStubTest." << std::endl;
}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Test NdefMsgCallbackStubTest OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMsgCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::shared_ptr<NdefMsgCallbackStub> ndefMsgCallbackStub = std::make_shared<NdefMsgCallbackStub>();
    int onRemoteRequest = ndefMsgCallbackStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(onRemoteRequest == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Test NdefMsgCallbackStubTest OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMsgCallbackStubTest, OnRemoteRequest002, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::u16string descriptor = u"ohos.nfc.kits.INdefMsgCallback";
    data.WriteInterfaceToken(descriptor);
    data.WriteInt32(1);
    std::shared_ptr<NdefMsgCallbackStub> ndefMsgCallbackStub = std::make_shared<NdefMsgCallbackStub>();
    int onRemoteRequest = ndefMsgCallbackStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(onRemoteRequest == 1);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Test NdefMsgCallbackStubTest OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMsgCallbackStubTest, OnRemoteRequest003, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::u16string descriptor = u"ohos.nfc.kits.INdefMsgCallback";
    data.WriteInterfaceToken(descriptor);
    data.WriteInt32(0);
    std::shared_ptr<NdefMsgCallbackStub> ndefMsgCallbackStub = std::make_shared<NdefMsgCallbackStub>();
    int onRemoteRequest = ndefMsgCallbackStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(onRemoteRequest);
}

/**
 * @tc.name: OnRemoteRequest004
 * @tc.desc: Test NdefMsgCallbackStubTest OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMsgCallbackStubTest, OnRemoteRequest004, TestSize.Level1)
{
    uint32_t code = 113;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::u16string descriptor = u"ohos.nfc.kits.INdefMsgCallback";
    data.WriteInterfaceToken(descriptor);
    data.WriteInt32(0);
    std::shared_ptr<NdefMsgCallbackStub> ndefMsgCallbackStub = std::make_shared<NdefMsgCallbackStub>();
    int onRemoteRequest = ndefMsgCallbackStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(!onRemoteRequest);
}

/**
 * @tc.name: RegisterCallback001
 * @tc.desc: Test NdefMsgCallbackStubTest RegisterCallback.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMsgCallbackStubTest, RegisterCallback001, TestSize.Level1)
{
    sptr<INdefMsgCallback> callback = nullptr;
    std::shared_ptr<NdefMsgCallbackStub> ndefMsgCallbackStub = std::make_shared<NdefMsgCallbackStub>();
    KITS::ErrorCode errorCode = ndefMsgCallbackStub->RegisterCallback(callback);
    ASSERT_TRUE(errorCode == KITS::ERR_NFC_PARAMETERS);
}
}
}
}