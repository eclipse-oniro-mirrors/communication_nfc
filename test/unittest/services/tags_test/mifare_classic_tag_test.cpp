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
    static constexpr const auto TEST_SECTOR_INDEX = 0x1F;
    static constexpr const auto TEST_MC_MAX_SECTOR_COUNT = 0x27;
    static constexpr const auto TEST_SAK = 0x28;
    static constexpr const auto TEST_BLOCK_INDEX = 64;
    static constexpr const auto VALUE = 257;
    static constexpr const auto TEST_MIFARE_CLASSIC_INDEX = 0;
    static constexpr const auto MC_MAX_BLOCK_INDEX = 256;
    static constexpr const auto TEST_ATQA = "0400";
    static constexpr const auto TEST_HEX_RESP_DATA = "0401";
    
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
    tagTechList.push_back(static_cast<int>(TagTechnology::NFC_A_TECH));

    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap mifareClassicExtrasData;
    AppExecFwk::PacMap nfcAExtrasData;
    nfcAExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    nfcAExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    tagTechExtras.push_back(mifareClassicExtrasData);
    tagTechExtras.push_back(nfcAExtrasData);

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
    ASSERT_TRUE(nfcA != nullptr);
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
/**
 * @tc.name: ReadSingleBlock001
 * @tc.desc: Test MifareClassicTag ReadSingleBlock.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, ReadSingleBlock001, TestSize.Level1)
{
    std::shared_ptr<MifareClassicTag> mifareClassic = MifareClassicTag::GetTag(tagInfo_);
    std::string testHexrespdata = TEST_HEX_RESP_DATA;
    int errorCode = mifareClassic->ReadSingleBlock(TEST_BLOCK_INDEX, testHexrespdata);

    // Error code returned when the chip and tag are not connected
    ASSERT_TRUE(errorCode == ErrorCode::ERR_TAG_STATE_DISCONNECTED);

    // Error code returned when the data exceeds the maximum value
    errorCode = mifareClassic->ReadSingleBlock(MC_MAX_BLOCK_INDEX, testHexrespdata);
    ASSERT_TRUE(errorCode == ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: WriteSingleBlock001
 * @tc.desc: Test MifareClassicTag WriteSingleBlock.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, WriteSingleBlock001, TestSize.Level1)
{
    std::shared_ptr<MifareClassicTag> mifareClassic = MifareClassicTag::GetTag(tagInfo_);
    int errorCode = mifareClassic->WriteSingleBlock(TEST_BLOCK_INDEX, TEST_HEX_RESP_DATA);

    // Error code returned when the chip and tag are not connected.
    ASSERT_TRUE(errorCode == ErrorCode::ERR_TAG_STATE_DISCONNECTED);
}
/**
 * @tc.name: IncrementBlock001
 * @tc.desc: Test MifareClassicTag IncrementBlock.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, IncrementBlock001, TestSize.Level1)
{
    std::shared_ptr<MifareClassicTag> mifareClassic = MifareClassicTag::GetTag(tagInfo_);
    int errorCode = mifareClassic->IncrementBlock(TEST_BLOCK_INDEX, VALUE);

    // Error code returned when the chip and tag are not connected.
    ASSERT_TRUE(errorCode == ErrorCode::ERR_TAG_STATE_DISCONNECTED);
}
/**
 * @tc.name: DecrementBlock001
 * @tc.desc: Test MifareClassicTag DecrementBlock.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, DecrementBlock001, TestSize.Level1)
{
    std::shared_ptr<MifareClassicTag> mifareClassic = MifareClassicTag::GetTag(tagInfo_);
    int errorCode = mifareClassic->DecrementBlock(TEST_BLOCK_INDEX, VALUE);

    // Error code returned when the chip and tag are not connected.
    ASSERT_TRUE(errorCode == ErrorCode::ERR_TAG_STATE_DISCONNECTED);
}
/**
 * @tc.name: TransferToBlock001
 * @tc.desc: Test MifareClassicTag TransferToBlock.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, TransferToBlock001, TestSize.Level1)
{
    std::shared_ptr<MifareClassicTag> mifareClassic = MifareClassicTag::GetTag(tagInfo_);
    int errorCode = mifareClassic->TransferToBlock(TEST_BLOCK_INDEX);

    // Error code returned when the chip and tag are not connected.
    ASSERT_TRUE(errorCode == ErrorCode::ERR_TAG_STATE_DISCONNECTED);
}
/**
 * @tc.name: RestoreFromBlock001
 * @tc.desc: Test MifareClassicTag RestoreFromBlock.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, RestoreFromBlock001, TestSize.Level1)
{
    std::shared_ptr<MifareClassicTag> mifareClassic = MifareClassicTag::GetTag(tagInfo_);
    int errorCode = mifareClassic->RestoreFromBlock(TEST_BLOCK_INDEX);

    // Error code returned when the chip and tag are not connected.
    ASSERT_TRUE(errorCode == ErrorCode::ERR_TAG_STATE_DISCONNECTED);
}
/**
 * @tc.name: GetSectorCount001
 * @tc.desc: Test MifareClassicTag GetSectorCount.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, GetSectorCount001, TestSize.Level1)
{
    std::shared_ptr<MifareClassicTag> mifareClassic = MifareClassicTag::GetTag(tagInfo_);
    int sectorCount = mifareClassic->GetSectorCount();
    ASSERT_TRUE(sectorCount == MifareClassicTag::MC_SECTOR_COUNT_OF_SIZE_1K);
}
/**
 * @tc.name: GetBlockCountInSector001
 * @tc.desc: Test MifareClassicTag GetBlockCountInSector.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, GetBlockCountInSector001, TestSize.Level1)
{
    std::shared_ptr<MifareClassicTag> mifareClassic = MifareClassicTag::GetTag(tagInfo_);

    // SectorIndex is between 0 and 32
    int sectorCount = mifareClassic->GetBlockCountInSector(TEST_SECTOR_INDEX);
    ASSERT_TRUE(sectorCount == MifareClassicTag::MC_BLOCK_COUNT);

    // sectorIndex > 40 is invalid
    sectorCount =  TEST_MC_MAX_SECTOR_COUNT + 1;
    sectorCount = mifareClassic->GetBlockCountInSector(sectorCount);
    ASSERT_TRUE(sectorCount == MifareClassicTag::MC_ERROR_VALUE);
}
/**
 * @tc.name: GetBlockIndexFromSector001
 * @tc.desc: Test MifareClassicTag GetBlockIndexFromSector.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, GetBlockIndexFromSector001, TestSize.Level1)
{
    std::shared_ptr<MifareClassicTag> mifareClassic = MifareClassicTag::GetTag(tagInfo_);

    // SectorIndex is between 0 and 32
    int sectorCount = mifareClassic->GetBlockIndexFromSector(TEST_SECTOR_INDEX);
    int expectResult = MifareClassicTag::MC_BLOCK_COUNT * TEST_SECTOR_INDEX;
    ASSERT_TRUE(sectorCount == expectResult);

    // sectorIndex > 40 is invalid
    sectorCount =  TEST_MC_MAX_SECTOR_COUNT + 1;
    sectorCount = mifareClassic->GetBlockIndexFromSector(sectorCount);
    ASSERT_TRUE(sectorCount == MifareClassicTag::MC_ERROR_VALUE);
}
/**
 * @tc.name: GetSectorIndexFromBlock001
 * @tc.desc: Test MifareClassicTag GetSectorIndexFromBlock.
 * @tc.type: FUNC
 */
HWTEST_F(MifareClassicTagTest, GetSectorIndexFromBlock001, TestSize.Level1)
{
    std::shared_ptr<MifareClassicTag> mifareClassic = MifareClassicTag::GetTag(tagInfo_);

    // SectorIndex is between 0 and 128
    int sectorCount = mifareClassic->GetSectorIndexFromBlock(TEST_BLOCK_INDEX);
    int expectResult = TEST_BLOCK_INDEX / MifareClassicTag::MC_BLOCK_COUNT;
    ASSERT_TRUE(sectorCount == expectResult);

    // blockindex > 256 is invalid
    int blockIndex =  MC_MAX_BLOCK_INDEX + 1;
    sectorCount = mifareClassic->GetBlockCountInSector(blockIndex);
    ASSERT_TRUE(sectorCount == MifareClassicTag::MC_ERROR_VALUE);
}
}
}
}
