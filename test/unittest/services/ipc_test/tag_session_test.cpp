/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "tag_session.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class TagSessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void TagSessionTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase TagSessionTest." << std::endl;
}

void TagSessionTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase TagSessionTest." << std::endl;
}

void TagSessionTest::SetUp()
{
    std::cout << " SetUp TagSessionTest." << std::endl;
}

void TagSessionTest::TearDown()
{
    std::cout << " TearDown TagSessionTest." << std::endl;
}

/**
 * @tc.name: FormatNdef001
 * @tc.desc: Test TagSessionTest FormatNdef.
 * @tc.type: FUNC
 */
HWTEST_F(TagSessionTest, FormatNdef001, TestSize.Level1)
{
    std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
    std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
    tagSession->FormatNdef(0, "");
    ASSERT_TRUE(tagSession != nullptr);
}
} // namespace TEST
} // namespace NFC
} // namespace OHOS