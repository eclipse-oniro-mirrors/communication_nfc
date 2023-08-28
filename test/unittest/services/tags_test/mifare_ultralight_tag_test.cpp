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

#include "mifare_ultralight_tag.h"
#include "nfca_tag.h"
#include "nfc_sdk_common.h"
#include "taginfo.h"
#include "tag_session_proxy.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class MifareUltralightTagTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TEST_UID = "0102";
    static constexpr const auto TEST_DISC_ID = 1;
    static constexpr const auto TEST_SAK = 0x36;
    static constexpr const auto TEST_MANUFACTURER_UID = "04";
    static constexpr const auto TEST_ATQA = "0400";
    static constexpr const auto TEST_MIFARE_ULTRALIGHT_INDEX = 0;
    std::shared_ptr<TagInfo> tagInfo_;
};

void MifareUltralightTagTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase MifareUltralightTagTest." << std::endl;
}

void MifareUltralightTagTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase MifareUltralightTagTest." << std::endl;
}

void MifareUltralightTagTest::SetUp()
{
    std::cout << " SetUp MifareUltralightTagTest." << std::endl;
    std::vector<int> tagTechList;

    // NFC_MIFARE_ULTRALIGHT_TECH must put at index 0, because defined TEST_MIFARE_ULTRALIGHT_INDEX = 0;
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH));
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_A_TECH));

    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap mifareUltralightExtrasData;
    AppExecFwk::PacMap nfcAExtrasData;
    nfcAExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    nfcAExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    tagTechExtras.push_back(mifareUltralightExtrasData);
    tagTechExtras.push_back(nfcAExtrasData);

    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_DISC_ID;
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
}

void MifareUltralightTagTest::TearDown()
{
    tagInfo_ = nullptr;
}

/**
 * @tc.name: GetTag001
 * @tc.desc: Test NfcATag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(MifareUltralightTagTest, GetTag001, TestSize.Level1)
{
    std::shared_ptr<NfcATag> nfcA = NfcATag::GetTag(tagInfo_);
    ASSERT_TRUE(nfcA != nullptr);
}
/**
 * @tc.name: GetTag002
 * @tc.desc: Test MifareUltralightTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(MifareUltralightTagTest, GetTag002, TestSize.Level1)
{
    std::shared_ptr<MifareUltralightTag> mifareUltralight = MifareUltralightTag::GetTag(tagInfo_);
    ASSERT_TRUE(mifareUltralight != nullptr);
}
/**
 * @tc.name: ReadMultiplePages001
 * @tc.desc: Test MifareUltralightTag ReadMultiplePages.
 * @tc.type: FUNC
 */
HWTEST_F(MifareUltralightTagTest, ReadMultiplePages001, TestSize.Level1)
{
    uint32_t pageIndex = MifareUltralightTag::MU_PAGE_SIZE;
    std::string hexRespData;
    std::shared_ptr<MifareUltralightTag> mifareUltralight = MifareUltralightTag::GetTag(tagInfo_);
    int result = mifareUltralight->ReadMultiplePages(pageIndex, hexRespData);
    ASSERT_TRUE(result == ErrorCode::ERR_TAG_STATE_DISCONNECTED);
}
/**
 * @tc.name: WriteSinglePage001
 * @tc.desc: Test MifareUltralightTag WriteSinglePage.
 * @tc.type: FUNC
 */
HWTEST_F(MifareUltralightTagTest, WriteSinglePage001, TestSize.Level1)
{
    uint32_t pageIndex = MifareUltralightTag::MU_PAGE_SIZE;
    const std::string hexRespData;
    std::shared_ptr<MifareUltralightTag> mifareUltralight = MifareUltralightTag::GetTag(tagInfo_);
    int result = mifareUltralight->WriteSinglePage(pageIndex, hexRespData);
    ASSERT_TRUE(result == ErrorCode::ERR_TAG_STATE_DISCONNECTED);
}
/**
 * @tc.name: GetType001
 * @tc.desc: Test MifareUltralightTag GetType.
 * @tc.type: FUNC
 */
