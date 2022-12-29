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
class IsoDepTagTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TEST_UID = "0102";
    static constexpr const auto TEST_DISC_ID = 1;
    static constexpr const auto TEST_HISTORICAL_BYTES = "1015";
    static constexpr const auto TEST_HILAYER_RESPONSE = "0106";
    static constexpr const auto TEST_ISODEP_INDEX = 2;
    std::shared_ptr<TagInfo> tagInfo_;
};

void IsoDepTagTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase IsoDepTagTest." << std::endl;
}

void IsoDepTagTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase IsoDepTagTest." << std::endl;
}

void IsoDepTagTest::SetUp()
{
    std::cout << " SetUp IsoDepTagTest." << std::endl;
    std::vector<int> tagTechList;

    // NFC_ISODEP_TECH must put at index 2, because defined TEST_ISODEP_INDEX = 2;
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_A_TECH));
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_B_TECH));
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_ISODEP_TECH));

    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap nfcAExtrasData;
    AppExecFwk::PacMap nfcBExtrasData;
    AppExecFwk::PacMap isodepExtrasData;
    isodepExtrasData.PutStringValue(TagInfo::HISTORICAL_BYTES, TEST_HISTORICAL_BYTES);
    isodepExtrasData.PutStringValue(TagInfo::HILAYER_RESPONSE, TEST_HILAYER_RESPONSE);
    tagTechExtras.push_back(nfcAExtrasData);
    tagTechExtras.push_back(nfcBExtrasData);
    tagTechExtras.push_back(isodepExtrasData);

    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_DISC_ID;
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
}

void IsoDepTagTest::TearDown()
{
    tagInfo_ = nullptr;
}

/**
 * @tc.name: GetTag001
 * @tc.desc: Test NfcATag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(IsoDepTagTest, GetTag001, TestSize.Level1)
{
    std::shared_ptr<NfcATag> nfcA = NfcATag::GetTag(tagInfo_);
    ASSERT_TRUE(nfcA != nullptr);
}
/**
 * @tc.name: GetTag002
 * @tc.desc: Test NfcBTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(IsoDepTagTest, GetTag002, TestSize.Level1)
{
    std::shared_ptr<NfcBTag> nfcB = NfcBTag::GetTag(tagInfo_);
    ASSERT_TRUE(nfcB != nullptr);
}
/**
 * @tc.name: GetTag003
 * @tc.desc: Test IsoDepTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(IsoDepTagTest, GetTag003, TestSize.Level1)
{
    std::shared_ptr<IsoDepTag> isoDep = IsoDepTag::GetTag(tagInfo_);
    ASSERT_TRUE(isoDep != nullptr);
}
/**
 * @tc.name: GetHistoricalBytes001
 * @tc.desc: Test NfcBTag GetHistoricalBytes.
 * @tc.type: FUNC
 */
HWTEST_F(IsoDepTagTest, GetHistoricalBytes001, TestSize.Level1)
{
    std::shared_ptr<IsoDepTag> isoDep = IsoDepTag::GetTag(tagInfo_);
    std::string historicalBytes = isoDep->GetHistoricalBytes();
    ASSERT_TRUE(strcmp(historicalBytes.c_str(), TEST_HISTORICAL_BYTES) == 0);
}
/**
 * @tc.name: GetHiLayerResponse001
 * @tc.desc: Test NfcBTag GetHiLayerResponse.
 * @tc.type: FUNC
 */
HWTEST_F(IsoDepTagTest, GetHiLayerResponse001, TestSize.Level1)
{
    std::shared_ptr<IsoDepTag> isoDep = IsoDepTag::GetTag(tagInfo_);
    std::string hiLayerResponse = isoDep->GetHiLayerResponse();
    ASSERT_TRUE(strcmp(hiLayerResponse.c_str(), TEST_HILAYER_RESPONSE) == 0);
}
/**
 * @tc.name: GetTagUid001
 * @tc.desc: Test NfcBTag GetTagUid.
 * @tc.type: FUNC
 */
HWTEST_F(IsoDepTagTest, GetTagUid001, TestSize.Level1)
{
    std::shared_ptr<IsoDepTag> isoDep = IsoDepTag::GetTag(tagInfo_);
    std::string uid = isoDep->GetTagUid();
    ASSERT_TRUE(strcmp(uid.c_str(), TEST_UID) == 0);
}
/**
 * @tc.name: IsExtendedApduSupported001
 * @tc.desc: Test NfcBTag IsExtendedApduSupported.
 * @tc.type: FUNC
 */
HWTEST_F(IsoDepTagTest, IsExtendedApduSupported001, TestSize.Level1)
{
    bool isSupported = false;
    std::shared_ptr<IsoDepTag> isoDep = IsoDepTag::GetTag(tagInfo_);
    int statusCode = isoDep->IsExtendedApduSupported(isSupported);
    ASSERT_TRUE(statusCode == ErrorCode::ERR_NONE);
    ASSERT_TRUE(isSupported == false);
}
}
}
}
