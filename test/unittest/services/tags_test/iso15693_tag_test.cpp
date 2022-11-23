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

#include "iso15693_tag.h"
#include "nfca_tag.h"
#include "nfc_sdk_common.h"
#include "taginfo.h"
#include "tag_session_proxy.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class Iso15693TagTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TEST_UID = "0102";
    static constexpr const auto TEST_DISC_ID = 1;
    static constexpr const auto TEST_RESPONSE_FLAGS = '0';
    static constexpr const auto TEST_DSF_ID = '1';
    static constexpr const auto TEST_ISO15693_INDEX = 0;
    static constexpr const auto TEST_FLAG = 100;
    static constexpr const auto TEST_BLOCK_INDEX = 101;
    static constexpr const auto TEST_HEX_CMD_DATA = "00A40400";
    static constexpr const auto TEST_BLOCK_NUM = 102 ;
    static constexpr const auto TEST_HEX_RESP_DATA = "103";
    std::shared_ptr<TagInfo> tagInfo_;
};

void Iso15693TagTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase Iso15693TagTest." << std::endl;
}

void Iso15693TagTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase Iso15693TagTest." << std::endl;
}

void Iso15693TagTest::SetUp()
{
    std::cout << " SetUp Iso15693TagTest." << std::endl;
    std::vector<int> tagTechList;

    // NFC_V_TECH must put at index 0, because defined TEST_ISO15693_INDEX = 0;
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_V_TECH));
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap iso15693ExtrasData;
    iso15693ExtrasData.PutIntValue(TagInfo::RESPONSE_FLAGS, TEST_RESPONSE_FLAGS);
    iso15693ExtrasData.PutIntValue(TagInfo::DSF_ID, TEST_DSF_ID);
    tagTechExtras.push_back(iso15693ExtrasData);

    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_DISC_ID;
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
}

void Iso15693TagTest::TearDown()
{
    tagInfo_ = nullptr;
}

/**
 * @tc.name: GetTag001
 * @tc.desc: Test NfcATag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(Iso15693TagTest, GetTag001, TestSize.Level1)
{
    std::shared_ptr<NfcATag> nfcA = NfcATag::GetTag(tagInfo_);
    ASSERT_TRUE(nfcA == nullptr);
}
/**
 * @tc.name: GetTag002
 * @tc.desc: Test Iso15693Tag GetTag.
 * @tc.type: FUNC
 */
HWTEST_F(Iso15693TagTest, GetTag002, TestSize.Level1)
{
    std::shared_ptr<Iso15693Tag> iso15693 = Iso15693Tag::GetTag(tagInfo_);
    ASSERT_TRUE(iso15693 != nullptr);
}

/**
 * @tc.name: ReadSingleBlock001
 * @tc.desc: Test Iso15693Tag ReadSingleBlock.
 * @tc.type: FUNC
 */
HWTEST_F(Iso15693TagTest, ReadSingleBlock001, TestSize.Level1)
{
    std::shared_ptr<Iso15693Tag> iso15693 = Iso15693Tag::GetTag(tagInfo_);
    std::string testHexRespData = TEST_HEX_RESP_DATA;
    int errorCode = iso15693->ReadSingleBlock(TEST_FLAG, TEST_BLOCK_INDEX, testHexRespData);
    
    // Error code returned when the chip and tag are not connected
    ASSERT_TRUE(errorCode == ErrorCode::ERR_TAG_STATE_DISCONNECTED);
}
/**
 * @tc.name: WriteSingleBlock001
 * @tc.desc: Test Iso15693Tag WriteSingleBlock.
 * @tc.type: FUNC
 */
