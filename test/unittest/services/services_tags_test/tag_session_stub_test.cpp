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

#include "tag_session_stub_test.h"

#include <gtest/gtest.h>
#include <thread>

#include "nfc_controller_impl.h"
#include "nfc_controller_stub.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include "nfc_service_tdd.h"
#include "nfc_service.h"
#include "nfc_permission_checker.h"
#include "tag_session.h"
#include "tag_session_stub.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class TagSessionStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TEST_REQUEST_INDEX_1 = 3100201;
    static constexpr const auto TEST_REQUEST_INDEX_2 = 3100202;
    static constexpr const auto TEST_REQUEST_INDEX_3 = 305;
    static constexpr const auto TEST_REQUEST_INDEX_4 = 0;
};

void TagSessionStubTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase TagSessionStubTest." << std::endl;
}

void TagSessionStubTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase TagSessionStubTest." << std::endl;
}

void TagSessionStubTest::SetUp()
{
    std::cout << " SetUp TagSessionStubTest." << std::endl;
}

void TagSessionStubTest::TearDown()
{
    std::cout << " TearDown TagSessionStubTest." << std::endl;
}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_CONNECT), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_2);
}
/**
 * @tc.name: OnRemoteRequest002
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_RECONNECT), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_2);
}
/**
 * @tc.name: OnRemoteRequest003
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest003, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_DISCONNECT), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_4);
}
/**
 * @tc.name: OnRemoteRequest004
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest004, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_SET_TIMEOUT), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_4);
}
/**
 * @tc.name: OnRemoteRequest005
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest005, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_GET_TIMEOUT), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_4);
}
/**
 * @tc.name: OnRemoteRequest006
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest006, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_GET_TECHLIST), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_4);
}
/**
 * @tc.name: OnRemoteRequest007
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest007, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_IS_PRESENT), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_4);
}
/**
 * @tc.name: OnRemoteRequest008
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest008, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_IS_NDEF), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_4);
}
/**
 * @tc.name: OnRemoteRequest009
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest009, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_SEND_RAW_FRAME), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_2);
}
/**
 * @tc.name: OnRemoteRequest010
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest010, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_NDEF_READ), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_4);
}
/**
 * @tc.name: OnRemoteRequest011
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest011, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_NDEF_WRITE), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_4);
}
/**
 * @tc.name: OnRemoteRequest012
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest012, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_NDEF_MAKE_READ_ONLY), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_4);
}
/**
 * @tc.name: OnRemoteRequest013
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest013, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_FORMAT_NDEF), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_4);
}
/**
 * @tc.name: OnRemoteRequest014
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest014, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_CAN_MAKE_READ_ONLY), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1);
}
/**
 * @tc.name: OnRemoteRequest015
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest015, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_GET_MAX_TRANSCEIVE_LENGTH), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_4);
}
/**
 * @tc.name: OnRemoteRequest016
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest016, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_IS_SUPPORTED_APDUS_EXTENDED), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1);
}
/**
 * @tc.name: OnRemoteRequest017
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest017, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        TAG_SESSION_START_ID), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_3);
}
/**
 * @tc.name: OnRemoteRequest018
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest018, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_REG_FOREGROUND), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_4);
}
/**
 * @tc.name: OnRemoteRequest019
 * @tc.desc: Test TagSessionStub OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionStubTest, OnRemoteRequest019, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    int onRemoteRequest = tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::
        COMMAND_UNREG_FOREGROUND), data, reply, option);
    ASSERT_TRUE(onRemoteRequest == TEST_REQUEST_INDEX_1 || onRemoteRequest == TEST_REQUEST_INDEX_4);
}
}
}
}
