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
    bool deinitialize = nciNfccProxy->Deinitialize();
    ASSERT_TRUE(deinitialize == false);
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
    int getNciVersion = nciNfccProxy->GetNciVersion();
    ASSERT_TRUE(getNciVersion == 0);
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
    int getNciVersion = nciNfccProxy->GetNciVersion();
    ASSERT_TRUE(getNciVersion == 0);
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
    bool setScreenStatus = nciNfccProxy->SetScreenStatus(screenStateMask);
    ASSERT_TRUE(setScreenStatus == true);
}

/**
 * @tc.name: GetNciVersion001
 * @tc.desc: Test NciNfccProxyTest GetNciVersion.
 * @tc.type: FUNC
 */
HWTEST_F(NciNfccProxyTest, GetNciVersion001, TestSize.Level1)
{
    std::shared_ptr<NciNfccProxy> nciNfccProxy = std::make_shared<NciNfccProxy>();
    int getNciVersion = nciNfccProxy->GetNciVersion();
    ASSERT_TRUE(getNciVersion == 0);
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
    int getNciVersion = nciNfccProxy->GetNciVersion();
    ASSERT_TRUE(getNciVersion == 0);
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
    int getNciVersion = nciNfccProxy->GetNciVersion();
    ASSERT_TRUE(getNciVersion == 0);
}
}
}
}