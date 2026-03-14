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

#include "nci_nfcc_proxy.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::NCI;
class NciNfccProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NciNfccProxyTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NciNfccProxyTest." << std::endl;
}

void NciNfccProxyTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NciNfccProxyTest." << std::endl;
}

void NciNfccProxyTest::SetUp()
{
    std::cout << " SetUp NciNfccProxyTest." << std::endl;
}

void NciNfccProxyTest::TearDown()
{
    std::cout << " TearDown NciNfccProxyTest." << std::endl;
}

/**
 * @tc.name: Deinitialize001
 * @tc.desc: Test NciNfccProxyTest Deinitialize.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, Deinitialize001, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    ASSERT_FALSE(nciNfccProxy->Deinitialize());
}

/**
 * @tc.name: Initialize001
 * @tc.desc: Test NciNfccProxyTest initialize.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, Initialize001, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    nciNfccProxy->nfccInterface_ = nullptr;
    ASSERT_TRUE(nciNfccProxy->Initialize());
}

/**
 * @tc.name: EnableDiscovery001
 * @tc.desc: Test NciNfccProxyTest EnableDiscovery.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, EnableDiscovery001, TestSize.Level1)
{
    uint16_t techMask = 0;
    bool enableReaderMode = false;
    bool enableHostRouting = false;
    bool restart = false;
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    nciNfccProxy->EnableDiscovery(techMask, enableReaderMode, enableHostRouting, restart);
    ASSERT_EQ(nciNfccProxy->GetNciVersion(), 0);
}

/**
 * @tc.name: DisableDiscovery001
 * @tc.desc: Test NciNfccProxyTest DisableDiscovery.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, DisableDiscovery001, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    nciNfccProxy->DisableDiscovery();
    ASSERT_EQ(nciNfccProxy->GetNciVersion(), 0);
}

/**
 * @tc.name: SetScreenStatus001
 * @tc.desc: Test NciNfccProxyTest SetScreenStatus.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, SetScreenStatus001, TestSize.Level1)
{
    uint8_t screenStateMask = 0;
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    ASSERT_TRUE(nciNfccProxy->SetScreenStatus(screenStateMask));
}

/**
 * @tc.name: GetNciVersion001
 * @tc.desc: Test NciNfccProxyTest GetNciVersion.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, GetNciVersion001, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    ASSERT_EQ(nciNfccProxy->GetNciVersion(), 0);
}

/**
 * @tc.name: FactoryReset001
 * @tc.desc: Test NciNfccProxyTest FactoryReset.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, FactoryReset001, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    nciNfccProxy->FactoryReset();
    ASSERT_EQ(nciNfccProxy->GetNciVersion(), 0);
}

/**
 * @tc.name: Shutdown001
 * @tc.desc: Test NciNfccProxyTest Shutdown.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, Shutdown001, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    nciNfccProxy->Shutdown();
    ASSERT_EQ(nciNfccProxy->GetNciVersion(), 0);
}

/**
 * @tc.name: Deinitialize002
 * @tc.desc: Test NciNfccProxyTest Deinitialize with nfccInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, Deinitialize002, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    nciNfccProxy->nfccInterface_ = nullptr;
    ASSERT_TRUE(nciNfccProxy->Deinitialize());
}

/**
 * @tc.name: Deinitialize003
 * @tc.desc: Test NciNfccProxyTest Deinitialize multiple times.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, Deinitialize003, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    for (int i = 0; i < 5; i++) {
        nciNfccProxy->Deinitialize();
    }
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: EnableDiscovery002
 * @tc.desc: Test NciNfccProxyTest EnableDiscovery with nfccInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, EnableDiscovery002, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    nciNfccProxy->nfccInterface_ = nullptr;
    uint16_t techMask = 0;
    bool enableReaderMode = false;
    bool enableHostRouting = false;
    bool restart = false;
    nciNfccProxy->EnableDiscovery(techMask, enableReaderMode, enableHostRouting, restart);
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: EnableDiscovery003
 * @tc.desc: Test NciNfccProxyTest EnableDiscovery with different parameters.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, EnableDiscovery003, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    std::vector<uint16_t> techMasks = {0, 1, 2, 3, 4};
    for (auto techMask : techMasks) {
        nciNfccProxy->EnableDiscovery(techMask, true, false, false);
        nciNfccProxy->EnableDiscovery(techMask, false, true, false);
        nciNfccProxy->EnableDiscovery(techMask, false, false, true);
        nciNfccProxy->EnableDiscovery(techMask, true, true, true);
    }
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: DisableDiscovery002
 * @tc.desc: Test NciNfccProxyTest DisableDiscovery with nfccInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, DisableDiscovery002, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    nciNfccProxy->nfccInterface_ = nullptr;
    nciNfccProxy->DisableDiscovery();
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: DisableDiscovery003
 * @tc.desc: Test NciNfccProxyTest DisableDiscovery multiple times.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, DisableDiscovery003, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    for (int i = 0; i < 5; i++) {
        nciNfccProxy->DisableDiscovery();
    }
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: SetScreenStatus002
 * @tc.desc: Test NciNfccProxyTest SetScreenStatus with nfccInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, SetScreenStatus002, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    nciNfccProxy->nfccInterface_ = nullptr;
    uint8_t screenStateMask = 0;
    ASSERT_TRUE(nciNfccProxy->SetScreenStatus(screenStateMask));
}

/**
 * @tc.name: SetScreenStatus003
 * @tc.desc: Test NciNfccProxyTest SetScreenStatus with different parameters.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, SetScreenStatus003, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    for (uint8_t i = 0; i < 5; i++) {
        nciNfccProxy->SetScreenStatus(i);
    }
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: GetNciVersion002
 * @tc.desc: Test NciNfccProxyTest GetNciVersion with nfccInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, GetNciVersion002, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    nciNfccProxy->nfccInterface_ = nullptr;
    ASSERT_EQ(nciNfccProxy->GetNciVersion(), 0x10);
}

/**
 * @tc.name: GetNciVersion003
 * @tc.desc: Test NciNfccProxyTest GetNciVersion multiple times.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, GetNciVersion003, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    for (int i = 0; i < 5; i++) {
        nciNfccProxy->GetNciVersion();
    }
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: Abort002
 * @tc.desc: Test NciNfccProxyTest Abort with nfccInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, Abort002, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    nciNfccProxy->nfccInterface_ = nullptr;
    nciNfccProxy->Abort();
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: FactoryReset002
 * @tc.desc: Test NciNfccProxyTest FactoryReset with nfccInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, FactoryReset002, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    nciNfccProxy->nfccInterface_ = nullptr;
    nciNfccProxy->FactoryReset();
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: FactoryReset003
 * @tc.desc: Test NciNfccProxyTest FactoryReset multiple times.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, FactoryReset003, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    for (int i = 0; i < 5; i++) {
        nciNfccProxy->FactoryReset();
    }
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: Shutdown002
 * @tc.desc: Test NciNfccProxyTest Shutdown with nfccInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, Shutdown002, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    nciNfccProxy->nfccInterface_ = nullptr;
    nciNfccProxy->Shutdown();
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: Shutdown003
 * @tc.desc: Test NciNfccProxyTest Shutdown multiple times.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, Shutdown003, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    for (int i = 0; i < 5; i++) {
        nciNfccProxy->Shutdown();
    }
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: NotifyMessageToVendor001
 * @tc.desc: Test NciNfccProxyTest NotifyMessageToVendor with nfccInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, NotifyMessageToVendor001, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    int key = 0;
    std::string value = "test_value";
    nciNfccProxy->NotifyMessageToVendor(key, value);
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: NotifyMessageToVendor002
 * @tc.desc: Test NciNfccProxyTest NotifyMessageToVendor with nfccInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, NotifyMessageToVendor002, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    nciNfccProxy->nfccInterface_ = nullptr;
    int key = 0;
    std::string value = "test_value";
    nciNfccProxy->NotifyMessageToVendor(key, value);
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: NotifyMessageToVendor003
 * @tc.desc: Test NciNfccProxyTest NotifyMessageToVendor with different parameters.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, NotifyMessageToVendor003, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    std::vector<int> keys = {0, 1, 2, 3, 4};
    std::vector<std::string> values = {"", "A", "AB", "ABC", "123456"};
    for (const auto& key : keys) {
        for (const auto& value : values) {
            nciNfccProxy->NotifyMessageToVendor(key, value);
        }
    }
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: UpdateWantExtInfoByVendor001
 * @tc.desc: Test NciNfccProxyTest UpdateWantExtInfoByVendor with nfccInterface_ not null.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, UpdateWantExtInfoByVendor001, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    AAFwk::Want want;
    std::string uri = "test_uri";
    nciNfccProxy->UpdateWantExtInfoByVendor(want, uri);
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: UpdateWantExtInfoByVendor002
 * @tc.desc: Test NciNfccProxyTest UpdateWantExtInfoByVendor with nfccInterface_ null.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, UpdateWantExtInfoByVendor002, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    nciNfccProxy->nfccInterface_ = nullptr;
    AAFwk::Want want;
    std::string uri = "test_uri";
    nciNfccProxy->UpdateWantExtInfoByVendor(want, uri);
    ASSERT_TRUE(nciNfccProxy != nullptr);
}

/**
 * @tc.name: UpdateWantExtInfoByVendor003
 * @tc.desc: Test NciNfccProxyTest UpdateWantExtInfoByVendor with different parameters.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, UpdateWantExtInfoByVendor003, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    std::vector<std::string> uris = {"", "A", "AB", "ABC", "123456"};
    for (const auto& uri : uris) {
        AAFwk::Want want;
        nciNfccProxy->UpdateWantExtInfoByVendor(want, uri);
    }
    ASSERT_TRUE(nciNfccProxy != nullptr);
}
}
}
}