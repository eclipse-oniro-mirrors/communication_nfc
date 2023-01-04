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
    tagInfo_ = std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
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
    std::string stingExtras = tagInfo_->GetStringExtrasData(extrasData, extrasName);
    ASSERT_TRUE(stingExtras == "");
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
    int inextrasData = tagInfo_->GetIntExtrasData(extrasData, extrasName);
    ASSERT_TRUE(inextrasData == ErrorCode::ERR_TAG_PARAMETERS);
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
    bool boolextrasData = tagInfo_->GetBoolExtrasData(extrasData, extrasName);
    ASSERT_TRUE(!boolextrasData);
}
/**
 * @tc.name: GetTagSessionProxy001
 * @tc.desc: Test NfcController GetTagSessionProxy.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetTagSessionProxy001, TestSize.Level1)
{
    OHOS::sptr<TAG::ITagSession> tagsessionProxy = tagInfo_->GetTagSessionProxy();
    ASSERT_TRUE(tagsessionProxy != nullptr);
}
/**
 * @tc.name: GetStringTach001
 * @tc.desc: Test NfcController GetStringTach.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoTest, GetStringTach001, TestSize.Level1)
{
    std::string stringTach = tagInfo_->GetStringTach(TEST_DISC_ID);
    ASSERT_TRUE(stringTach == "NfcA");
}
}
}
}
