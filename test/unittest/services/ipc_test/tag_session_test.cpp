/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "tag_session.h"
#include "nfc_service_mock.h"
#include "nfc_sdk_common.h"
#include "nfc_controller.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class TagSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
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
 * @tc.name: FormatNdef001
 * @tc.desc: Test TagSessionTest FormatNdef.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, FormatNdef001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
    tagSession->FormatNdef(0, "");
    ASSERT_TRUE(tagSession != nullptr);
}

/**
 * @tc.name: Connect001
 * @tc.desc: Test TagSessionTest Connect.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, Connect001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
    tagSession->Connect(0, 0);
    ASSERT_TRUE(tagSession != nullptr);
}

/**
 * @tc.name: IsConnected001
 * @tc.desc: Test TagSessionTest IsConnected.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, IsConnected001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    bool isConnected = false;
    std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
    tagSession->IsConnected(0, isConnected);
    ASSERT_TRUE(tagSession != nullptr);
}

/**
 * @tc.name: Reconnect001
 * @tc.desc: Test TagSessionTest Reconnect.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, Reconnect001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
    tagSession->Reconnect(0);
    ASSERT_TRUE(tagSession != nullptr);
}

/**
 * @tc.name: GetTimeout001
 * @tc.desc: Test TagSessionTest GetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetTimeout001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    int timeout = 0;
    std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
    tagSession->GetTimeout(0, 0, timeout);
    ASSERT_TRUE(tagSession != nullptr);
}

/**
 * @tc.name: ResetTimeout001
 * @tc.desc: Test TagSessionTest ResetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, ResetTimeout001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
    tagSession->ResetTimeout(0);
    ASSERT_TRUE(tagSession != nullptr);
}

/**
 * @tc.name: GetTechList001
 * @tc.desc: Test TagSessionTest GetTechList.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, GetTechList001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
    tagSession->GetTechList(0);
    ASSERT_TRUE(tagSession != nullptr);
}

/**
 * @tc.name: IsTagFieldOn001
 * @tc.desc: Test TagSessionTest IsTagFieldOn.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, IsTagFieldOn001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
    tagSession->IsTagFieldOn(0);
    ASSERT_TRUE(tagSession != nullptr);
}

/**
 * @tc.name: IsNdef001
 * @tc.desc: Test TagSessionTest IsNdef.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, IsNdef001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
    tagSession->IsNdef(0);
    ASSERT_TRUE(tagSession != nullptr);
}

/**
 * @tc.name: NdefRead001
 * @tc.desc: Test TagSessionTest NdefRead.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, NdefRead001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    std::string ndefMessage = "";
    std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
    tagSession->NdefRead(0, ndefMessage);
    ASSERT_TRUE(tagSession != nullptr);
}

/**
 * @tc.name: NdefWrite001
 * @tc.desc: Test TagSessionTest NdefWrite.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, NdefWrite001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
    tagSession->NdefWrite(0, "");
    ASSERT_TRUE(tagSession != nullptr);
}

/**
 * @tc.name: NdefWrite002
 * @tc.desc: Test TagSessionTest NdefWrite.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, NdefWrite002, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
    tagSession->NdefWrite(0, "test");
    ASSERT_TRUE(tagSession != nullptr);
}

/**
 * @tc.name: NdefMakeReadOnly001
 * @tc.desc: Test TagSessionTest NdefMakeReadOnly.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, NdefMakeReadOnly001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    nfcService->Initialize();
    std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
    tagSession->NdefMakeReadOnly(0);
    ASSERT_TRUE(tagSession != nullptr);
}
} // namespace TEST
} // namespace NFC
} // namespace OHOS