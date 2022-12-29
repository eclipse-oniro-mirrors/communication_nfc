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
    static constexpr const auto TEST_NDEF_TAG_LENGTH = 2;
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
    AppExecFwk::PacMap ndefExtrasData;
    ndefExtrasData.PutIntValue(TagInfo::NDEF_FORUM_TYPE, TEST_NDEF_FORUM_TYPE);
    ndefExtrasData.PutIntValue(TagInfo::NDEF_TAG_MODE, TEST_NDEF_TAG_MODE);
    ndefExtrasData.PutIntValue(TagInfo::NDEF_TAG_LENGTH, TEST_NDEF_TAG_LENGTH);
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
/**
 * @tc.name: GetMaxTagSize001
 * @tc.desc: Test NdefTag GetMaxTagSize.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, GetMaxTagSize001, TestSize.Level1)
{
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    uint32_t maxTagSize = ndef->GetMaxTagSize();
    ASSERT_TRUE(maxTagSize == TEST_NDEF_TAG_LENGTH);
}
/**
 * @tc.name: GetCachedNdefMsg001
 * @tc.desc: Test NdefTag GetCachedNdefMsg.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, GetCachedNdefMsg001, TestSize.Level1)
{
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    std::shared_ptr<NdefMessage> ndefMsg = ndef->GetCachedNdefMsg();
    ASSERT_TRUE(ndefMsg == std::shared_ptr<NdefMessage>());
}
/**
 * @tc.name: IsNdefWritable001
 * @tc.desc: Test NdefTag IsNdefWritable.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, IsNdefWritable001, TestSize.Level1)
{
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    bool writable = ndef->IsNdefWritable();
    ASSERT_TRUE(!writable);
}
/**
 * @tc.name: ReadNdef001
 * @tc.desc: Test NdefTag ReadNdef.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, ReadNdef001, TestSize.Level1)
{
    std::shared_ptr<NdefMessage> ndefMessage;
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    int readNdef = ndef->ReadNdef(ndefMessage);
    ASSERT_TRUE(readNdef == ErrorCode::ERR_TAG_STATE_LOST);
}
/**
 * @tc.name: WriteNdef001
 * @tc.desc: Test NdefTag WriteNdef.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, WriteNdef001, TestSize.Level1)
{
    std::shared_ptr<NdefMessage> msg;
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    int writeNdef = ndef->WriteNdef(msg);
    ASSERT_TRUE(writeNdef == ErrorCode::ERR_TAG_STATE_DISCONNECTED);
}
/**
 * @tc.name: IsEnableReadOnly001
 * @tc.desc: Test NdefTag IsEnableReadOnly.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, IsEnableReadOnly001, TestSize.Level1)
{
    bool canSetReadOnly;
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    int readOnly = ndef->IsEnableReadOnly(canSetReadOnly);
    ASSERT_TRUE(readOnly == ErrorCode::ERR_NONE);
}
/**
 * @tc.name: EnableReadOnly001
 * @tc.desc: Test NdefTag EnableReadOnly.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, EnableReadOnly001, TestSize.Level1)
{
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    int readOnly = ndef->EnableReadOnly();
    ASSERT_TRUE(readOnly == ErrorCode::ERR_TAG_STATE_DISCONNECTED);
}
/**
 * @tc.name: GetNdefTagTypeString001
 * @tc.desc: Test NdefTag GetNdefTagTypeString.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, GetNdefTagTypeString001, TestSize.Level1)
{
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    std::string typeString = ndef->GetNdefTagTypeString(TEST_NDEF_FORUM_TYPE);
    ASSERT_TRUE(typeString == "org.nfcforum.ndef.type1");
}
/**
 * @tc.name: GetNdefTagTypeString002
 * @tc.desc: Test NdefTag GetNdefTagTypeString.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, GetNdefTagTypeString002, TestSize.Level1)
{
    static constexpr const auto TEST_NDEF_FORUM_TYPE_2 = NdefTag::EmNfcForumType::NFC_FORUM_TYPE_2;
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    std::string typeString = ndef->GetNdefTagTypeString(TEST_NDEF_FORUM_TYPE_2);
    ASSERT_TRUE(typeString == "org.nfcforum.ndef.type2");
}
/**
 * @tc.name: SetTimeout001
 * @tc.desc: Test NdefTag SetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, SetTimeout001, TestSize.Level1)
{
    uint32_t timeout = 20;
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    int statusCode = ndef->SetTimeout(timeout);
    ASSERT_TRUE(statusCode == ErrorCode::ERR_NONE);
}
/**
 * @tc.name: GetTimeout001
 * @tc.desc: Test NdefTag GetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, GetTimeout001, TestSize.Level1)
{
    uint32_t experctedTimeout = 20;
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    int timeout = 0;
    int statusCode = ndef->GetTimeout(timeout);
    ASSERT_TRUE(statusCode == ErrorCode::ERR_NONE);
    ASSERT_TRUE(timeout == experctedTimeout);
}
/**
 * @tc.name: GetMaxSendCommandLength001
 * @tc.desc: Test NdefTag GetMaxSendCommandLength.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, GetMaxSendCommandLength001, TestSize.Level1)
{
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    int maxSize = 0;
    int statusCode = ndef->GetMaxSendCommandLength(maxSize);
    ASSERT_TRUE(statusCode == ErrorCode::ERR_NONE);
    ASSERT_TRUE(maxSize == 0);
}
/**
 * @tc.name: GetTagInfo001
 * @tc.desc: Test NdefTag GetTagInfo.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, GetTagInfo001, TestSize.Level1)
{
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    std::weak_ptr<TagInfo> tagInfo = ndef->GetTagInfo();
    ASSERT_TRUE(tagInfo.use_count() != 0);
}
/**
 * @tc.name: IsConnected001
 * @tc.desc: Test NdefTag IsConnected.
 * @tc.type: FUNC
 */
HWTEST_F(NdefTagTest, IsConnected001, TestSize.Level1)
{
    std::shared_ptr<NdefTag> ndef = NdefTag::GetTag(tagInfo_);
    bool isConnected = ndef->IsConnected();
    ASSERT_TRUE(isConnected == false);
}
}
}
}
