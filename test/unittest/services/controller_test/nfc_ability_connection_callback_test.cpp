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

#include "nfc_ability_connection_callback.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class NfcAbilityConnectionCallbackTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NfcAbilityConnectionCallbackTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcAbilityConnectionCallbackTest." << std::endl;
}

void NfcAbilityConnectionCallbackTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcAbilityConnectionCallbackTest." << std::endl;
}

void NfcAbilityConnectionCallbackTest::SetUp()
{
    std::cout << " SetUp NfcAbilityConnectionCallbackTest." << std::endl;
}

void NfcAbilityConnectionCallbackTest::TearDown()
{
    std::cout << " TearDown NfcAbilityConnectionCallbackTest." << std::endl;
}

/**
 * @tc.name: OnAbilityConnectDone001
 * @tc.desc: Test NfcAbilityConnectionCallbackTest OnAbilityConnectDone.
 * @tc.type: FUNC
 */
HWTEST_F(NfcAbilityConnectionCallbackTest, OnAbilityConnectDone001, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    sptr<IRemoteObject> remoteObject = nullptr;
    int resultCode = 0;
    std::shared_ptr<NfcAbilityConnectionCallback> nfcAbilityConnectionCallback =
        std::make_shared<NfcAbilityConnectionCallback>();
    nfcAbilityConnectionCallback->OnAbilityConnectDone(element, remoteObject, resultCode);
    bool serviceConnected = nfcAbilityConnectionCallback->ServiceConnected();
    ASSERT_TRUE(serviceConnected);
}

/**
 * @tc.name: OnAbilityDisconnectDone001
 * @tc.desc: Test NfcAbilityConnectionCallbackTest OnAbilityDisconnectDone.
 * @tc.type: FUNC
 */
HWTEST_F(NfcAbilityConnectionCallbackTest, OnAbilityDisconnectDone001, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    int resultCode = 0;
    std::shared_ptr<NfcAbilityConnectionCallback> nfcAbilityConnectionCallback =
        std::make_shared<NfcAbilityConnectionCallback>();
    nfcAbilityConnectionCallback->OnAbilityDisconnectDone(element, resultCode);
    bool serviceConnected = nfcAbilityConnectionCallback->ServiceConnected();
    ASSERT_TRUE(!serviceConnected);
}

/**
 * @tc.name: ServiceConnected001
 * @tc.desc: Test NfcAbilityConnectionCallbackTest ServiceConnected.
 * @tc.type: FUNC
 */
HWTEST_F(NfcAbilityConnectionCallbackTest, ServiceConnected001, TestSize.Level1)
{
    std::shared_ptr<NfcAbilityConnectionCallback> nfcAbilityConnectionCallback =
        std::make_shared<NfcAbilityConnectionCallback>();
    bool serviceConnected = nfcAbilityConnectionCallback->ServiceConnected();
    ASSERT_TRUE(!serviceConnected);
}

/**
 * @tc.name: SetHceManager001
 * @tc.desc: Test NfcAbilityConnectionCallbackTest SetHceManager.
 * @tc.type: FUNC
 */
HWTEST_F(NfcAbilityConnectionCallbackTest, SetHceManager001, TestSize.Level1)
{
    std::shared_ptr<HostCardEmulationManager> hceManager = nullptr;
    std::shared_ptr<NfcAbilityConnectionCallback> nfcAbilityConnectionCallback =
        std::make_shared<NfcAbilityConnectionCallback>();
    nfcAbilityConnectionCallback->SetHceManager(hceManager);
    bool serviceConnected = nfcAbilityConnectionCallback->ServiceConnected();
    ASSERT_TRUE(!serviceConnected);
}

/**
 * @tc.name: GetConnectedElement001
 * @tc.desc: Test NfcAbilityConnectionCallbackTest GetConnectedElement.
 * @tc.type: FUNC
 */
HWTEST_F(NfcAbilityConnectionCallbackTest, GetConnectedElement001, TestSize.Level1)
{
    std::shared_ptr<NfcAbilityConnectionCallback> nfcAbilityConnectionCallback =
        std::make_shared<NfcAbilityConnectionCallback>();
    nfcAbilityConnectionCallback->GetConnectedElement();
    bool serviceConnected = nfcAbilityConnectionCallback->ServiceConnected();
    ASSERT_TRUE(!serviceConnected);
}
}
}
}