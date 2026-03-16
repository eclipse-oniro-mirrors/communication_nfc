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
#define private public

#include <gtest/gtest.h>
#include <thread>

#include "nci_tag_proxy.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::NCI;

class NciTagProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NciTagProxyTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NciTagProxyTest." << std::endl;
}

void NciTagProxyTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NciTagProxyTest." << std::endl;
}

void NciTagProxyTest::SetUp()
{
    std::cout << " SetUp NciTagProxyTest." << std::endl;
}

void NciTagProxyTest::TearDown()
{
    std::cout << " TearDown NciTagProxyTest." << std::endl;
}

/**
 * @tc.name: SetTagListener001
 * @tc.desc: Test NciTagProxy SetTagListener with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, SetTagListener001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    std::shared_ptr<NCI::INciTagInterface::ITagListener> listener = nullptr;
    nciTagProxy->SetTagListener(listener);
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: SetTagListener002
 * @tc.desc: Test NciTagProxy SetTagListener with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, SetTagListener002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    std::shared_ptr<NCI::INciTagInterface::ITagListener> listener = nullptr;
    nciTagProxy->SetTagListener(listener);
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: SetTagListener003
 * @tc.desc: Test NciTagProxy SetTagListener with expired listener.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, SetTagListener003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    std::weak_ptr<NCI::INciTagInterface::ITagListener> listener;
    nciTagProxy->SetTagListener(listener);
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: GetTechList001
 * @tc.desc: Test NciTagProxy GetTechList with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTechList001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    std::vector<int> result = nciTagProxy->GetTechList(tagDiscId);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: GetTechList002
 * @tc.desc: Test NciTagProxy GetTechList with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTechList002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    std::vector<int> result = nciTagProxy->GetTechList(tagDiscId);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: GetTechList003
 * @tc.desc: Test NciTagProxy GetTechList with different tagDiscId.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTechList003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        nciTagProxy->GetTechList(i);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: GetConnectedTech001
 * @tc.desc: Test NciTagProxy GetConnectedTech with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetConnectedTech001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    uint32_t result = nciTagProxy->GetConnectedTech(tagDiscId);
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: GetConnectedTech002
 * @tc.desc: Test NciTagProxy GetConnectedTech with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetConnectedTech002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    uint32_t result = nciTagProxy->GetConnectedTech(tagDiscId);
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: GetConnectedTech003
 * @tc.desc: Test NciTagProxy GetConnectedTech with different tagDiscId.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetConnectedTech003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        nciTagProxy->GetConnectedTech(i);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: GetTechExtrasData001
 * @tc.desc: Test NciTagProxy GetTechExtrasData with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTechExtrasData001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    std::vector<AppExecFwk::PacMap> result = nciTagProxy->GetTechExtrasData(tagDiscId);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: GetTechExtrasData002
 * @tc.desc: Test NciTagProxy GetTechExtrasData with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTechExtrasData002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    std::vector<AppExecFwk::PacMap> result = nciTagProxy->GetTechExtrasData(tagDiscId);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: GetTechExtrasData003
 * @tc.desc: Test NciTagProxy GetTechExtrasData with different tagDiscId.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTechExtrasData003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        nciTagProxy->GetTechExtrasData(i);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: GetTagUid001
 * @tc.desc: Test NciTagProxy GetTagUid with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTagUid001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    std::string result = nciTagProxy->GetTagUid(tagDiscId);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: GetTagUid002
 * @tc.desc: Test NciTagProxy GetTagUid with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTagUid002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    std::string result = nciTagProxy->GetTagUid(tagDiscId);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: GetTagUid003
 * @tc.desc: Test NciTagProxy GetTagUid with different tagDiscId.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTagUid003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        nciTagProxy->GetTagUid(i);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: Connect001
 * @tc.desc: Test NciTagProxy Connect with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Connect001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    uint32_t technology = 0;
    bool result = nciTagProxy->Connect(tagDiscId, technology);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: Connect002
 * @tc.desc: Test NciTagProxy Connect with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Connect002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    uint32_t technology = 0;
    bool result = nciTagProxy->Connect(tagDiscId, technology);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: Connect003
 * @tc.desc: Test NciTagProxy Connect with different parameters.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Connect003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        for (uint32_t j = 0; j < 5; j++) {
            nciTagProxy->Connect(i, j);
        }
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: Disconnect001
 * @tc.desc: Test NciTagProxy Disconnect with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Disconnect001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    bool result = nciTagProxy->Disconnect(tagDiscId);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: Disconnect002
 * @tc.desc: Test NciTagProxy Disconnect with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Disconnect002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    bool result = nciTagProxy->Disconnect(tagDiscId);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: Disconnect003
 * @tc.desc: Test NciTagProxy Disconnect with different tagDiscId.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Disconnect003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        nciTagProxy->Disconnect(i);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: Reconnect001
 * @tc.desc: Test NciTagProxy Reconnect with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Reconnect001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    bool result = nciTagProxy->Reconnect(tagDiscId);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: Reconnect002
 * @tc.desc: Test NciTagProxy Reconnect with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Reconnect002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    bool result = nciTagProxy->Reconnect(tagDiscId);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: Reconnect003
 * @tc.desc: Test NciTagProxy Reconnect with different tagDiscId.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Reconnect003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        nciTagProxy->Reconnect(i);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: Transceive001
 * @tc.desc: Test NciTagProxy Transceive with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Transceive001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    std::string command = "ABCDEF";
    std::string response;
    int result = nciTagProxy->Transceive(tagDiscId, command, response);
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: Transceive002
 * @tc.desc: Test NciTagProxy Transceive with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Transceive002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    std::string command = "ABCDEF";
    std::string response;
    int result = nciTagProxy->Transceive(tagDiscId, command, response);
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: Transceive003
 * @tc.desc: Test NciTagProxy Transceive with empty command.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Transceive003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    std::string command = "";
    std::string response;
    int result = nciTagProxy->Transceive(tagDiscId, command, response);
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: Transceive004
 * @tc.desc: Test NciTagProxy Transceive with different commands.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Transceive004, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    std::vector<std::string> commands = {"", "A", "AB", "ABC", "123456", "FEDCBA"};
    for (const auto& cmd : commands) {
        std::string response;
        nciTagProxy->Transceive(0, cmd, response);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: ReadNdef001
 * @tc.desc: Test NciTagProxy ReadNdef with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, ReadNdef001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    std::string result = nciTagProxy->ReadNdef(tagDiscId);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: ReadNdef002
 * @tc.desc: Test NciTagProxy ReadNdef with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, ReadNdef002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    std::string result = nciTagProxy->ReadNdef(tagDiscId);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: ReadNdef003
 * @tc.desc: Test NciTagProxy ReadNdef with different tagDiscId.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, ReadNdef003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        nciTagProxy->ReadNdef(i);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: FindNdefTech001
 * @tc.desc: Test NciTagProxy FindNdefTech with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, FindNdefTech001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    std::string result = nciTagProxy->FindNdefTech(tagDiscId);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: FindNdefTech002
 * @tc.desc: Test NciTagProxy FindNdefTech with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, FindNdefTech002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    std::string result = nciTagProxy->FindNdefTech(tagDiscId);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: FindNdefTech003
 * @tc.desc: Test NciTagProxy FindNdefTech with different tagDiscId.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, FindNdefTech003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        nciTagProxy->FindNdefTech(i);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: WriteNdef001
 * @tc.desc: Test NciTagProxy WriteNdef with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, WriteNdef001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    std::string command = "ABCDEF";
    bool result = nciTagProxy->WriteNdef(tagDiscId, command);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: WriteNdef002
 * @tc.desc: Test NciTagProxy WriteNdef with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, WriteNdef002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    std::string command = "ABCDEF";
    bool result = nciTagProxy->WriteNdef(tagDiscId, command);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: WriteNdef003
 * @tc.desc: Test NciTagProxy WriteNdef with empty command.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, WriteNdef003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    std::string command = "";
    bool result = nciTagProxy->WriteNdef(tagDiscId, command);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: WriteNdef004
 * @tc.desc: Test NciTagProxy WriteNdef with different commands.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, WriteNdef004, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    std::vector<std::string> commands = {"", "A", "AB", "ABC", "123456", "FEDCBA"};
    for (const auto& cmd : commands) {
        nciTagProxy->WriteNdef(0, cmd);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: FormatNdef001
 * @tc.desc: Test NciTagProxy FormatNdef with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, FormatNdef001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    std::string key = "ABCDEF";
    bool result = nciTagProxy->FormatNdef(tagDiscId, key);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: FormatNdef002
 * @tc.desc: Test NciTagProxy FormatNdef with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, FormatNdef002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    std::string key = "ABCDEF";
    bool result = nciTagProxy->FormatNdef(tagDiscId, key);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: FormatNdef003
 * @tc.desc: Test NciTagProxy FormatNdef with empty key.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, FormatNdef003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    std::string key = "";
    bool result = nciTagProxy->FormatNdef(tagDiscId, key);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: FormatNdef004
 * @tc.desc: Test NciTagProxy FormatNdef with different keys.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, FormatNdef004, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    std::vector<std::string> keys = {"", "A", "AB", "ABC", "123456", "FEDCBA"};
    for (const auto& k : keys) {
        nciTagProxy->FormatNdef(0, k);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: CanMakeReadOnly001
 * @tc.desc: Test NciTagProxy CanMakeReadOnly with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, CanMakeReadOnly001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t ndefType = 0;
    bool result = nciTagProxy->CanMakeReadOnly(ndefType);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: CanMakeReadOnly002
 * @tc.desc: Test NciTagProxy CanMakeReadOnly with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, CanMakeReadOnly002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t ndefType = 0;
    bool result = nciTagProxy->CanMakeReadOnly(ndefType);
    ASSERT_TRUE(result);
}

/**
 * @tc.name: CanMakeReadOnly003
 * @tc.desc: Test NciTagProxy CanMakeReadOnly with different ndefType.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, CanMakeReadOnly003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        nciTagProxy->CanMakeReadOnly(i);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: SetNdefReadOnly001
 * @tc.desc: Test NciTagProxy SetNdefReadOnly with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, SetNdefReadOnly001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    bool result = nciTagProxy->SetNdefReadOnly(tagDiscId);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: SetNdefReadOnly002
 * @tc.desc: Test NciTagProxy SetNdefReadOnly with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, SetNdefReadOnly002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    bool result = nciTagProxy->SetNdefReadOnly(tagDiscId);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: SetNdefReadOnly003
 * @tc.desc: Test NciTagProxy SetNdefReadOnly with different tagDiscId.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, SetNdefReadOnly003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        nciTagProxy->SetNdefReadOnly(i);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: DetectNdefInfo001
 * @tc.desc: Test NciTagProxy DetectNdefInfo with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, DetectNdefInfo001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    std::vector<int> ndefInfo;
    bool result = nciTagProxy->DetectNdefInfo(tagDiscId, ndefInfo);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: DetectNdefInfo002
 * @tc.desc: Test NciTagProxy DetectNdefInfo with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, DetectNdefInfo002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    std::vector<int> ndefInfo;
    bool result = nciTagProxy->DetectNdefInfo(tagDiscId, ndefInfo);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: DetectNdefInfo003
 * @tc.desc: Test NciTagProxy DetectNdefInfo with different tagDiscId.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, DetectNdefInfo003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        std::vector<int> ndefInfo;
        nciTagProxy->DetectNdefInfo(i, ndefInfo);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: IsTagFieldOn001
 * @tc.desc: Test NciTagProxy IsTagFieldOn with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, IsTagFieldOn001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    bool result = nciTagProxy->IsTagFieldOn(tagDiscId);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsTagFieldOn002
 * @tc.desc: Test NciTagProxy IsTagFieldOn with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, IsTagFieldOn002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    bool result = nciTagProxy->IsTagFieldOn(tagDiscId);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: IsTagFieldOn003
 * @tc.desc: Test NciTagProxy IsTagFieldOn with different tagDiscId.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, IsTagFieldOn003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        nciTagProxy->IsTagFieldOn(i);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: StartFieldOnChecking001
 * @tc.desc: Test NciTagProxy StartFieldOnChecking with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, StartFieldOnChecking001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    uint32_t delayedMs = 0;
    nciTagProxy->StartFieldOnChecking(tagDiscId, delayedMs);
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: StartFieldOnChecking002
 * @tc.desc: Test NciTagProxy StartFieldOnChecking with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, StartFieldOnChecking002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    uint32_t delayedMs = 0;
    nciTagProxy->StartFieldOnChecking(tagDiscId, delayedMs);
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: StartFieldOnChecking003
 * @tc.desc: Test NciTagProxy StartFieldOnChecking with different parameters.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, StartFieldOnChecking003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        for (uint32_t j = 0; j < 5; j++) {
            nciTagProxy->StartFieldOnChecking(i, j);
        }
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: StopFieldChecking001
 * @tc.desc: Test NciTagProxy StopFieldChecking with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, StopFieldChecking001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->StopFieldChecking();
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: StopFieldChecking002
 * @tc.desc: Test NciTagProxy StopFieldChecking with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, StopFieldChecking002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    nciTagProxy->StopFieldChecking();
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: StopFieldChecking003
 * @tc.desc: Test NciTagProxy StopFieldChecking multiple times.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, StopFieldChecking003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (int i = 0; i < 5; i++) {
        nciTagProxy->StopFieldChecking();
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: SetTimeout001
 * @tc.desc: Test NciTagProxy SetTimeout with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, SetTimeout001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    uint32_t timeout = 0;
    uint32_t technology = 0;
    nciTagProxy->SetTimeout(tagDiscId, timeout, technology);
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: SetTimeout002
 * @tc.desc: Test NciTagProxy SetTimeout with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, SetTimeout002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    uint32_t timeout = 0;
    uint32_t technology = 0;
    nciTagProxy->SetTimeout(tagDiscId, timeout, technology);
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: SetTimeout003
 * @tc.desc: Test NciTagProxy SetTimeout with different parameters.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, SetTimeout003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        for (uint32_t j = 0; j < 5; j++) {
            nciTagProxy->SetTimeout(0, i, j);
        }
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: GetTimeout001
 * @tc.desc: Test NciTagProxy GetTimeout with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTimeout001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    uint32_t timeout = 0;
    uint32_t technology = 0;
    nciTagProxy->GetTimeout(tagDiscId, timeout, technology);
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: GetTimeout002
 * @tc.desc: Test NciTagProxy GetTimeout with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTimeout002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    uint32_t timeout = 0;
    uint32_t technology = 0;
    nciTagProxy->GetTimeout(tagDiscId, timeout, technology);
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: GetTimeout003
 * @tc.desc: Test NciTagProxy GetTimeout with different parameters.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTimeout003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        for (uint32_t j = 0; j < 5; j++) {
            uint32_t timeout = 0;
            nciTagProxy->GetTimeout(i, timeout, j);
        }
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: ResetTimeout001
 * @tc.desc: Test NciTagProxy ResetTimeout with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, ResetTimeout001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t tagDiscId = 0;
    nciTagProxy->ResetTimeout(tagDiscId);
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: ResetTimeout002
 * @tc.desc: Test NciTagProxy ResetTimeout with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, ResetTimeout002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t tagDiscId = 0;
    nciTagProxy->ResetTimeout(tagDiscId);
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: ResetTimeout003
 * @tc.desc: Test NciTagProxy ResetTimeout with different tagDiscId.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, ResetTimeout003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint32_t i = 0; i < 5; i++) {
        nciTagProxy->ResetTimeout(i);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: GetIsoDepMaxTransceiveLength001
 * @tc.desc: Test NciTagProxy GetIsoDepMaxTransceiveLength with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetIsoDepMaxTransceiveLength001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint32_t result = nciTagProxy->GetIsoDepMaxTransceiveLength();
    ASSERT_NE(result, 0);
}

/**
 * @tc.name: GetIsoDepMaxTransceiveLength002
 * @tc.desc: Test NciTagProxy GetIsoDepMaxTransceiveLength with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetIsoDepMaxTransceiveLength002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint32_t result = nciTagProxy->GetIsoDepMaxTransceiveLength();
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: GetIsoDepMaxTransceiveLength003
 * @tc.desc: Test NciTagProxy GetIsoDepMaxTransceiveLength multiple times.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetIsoDepMaxTransceiveLength003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (int i = 0; i < 5; i++) {
        nciTagProxy->GetIsoDepMaxTransceiveLength();
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: IsExtendedLengthApduSupported001
 * @tc.desc: Test NciTagProxy IsExtendedLengthApduSupported with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, IsExtendedLengthApduSupported001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    bool result = nciTagProxy->IsExtendedLengthApduSupported();
    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsExtendedLengthApduSupported002
 * @tc.desc: Test NciTagProxy IsExtendedLengthApduSupported with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, IsExtendedLengthApduSupported002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    bool result = nciTagProxy->IsExtendedLengthApduSupported();
    ASSERT_TRUE(result);
}

/**
 * @tc.name: IsExtendedLengthApduSupported003
 * @tc.desc: Test NciTagProxy IsExtendedLengthApduSupported multiple times.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, IsExtendedLengthApduSupported003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (int i = 0; i < 5; i++) {
        nciTagProxy->IsExtendedLengthApduSupported();
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: GetTechMaskFromTechList001
 * @tc.desc: Test NciTagProxy GetTechMaskFromTechList with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTechMaskFromTechList001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    std::vector<uint32_t> discTech = {0, 1, 2};
    uint16_t result = nciTagProxy->GetTechMaskFromTechList(discTech);
    ASSERT_NE(result, 0);
}

/**
 * @tc.name: GetTechMaskFromTechList002
 * @tc.desc: Test NciTagProxy GetTechMaskFromTechList with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTechMaskFromTechList002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    std::vector<uint32_t> discTech = {0, 1, 2};
    uint16_t result = nciTagProxy->GetTechMaskFromTechList(discTech);
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: GetTechMaskFromTechList003
 * @tc.desc: Test NciTagProxy GetTechMaskFromTechList with empty list.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTechMaskFromTechList003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    std::vector<uint32_t> discTech;
    uint16_t result = nciTagProxy->GetTechMaskFromTechList(discTech);
    ASSERT_EQ(result, 0);
}

/**
 * @tc.name: GetTechMaskFromTechList004
 * @tc.desc: Test NciTagProxy GetTechMaskFromTechList with different tech lists.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTechMaskFromTechList004, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    std::vector<std::vector<uint32_t>> techLists = {
        {},
        {0},
        {0, 1},
        {0, 1, 2},
        {1, 2, 3, 4}
    };
    for (const auto& techList : techLists) {
        nciTagProxy->GetTechMaskFromTechList(techList);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

/**
 * @tc.name: VendorParseHarPackage001
 * @tc.desc: Test NciTagProxy VendorParseHarPackage with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, VendorParseHarPackage001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    std::vector<std::string> harPackages;
    std::string uri = "test_uri";
    bool result = nciTagProxy->VendorParseHarPackage(harPackages, uri);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: VendorParseHarPackage002
 * @tc.desc: Test NciTagProxy VendorParseHarPackage with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, VendorParseHarPackage002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    std::vector<std::string> harPackages;
    std::string uri = "test_uri";
    bool result = nciTagProxy->VendorParseHarPackage(harPackages, uri);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: VendorParseHarPackage003
 * @tc.desc: Test NciTagProxy VendorParseHarPackage with empty parameters.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, VendorParseHarPackage003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    std::vector<std::string> harPackages;
    std::string uri = "";
    bool result = nciTagProxy->VendorParseHarPackage(harPackages, uri);
    ASSERT_FALSE(result);
}

/**
 * @tc.name: GetVendorInfo001
 * @tc.desc: Test NciTagProxy GetVendorInfo with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetVendorInfo001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    uint16_t type = 0;
    std::string result = nciTagProxy->GetVendorInfo(type);
    ASSERT_FALSE(result.empty());
}

/**
 * @tc.name: GetVendorInfo002
 * @tc.desc: Test NciTagProxy GetVendorInfo with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetVendorInfo002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    uint16_t type = 0;
    std::string result = nciTagProxy->GetVendorInfo(type);
    ASSERT_TRUE(result.empty());
}

/**
 * @tc.name: GetVendorInfo003
 * @tc.desc: Test NciTagProxy GetVendorInfo with different types.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetVendorInfo003, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    for (uint16_t i = 0; i < 5; i++) {
        nciTagProxy->GetVendorInfo(i);
    }
    ASSERT_TRUE(nciTagProxy != nullptr);
}

#ifdef VENDOR_APPLICATIONS_ENABLED
/**
 * @tc.name: IsVendorProcess001
 * @tc.desc: Test NciTagProxy IsVendorProcess with nciTagInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, IsVendorProcess001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    bool result = nciTagProxy->IsVendorProcess();
    ASSERT_TRUE(!result);
}

/**
 * @tc.name: IsVendorProcess002
 * @tc.desc: Test NciTagProxy IsVendorProcess with nciTagInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, IsVendorProcess002, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->nciTagInterface_ = nullptr;
    bool result = nciTagProxy->IsVendorProcess();
    ASSERT_TRUE(!result);
}
#endif
}
}
}