HWTEST_F(MifareUltralightTagTest, GetType001, TestSize.Level1)
{
    std::shared_ptr<MifareUltralightTag> mifareUltralight = MifareUltralightTag::GetTag(tagInfo_);
    MifareUltralightTag::EmType getType = mifareUltralight->GetType();
    ASSERT_TRUE(getType == MifareUltralightTag::TYPE_UNKNOWN);
}
/**
 * @tc.name: GetTag003
 * @tc.desc: Test MifareUltralightTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(MifareUltralightTagTest, GetTag003, TestSize.Level1)
{
    std::shared_ptr<TagInfo> tagInfo = nullptr;
    std::shared_ptr<MifareUltralightTag> mifareUltralight = MifareUltralightTag::GetTag(tagInfo);
    ASSERT_TRUE(mifareUltralight == nullptr);
}
/**
 * @tc.name: GetTag004
 * @tc.desc: Test MifareUltralightTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(MifareUltralightTagTest, GetTag004, TestSize.Level1)
{
    std::shared_ptr<TagInfo> tagInfo = nullptr;
    std::vector<int> tagTechList;
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH));
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_A_TECH));
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap mifareUltralightExtrasData;
    AppExecFwk::PacMap nfcAExtrasData;
    nfcAExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    nfcAExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    tagTechExtras.push_back(mifareUltralightExtrasData);
    tagTechExtras.push_back(nfcAExtrasData);
    std::string tagUid = "";
    int tagRfDiscId = TEST_DISC_ID;
    tagInfo = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
    std::shared_ptr<MifareUltralightTag> mifareUltralight = MifareUltralightTag::GetTag(tagInfo);
    ASSERT_TRUE(mifareUltralight != nullptr);
}
/**
 * @tc.name: GetTag005
 * @tc.desc: Test MifareUltralightTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(MifareUltralightTagTest, GetTag005, TestSize.Level1)
{
    std::shared_ptr<TagInfo> tagInfo = nullptr;
    std::vector<int> tagTechList;
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH));
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_A_TECH));
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap mifareUltralightExtrasData;
    AppExecFwk::PacMap nfcAExtrasData;
    nfcAExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    nfcAExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    tagTechExtras.push_back(mifareUltralightExtrasData);
    tagTechExtras.push_back(nfcAExtrasData);
    std::string tagUid = TEST_MANUFACTURER_UID;
    int tagRfDiscId = TEST_DISC_ID;
    tagInfo = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
    std::shared_ptr<MifareUltralightTag> mifareUltralight = MifareUltralightTag::GetTag(tagInfo);
    ASSERT_TRUE(mifareUltralight != nullptr);
}
/**
 * @tc.name: GetTag006
 * @tc.desc: Test MifareUltralightTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(MifareUltralightTagTest, GetTag006, TestSize.Level1)
{
    std::shared_ptr<TagInfo> tagInfo = nullptr;
    std::vector<int> tagTechList;
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH));
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_A_TECH));
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap mifareUltralightExtrasData;
    mifareUltralightExtrasData.PutBooleanValue(TagInfo::MIFARE_ULTRALIGHT_C_TYPE, true);
    AppExecFwk::PacMap nfcAExtrasData;
    nfcAExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    nfcAExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    tagTechExtras.push_back(mifareUltralightExtrasData);
    tagTechExtras.push_back(nfcAExtrasData);
    std::string tagUid = TEST_MANUFACTURER_UID;
    int tagRfDiscId = TEST_DISC_ID;
    tagInfo = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
    std::shared_ptr<MifareUltralightTag> mifareUltralight = MifareUltralightTag::GetTag(tagInfo);
    ASSERT_TRUE(mifareUltralight != nullptr);
}
/**
 * @tc.name: GetTag007
 * @tc.desc: Test MifareUltralightTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(MifareUltralightTagTest, GetTag007, TestSize.Level1)
{
    std::shared_ptr<TagInfo> tagInfo = nullptr;
    std::vector<int> tagTechList;
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH));
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_A_TECH));
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap mifareUltralightExtrasData;
    mifareUltralightExtrasData.PutBooleanValue(TagInfo::MIFARE_ULTRALIGHT_C_TYPE, false);
    AppExecFwk::PacMap nfcAExtrasData;
    nfcAExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    nfcAExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    tagTechExtras.push_back(mifareUltralightExtrasData);
    tagTechExtras.push_back(nfcAExtrasData);
    std::string tagUid = TEST_MANUFACTURER_UID;
    int tagRfDiscId = TEST_DISC_ID;
    tagInfo = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
    std::shared_ptr<MifareUltralightTag> mifareUltralight = MifareUltralightTag::GetTag(tagInfo);
    ASSERT_TRUE(mifareUltralight != nullptr);
}
}
}
}