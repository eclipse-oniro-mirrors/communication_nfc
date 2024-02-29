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

#include "hce_cmd_callback_stub.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class HceCmdCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void HceCmdCallbackStubTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase HceCmdCallbackStubTest." << std::endl;
}

void HceCmdCallbackStubTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase HceCmdCallbackStubTest." << std::endl;
}

void HceCmdCallbackStubTest::SetUp()
{
    std::cout << " SetUp HceCmdCallbackStubTest." << std::endl;
}

void HceCmdCallbackStubTest::TearDown()
{
    std::cout << " TearDown HceCmdCallbackStubTest." << std::endl;
}

/**
 * @tc.name: RegHceCmdCallback001
 * @tc.desc: Test HceCmdCallbackStubTest RegHceCmdCallback.
 * @tc.type: FUNC
 */
HWTEST_F(HceCmdCallbackStubTest, RegHceCmdCallback001, TestSize.Level1)
{
    const sptr<IHceCmdCallback> callback = nullptr;
    std::string type = "";
    HCE::HceCmdCallbackStub& hceCmdCallbackStub = HCE::HceCmdCallbackStub::GetInstance();
    KITS::ErrorCode regHceCmdCallback = hceCmdCallbackStub.RegHceCmdCallback(callback, type);
    ASSERT_TRUE(regHceCmdCallback == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Test HceCmdCallbackStubTest OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(HceCmdCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_ON_NOTIFY);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    HCE::HceCmdCallbackStub& hceCmdCallbackStub = HCE::HceCmdCallbackStub::GetInstance();
    int onRemoteRequest = hceCmdCallbackStub.OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(onRemoteRequest == ERR_NFC_PARAMETERS);
}
}
}
}