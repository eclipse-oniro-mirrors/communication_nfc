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

#define private public
#define protected public

#include "tag_session_stub_test.h"

#include <gtest/gtest.h>
#include <thread>

#include "nfc_controller.h"
#include "nfc_controller_impl.h"
#include "nfc_controller_stub.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include "nfc_service_tdd.h"
#include "nfc_permission_checker.h"
#include "tag_session.h"
#include <iostream>

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
using namespace OHOS::NFC::KITS;
std::shared_ptr<NFC::AppStateObserver> g_appStateObserver = nullptr;
class TagSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto MAX_TECH = 12;
    static constexpr const auto TEST_DISC_ID = 1;
    const int MAX_TRANS_LENGTH[MAX_TECH] = {0, 253, 253, 261, 255, 253, 0, 0, 253, 253, 0, 0};
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
 * @tc.name: GetMaxTransceiveLength001
 * @tc.desc: Test TagSession GetMaxTransceiveLength.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetMaxTransceiveLength001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int technology = MAX_TECH;
    int maxSize = MAX_TRANS_LENGTH[0];
    int result = tagSession->GetMaxTransceiveLength(technology, maxSize);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
    ASSERT_TRUE(maxSize == MAX_TRANS_LENGTH[0]);
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
    int maxSize = MAX_TRANS_LENGTH[0];
    int result = tagSession->GetMaxTransceiveLength(technology, maxSize);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS);
    ASSERT_TRUE(maxSize == MAX_TRANS_LENGTH[0]);
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
    int maxSize = MAX_TRANS_LENGTH[0];
    int result = tagSession->GetMaxTransceiveLength(technology, maxSize);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_NONE);
    ASSERT_TRUE(maxSize == MAX_TRANS_LENGTH[1]);
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
    int result = tagSession->UnregForegroundDispatch(element);
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
    int tagRfDiscId = TEST_DISC_ID;
    int result = tagSession->NdefMakeReadOnly(tagRfDiscId);
    tagSession->RegForegroundDispatch(element, discTech, callback);
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_UNBIND);
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
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_UNBIND);
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
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_UNBIND);
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
    std::string ndefRead {};
    tagSession->NdefRead(tagRfDiscId, ndefRead);
service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession1 = new NFC::TAG::TagSession(service);
    tagSession1->NdefRead(tagRfDiscId, ndefRead);
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
    bool isTagFieldOn = false;
    tagSession->IsTagFieldOn(tagRfDiscId, isTagFieldOn);
service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession1 = new NFC::TAG::TagSession(service);
    tagSession1->IsTagFieldOn(tagRfDiscId, isTagFieldOn);
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
    std::vector<int> getTechList = {};
    tagSession->GetTechList(tagRfDiscId, getTechList);
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
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_UNBIND);
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
service->Initialize();
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
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_UNBIND);
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
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_UNBIND);
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
    bool isNdef = false;
    tagSession->IsNdef(tagRfDiscId, isNdef);
    ASSERT_TRUE(!isNdef);
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
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_UNBIND);
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
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_UNBIND);
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
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_UNBIND);
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
    ASSERT_TRUE(result == NFC::KITS::ErrorCode::ERR_TAG_STATE_UNBIND);
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
    int errorCode = tagSession->RegReaderMode(element, discTech, callback);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_NFC_PARAMETERS);
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
    int errorCode = tagSession->UnregReaderMode(element);
    ASSERT_TRUE(errorCode == NFC::KITS::ErrorCode::ERR_NONE);
}

