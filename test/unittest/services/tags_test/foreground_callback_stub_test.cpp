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

#include "foreground_callback_stub.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::TAG;
class ForegroundCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ForegroundCallbackStubTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase ForegroundCallbackStubTest." << std::endl;
}

void ForegroundCallbackStubTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase ForegroundCallbackStubTest." << std::endl;
}

void ForegroundCallbackStubTest::SetUp()
{
    std::cout << " SetUp ForegroundCallbackStubTest." << std::endl;
}

void ForegroundCallbackStubTest::TearDown()
{
    std::cout << " TearDown ForegroundCallbackStubTest." << std::endl;
}

/**
 * @tc.name: RegForegroundDispatch001
 * @tc.desc: Test ForegroundCallbackStub RegForegroundDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(ForegroundCallbackStubTest, RegForegroundDispatch001, TestSize.Level1)
{
    const sptr<KITS::IForegroundCallback> callback;
    std::shared_ptr<ForegroundCallbackStub> foregroundCallbackStub = std::make_shared<ForegroundCallbackStub>();
    KITS::ErrorCode result = foregroundCallbackStub->RegForegroundDispatch(callback);
    ASSERT_TRUE(result == KITS::ERR_NFC_PARAMETERS);
}
/**
 * @tc.name: RegForegroundDispatch002
 * @tc.desc: Test ForegroundCallbackStub RegForegroundDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(ForegroundCallbackStubTest, RegForegroundDispatch002, TestSize.Level1)
{
    MessageParcel data;
    KITS::TagInfoParcelable* tagInfo = KITS::TagInfoParcelable::Unmarshalling(data);
    const sptr<KITS::IForegroundCallback> callback = new TAG::ForegroundCallbackStub();
    std::shared_ptr<ForegroundCallbackStub> foregroundCallbackStub = std::make_shared<ForegroundCallbackStub>();
    KITS::ErrorCode result = foregroundCallbackStub->RegForegroundDispatch(callback);
    delete tagInfo;
    tagInfo = nullptr;
    ASSERT_TRUE(result == KITS::ERR_NONE);
}

/**
 * @tc.name: RegForegroundDispatch003
 * @tc.desc: Test ForegroundCallbackStub RegForegroundDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(ForegroundCallbackStubTest, RegForegroundDispatch003, TestSize.Level1)
{
    const sptr<KITS::IForegroundCallback> callback;
    ForegroundCallbackStub* foregroundCallbackStub = ForegroundCallbackStub::GetInstance();
    KITS::ErrorCode result = foregroundCallbackStub->RegForegroundDispatch(callback);
    ASSERT_TRUE(result == KITS::ERR_NFC_PARAMETERS);
}
}
}
}
