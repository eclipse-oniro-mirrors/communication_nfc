/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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
#include "ndef_record_parser.h"
#include "ndef_message.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;

class NdefRecordParserTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    std::shared_ptr<NdefRecord> CreateTestRecord(int tnf, const std::string &tagRtdType, const std::string &payload);
};

void NdefRecordParserTest::SetUpTestCase()
{
    std::cout << "SetUpTestCase NdefRecordParserTest." << std::endl;
}

void NdefRecordParserTest::TearDownTestCase()
{
    std::cout << "TearDownTestCase NdefRecordParserTest." << std::endl;
}

void NdefRecordParserTest::SetUp()
{
    std::cout << "SetUp NdefRecordParserTest." << std::endl;
}

void NdefRecordParserTest::TearDown()
{
    std::cout << "TearDown NdefRecordParserTest." << std::endl;
}

std::shared_ptr<NdefRecord> NdefRecordParserTest::CreateTestRecord(int tnf, const std::string &tagRtdType,
                                                                   const std::string &payload)
{
    auto record = std::make_shared<NdefRecord>();
    record->tnf_ = tnf;
    record->tagRtdType_ = tagRtdType;
    record->payload_ = payload;
    return record;
}

/**
 * @tc.name: ExtractHarPackages001
 * @tc.desc: Test NdefRecordParser ExtractHarPackages with empty records.
 * @tc.type: FUNC
 */
HWTEST_F(NdefRecordParserTest, ExtractHarPackages001, TestSize.Level1)
{
    std::vector<std::shared_ptr<NdefRecord>> records;
    auto result = NdefRecordParser::ExtractHarPackages(records);
    EXPECT_TRUE(result.empty());
}

/**
 * @tc.name: ExtractHarPackages002
 * @tc.desc: Test NdefRecordParser ExtractHarPackages with valid OHOS app record.
 * @tc.type: FUNC
 */
HWTEST_F(NdefRecordParserTest, ExtractHarPackages002, TestSize.Level1)
{
    std::vector<std::shared_ptr<NdefRecord>> records;
    records.push_back(CreateTestRecord(
        NdefMessage::TNF_EXTERNAL_TYPE,
        NfcSdkCommon::StringToHexString(NdefMessage::GetTagRtdType(NdefMessage::RTD_OHOS_APP)), "com.example.app"));

    auto result = NdefRecordParser::ExtractHarPackages(records);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], "com.example.app");
}

/**
 * @tc.name: GetNdefRecordMimeType001
 * @tc.desc: Test NdefRecordParser GetNdefRecordMimeType with text record.
 * @tc.type: FUNC
 */
HWTEST_F(NdefRecordParserTest, GetNdefRecordMimeType001, TestSize.Level1)
{
    auto record = CreateTestRecord(NdefMessage::TNF_WELL_KNOWN,
                                   NfcSdkCommon::StringToHexString(NdefMessage::GetTagRtdType(NdefMessage::RTD_TEXT)),
                                   "test");

    auto result = NdefRecordParser::GetNdefRecordMimeType(record);
    EXPECT_EQ(result, "text/plain");
}

/**
 * @tc.name: GetUriPayload001
 * @tc.desc: Test NdefRecordParser GetUriPayload with valid URI record.
 * @tc.type: FUNC
 */
HWTEST_F(NdefRecordParserTest, GetUriPayload001, TestSize.Level1)
{
    auto record = CreateTestRecord(NdefMessage::TNF_WELL_KNOWN,
                                   NfcSdkCommon::StringToHexString(NdefMessage::GetTagRtdType(NdefMessage::RTD_URI)),
                                   "02313233342E636F6D");

    auto result = NdefRecordParser::GetUriPayload(record);
    EXPECT_EQ(result, "https://www.1234.com");
}
}  // namespace TEST
}  // namespace NFC
}  // namespace OHOS