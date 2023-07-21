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
}
}
}
