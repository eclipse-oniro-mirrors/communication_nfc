/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "taginfo_parcelable.h"
#include "nfc_sdk_common.h"
#include "parcel.h"
#include "refbase.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class TagInfoParcelableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
    static constexpr const auto TEST_UID = "0102";
    static constexpr const auto TEST_DISC_ID = 1;

    std::shared_ptr<TagInfoParcelable> tagInfoParcelable;
};

void TagInfoParcelableTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase TagInfoParcelableTest." << std::endl;
}

void TagInfoParcelableTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase TagInfoParcelableTest." << std::endl;
}

void TagInfoParcelableTest::SetUp()
{
    std::cout << " SetUp TagInfoParcelableTest." << std::endl;
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
    tagInfoParcelable = std::make_shared<TagInfoParcelable>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
}

void TagInfoParcelableTest::TearDown()
{
    std::cout << " TearDown TagInfoParcelableTest." << std::endl;
    tagInfoParcelable = nullptr;
}

/**
 * @tc.name: ToString001
 * @tc.desc: Test TagInfoParcelable ToString.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoParcelableTest, ToString001, TestSize.Level1)
{
    std::string toString = tagInfoParcelable->ToString();
    ASSERT_TRUE(toString == "tagTechList: [1, 3]");
}
/**
 * @tc.name: ToString002
 * @tc.desc: Test TagInfoParcelable ToString.
 * @tc.type: FUNC
 */
HWTEST_F(TagInfoParcelableTest, ToString002, TestSize.Level1)
{
    std::vector<int> tagTechList;
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    AppExecFwk::PacMap tagTechExtrasData;
    AppExecFwk::PacMap isoDepExtrasData;
    tagTechExtras.push_back(tagTechExtrasData);
    tagTechExtras.push_back(isoDepExtrasData);
    std::string tagUid = TEST_UID;
    int tagRfDiscId = TEST_DISC_ID;
    std::shared_ptr<TagInfoParcelable> tagInfo = std::make_shared<TagInfoParcelable>(tagTechList, tagTechExtras,
        tagUid, tagRfDiscId, nullptr);
    std::string toString = tagInfo->ToString();
    ASSERT_TRUE(toString == "tagTechList: []");
}
}
}
}
