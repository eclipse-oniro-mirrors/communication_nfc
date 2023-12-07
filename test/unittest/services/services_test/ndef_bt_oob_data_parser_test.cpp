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

#include "ndef_bt_oob_data_parser.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class NdefBtOobDataParserTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NdefBtOobDataParserTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NdefBtOobDataParserTest." << std::endl;
}

void NdefBtOobDataParserTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NdefBtOobDataParserTest." << std::endl;
}

void NdefBtOobDataParserTest::SetUp()
{
    std::cout << " SetUp NdefBtOobDataParserTest." << std::endl;
}

void NdefBtOobDataParserTest::TearDown()
{
    std::cout << " TearDown NdefBtOobDataParserTest." << std::endl;
}

/**
 * @tc.name: CheckBtRecord001
 * @tc.desc: Test NdefBtOobDataParserTest CheckBtRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefBtOobDataParserTest, CheckBtRecord001, TestSize.Level1)
{
    std::string msg = "";
    std::shared_ptr<NdefBtOobDataParser> ndefBtOobDataParser = std::make_shared<NdefBtOobDataParser>();
    std::shared_ptr<BtOobData> btOobData = ndefBtOobDataParser->CheckBtRecord(msg);
    ASSERT_TRUE(btOobData != nullptr);
}

/**
 * @tc.name: CheckBtRecord002
 * @tc.desc: Test NdefBtOobDataParserTest CheckBtRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefBtOobDataParserTest, CheckBtRecord002, TestSize.Level1)
{
    std::string msg = "CheckBtRecord";
    std::shared_ptr<NdefBtOobDataParser> ndefBtOobDataParser = std::make_shared<NdefBtOobDataParser>();
    std::shared_ptr<BtOobData> btOobData = ndefBtOobDataParser->CheckBtRecord(msg);
    ASSERT_TRUE(btOobData != nullptr);
}
}
}
}