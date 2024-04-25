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

#include "reader_mode_callback_stub.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::TAG;
class ReaderModeCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ReaderModeCallbackStubTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase ReaderModeCallbackStubTest." << std::endl;
}

void ReaderModeCallbackStubTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase ReaderModeCallbackStubTest." << std::endl;
}

void ReaderModeCallbackStubTest::SetUp()
{
    std::cout << " SetUp ReaderModeCallbackStubTest." << std::endl;
}

void ReaderModeCallbackStubTest::TearDown()
{
    std::cout << " TearDown ReaderModeCallbackStubTest." << std::endl;
}

/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Test ReaderModeCallbackStubTest OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(ReaderModeCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::shared_ptr<ReaderModeCallbackStub> readerModeCallbackStub = std::make_shared<ReaderModeCallbackStub>();
    int onRemoteRequest = readerModeCallbackStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(onRemoteRequest == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: RegReaderMode001
 * @tc.desc: Test ReaderModeCallbackStubTest RegReaderMode.
 * @tc.type: FUNC
 */
HWTEST_F(ReaderModeCallbackStubTest, RegReaderMode001, TestSize.Level1)
{
    sptr<KITS::IReaderModeCallback> callback = nullptr;
    std::shared_ptr<ReaderModeCallbackStub> readerModeCallbackStub = std::make_shared<ReaderModeCallbackStub>();
    KITS::ErrorCode errorCode = readerModeCallbackStub->RegReaderMode(callback);
    ASSERT_TRUE(errorCode == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: RegReaderMode002
 * @tc.desc: Test ReaderModeCallbackStubTest RegReaderMode.
 * @tc.type: FUNC
 */
HWTEST_F(ReaderModeCallbackStubTest, RegReaderMode002, TestSize.Level1)
{
    sptr<KITS::IReaderModeCallback> callback = nullptr;
    ReaderModeCallbackStub* readerModeCallbackStub = ReaderModeCallbackStub::GetInstance();
    KITS::ErrorCode errorCode = readerModeCallbackStub->RegReaderMode(callback);
    ASSERT_TRUE(errorCode == KITS::ERR_NFC_PARAMETERS);
}

/**
 * @tc.name: RegReaderMode003
 * @tc.desc: Test ReaderModeCallbackStubTest RegReaderMode.
 * @tc.type: FUNC
 */
HWTEST_F(ReaderModeCallbackStubTest, RegReaderMode003, TestSize.Level1)
{
    sptr<KITS::IReaderModeCallback> callback = new TAG::ReaderModeCallbackStub();
    std::shared_ptr<ReaderModeCallbackStub> readerModeCallbackStub = std::make_shared<ReaderModeCallbackStub>();
    KITS::ErrorCode errorCode = readerModeCallbackStub->RegReaderMode(callback);
    ASSERT_TRUE(errorCode == KITS::ERR_NONE);
}
}
}
}