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

#include "nfc_controller_death_recipient.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class NfcControllerDeathRecipientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NfcControllerDeathRecipientTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcControllerDeathRecipientTest." << std::endl;
}

void NfcControllerDeathRecipientTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcControllerDeathRecipientTest." << std::endl;
}

void NfcControllerDeathRecipientTest::SetUp()
{
    std::cout << " SetUp NfcControllerDeathRecipientTest." << std::endl;
}

void NfcControllerDeathRecipientTest::TearDown()
{
    std::cout << " TearDown NfcControllerDeathRecipientTest." << std::endl;
}

/**
 * @tc.name: OnRemoteDied001
 * @tc.desc: Test NfcControllerDeathRecipientTest OnRemoteDied.
 * @tc.type: FUNC
 */
HWTEST_F(NfcControllerDeathRecipientTest, OnRemoteDied001, TestSize.Level1)
{
    sptr<NfcControllerStub> nfcConctrolService = nullptr;
    Security::AccessToken::AccessTokenID callerToken = 0;
    wptr<IRemoteObject> remote = nullptr;
    std::shared_ptr<NfcControllerDeathRecipient> nfcControllerDeathRecipient =
        std::make_shared<NfcControllerDeathRecipient>(nfcConctrolService, callerToken);
    nfcControllerDeathRecipient->OnRemoteDied(remote);
}
}
}
}