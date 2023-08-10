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
#include "permission_tools.h"
#include "tag_session.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class TagSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TEST_INDEX_1 = 12;
    static constexpr const auto TEST_INDEX_2 = -1;
    static constexpr const auto TEST_INDEX_3 = 15;
};

void TagSessionTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase TagSessionTest." << std::endl;
}

void TagSessionTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase TagSessionTest." << std::endl;
}

void TagSessionTest::SetUp()
{
    std::cout << " SetUp TagSessionTest." << std::endl;
}

void TagSessionTest::TearDown()
{
    std::cout << " TearDown TagSessionTest." << std::endl;
}

/**
 * @tc.name: Dump001
 * @tc.desc: Test TagSession Dump.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, Dump001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int32_t fd = TEST_INDEX_1;
    const std::vector<std::u16string> args;
    int32_t dump = tagSession->Dump(fd, args);
    ASSERT_TRUE(dump == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: Dump002
 * @tc.desc: Test TagSession Dump.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, Dump002, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int32_t fd = 1;
    const std::vector<std::u16string> args;
    int32_t dump = tagSession->Dump(fd, args);
    ASSERT_TRUE(dump == NFC::KITS::ErrorCode::ERR_NONE);
}
/**
 * @tc.name: GetMaxTransceiveLength001
 * @tc.desc: Test TagSession GetMaxTransceiveLength.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetMaxTransceiveLength001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int technology = TEST_INDEX_1;
    int maxSize;
    int getMaxTransceiveLength = tagSession->GetMaxTransceiveLength(technology, maxSize);
    ASSERT_TRUE(getMaxTransceiveLength == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: GetMaxTransceiveLength002
 * @tc.desc: Test TagSession GetMaxTransceiveLength.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetMaxTransceiveLength002, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int technology = TEST_INDEX_2;
    int maxSize;
    int getMaxTransceiveLength = tagSession->GetMaxTransceiveLength(technology, maxSize);
    ASSERT_TRUE(getMaxTransceiveLength == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: GetMaxTransceiveLength003
 * @tc.desc: Test TagSession GetMaxTransceiveLength.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetMaxTransceiveLength003, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int technology = 0;
    int maxSize;
    int getMaxTransceiveLength = tagSession->GetMaxTransceiveLength(technology, maxSize);
    ASSERT_TRUE(getMaxTransceiveLength == NFC::KITS::ErrorCode::ERR_NONE);
}
/**
 * @tc.name: UnregForegroundDispatch001
 * @tc.desc: Test TagSession UnregForegroundDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, UnregForegroundDispatch001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    AppExecFwk::ElementName element;
    KITS::ErrorCode unregForegroundDispatch = tagSession->UnregForegroundDispatch(element);
    ASSERT_TRUE(unregForegroundDispatch == KITS::ERR_NONE);
}
/**
 * @tc.name: RegForegroundDispatch001
 * @tc.desc: Test TagSession RegForegroundDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, RegForegroundDispatch001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    AppExecFwk::ElementName element;
    std::vector<uint32_t> discTech;
    const sptr<KITS::IForegroundCallback> callback;
    KITS::ErrorCode regForegroundDispatch = tagSession->RegForegroundDispatch(element, discTech, callback);
    ASSERT_TRUE(regForegroundDispatch == KITS::ERR_NONE);
}
/**
 * @tc.name: NdefMakeReadOnly001
 * @tc.desc: Test TagSession NdefMakeReadOnly.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, NdefMakeReadOnly001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_INDEX_1;
    int ndefMakeReadOnly = tagSession->NdefMakeReadOnly(tagRfDiscId);
    ASSERT_TRUE(ndefMakeReadOnly == NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}
/**
 * @tc.name: NdefWrite001
 * @tc.desc: Test TagSession NdefWrite.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, NdefWrite001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_INDEX_1;
    std::string msg = "";
    int ndefWrite = tagSession->NdefWrite(tagRfDiscId, msg);
    ASSERT_TRUE(ndefWrite == NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}
/**
 * @tc.name: NdefRead001
 * @tc.desc: Test TagSession NdefRead.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, NdefRead001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_INDEX_1;
    std::string ndefRead = tagSession->NdefRead(tagRfDiscId);
    ASSERT_TRUE(ndefRead == "");
}
/**
 * @tc.name: IsTagFieldOn001
 * @tc.desc: Test TagSession IsTagFieldOn.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, IsTagFieldOn001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_INDEX_1;
    bool isTagFieldOn = tagSession->IsTagFieldOn(tagRfDiscId);
    ASSERT_TRUE(!isTagFieldOn);
}
/**
 * @tc.name: GetTechList001
 * @tc.desc: Test TagSession GetTechList.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetTechList001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_INDEX_1;
    std::vector<int> getTechList = tagSession->GetTechList(tagRfDiscId);
    ASSERT_TRUE(getTechList.empty());
}
/**
 * @tc.name: GetTimeout001
 * @tc.desc: Test TagSession GetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetTimeout001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int technology = TEST_INDEX_2;
    int timeout = TEST_INDEX_1;
    int getTimeout = tagSession->GetTimeout(technology, timeout);
    ASSERT_TRUE(getTimeout == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: GetTimeout002
 * @tc.desc: Test TagSession GetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetTimeout002, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int technology = TEST_INDEX_3;
    int timeout = TEST_INDEX_1;
    int getTimeout = tagSession->GetTimeout(technology, timeout);
    ASSERT_TRUE(getTimeout == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: GetTimeout003
 * @tc.desc: Test TagSession GetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetTimeout003, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int technology = 0;
    int timeout = TEST_INDEX_1;
    int getTimeout = tagSession->GetTimeout(technology, timeout);
    ASSERT_TRUE(getTimeout == NFC::KITS::ErrorCode::ERR_NONE);
}
/**
 * @tc.name: SetTimeout001
 * @tc.desc: Test TagSession SetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, SetTimeout001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int timeout = TEST_INDEX_1;
    int technology = TEST_INDEX_2;
    int setTimeout = tagSession->SetTimeout(timeout, technology);
    ASSERT_TRUE(setTimeout == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: SetTimeout002
 * @tc.desc: Test TagSession SetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, SetTimeout002, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int timeout = TEST_INDEX_1;
    int technology = TEST_INDEX_3;
    tagSession->Disconnect(technology);
    int setTimeout = tagSession->SetTimeout(timeout, technology);
    ASSERT_TRUE(setTimeout == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: SetTimeout003
 * @tc.desc: Test TagSession SetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, SetTimeout003, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int timeout = TEST_INDEX_1;
    int technology = 0;
    tagSession->Disconnect(technology);
    int setTimeout = tagSession->SetTimeout(timeout, technology);
    ASSERT_TRUE(setTimeout == NFC::KITS::ErrorCode::ERR_NONE);
}
/**
 * @tc.name: Reconnect001
 * @tc.desc: Test TagSession Reconnect.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, Reconnect001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_INDEX_1;
    int reconnect = tagSession->Reconnect(tagRfDiscId);
    ASSERT_TRUE(reconnect == NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}
/**
 * @tc.name: Connect001
 * @tc.desc: Test TagSession Connect.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, Connect001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_INDEX_1;
    int technology = TEST_INDEX_2;
    int connect = tagSession->Connect(tagRfDiscId, technology);
    std::shared_ptr<INfcService> service_ = nullptr;
    sptr<NFC::TAG::TagSession> tagSession_ = new NFC::TAG::TagSession(service_);
    ASSERT_TRUE(connect == NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}
/**
 * @tc.name: IsNdef001
 * @tc.desc: Test TagSession IsNdef.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, IsNdef001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_INDEX_1;
    bool isNdef = tagSession->IsNdef(tagRfDiscId);
    ASSERT_TRUE(isNdef == false);
}
/**
 * @tc.name: SendRawFrame001
 * @tc.desc: Test TagSession SendRawFrame.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, SendRawFrame001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_INDEX_1;
    std::string hexCmdData = "";
    bool raw = true;
    std::string hexRespData = "";
    int sendRawFrame = tagSession->SendRawFrame(tagRfDiscId, hexCmdData, raw, hexRespData);
    ASSERT_TRUE(sendRawFrame == NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}
/**
 * @tc.name: FormatNdef001
 * @tc.desc: Test TagSession FormatNdef.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, FormatNdef001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_INDEX_1;
    const std::string key = "";
    int formatNdef = tagSession->FormatNdef(tagRfDiscId, key);
    ASSERT_TRUE(formatNdef == NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}
/**
 * @tc.name: CanMakeReadOnly001
 * @tc.desc: Test TagSession CanMakeReadOnly.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, CanMakeReadOnly001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int ndefType = TEST_INDEX_1;
    bool canSetReadOnly = true;
    int canMakeReadOnly = tagSession->CanMakeReadOnly(ndefType, canSetReadOnly);
    ASSERT_TRUE(canMakeReadOnly == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: IsSupportedApdusExtended001
 * @tc.desc: Test TagSession IsSupportedApdusExtended.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, IsSupportedApdusExtended001, TestSize.Level1)
{
    std::shared_ptr<INfcService> service = std::make_shared<NfcServiceImpl>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    bool isSupported = true;
    int isSupportedApdusExtended = tagSession->IsSupportedApdusExtended(isSupported);
    ASSERT_TRUE(isSupportedApdusExtended == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
}
}
}
}