/**
 * @tc.name: ResetTimeout001
 * @tc.desc: Test TagSession ResetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, ResetTimeout001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = nullptr;
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = 0;
    tagSession->ResetTimeout(tagRfDiscId);
    ASSERT_TRUE(tagSession != nullptr);
}

/**
 * @tc.name: ResetTimeout002
 * @tc.desc: Test TagSession ResetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, ResetTimeout002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = 0;
    tagSession->ResetTimeout(tagRfDiscId);
    ASSERT_TRUE(tagSession != nullptr);
}

/**
 * @tc.name: IsConnected001
 * @tc.desc: Test TagSession IsConnected.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, IsConnected001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = nullptr;
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = 0;
    bool isConnected = false;
    int ret = tagSession->IsConnected(tagRfDiscId, isConnected);
    ASSERT_TRUE(ret == NFC::KITS::ErrorCode::ERR_TAG_STATE_UNBIND);
}

/**
 * @tc.name: IsConnected002
 * @tc.desc: Test TagSession IsConnected.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, IsConnected002, TestSize.Level1)
{
    NfcController ctrl = NfcController::GetInstance();
    ctrl.TurnOff();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    int tagRfDiscId = 0;
    bool isConnected = false;
    int ret = tagSession->IsConnected(tagRfDiscId, isConnected);
    ASSERT_TRUE(ret == NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED);
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

    ElementName element;
    TAG::FgData fgData(true, element, std::vector<uint32_t>(), nullptr);
    TAG::ReaderData readerData(true, element, std::vector<uint32_t>(), nullptr, 0);
    tagSession->fgDataVec_.push_back(fgData);
    tagSession->HandleAppStateChanged(bundleName, abilityName, abilityState);

    tagSession->fgDataVec_.clear();
    tagSession->readerDataVec_.push_back(readerData);
    tagSession->HandleAppStateChanged(bundleName, abilityName, abilityState);

    tagSession->fgDataVec_.push_back(fgData);
    tagSession->HandleAppStateChanged(bundleName, abilityName, abilityState);
    ASSERT_TRUE(tagSession != nullptr);
}

/**
 * @tc.name: IsSameAppAbility001
 * @tc.desc: Test TagSession IsSameAppAbility.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, IsSameAppAbility001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);

    ElementName element("", "bundleName", "abilityName", "");
    ElementName fgElement("", "bundleName", "abilityName", "");
    ASSERT_TRUE(tagSession->IsSameAppAbility(element, fgElement));

    ElementName element1("", "bundleName1", "abilityName", "");
    ElementName fgElement1("", "bundleName", "abilityName", "");
    ASSERT_FALSE(tagSession->IsSameAppAbility(element1, fgElement1));

    ElementName element2("", "bundleName", "abilityName1", "");
    ElementName fgElement2("", "bundleName", "abilityName", "");
    ASSERT_FALSE(tagSession->IsSameAppAbility(element2, fgElement2));

    ElementName element3("", "bundleName1", "abilityName1", "");
    ElementName fgElement3("", "bundleName", "abilityName", "");
    ASSERT_FALSE(tagSession->IsSameAppAbility(element3, fgElement3));
}

/**
 * @tc.name: IsSameDiscoveryPara001
 * @tc.desc: Test TagSession IsSameDiscoveryPara.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, IsSameDiscoveryPara001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);

    std::vector<uint32_t> discoveryPara = {1, 2, 3};
    std::vector<uint32_t> discTech = {1, 2, 3};
    ASSERT_TRUE(tagSession->IsSameDiscoveryPara(discoveryPara, discTech));

    std::vector<uint32_t> discoveryPara1 = {};
    std::vector<uint32_t> discTech1 = {};
    ASSERT_TRUE(tagSession->IsSameDiscoveryPara(discoveryPara1, discTech1));

    std::vector<uint32_t> discoveryPara2 = {1, 2, 3};
    std::vector<uint32_t> discTech2 = {1, 3, 2};
    ASSERT_TRUE(tagSession->IsSameDiscoveryPara(discoveryPara2, discTech2));

    std::vector<uint32_t> discoveryPara3 = {1, 2, 3};
    std::vector<uint32_t> discTech3 = {1, 2, 4};
    ASSERT_FALSE(tagSession->IsSameDiscoveryPara(discoveryPara3, discTech3));

    std::vector<uint32_t> discoveryPara4 = {1, 2, 3};
    std::vector<uint32_t> discTech4 = {1, 2, 2};
    ASSERT_FALSE(tagSession->IsSameDiscoveryPara(discoveryPara4, discTech4));
}

/**
 * @tc.name: RegForegroundDispatchInner001
 * @tc.desc: Test TagSession RegForegroundDispatchInner.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, RegForegroundDispatchInner001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);

    ElementName element("", "bundleName", "abilityName", "");
    int result = tagSession->RegForegroundDispatchInner(element, std::vector<uint32_t>(), nullptr, true);
    std::cout << "result " << result << std::endl;
    ASSERT_TRUE(result == KITS::ERR_TAG_STATE_UNBIND);

    TAG::FgData fgData(true, element, std::vector<uint32_t>(), nullptr);
    tagSession->fgDataVec_.push_back(fgData);
    result = tagSession->RegForegroundDispatchInner(element, std::vector<uint32_t>(), nullptr, true);
    std::cout << "result " << result << std::endl;
    ASSERT_TRUE(result == KITS::ERR_NONE);
}

/**
 * @tc.name: RegForegroundDispatchInner002
 * @tc.desc: Test TagSession RegForegroundDispatchInner.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, RegForegroundDispatchInner002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);

    ElementName element("", "bundleName", "abilityName", "");
    int result = tagSession->RegForegroundDispatchInner(element, std::vector<uint32_t>(), nullptr, true);
    std::cout << "result " << result << std::endl;
    ASSERT_TRUE(result == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: UnregForegroundDispatchInner001
 * @tc.desc: Test TagSession UnregForegroundDispatchInner.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, UnregForegroundDispatchInner001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);

    ElementName element("", "bundleName", "abilityName", "");
    int result = tagSession->UnregForegroundDispatchInner(element, true);
    std::cout << "result " << result << std::endl;
    ASSERT_TRUE(result == KITS::ERR_NONE);

    TAG::FgData fgData(true, element, std::vector<uint32_t>(), nullptr);
    tagSession->fgDataVec_.push_back(fgData);
    result = tagSession->UnregForegroundDispatchInner(element, true);
    std::cout << "result " << result << std::endl;
    ASSERT_TRUE(result == KITS::ERR_TAG_STATE_UNBIND);
}

/**
 * @tc.name: UnregForegroundDispatchInner002
 * @tc.desc: Test TagSession UnregForegroundDispatchInner.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, UnregForegroundDispatchInner002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);

    ElementName element("", "bundleName", "abilityName", "");
    TAG::FgData fgData(true, element, std::vector<uint32_t>(), nullptr);
    tagSession->fgDataVec_.push_back(fgData);
    int result = tagSession->UnregForegroundDispatchInner(element, true);
    std::cout << "result " << result << std::endl;
    ASSERT_TRUE(result == KITS::ERR_NONE);
}

/**
 * @tc.name: RegReaderModeInner001
 * @tc.desc: Test TagSession RegReaderModeInner.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, RegReaderModeInner001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);

    ElementName element("", "bundleName", "abilityName", "");
    int result = tagSession->RegReaderModeInner(element, std::vector<uint32_t>(), nullptr, true);
    std::cout << "result " << result << std::endl;
    ASSERT_TRUE(result == KITS::ERR_TAG_STATE_UNBIND);

    TAG::ReaderData readerData(true, element, std::vector<uint32_t>(), nullptr, 0);
    tagSession->readerDataVec_.push_back(readerData);
    result = tagSession->RegReaderModeInner(element, std::vector<uint32_t>(), nullptr, true);
    std::cout << "result " << result << std::endl;
    ASSERT_TRUE(result == KITS::ERR_NONE);
}

/**
 * @tc.name: RegReaderModeInner002
 * @tc.desc: Test TagSession RegReaderModeInner.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, RegReaderModeInner002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);

    ElementName element("", "bundleName", "abilityName", "");
    int result = tagSession->RegReaderModeInner(element, std::vector<uint32_t>(), nullptr, true);
    std::cout << "result " << result << std::endl;
    ASSERT_TRUE(result == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: UnregReaderModeInner001
 * @tc.desc: Test TagSession UnregReaderModeInner.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, UnregReaderModeInner001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);

    ElementName element("", "bundleName", "abilityName", "");
    int result = tagSession->UnregReaderModeInner(element, true);
    std::cout << "result " << result << std::endl;
    ASSERT_TRUE(result == KITS::ERR_NONE);

    TAG::ReaderData readerData(true, element, std::vector<uint32_t>(), nullptr, 0);
    tagSession->readerDataVec_.push_back(readerData);
    result = tagSession->UnregReaderModeInner(element, true);
    std::cout << "result " << result << std::endl;
    ASSERT_TRUE(result == KITS::ERR_TAG_STATE_UNBIND);
}

/**
 * @tc.name: UnregReaderModeInner002
 * @tc.desc: Test TagSession UnregReaderModeInner.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, UnregReaderModeInner002, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    service->Initialize();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);

    ElementName element("", "bundleName", "abilityName", "");
    TAG::ReaderData readerData(true, element, std::vector<uint32_t>(), nullptr);
    tagSession->readerDataVec_.push_back(readerData);
    int result = tagSession->UnregReaderModeInner(element, true);
    std::cout << "result " << result << std::endl;
    ASSERT_TRUE(result == KITS::ERR_NONE);
}

/**
 * @tc.name: IsVendorProcess001
 * @tc.desc: Test TagSession IsVendorProcess.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, IsVendorProcess001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    bool ret = tagSession->IsVendorProcess();
    ASSERT_TRUE(!ret);
}

/**
 * @tc.name: IsForegroundApp001
 * @tc.desc: Test TagSession IsForegroundApp.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, IsForegroundApp001, TestSize.Level1)
{
    std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
    sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
    ElementName element;
    element.bundleName_ = "test";
    bool ret = g_appStateObserver->IsForegroundApp(element.GetBundleName());
    ASSERT_TRUE(!ret);
}
}
}
}
