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

#include "nfc_sdk_common.h"
#include "nfc_controller.h"
#include "taginfo.h"
#include "tag_session_proxy.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class TagInfoTest : public testing::Test {
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
    static constexpr const auto TEST_INDEX_1 = 1;
    std::shared_ptr<TagInfo> tagInfo_;
};

void TagInfoTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase TagInfoTest." << std::endl;
}

void TagInfoTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase TagInfoTest." << std::endl;
}

void TagInfoTest::SetUp()
{
    std::cout << " SetUp TagInfoTest." << std::endl;
    std::vector<int> tagTechList;
    tagTechList.push_back((int)TagTechnology::NFC_A_TECH);
    tagTechList.push_back((int)TagTechnology::NFC_ISODEP_TECH);

    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap tagTechExtrasData;
    AppExecFwk::PacMap isoDepExtrasData;
    tagTechExtras.push_back(tagTechExtrasData);
    tagTechExtras.push_back(isoDepExtrasData);
    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_DISC_ID;
    OHOS::sptr<IRemoteObject> iface = NfcController::GetInstance().GetTagServiceIface();
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, iface);
}

void TagInfoTest::TearDown()
{
    std::cout << " TearDown TagInfoTest." << std::endl;
    tagInfo_ = nullptr;
}