HWTEST_F(Iso15693TagTest, WriteSingleBlock001, TestSize.Level1)
{
    std::shared_ptr<Iso15693Tag> iso15693 = Iso15693Tag::GetTag(tagInfo_);
    int errorCode = iso15693->WriteSingleBlock(TEST_FLAG, TEST_BLOCK_INDEX, TEST_HEX_CMD_DATA);
    
    // Error code returned when the chip and tag are not connected
    ASSERT_TRUE(errorCode == ErrorCode::ERR_TAG_STATE_DISCONNECTED);
}
/**
 * @tc.name: LockSingleBlock001
 * @tc.desc: Test Iso15693Tag LockSingleBlock.
 * @tc.type: FUNC
 */
HWTEST_F(Iso15693TagTest, LockSingleBlock001, TestSize.Level1)
{
    std::shared_ptr<Iso15693Tag> iso15693 = Iso15693Tag::GetTag(tagInfo_);
    int errorCode = iso15693->LockSingleBlock(TEST_FLAG, TEST_BLOCK_INDEX);
    
    // Error code returned when the chip and tag are not connected
    ASSERT_TRUE(errorCode == ErrorCode::ERR_TAG_STATE_DISCONNECTED);
}
/**
 * @tc.name: ReadMultipleBlock001
 * @tc.desc: Test Iso15693Tag ReadMultipleBlock.
 * @tc.type: FUNC
 */
HWTEST_F(Iso15693TagTest, ReadMultipleBlock001, TestSize.Level1)
{
    std::shared_ptr<Iso15693Tag> iso15693 = Iso15693Tag::GetTag(tagInfo_);
    std::string testHexRespData = TEST_HEX_RESP_DATA;
    int errorCode = iso15693->ReadMultipleBlock(TEST_FLAG, TEST_BLOCK_INDEX, TEST_BLOCK_NUM, testHexRespData);
    
    // Error code returned when the chip and tag are not connected
    ASSERT_TRUE(errorCode == ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: WriteMultipleBlock001
 * @tc.desc: Test Iso15693Tag ReadSingleBlock.
 * @tc.type: FUNC
 */
HWTEST_F(Iso15693TagTest, v001, TestSize.Level1)
{
    std::shared_ptr<Iso15693Tag> iso15693 = Iso15693Tag::GetTag(tagInfo_);
    int errorCode = iso15693->WriteMultipleBlock(TEST_FLAG, TEST_BLOCK_INDEX, TEST_BLOCK_NUM, TEST_HEX_RESP_DATA);
    
    // Error code returned when the chip and tag are not connected
    ASSERT_TRUE(errorCode == ErrorCode::ERR_TAG_STATE_DISCONNECTED);
}
/**
 * @tc.name: GetRespFlags001
 * @tc.desc: Test Iso15693Tag GetRespFlags.
 * @tc.type: FUNC
 */
HWTEST_F(Iso15693TagTest, GetRespFlags001, TestSize.Level1)
{
    std::shared_ptr<Iso15693Tag> iso15693 = Iso15693Tag::GetTag(tagInfo_);
    char respFlags = iso15693->GetRespFlags();
    ASSERT_TRUE(respFlags == TEST_RESPONSE_FLAGS);
}
/**
 * @tc.name: GetDsfId001
 * @tc.desc: Test Iso15693Tag GetDsfId.
 * @tc.type: FUNC
 */
HWTEST_F(Iso15693TagTest, GetDsfId001, TestSize.Level1)
{
    std::shared_ptr<Iso15693Tag> iso15693 = Iso15693Tag::GetTag(tagInfo_);
    char dsfId = iso15693->GetDsfId();
    ASSERT_TRUE(dsfId == TEST_DSF_ID);
}
/**
 * @tc.name: GetTagUid001
 * @tc.desc: Test Iso15693Tag GetTagUid.
 * @tc.type: FUNC
 */
HWTEST_F(Iso15693TagTest, GetTagUid001, TestSize.Level1)
{
    std::shared_ptr<Iso15693Tag> iso15693 = Iso15693Tag::GetTag(tagInfo_);
    std::string uid = iso15693->GetTagUid();
    ASSERT_TRUE(strcmp(uid.c_str(), TEST_UID) == 0);
}
}
}
}
