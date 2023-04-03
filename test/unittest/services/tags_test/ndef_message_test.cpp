/*
 * Copyright (C) 2023-2023 Huawei Device Co., Ltd.
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

#include "ndef_message.h"
#include "nfc_sdk_common.h"
#include "taginfo.h"
#include "tag_session_proxy.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class NdefMessageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TEST_MIME_TYPE = "mimeType";
    static constexpr const auto TEST_MIME_DATA = "mimeData";
};

void NdefMessageTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NdefMessageTest." << std::endl;
}

void NdefMessageTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NdefMessageTest." << std::endl;
}

void NdefMessageTest::SetUp()
{
    std::cout << " SetUp NdefMessageTest." << std::endl;
}

void NdefMessageTest::TearDown()
{
    std::cout << " TearDown NdefMessageTest." << std::endl;
}

/**
 * @tc.name: GetNdefMessage001
 * @tc.desc: Test NdefMessage GetNdefMessage.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMessageTest, GetNdefMessage001, TestSize.Level1)
{
    std::vector<std::shared_ptr<NdefRecord>> ndefRecords;
    std::shared_ptr<NdefMessage> getNdefMessage = NdefMessage::GetNdefMessage(ndefRecords);
    ASSERT_TRUE(getNdefMessage != nullptr);
}
/**
 * @tc.name: MakeUriRecord001
 * @tc.desc: Test NdefMessage MakeUriRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMessageTest, MakeUriRecord001, TestSize.Level1)
{
    const std::string uriString = "";
    std::shared_ptr<NdefRecord> getNdefMessage = NdefMessage::MakeUriRecord(uriString);
    ASSERT_TRUE(getNdefMessage == nullptr);
}
/**
 * @tc.name: MakeUriRecord002
 * @tc.desc: Test NdefMessage MakeUriRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMessageTest, MakeUriRecord002, TestSize.Level1)
{
    std::shared_ptr<NdefRecord> getNdefMessage = NdefMessage::MakeUriRecord(TEST_MIME_TYPE);
    ASSERT_TRUE(getNdefMessage != nullptr);
}
/**
 * @tc.name: MakeTextRecord001
 * @tc.desc: Test NdefMessage MakeTextRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMessageTest, MakeTextRecord001, TestSize.Level1)
{
    const std::string mimeType;
    const std::string mimeData;
    std::shared_ptr<NdefRecord> getNdefMessage = NdefMessage::MakeTextRecord(mimeType, mimeData);
    ASSERT_TRUE(getNdefMessage != nullptr);
}
/**
 * @tc.name: MakeMimeRecord001
 * @tc.desc: Test NdefMessage MakeMimeRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMessageTest, MakeMimeRecord001, TestSize.Level1)
{
    const std::string mimeType = "";
    std::shared_ptr<NdefRecord> getNdefMessage = NdefMessage::MakeMimeRecord(mimeType, TEST_MIME_DATA);
    ASSERT_TRUE(getNdefMessage == nullptr);
}
/**
 * @tc.name: MakeMimeRecord002
 * @tc.desc: Test NdefMessage MakeMimeRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMessageTest, MakeMimeRecord002, TestSize.Level1)
{
    const std::string mimeData = "";
    std::shared_ptr<NdefRecord> getNdefMessage = NdefMessage::MakeMimeRecord(TEST_MIME_TYPE, mimeData);
    ASSERT_TRUE(getNdefMessage == nullptr);
}
/**
 * @tc.name: MakeMimeRecord003
 * @tc.desc: Test NdefMessage MakeMimeRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMessageTest, MakeMimeRecord003, TestSize.Level1)
{
    std::shared_ptr<NdefRecord> getNdefMessage = NdefMessage::MakeMimeRecord(TEST_MIME_TYPE, TEST_MIME_DATA);
    ASSERT_TRUE(getNdefMessage != nullptr);
}
/**
 * @tc.name: MakeExternalRecord001
 * @tc.desc: Test NdefMessage MakeExternalRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMessageTest, MakeExternalRecord001, TestSize.Level1)
{
    const std::string domainName = "";
    const std::string serviceName = "serviceName";
    const std::string externalData = "externalData";
    std::shared_ptr<NdefRecord> getNdefMessage = NdefMessage::MakeExternalRecord(domainName, serviceName, externalData);
    ASSERT_TRUE(getNdefMessage == nullptr);
}
/**
 * @tc.name: MakeExternalRecord002
 * @tc.desc: Test NdefMessage MakeExternalRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMessageTest, MakeExternalRecord002, TestSize.Level1)
{
    const std::string domainName = "domainName";
    const std::string serviceName = "";
    const std::string externalData = "externalData";
    std::shared_ptr<NdefRecord> getNdefMessage = NdefMessage::MakeExternalRecord(domainName, serviceName, externalData);
    ASSERT_TRUE(getNdefMessage == nullptr);
}
/**
 * @tc.name: MakeExternalRecord003
 * @tc.desc: Test NdefMessage MakeExternalRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMessageTest, MakeExternalRecord003, TestSize.Level1)
{
    const std::string domainName = "domainName";
    const std::string serviceName = "serviceName";
    const std::string externalData = "";
    std::shared_ptr<NdefRecord> getNdefMessage = NdefMessage::MakeExternalRecord(domainName, serviceName, externalData);
    ASSERT_TRUE(getNdefMessage == nullptr);
}
/**
 * @tc.name: MakeExternalRecord004
 * @tc.desc: Test NdefMessage MakeExternalRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMessageTest, MakeExternalRecord004, TestSize.Level1)
{
    const std::string domainName = "domainName";
    const std::string serviceName = "serviceName";
    const std::string externalData = "externalData";
    std::shared_ptr<NdefRecord> getNdefMessage = NdefMessage::MakeExternalRecord(domainName, serviceName, externalData);
    ASSERT_TRUE(getNdefMessage != nullptr);
}
/**
 * @tc.name: MessageToString001
 * @tc.desc: Test NdefMessage MessageToString.
 * @tc.type: FUNC
 */
HWTEST_F(NdefMessageTest, MessageToString001, TestSize.Level1)
{
    std::shared_ptr<NdefMessage> ndefMessage = nullptr;
    std::string messageToString = NdefMessage::MessageToString(ndefMessage);
    ASSERT_TRUE(messageToString == "");
}
}
}
}