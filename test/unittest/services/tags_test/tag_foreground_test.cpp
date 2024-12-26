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

#include "tag_foreground.h"
#include "foreground_callback_stub.h"

#include "iforeground_callback.h"
#include "nfc_controller.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
using namespace OHOS::NFC::KITS;
using namespace OHOS::NFC::TAG;
class TagForegroundTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void TagForegroundTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase TagForegroundTest." << std::endl;
}

void TagForegroundTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase TagForegroundTest." << std::endl;
}

void TagForegroundTest::SetUp()
{
    std::cout << " SetUp TagForegroundTest." << std::endl;
}

void TagForegroundTest::TearDown()
{
    std::cout << " TearDown TagForegroundTest." << std::endl;
}

/**
 * @tc.name: RegForeground001
 * @tc.desc: Test TagForeground RegForeground.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, RegForeground001, TestSize.Level1)
{
    NfcController ctrl = NfcController::GetInstance();
    ctrl.TurnOn();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    AppExecFwk::ElementName element;
    std::vector<uint32_t> discTech = {1, 2, 3, 4, 5};
    const sptr<KITS::IForegroundCallback> callback = nullptr;
    TagForeground instance = TagForeground::GetInstance();
    instance.RegForeground(element, discTech, callback);
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int ret = ForegroundCallbackStub::GetInstance()->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(ret == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: RegForeground002
 * @tc.desc: Test TagForeground RegForeground.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, RegForeground002, TestSize.Level1)
{
    NfcController ctrl = NfcController::GetInstance();
    ctrl.TurnOff();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    AppExecFwk::ElementName element;
    std::vector<uint32_t> discTech = {1, 2, 3, 4, 5};
    const sptr<KITS::IForegroundCallback> callback = nullptr;
    TagForeground instance = TagForeground::GetInstance();
    int result = instance.RegForeground(element, discTech, callback);
    ASSERT_TRUE(result == ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}

/**
 * @tc.name: UnregForeground001
 * @tc.desc: Test TagForeground UnregForeground.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, UnregForeground001, TestSize.Level1)
{
    NfcController ctrl = NfcController::GetInstance();
    ctrl.TurnOn();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    AppExecFwk::ElementName element;
    TagForeground instance = TagForeground::GetInstance();
    instance.UnregForeground(element);
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int ret = ForegroundCallbackStub::GetInstance()->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(ret == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: UnregForeground002
 * @tc.desc: Test TagForeground UnregForeground.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, UnregForeground002, TestSize.Level1)
{
    NfcController ctrl = NfcController::GetInstance();
    ctrl.TurnOff();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    AppExecFwk::ElementName element;
    TagForeground instance = TagForeground::GetInstance();
    int result = instance.UnregForeground(element);
    ASSERT_TRUE(result == ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}

/**
 * @tc.name: RegReaderMode001
 * @tc.desc: Test TagForeground RegReaderMode.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, RegReaderMode001, TestSize.Level1)
{
    NfcController ctrl = NfcController::GetInstance();
    ctrl.TurnOn();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    AppExecFwk::ElementName element;
    std::vector<uint32_t> discTech;
    sptr<KITS::IReaderModeCallback> callback = nullptr;
    TagForeground instance = TagForeground::GetInstance();
    instance.RegReaderMode(element, discTech, callback);
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int ret = ForegroundCallbackStub::GetInstance()->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(ret == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: RegReaderMode002
 * @tc.desc: Test TagForeground RegReaderMode.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, RegReaderMode002, TestSize.Level1)
{
    NfcController ctrl = NfcController::GetInstance();
    ctrl.TurnOff();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    AppExecFwk::ElementName element;
    std::vector<uint32_t> discTech;
    sptr<KITS::IReaderModeCallback> callback = nullptr;
    TagForeground instance = TagForeground::GetInstance();
    int result = instance.RegReaderMode(element, discTech, callback);
    ASSERT_TRUE(result == ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}

/**
 * @tc.name: UnregReaderMode001
 * @tc.desc: Test TagForeground UnregReaderMode.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, UnregReaderMode001, TestSize.Level1)
{
    NfcController ctrl = NfcController::GetInstance();
    ctrl.TurnOn();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    AppExecFwk::ElementName element;
    TagForeground instance = TagForeground::GetInstance();
    instance.UnregReaderMode(element);
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int ret = ForegroundCallbackStub::GetInstance()->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(ret == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: UnregReaderMode002
 * @tc.desc: Test TagForeground UnregReaderMode.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, UnregReaderMode002, TestSize.Level1)
{
    NfcController ctrl = NfcController::GetInstance();
    ctrl.TurnOff();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    AppExecFwk::ElementName element;
    TagForeground instance = TagForeground::GetInstance();
    int result = instance.UnregReaderMode(element);
    ASSERT_TRUE(result == ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Test TagForeground OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, OnRemoteRequest001, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int ret = ForegroundCallbackStub::GetInstance()->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(ret == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Test TagForeground OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, OnRemoteRequest002, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::u16string descriptor = u"ohos.nfc.kits.IForegroundCallback";
    data.WriteInterfaceToken(descriptor);
    data.WriteInt32(1);
    int ret = ForegroundCallbackStub::GetInstance()->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(ret == 1);
}

/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Test TagForeground OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, OnRemoteRequest003, TestSize.Level1)
{
    uint32_t code = 111;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::u16string descriptor = u"ohos.nfc.kits.IForegroundCallback";
    data.WriteInterfaceToken(descriptor);
    data.WriteInt32(0);
    int ret = ForegroundCallbackStub::GetInstance()->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(ret == KITS::ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequest004
 * @tc.desc: Test TagForeground OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagForegroundTest, OnRemoteRequest004, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::u16string descriptor = u"ohos.nfc.kits.IForegroundCallback";
    data.WriteInterfaceToken(descriptor);
    data.WriteInt32(0);
    int ret = ForegroundCallbackStub::GetInstance()->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(ret == 305);
}
}
}
}
