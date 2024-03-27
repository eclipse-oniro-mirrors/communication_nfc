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

#include "query_app_info_callback_stub.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class QueryAppInfoCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void QueryAppInfoCallbackStubTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase QueryAppInfoCallbackStubTest." << std::endl;
}

void QueryAppInfoCallbackStubTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase QueryAppInfoCallbackStubTest." << std::endl;
}

void QueryAppInfoCallbackStubTest::SetUp()
{
    std::cout << " SetUp QueryAppInfoCallbackStubTest." << std::endl;
}

void QueryAppInfoCallbackStubTest::TearDown()
{
    std::cout << " TearDown QueryAppInfoCallbackStubTest." << std::endl;
}

#ifdef VENDOR_APPLICATIONS_ENABLED
/**
 * @tc.name: OnRemoteRequest001
 * @tc.desc: Test QueryAppInfoCallbackStubTest OnRemoteRequest.
 * @tc.type: FUNC
 */
HWTEST_F(QueryAppInfoCallbackStubTest, OnRemoteRequest001, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::shared_ptr<QueryAppInfoCallbackStub> queryAppInfoCallbackStub = std::make_shared<QueryAppInfoCallbackStub>();
    int onRemoteRequest = queryAppInfoCallbackStub->OnRemoteRequest(code, data, reply, option);
    ASSERT_TRUE(onRemoteRequest == KITS::ERR_NFC_PARAMETERS);
}
#endif
}
}
}