/**
 * @tc.name: GetTagUid001
 * @tc.desc: Test NfcController GetTagUid.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetTagUid001, TestSize.Level1)
{
    std::string uid = tagInfo_->GetTagUid();
    ASSERT_TRUE(strcmp(uid.c_str(), TEST_UID) == 0);
}
/**
 * @tc.name: IsTechSupported001
 * @tc.desc: Test NfcController IsTechSupported.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, IsTechSupported001, TestSize.Level1)
{
    bool supported = tagInfo_->IsTechSupported(TagTechnology::NFC_A_TECH);
    ASSERT_TRUE(supported);
}
/**
 * @tc.name: IsTechSupported002
 * @tc.desc: Test NfcController IsTechSupported.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, IsTechSupported002, TestSize.Level1)
{
    bool supported = tagInfo_->IsTechSupported(TagTechnology::NFC_ISODEP_TECH);
    ASSERT_TRUE(supported);
}
/**
 * @tc.name: IsTechSupported003
 * @tc.desc: Test NfcController IsTechSupported.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, IsTechSupported003, TestSize.Level1)
{
    bool supported = tagInfo_->IsTechSupported(TagTechnology::NFC_NDEF_TECH);
    ASSERT_TRUE(!supported);
}

/**
 * @tc.name: SetConnectedTagTech001
 * @tc.desc: Test NfcController SetConnectedTagTech.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, SetConnectedTagTech001, TestSize.Level1)
{
    tagInfo_->SetConnectedTagTech(TagTechnology::NFC_NDEF_TECH);
    ASSERT_TRUE(tagInfo_->GetConnectedTagTech() == TagTechnology::NFC_NDEF_TECH);
}
/**
 * @tc.name: SetConnectedTagTech002
 * @tc.desc: Test NfcController SetConnectedTagTech.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, SetConnectedTagTech002, TestSize.Level1)
{
    tagInfo_->SetConnectedTagTech(TagTechnology::NFC_ISODEP_TECH);
    ASSERT_TRUE(!(tagInfo_->GetConnectedTagTech() == TagTechnology::NFC_NDEF_TECH));
}
/**
 * @tc.name: GetTagRfDiscId001
 * @tc.desc: Test NfcController GetTagRfDiscId.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetTagRfDiscId001, TestSize.Level1)
{
    int discId = tagInfo_->GetTagRfDiscId();
    ASSERT_TRUE(discId == TEST_DISC_ID);
}
/**
 * @tc.name: GetTagTechList001
 * @tc.desc: Test NfcController GetTagTechList.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetTagTechList001, TestSize.Level1)
{
    std::vector<int> techList_;
    std::vector<int> techList = tagInfo_->GetTagTechList();
    techList_ = std::move(techList);
    ASSERT_TRUE(techList.size() == 0);
}
/**
 * @tc.name: GetStringExtrasData001
 * @tc.desc: Test NfcController GetStringExtrasData.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetStringExtrasData001, TestSize.Level1)
{
    AppExecFwk::PacMap extrasData;
    const std::string extrasName;
    std::string stringExtrasData = tagInfo_->GetStringExtrasData(extrasData, extrasName);
    ASSERT_TRUE(stringExtrasData == "");
}
/**
 * @tc.name: GetIntExtrasData001
 * @tc.desc: Test NfcController GetIntExtrasData.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetIntExtrasData001, TestSize.Level1)
{
    AppExecFwk::PacMap extrasData;
    const std::string extrasName;
    int result = tagInfo_->GetIntExtrasData(extrasData, extrasName);
    ASSERT_TRUE(result == ErrorCode::ERR_TAG_PARAMETERS);
}
/**
 * @tc.name: GetBoolExtrasData001
 * @tc.desc: Test NfcController GetBoolExtrasData.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetBoolExtrasData001, TestSize.Level1)
{
    AppExecFwk::PacMap extrasData;
    const std::string extrasName;
    bool boolExtrasData = tagInfo_->GetBoolExtrasData(extrasData, extrasName);
    ASSERT_TRUE(!boolExtrasData);
}

/**
 * @tc.name: GetStringTech001
 * @tc.desc: Test NfcController GetStringTech.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetStringTech001, TestSize.Level1)
{
    std::string stringTech = tagInfo_->GetStringTech(TEST_DISC_ID);
    ASSERT_TRUE(stringTech == "NfcA");
}
/**
 * @tc.name: GetStringTech002
 * @tc.desc: Test NfcController GetStringTech.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetStringTech002, TestSize.Level1)
{
    std::string stringTech = tagInfo_->GetStringTech(static_cast<int>(TagTechnology::NFC_B_TECH));
    ASSERT_TRUE(stringTech == "NfcB");
}
/**
 * @tc.name: GetStringTech003
 * @tc.desc: Test NfcController GetStringTech.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetStringTech003, TestSize.Level1)
{
    std::string stringTech = tagInfo_->GetStringTech(static_cast<int>(TagTechnology::NFC_F_TECH));
    ASSERT_TRUE(stringTech == "NfcF");
}
/**
 * @tc.name: GetStringTech004
 * @tc.desc: Test NfcController GetStringTech.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetStringTech004, TestSize.Level1)
{
    std::string stringTech = tagInfo_->GetStringTech(static_cast<int>(TagTechnology::NFC_V_TECH));
    ASSERT_TRUE(stringTech == "NfcV");
}
/**
 * @tc.name: GetStringTech005
 * @tc.desc: Test NfcController GetStringTech.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetStringTech005, TestSize.Level1)
{
    std::string stringTech = tagInfo_->GetStringTech(static_cast<int>(TagTechnology::NFC_ISODEP_TECH));
    ASSERT_TRUE(stringTech == "IsoDep");
}
/**
 * @tc.name: GetStringTech006
 * @tc.desc: Test NfcController GetStringTech.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetStringTech006, TestSize.Level1)
{
    std::string stringTech = tagInfo_->GetStringTech(static_cast<int>(TagTechnology::NFC_MIFARE_CLASSIC_TECH));
    ASSERT_TRUE(stringTech == "MifareClassic");
}
/**
 * @tc.name: GetStringTech007
 * @tc.desc: Test NfcController GetStringTech.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetStringTech007, TestSize.Level1)
{
    std::string stringTech = tagInfo_->GetStringTech(static_cast<int>(TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH));
    ASSERT_TRUE(stringTech == "MifareUL");
}
/**
 * @tc.name: GetStringTech008
 * @tc.desc: Test NfcController GetStringTech.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetStringTech008, TestSize.Level1)
{
    std::string stringTech = tagInfo_->GetStringTech(static_cast<int>(TagTechnology::NFC_NDEF_TECH));
    ASSERT_TRUE(stringTech == "Ndef");
}
/**
 * @tc.name: GetStringTech009
 * @tc.desc: Test NfcController GetStringTech.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetStringTech009, TestSize.Level1)
{
    std::string stringTech = tagInfo_->GetStringTech(static_cast<int>(TagTechnology::NFC_NDEF_FORMATABLE_TECH));
    ASSERT_TRUE(stringTech == "NdefFormatable");
}
/**
 * @tc.name: GetStringTech0010
 * @tc.desc: Test NfcController GetStringTech.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetStringTech0010, TestSize.Level1)
{
    std::string stringTech = tagInfo_->GetStringTech(static_cast<int>(TagTechnology::NFC_INVALID_TECH));
    ASSERT_TRUE(stringTech == "");
}
/**
 * @tc.name: GetBoolExtrasData002
 * @tc.desc: Test NfcController GetBoolExtrasData.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetBoolExtrasData002, TestSize.Level1)
{
    AppExecFwk::PacMap tagTechExtrasData;
    tagTechExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    tagTechExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    bool getBoolExtrasData = tagInfo_->GetBoolExtrasData(tagTechExtrasData, TEST_UID);
    ASSERT_TRUE(getBoolExtrasData == tagTechExtrasData.GetBooleanValue(TEST_UID, false));
}
/**
 * @tc.name: GetTechExtrasByIndex001
 * @tc.desc: Test NfcController GetTechExtrasByIndex.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetTechExtrasByIndex001, TestSize.Level1)
{
    std::vector<AppExecFwk::PacMap> tagTechExtrasData;
    size_t techIndex = -1;
    AppExecFwk::PacMap getTechExtrasByIndex = tagInfo_->GetTechExtrasByIndex(techIndex);
    ASSERT_TRUE(getTechExtrasByIndex.IsEmpty() == true);
}
/**
 * @tc.name: GetTechExtrasByIndex002
 * @tc.desc: Test NfcController GetTechExtrasByIndex.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetTechExtrasByIndex002, TestSize.Level1)
{
    std::vector<int> tagTechList;
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap tagTechExtrasData;
    tagTechExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    tagTechExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    tagTechExtras.push_back(tagTechExtrasData);
    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_INDEX_1;
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
    size_t techIndex = -1;
    AppExecFwk::PacMap getTechExtrasByIndex = tagInfo_->GetTechExtrasByIndex(techIndex);
    ASSERT_TRUE(getTechExtrasByIndex.IsEmpty() == true);
}
/**
 * @tc.name: GetTechExtrasByIndex003
 * @tc.desc: Test NfcController GetTechExtrasByIndex.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetTechExtrasByIndex003, TestSize.Level1)
{
    std::vector<int> tagTechList;
    tagTechList.push_back((int)TagTechnology::NFC_A_TECH);
    tagTechList.push_back((int)TagTechnology::NFC_ISODEP_TECH);
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap tagTechExtrasData;
    tagTechExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    tagTechExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    tagTechExtras.push_back(tagTechExtrasData);
    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_INDEX_1;
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
    size_t techIndex = -1;
    AppExecFwk::PacMap getTechExtrasByIndex = tagInfo_->GetTechExtrasByIndex(techIndex);
    ASSERT_TRUE(getTechExtrasByIndex.IsEmpty() == true);
}
/**
 * @tc.name: GetTechExtrasByIndex004
 * @tc.desc: Test NfcController GetTechExtrasByIndex.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetTechExtrasByIndex004, TestSize.Level1)
{
    std::vector<int> tagTechList;
    tagTechList.push_back((int)TagTechnology::NFC_A_TECH);
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap tagTechExtrasData;
    tagTechExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    tagTechExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    tagTechExtras.push_back(tagTechExtrasData);
    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_INDEX_1;
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
    size_t techIndex = TEST_DISC_ID;
    AppExecFwk::PacMap getTechExtrasByIndex = tagInfo_->GetTechExtrasByIndex(techIndex);
    ASSERT_TRUE(getTechExtrasByIndex.IsEmpty() == true);
}
/**
 * @tc.name: GetTechExtrasByIndex005
 * @tc.desc: Test NfcController GetTechExtrasByIndex.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetTechExtrasByIndex005, TestSize.Level1)
{
    std::vector<int> tagTechList;
    tagTechList.push_back((int)TagTechnology::NFC_A_TECH);
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap tagTechExtrasData;
    tagTechExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    tagTechExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    tagTechExtras.push_back(tagTechExtrasData);
    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_INDEX_1;
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
    size_t techIndex = 0;
    AppExecFwk::PacMap getTechExtrasByIndex = tagInfo_->GetTechExtrasByIndex(techIndex);
    ASSERT_TRUE(getTechExtrasByIndex.IsEmpty() == false);
}
/**
 * @tc.name: GetTechExtrasByTech001
 * @tc.desc: Test NfcController GetTechExtrasByTech.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetTechExtrasByTech001, TestSize.Level1)
{
    std::vector<int> tagTechList;
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap tagTechExtrasData;
    tagTechExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    tagTechExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    tagTechExtras.push_back(tagTechExtrasData);
    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_INDEX_1;
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
    AppExecFwk::PacMap getTechExtrasByTech = tagInfo_->GetTechExtrasByTech(TagTechnology::NFC_NDEF_FORMATABLE_TECH);
    ASSERT_TRUE(getTechExtrasByTech.IsEmpty() == true);
}
/**
 * @tc.name: GetTechExtrasByTech002
 * @tc.desc: Test NfcController GetTechExtrasByTech.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetTechExtrasByTech002, TestSize.Level1)
{
    std::vector<int> tagTechList;
    tagTechList.push_back((int)TagTechnology::NFC_A_TECH);
    tagTechList.push_back((int)TagTechnology::NFC_ISODEP_TECH);
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap tagTechExtrasData;
    tagTechExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    tagTechExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    tagTechExtras.push_back(tagTechExtrasData);
    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_INDEX_1;
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
    AppExecFwk::PacMap getTechExtrasByTech = tagInfo_->GetTechExtrasByTech(TagTechnology::NFC_NDEF_FORMATABLE_TECH);
    ASSERT_TRUE(getTechExtrasByTech.IsEmpty() == true);
}
/**
 * @tc.name: GetBoolExtrasData003
 * @tc.desc: Test NfcController GetBoolExtrasData.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetBoolExtrasData003, TestSize.Level1)
{
    AppExecFwk::PacMap extrasData;
    extrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
    extrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
    const std::string extrasName = "";
    bool boolExtrasData = tagInfo_->GetBoolExtrasData(extrasData, extrasName);
    ASSERT_TRUE(!boolExtrasData);
}
}
}
}
