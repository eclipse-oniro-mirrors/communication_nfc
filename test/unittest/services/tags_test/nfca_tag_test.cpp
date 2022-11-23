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

#include "isodep_tag.h"
#include "nfca_tag.h"
#include "nfcb_tag.h"
#include "nfc_sdk_common.h"
#include "taginfo.h"
#include "tag_session_proxy.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class NfcATagTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TEST_UID = "0102";
    static constexpr const auto TEST_DISC_ID = 1;
    static constexpr const auto TEST_SAK = 0x20;
    static constexpr const auto TEST_ATQA = "0400";
    static constexpr const auto TEST_NFCA_INDEX = 0;
    std::shared_ptr<TagInfo> tagInfo_;
};

void NfcATagTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcATagTest." << std::endl;
}

void NfcATagTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcATagTest." << std::endl;
}

void NfcATagTest::SetUp()
{
    std::cout << " SetUp NfcATagTest." << std::endl;
    std::vector<int> tagTechList;

    // NFC_A_TECH must put at index 0, because defined TEST_NFCA_INDEX = 0;
    tagTechList.push_back((int)TagTechnology::NFC_A_TECH);
    tagTechList.push_back((int)TagTechnology::NFC_ISODEP_TECH);

    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap nfcAExtrasData;
    AppExecFwk::PacMap isoDepExtrasData;
    nfcAExtrasData.PutIntValue(KITS::TagInfo::SAK, TEST_SAK);
    nfcAExtrasData.PutStringValue(KITS::TagInfo::ATQA, TEST_ATQA);
    tagTechExtras.push_back(nfcAExtrasData);
    tagTechExtras.push_back(isoDepExtrasData);

    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_DISC_ID;
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
}

void NfcATagTest::TearDown()
{
    tagInfo_ = nullptr;
}

/**
 * @tc.name: GetTag001
 * @tc.desc: Test NfcATag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(NfcATagTest, GetTag001, TestSize.Level1)
{
    std::shared_ptr<NfcATag> nfcA = NfcATag::GetTag(tagInfo_);
    ASSERT_TRUE(nfcA != nullptr);
}
/**
 * @tc.name: GetTag002
 * @tc.desc: Test IsoDepTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(NfcATagTest, GetTag002, TestSize.Level1)
{
    std::shared_ptr<IsoDepTag> isoDep = IsoDepTag::GetTag(tagInfo_);
    ASSERT_TRUE(isoDep != nullptr);
}
/**
 * @tc.name: GetTag003
 * @tc.desc: Test NfcBTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(NfcATagTest, GetTag003, TestSize.Level1)
{
    std::shared_ptr<NfcBTag> nfcB = NfcBTag::GetTag(tagInfo_);
    ASSERT_TRUE(nfcB == nullptr);
}
/**
 * @tc.name: GetSak001
 * @tc.desc: Test NfcATag GetSak.
 * @tc.type: FUNC
 */
HWTEST_F(NfcATagTest, GetSak001, TestSize.Level1)
{
    std::shared_ptr<NfcATag> nfcA = NfcATag::GetTag(tagInfo_);
    int sak = nfcA->GetSak();
    ASSERT_TRUE(sak == TEST_SAK);
}
/**
 * @tc.name: GetAtqa001
 * @tc.desc: Test NfcATag GetAtqa.
 * @tc.type: FUNC
 */
HWTEST_F(NfcATagTest, GetAtqa001, TestSize.Level1)
{
    std::shared_ptr<NfcATag> nfcA = NfcATag::GetTag(tagInfo_);
    std::string atqa = nfcA->GetAtqa();
    ASSERT_TRUE(strcmp(atqa.c_str(), TEST_ATQA) == 0);
}
/**
 * @tc.name: GetTagUid001
 * @tc.desc: Test NfcATag GetTagUid.
 * @tc.type: FUNC
 */
HWTEST_F(NfcATagTest, GetTagUid001, TestSize.Level1)
{
    std::shared_ptr<NfcATag> nfcA = NfcATag::GetTag(tagInfo_);
    std::string uid = nfcA->GetTagUid();
    ASSERT_TRUE(strcmp(uid.c_str(), TEST_UID) == 0);
}
}
}
}
