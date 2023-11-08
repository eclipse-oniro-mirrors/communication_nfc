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
#include "nfc_service.h"
#include "tag_host.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::NCI;

std::vector<int> tagTechList = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
std::vector<uint32_t> tagRfDiscIdList = {0, 1, 2};
std::vector<uint32_t> tagActivatedProtocols = {0x04};
std::string tagUid = "5B7FCFA9";
std::vector<std::string> tagPollBytes = {"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "0A", "0B",
    "0C", "0D", "0E", "0F", "10", "11"};
std::vector<std::string> tagActivatedBytes = tagPollBytes;
int g_connectedTechIndex = 0;

class TagHostTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown();

    std::shared_ptr<TagHost> tag_;
};

void TagHostTest::SetUp()
{
    tag_ = std::make_shared<TagHost>(
        tagTechList, tagRfDiscIdList, tagActivatedProtocols, tagUid, tagPollBytes, tagActivatedBytes,
        g_connectedTechIndex);
    std::shared_ptr<NCI::TagHost> tag = std::make_shared<TagHost>(
        tagTechList, tagRfDiscIdList, tagActivatedProtocols, tagUid, tagPollBytes, tagActivatedBytes,
        g_connectedTechIndex);
    tag = nullptr;
}

void TagHostTest::TearDown()
{
    tag_ = nullptr;
}

/**
 * @tc.name: ConnectTest001
 * @tc.desc: Test Connect
 * @tc.type: FUNC
 */
HWTEST_F(TagHostTest, ConnectTest001, TestSize.Level1)
{
    std::vector<int> techList = tag_->GetTechList();
    EXPECT_FALSE(techList.empty());
    EXPECT_FALSE(tag_->Connect(static_cast<int>(KITS::TagTechnology::NFC_A_TECH)));
    EXPECT_FALSE(tag_->FieldOnCheckingThread());
    EXPECT_FALSE(tag_->IsTagFieldOn());
    std::string uid = tag_->GetTagUid();
    EXPECT_STREQ(uid.c_str(), "5B7FCFA9");
    std::string req = "9060000000";
    std::string res = "";
    tag_->Transceive(req, res);
    EXPECT_STREQ(res.c_str(), "");
    tag_->StopFieldChecking();
    EXPECT_FALSE(tag_->Connect(-1));
    EXPECT_TRUE(tag_->Disconnect());
}

/**
 * @tc.name: RemoveTechTest001
 * @tc.desc: Test RemoveTech
 * @tc.type: FUNC
 */
HWTEST_F(TagHostTest, RemoveTechTest001, TestSize.Level1)
{
    static const int INVALID_VALUE = -1;
    tag_->RemoveTech(INVALID_VALUE);
    int tagRfDiscId = tag_->GetTagRfDiscId();
    EXPECT_EQ(tagRfDiscId, 0);
    int fieldOnCheckInterval = 125;
    tag_->StartFieldOnChecking(fieldOnCheckInterval);
    fieldOnCheckInterval = 0;
    tag_->StartFieldOnChecking(fieldOnCheckInterval);
}

/**
 * @tc.name: GetTechExtrasDataTest001
 * @tc.desc: Test GetTechExtrasData
 * @tc.type: FUNC
 */
HWTEST_F(TagHostTest, GetTechExtrasDataTest001, TestSize.Level1)
{
    int tagRfDiscId = tag_->GetTagRfDiscId();
    EXPECT_EQ(tagRfDiscId, 0);
    std::vector<AppExecFwk::PacMap> tagTechExtras = tag_->GetTechExtrasData();
    tagTechList.clear();
    tagPollBytes.clear();
    tagActivatedBytes.clear();
    tag_ = std::make_shared<TagHost>(
        tagTechList, tagRfDiscIdList, tagActivatedProtocols, tagUid, tagPollBytes, tagActivatedBytes,
        g_connectedTechIndex);
    tag_->GetTechExtrasData();
    tagTechList = {1, 2, 3, 4, 5, 6};
    tagPollBytes = {"00", "01", "02", "03", "04", "05", "06"};
    tagActivatedBytes = tagPollBytes;
    tag_ = std::make_shared<TagHost>(
        tagTechList, tagRfDiscIdList, tagActivatedProtocols, tagUid, tagPollBytes, tagActivatedBytes,
        g_connectedTechIndex);
    tag_->GetTechExtrasData();
    EXPECT_FALSE(tag_->GetConnectedTech() == static_cast<int>(KITS::TagTechnology::NFC_ISODEP_TECH));
}

/**
 * @tc.name: ReadNdefTest001
 * @tc.desc: Test ReadNdef
 * @tc.type: FUNC
 */
HWTEST_F(TagHostTest, ReadNdefTest001, TestSize.Level1)
{
    std::string response = tag_->ReadNdef();
    EXPECT_STREQ(response.c_str(), "");
    std::string data = "";
    EXPECT_FALSE(tag_->WriteNdef(data));
    std::string key = "";
    EXPECT_FALSE(tag_->FormatNdef(key));
    key = "01";
    EXPECT_FALSE(tag_->FormatNdef(key));
    EXPECT_FALSE(tag_->IsNdefFormatable());
    std::vector<int> ndefInfo;
    EXPECT_FALSE(tag_->DetectNdefInfo(ndefInfo));
}
}
}
}