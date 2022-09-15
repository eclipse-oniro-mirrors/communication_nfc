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

#include "ndef_tag.h"
#include "nfca_tag.h"
#include "nfc_sdk_common.h"
#include "taginfo.h"
#include "tag_session_proxy.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class NdefTagTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TEST_UID = "0102";
    static constexpr const auto TEST_DISC_ID = 1;
    static constexpr const auto TEST_NDEF_FORUM_TYPE = NdefTag::EmNfcForumType::NFC_FORUM_TYPE_1;
    static constexpr const auto TEST_NDEF_TAG_MODE = NdefTag::EmNdefTagMode::MODE_READ_ONLY;
    static constexpr const auto TEST_NDEF_MSG = "ndef";
    static constexpr const auto TEST_NDEF_INDEX = 0;
    std::shared_ptr<TagInfo> tagInfo_;
};

void NdefTagTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NdefTagTest." << std::endl;
}

void NdefTagTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NdefTagTest." << std::endl;
}

void NdefTagTest::SetUp()
{
    std::cout << " SetUp NdefTagTest." << std::endl;
    std::vector<int> tagTechList;

    // NFC_NDEF_TECH must put at index 0, because defined TEST_NDEF_INDEX = 0;
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_NDEF_TECH));

    std::vector<AppExecFwk::PacMap> tagTechExtras;
    std::shared_ptr<AppExecFwk::PacMap> tagTechExtrasData = std::make_shared<AppExecFwk::PacMap>();
    AppExecFwk::PacMap ndefExtrasData;
    ndefExtrasData.PutLongValue(TagInfo::NDEF_FORUM_TYPE, TEST_NDEF_FORUM_TYPE);
    ndefExtrasData.PutLongValue(TagInfo::NDEF_TAG_MODE, TEST_NDEF_TAG_MODE);
    ndefExtrasData.PutStringValue(TagInfo::NDEF_MSG, TEST_NDEF_MSG);
    tagTechExtras.push_back(ndefExtrasData);

    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_DISC_ID;
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
}

void NdefTagTest::TearDown()
{
    tagInfo_ = nullptr;
}

/**
 * @tc.name: GetTag001
 * @tc.desc: Test NfcATag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, GetTag001, TestSize.Level1)
{
    std::shared_ptr<NfcATag> nfcA = NfcATag::GetTag(tagInfo_);
    ASSERT_TRUE(nfcA == nullptr);
}
/**
 * @tc.name: GetTag002
 * @tc.desc: Test NdefTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, GetTag002, TestSize.Level1)
{
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    ASSERT_TRUE(ndef != nullptr);
}
/**
 * @tc.name: GetNdefTagType001
 * @tc.desc: Test NdefTag GetNdefTagType.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, GetNdefTagType001, TestSize.Level1)
{
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    NdefTag::EmNfcForumType ndefTagType = ndef->GetNdefTagType();
    ASSERT_TRUE(ndefTagType == TEST_NDEF_FORUM_TYPE);
}
/**
 * @tc.name: GetNdefTagMode001
 * @tc.desc: Test NdefTag GetNdefTagMode.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, GetNdefTagMode001, TestSize.Level1)
{
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    NdefTag::EmNdefTagMode ndefTagMode = ndef->GetNdefTagMode();
    ASSERT_TRUE(ndefTagMode == TEST_NDEF_TAG_MODE);
}
/**
 * @tc.name: GetTagUid001
 * @tc.desc: Test NdefTag GetTagUid.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, GetTagUid001, TestSize.Level1)
{
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    std::string uid = ndef->GetTagUid();
    ASSERT_TRUE(strcmp(uid.c_str(), TEST_UID) == 0);
}
}
}
}
