/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ndef_formatable_tag.h"
#include "nfca_tag.h"
#include "nfc_sdk_common.h"
#include "taginfo.h"
#include "tag_session_proxy.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class NdefFormatableTagTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TEST_UID = "0102";
    static constexpr const auto TEST_DISC_ID = 1;
    static constexpr const auto TEST_SAK = 0x44;
    static constexpr const auto TEST_ATQA = "0400";
    static constexpr const auto TEST_NDEF_FORMATABLE_INDEX = 0;
    std::shared_ptr<TagInfo> tagInfo_;
};

void NdefFormatableTagTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NdefFormatableTagTest." << std::endl;
}

void NdefFormatableTagTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NdefFormatableTagTest." << std::endl;
}

void NdefFormatableTagTest::SetUp()
{
    std::cout << " SetUp NdefFormatableTagTest." << std::endl;
    std::vector<int> tagTechList;

    // NFC_NDEF_FORMATABLE_TECH must put at index 0, because defined TEST_NDEF_FORMATABLE_INDEX = 0;
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_NDEF_FORMATABLE_TECH));
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_A_TECH));

    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap formaTableExtrasData;
    AppExecFwk::PacMap nfcAExtrasData;
    nfcAExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    nfcAExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    tagTechExtras.push_back(formaTableExtrasData);
    tagTechExtras.push_back(nfcAExtrasData);

    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_DISC_ID;
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
}

void NdefFormatableTagTest::TearDown()
{
    tagInfo_ = nullptr;
}

/**
 * @tc.name: GetTag001
 * @tc.desc: Test NfcATag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(NdefFormatableTagTest, GetTag001, TestSize.Level1)
{
    std::shared_ptr<NfcATag> nfcA = NfcATag::GetTag(tagInfo_);
    ASSERT_TRUE(nfcA != nullptr);
}
/**
 * @tc.name: GetTag002
 * @tc.desc: Test NdefFormatableTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(NdefFormatableTagTest, GetTag002, TestSize.Level1)
{
    std::shared_ptr<NdefFormatableTag> ndefFor = NdefFormatableTag::GetTag(tagInfo_);
    ASSERT_TRUE(ndefFor != nullptr);
}
/**
 * @tc.name: Format001
 * @tc.desc: Test NdefFormatableTag Format.
 * @tc.type: FUNC
 */
HWTEST_F(NdefFormatableTagTest, Format001, TestSize.Level1)
{
    std::weak_ptr<NdefMessage> firstMessage;
    std::shared_ptr<NdefFormatableTag> ndefFor = NdefFormatableTag::GetTag(tagInfo_);
    int result = ndefFor->Format(firstMessage);
    ASSERT_TRUE(result == ErrorCode::ERR_TAG_STATE_UNBIND);
}
/**
 * @tc.name: FormatReadOnly001
 * @tc.desc: Test NdefFormatableTag FormatReadOnly.
 * @tc.type: FUNC
 */
HWTEST_F(NdefFormatableTagTest, FormatReadOnly001, TestSize.Level1)
{
    std::weak_ptr<NdefMessage> firstMessage;
    std::shared_ptr<NdefFormatableTag> ndefFor = NdefFormatableTag::GetTag(tagInfo_);
    int result = ndefFor->FormatReadOnly(firstMessage);
    ASSERT_TRUE(result == ErrorCode::ERR_TAG_STATE_UNBIND);
}
/**
 * @tc.name: GetTag003
 * @tc.desc: Test NdefFormatableTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(NdefFormatableTagTest, GetTag003, TestSize.Level1)
{
    std::shared_ptr<TagInfo> tagInfo = nullptr;
    std::shared_ptr<NdefFormatableTag> ndefFor = NdefFormatableTag::GetTag(tagInfo);
    ASSERT_TRUE(ndefFor == nullptr);
}
}
}
}