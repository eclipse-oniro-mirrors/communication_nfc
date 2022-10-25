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
#include "nfcf_tag.h"
#include "nfc_sdk_common.h"
#include "taginfo.h"
#include "tag_session_proxy.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class NfcFTagTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TEST_UID = "0102";
    static constexpr const auto TEST_DISC_ID = 1;
    static constexpr const auto TEST_SYSTEMCODE_DATA = "0103";
    static constexpr const auto TEST_PMM_INFO = "0104";
    static constexpr const auto TEST_NFCF_INDEX = 0;
    std::shared_ptr<TagInfo> tagInfo_;
};

void NfcFTagTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcFTagTest." << std::endl;
}

void NfcFTagTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcFTagTest." << std::endl;
}

void NfcFTagTest::SetUp()
{
    std::cout << " SetUp NfcFTagTest." << std::endl;
    std::vector<int> tagTechList;

    // NFC_F_TECH must put at index 0, because defined TEST_NFCF_INDEX = 0;
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_F_TECH));

    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap nfcFExtrasData;
    nfcFExtrasData.PutStringValue(TagInfo::NFCF_SC, TEST_SYSTEMCODE_DATA);
    nfcFExtrasData.PutStringValue(TagInfo::NFCF_PMM, TEST_PMM_INFO);
    tagTechExtras.push_back(nfcFExtrasData);

    std::cout << " SetUp down" << std::endl;
    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_DISC_ID;
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
}

void NfcFTagTest::TearDown()
{
    tagInfo_ = nullptr;
}

/**
 * @tc.name: GetTag001
 * @tc.desc: Test NfcATag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(NfcFTagTest, GetTag001, TestSize.Level1)
{
    std::shared_ptr<NfcATag> nfcA = NfcATag::GetTag(tagInfo_);
    ASSERT_TRUE(nfcA == nullptr);
}
/**
 * @tc.name: GetTag002
 * @tc.desc: Test IsoDepTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(NfcFTagTest, GetTag002, TestSize.Level1)
{
    std::shared_ptr<IsoDepTag> isoDep = IsoDepTag::GetTag(tagInfo_);
    ASSERT_TRUE(isoDep == nullptr);
}
/**
 * @tc.name: GetTag003
 * @tc.desc: Test NfcFTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(NfcFTagTest, GetTag004, TestSize.Level1)
{
    std::shared_ptr<NfcFTag> nfcF = NfcFTag::GetTag(tagInfo_);
    ASSERT_TRUE(nfcF != nullptr);
}
/**
 * @tc.name: getSystemCode001
 * @tc.desc: Test NfcFTag getSystemCode.
 * @tc.type: FUNC
 */
HWTEST_F(NfcFTagTest, getSystemCode001, TestSize.Level1)
{
    std::shared_ptr<NfcFTag> nfcF = NfcFTag::GetTag(tagInfo_);
    std::string systemCode = nfcF->getSystemCode();
    ASSERT_TRUE(strcmp(systemCode.c_str(), TEST_SYSTEMCODE_DATA) == 0);
}
/**
 * @tc.name: getPmm001
 * @tc.desc: Test NfcBTag GetProtocolInfo.
 * @tc.type: FUNC
 */
HWTEST_F(NfcFTagTest, getPmm001, TestSize.Level1)
{
    std::shared_ptr<NfcFTag> nfcF = NfcFTag::GetTag(tagInfo_);
    std::string pmm = nfcF->getPmm();
    ASSERT_TRUE(strcmp(pmm.c_str(), TEST_PMM_INFO) == 0);
}
/**
 * @tc.name: GetTagUid001
 * @tc.desc: Test NfcFTag GetTagUid.
 * @tc.type: FUNC
 */
HWTEST_F(NfcFTagTest, GetTagUid001, TestSize.Level1)
{
    std::shared_ptr<NfcFTag> nfcF = NfcFTag::GetTag(tagInfo_);
    std::string uid = nfcF->GetTagUid();
    ASSERT_TRUE(strcmp(uid.c_str(), TEST_UID) == 0);
}
}
}
}
