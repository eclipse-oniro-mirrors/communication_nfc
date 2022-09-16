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

#include "mifare_classic_tag.h"
#include "nfca_tag.h"
#include "nfc_sdk_common.h"
#include "taginfo.h"
#include "tag_session_proxy.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class MifareClassicTagTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TEST_UID = "0102";
    static constexpr const auto TEST_DISC_ID = 1;
    static constexpr const auto TEST_SAK = 0x28;
    static constexpr const auto TEST_ATQA = "0400";
    static constexpr const auto TEST_MIFARE_CLASSIC_INDEX = 0;
    std::shared_ptr<TagInfo> tagInfo_;
};

void MifareClassicTagTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase MifareClassicTagTest." << std::endl;
}

void MifareClassicTagTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase MifareClassicTagTest." << std::endl;
}

void MifareClassicTagTest::SetUp()
{
    std::cout << " SetUp MifareClassicTagTest." << std::endl;
    std::vector<int> tagTechList;

    // NFC_MIFARE_CLASSIC_TECH must put at index 0, because defined TEST_MIFARE_CLASSIC_INDEX = 0;
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_MIFARE_CLASSIC_TECH));

    std::vector<AppExecFwk::PacMap> tagTechExtras;
    std::shared_ptr<AppExecFwk::PacMap> tagTechExtrasData = std::make_shared<AppExecFwk::PacMap>();
    AppExecFwk::PacMap mifareClassicExtrasData;
    mifareClassicExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    mifareClassicExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    tagTechExtras.push_back(mifareClassicExtrasData);

    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_DISC_ID;
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
}

void MifareClassicTagTest::TearDown()
{
    tagInfo_ = nullptr;
}

/**
 * @tc.name: GetTag001
 * @tc.desc: Test NfcATag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, GetTag001, TestSize.Level1)
{
    std::shared_ptr<NfcATag> nfcA = NfcATag::GetTag(tagInfo_);
    ASSERT_TRUE(nfcA == nullptr);
}
/**
 * @tc.name: GetTag002
 * @tc.desc: Test MifareClassicTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, GetTag002, TestSize.Level1)
{
    std::shared_ptr<MifareClassicTag> mifareClassic = MifareClassicTag::GetTag(tagInfo_);
    ASSERT_TRUE(mifareClassic != nullptr);
}
/**
 * @tc.name: GetMifareTagType001
 * @tc.desc: Test MifareClassicTag GetMifareTagType.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, GetMifareTagType001, TestSize.Level1)
{
    std::shared_ptr<MifareClassicTag> mifareClassic = MifareClassicTag::GetTag(tagInfo_);
    size_t mifareTagType = mifareClassic->GetMifareTagType();
    ASSERT_TRUE(mifareTagType == MifareClassicTag::TYPE_CLASSIC);
}
/**
 * @tc.name: GetSize001
 * @tc.desc: Test MifareClassicTag GetSize.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, GetSize001, TestSize.Level1)
{
    std::shared_ptr<MifareClassicTag> mifareClassic = MifareClassicTag::GetTag(tagInfo_);
    int size = mifareClassic->GetSize();
    ASSERT_TRUE(size == MifareClassicTag::MC_SIZE_1K);
}
/**
 * @tc.name: IsEmulated001
 * @tc.desc: Test MifareClassicTag IsEmulated.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, IsEmulated001, TestSize.Level1)
{
    std::shared_ptr<MifareClassicTag> mifareClassic = MifareClassicTag::GetTag(tagInfo_);
    bool isEmulated = mifareClassic->IsEmulated();
    ASSERT_TRUE(isEmulated);
}
/**
 * @tc.name: GetTagUid001
 * @tc.desc: Test MifareClassicTag GetTagUid.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, GetTagUid001, TestSize.Level1)
{
    std::shared_ptr<MifareClassicTag> mifareClassic = MifareClassicTag::GetTag(tagInfo_);
    std::string uid = mifareClassic->GetTagUid();
    ASSERT_TRUE(strcmp(uid.c_str(), TEST_UID) == 0);
}
}
}
}
