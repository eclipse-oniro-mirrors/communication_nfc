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
#include "nfc_permission_checker.h"
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
    static constexpr const auto MAX_TECH = 12;
    static constexpr const auto TEST_DISC_ID = 1;
    int g_maxTransLength[MAX_TECH] = {0, 253, 253, 261, 255, 253, 0, 0, 253, 253, 0, 0};
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
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int32_t fd = -1;
    const std::vector<std::u16string> args;
    int32_t result = tagSession->Dump(fd, args);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: Dump002
 * @tc.desc: Test TagSession Dump.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, Dump002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int32_t fd = 1;
    const std::vector<std::u16string> args;
    int32_t result = tagSession->Dump(fd, args);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_NONE);
}
/**
 * @tc.name: GetMaxTransceiveLength001
 * @tc.desc: Test TagSession GetMaxTransceiveLength.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetMaxTransceiveLength001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int technology = MAX_TECH;
    int maxSize = g_maxTransLength[0];
    int result = tagSession->GetMaxTransceiveLength(technology, maxSize);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
    ASSERT_TRUE(maxSize == g_maxTransLength[0]);
}
/**
 * @tc.name: GetMaxTransceiveLength002
 * @tc.desc: Test TagSession GetMaxTransceiveLength.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetMaxTransceiveLength002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int technology = -1;
    int maxSize = g_maxTransLength[0];
    int result = tagSession->GetMaxTransceiveLength(technology, maxSize);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
    ASSERT_TRUE(maxSize == g_maxTransLength[0]);
}
/**
 * @tc.name: GetMaxTransceiveLength003
 * @tc.desc: Test TagSession GetMaxTransceiveLength.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetMaxTransceiveLength003, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int technology = static_cast<int>(KITS::TagTechnology::NFC_A_TECH);
    int maxSize = g_maxTransLength[0];
    int result = tagSession->GetMaxTransceiveLength(technology, maxSize);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_NONE);
    ASSERT_TRUE(maxSize == g_maxTransLength[1]);
}
/**
 * @tc.name: UnregForegroundDispatch001
 * @tc.desc: Test TagSession UnregForegroundDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, UnregForegroundDispatch001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    AppExecFwk::ElementName element;
    KITS::ErrorCode result = tagSession->UnregForegroundDispatch(element);
    ASSERT_TRUE(result == KITS::ERR_NONE);
}
/**
 * @tc.name: RegForegroundDispatch001
 * @tc.desc: Test TagSession RegForegroundDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, RegForegroundDispatch001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    AppExecFwk::ElementName element;
    std::vector<uint32_t> discTech;
    const sptr<KITS::IForegroundCallback> callback;
    KITS::ErrorCode result = tagSession->RegForegroundDispatch(element, discTech, callback);
    ASSERT_TRUE(result != KITS::ERR_NONE);
}
/**
 * @tc.name: NdefMakeReadOnly001
 * @tc.desc: Test TagSession NdefMakeReadOnly.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, NdefMakeReadOnly001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
    int result = tagSession->NdefMakeReadOnly(tagRfDiscId);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND);
}
/**
 * @tc.name: NdefMakeReadOnly002
 * @tc.desc: Test TagSession NdefMakeReadOnly.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, NdefMakeReadOnly002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
    int result = tagSession->NdefMakeReadOnly(tagRfDiscId);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}
/**
 * @tc.name: NdefWrite001
 * @tc.desc: Test TagSession NdefWrite.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, NdefWrite001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
    std::string msg = "";
    int result = tagSession->NdefWrite(tagRfDiscId, msg);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND);
}
/**
 * @tc.name: NdefWrite002
 * @tc.desc: Test TagSession NdefWrite.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, NdefWrite002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
    std::string msg = "";
    int result = tagSession->NdefWrite(tagRfDiscId, msg);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}
/**
 * @tc.name: NdefRead001
 * @tc.desc: Test TagSession NdefRead.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, NdefRead001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
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
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
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
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
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
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int technology = -1;
    int tagRfDiscId = TEST_DISC_ID;
    int timeout = 0;
    int result = tagSession->GetTimeout(tagRfDiscId, technology, timeout);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: GetTimeout002
 * @tc.desc: Test TagSession GetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetTimeout002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int technology = MAX_TECH;
    int tagRfDiscId = TEST_DISC_ID;
    int timeout = 0;
    int result = tagSession->GetTimeout(tagRfDiscId, technology, timeout);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: GetTimeout003
 * @tc.desc: Test TagSession GetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetTimeout003, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int technology = static_cast<int>(KITS::TagTechnology::NFC_A_TECH);
    int tagRfDiscId = TEST_DISC_ID;
    int timeout = 0;
    int result = tagSession->GetTimeout(tagRfDiscId, technology, timeout);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND);
}
/**
 * @tc.name: GetTimeout004
 * @tc.desc: Test TagSession GetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetTimeout004, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int technology = static_cast<int>(KITS::TagTechnology::NFC_A_TECH);
    int tagRfDiscId = TEST_DISC_ID;
    int timeout = 0;
    int result = tagSession->GetTimeout(tagRfDiscId, technology, timeout);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}
/**
 * @tc.name: SetTimeout001
 * @tc.desc: Test TagSession SetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, SetTimeout001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int timeout = 0;
    int technology = -1;
    int result = tagSession->SetTimeout(TEST_DISC_ID, timeout, technology);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: SetTimeout002
 * @tc.desc: Test TagSession SetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, SetTimeout002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int timeout = 0;
    int technology = MAX_TECH;
    tagSession->Disconnect(technology);
    int result = tagSession->SetTimeout(TEST_DISC_ID, timeout, technology);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: SetTimeout003
 * @tc.desc: Test TagSession SetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, SetTimeout003, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int timeout = 0;
    int technology = static_cast<int>(KITS::TagTechnology::NFC_A_TECH);
    tagSession->Disconnect(technology);
    int result = tagSession->SetTimeout(TEST_DISC_ID, timeout, technology);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED || result == NFC::KITS::ErrorCode::ERR_NONE);
}
/**
 * @tc.name: Reconnect001
 * @tc.desc: Test TagSession Reconnect.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, Reconnect001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
    int result = tagSession->Reconnect(tagRfDiscId);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND);
}
/**
 * @tc.name: Reconnect002
 * @tc.desc: Test TagSession Reconnect.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, Reconnect002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
    int result = tagSession->Reconnect(tagRfDiscId);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}
/**
 * @tc.name: Connect001
 * @tc.desc: Test TagSession Connect.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, Connect001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
    int technology = -1;
    int result = tagSession->Connect(tagRfDiscId, technology);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: Connect002
 * @tc.desc: Test TagSession Connect.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, Connect002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
    int technology = 1;
    int result = tagSession->Connect(tagRfDiscId, technology);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND);
}
/**
 * @tc.name: Connect003
 * @tc.desc: Test TagSession Connect.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, Connect003, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
    int technology = 1;
    int result = tagSession->Connect(tagRfDiscId, technology);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}
/**
 * @tc.name: IsNdef001
 * @tc.desc: Test TagSession IsNdef.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, IsNdef001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
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
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
    std::string hexCmdData = "";
    bool raw = true;
    std::string hexRespData = "";
    int result = tagSession->SendRawFrame(tagRfDiscId, hexCmdData, raw, hexRespData);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND);
}
/**
 * @tc.name: SendRawFrame002
 * @tc.desc: Test TagSession SendRawFrame.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, SendRawFrame002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
    std::string hexCmdData = "";
    bool raw = true;
    std::string hexRespData = "";
    int result = tagSession->SendRawFrame(tagRfDiscId, hexCmdData, raw, hexRespData);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}
/**
 * @tc.name: FormatNdef001
 * @tc.desc: Test TagSession FormatNdef.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, FormatNdef001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
    const std::string key = "";
    int result = tagSession->FormatNdef(tagRfDiscId, key);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND);
}
/**
 * @tc.name: FormatNdef002
 * @tc.desc: Test TagSession FormatNdef.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, FormatNdef002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = TEST_DISC_ID;
    const std::string key = "";
    int result = tagSession->FormatNdef(tagRfDiscId, key);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
}
/**
 * @tc.name: CanMakeReadOnly001
 * @tc.desc: Test TagSession CanMakeReadOnly.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, CanMakeReadOnly001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    static const auto NDEF_TYPE1_TAG = 1;
    int ndefType = NDEF_TYPE1_TAG;
    bool canSetReadOnly = true;
    int result = tagSession->CanMakeReadOnly(ndefType, canSetReadOnly);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND);
}
/**
 * @tc.name: CanMakeReadOnly002
 * @tc.desc: Test TagSession CanMakeReadOnly.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, CanMakeReadOnly002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    static const auto NDEF_TYPE1_TAG = 1;
    int ndefType = NDEF_TYPE1_TAG;
    bool canSetReadOnly = true;
    int result = tagSession->CanMakeReadOnly(ndefType, canSetReadOnly);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_NONE);
}
/**
 * @tc.name: IsSupportedApdusExtended001
 * @tc.desc: Test TagSession IsSupportedApdusExtended.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, IsSupportedApdusExtended001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    bool isSupported = true;
    int result = tagSession->IsSupportedApdusExtended(isSupported);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND);
}
/**
 * @tc.name: IsSupportedApdusExtended002
 * @tc.desc: Test TagSession IsSupportedApdusExtended.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, IsSupportedApdusExtended002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    bool isSupported = true;
    int result = tagSession->IsSupportedApdusExtended(isSupported);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_NONE);
}
/**
 * @tc.name: RegReaderMode001
 * @tc.desc: Test TagSession RegReaderMode.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, RegReaderMode001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    AppExecFwk::ElementName element;
    std::vector<uint32_t> discTech;
    const sptr<KITS::IReaderModeCallback> callback = nullptr;
    KITS::ErrorCode errorCode = tagSession->RegReaderMode(element, discTech, callback);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_TAG_APP_NOT_FOREGROUND);
}

/**
 * @tc.name: UnregReaderMode001
 * @tc.desc: Test TagSession UnregReaderMode.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, UnregReaderMode001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    AppExecFwk::ElementName element;
    KITS::ErrorCode errorCode = tagSession->UnregReaderMode(element);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_NONE);
}

/**
 * @tc.name: HandleAppStateChanged001
 * @tc.desc: Test TagSession HandleAppStateChanged.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, HandleAppStateChanged001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    std::string bundleName = "";
    std::string abilityName = "";
    int abilityState = 0;
    tagSession->HandleAppStateChanged(bundleName, abilityName, abilityState);
}
}
}
}
