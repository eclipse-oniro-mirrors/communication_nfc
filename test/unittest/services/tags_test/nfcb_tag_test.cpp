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
class NfcBTagTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TEST_UID = "0102";
    static constexpr const auto TEST_DISC_ID = 1;
    static constexpr const auto TEST_APP_DATA = "App";
    static constexpr const auto TEST_PROTOCOL_INFO = "Protocol";
    static constexpr const auto TEST_NFCB_INDEX = 0;
    std::shared_ptr<TagInfo> tagInfo_;
};

void NfcBTagTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcBTagTest." << std::endl;
}

void NfcBTagTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcBTagTest." << std::endl;
}

void NfcBTagTest::SetUp()
{
    std::cout << " SetUp NfcBTagTest." << std::endl;
    std::vector<int> tagTechList;

    // NFC_B_TECH must put at index 0, because defined TEST_NFCB_INDEX = 0;
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_B_TECH));
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_ISODEP_TECH));

    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap nfcBExtrasData;
    AppExecFwk::PacMap isoDepExtrasData;
    nfcBExtrasData.PutStringValue(TagInfo::APP_DATA, TEST_APP_DATA);
    nfcBExtrasData.PutStringValue(TagInfo::PROTOCOL_INFO, TEST_PROTOCOL_INFO);
    tagTechExtras.push_back(nfcBExtrasData);
    tagTechExtras.push_back(isoDepExtrasData);

    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_DISC_ID;
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
}

void NfcBTagTest::TearDown()
{
    tagInfo_ = nullptr;
}

/**
 * @tc.name: GetTag001
 * @tc.desc: Test NfcATag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(NfcBTagTest, GetTag001, TestSize.Level1)
{
    std::shared_ptr<NfcATag> nfcA = NfcATag::GetTag(tagInfo_);
    ASSERT_TRUE(nfcA == nullptr);
}
/**
 * @tc.name: GetTag002
 * @tc.desc: Test IsoDepTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(NfcBTagTest, GetTag002, TestSize.Level1)
{
    std::shared_ptr<IsoDepTag> isoDep = IsoDepTag::GetTag(tagInfo_);
    ASSERT_TRUE(isoDep != nullptr);
}
/**
 * @tc.name: GetTag003
 * @tc.desc: Test NfcBTag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(NfcBTagTest, GetTag003, TestSize.Level1)
{
    std::shared_ptr<NfcBTag> nfcB = NfcBTag::GetTag(tagInfo_);
    ASSERT_TRUE(nfcB != nullptr);
}
/**
 * @tc.name: GetAppData001
 * @tc.desc: Test NfcBTag GetAppData.
 * @tc.type: FUNC
 */
HWTEST_F(NfcBTagTest, GetAppData001, TestSize.Level1)
{
    std::shared_ptr<NfcBTag> nfcB = NfcBTag::GetTag(tagInfo_);
    std::string appData = nfcB->GetAppData();
    ASSERT_TRUE(strcmp(appData.c_str(), TEST_APP_DATA) == 0);
}
/**
 * @tc.name: GetProtocolInfo001
 * @tc.desc: Test NfcBTag GetProtocolInfo.
 * @tc.type: FUNC
 */
HWTEST_F(NfcBTagTest, GetProtocolInfo001, TestSize.Level1)
{
    std::shared_ptr<NfcBTag> nfcB = NfcBTag::GetTag(tagInfo_);
    std::string protocolInfo = nfcB->GetProtocolInfo();
    ASSERT_TRUE(strcmp(protocolInfo.c_str(), TEST_PROTOCOL_INFO) == 0);
}
/**
 * @tc.name: GetTagUid001
 * @tc.desc: Test NfcBTag GetTagUid.
 * @tc.type: FUNC
 */
HWTEST_F(NfcBTagTest, GetTagUid001, TestSize.Level1)
{
    std::shared_ptr<NfcBTag> nfcB = NfcBTag::GetTag(tagInfo_);
    std::string uid = nfcB->GetTagUid();
    ASSERT_TRUE(strcmp(uid.c_str(), TEST_UID) == 0);
}
}
}
}
