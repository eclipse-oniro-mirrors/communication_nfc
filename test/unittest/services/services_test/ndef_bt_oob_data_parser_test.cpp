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

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class NdefBtDataParserTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NdefBtDataParserTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NdefBtDataParserTest." << std::endl;
}

void NdefBtDataParserTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NdefBtDataParserTest." << std::endl;
}

void NdefBtDataParserTest::SetUp()
{
    std::cout << " SetUp NdefBtDataParserTest." << std::endl;
}

void NdefBtDataParserTest::TearDown()
{
    std::cout << " TearDown NdefBtDataParserTest." << std::endl;
}

/**
 * @tc.name: CheckBtRecord001
 * @tc.desc: Test NdefBtDataParserTest CheckBtRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefBtDataParserTest, CheckBtRecord001, TestSize.Level1)
{
    std::string msg = "";
    std::shared_ptr<NdefBtDataParser> ndefBtDataParser = std::make_shared<NdefBtDataParser>();
    std::shared_ptr<BtData> BtData = ndefBtDataParser->CheckBtRecord(msg);
    ASSERT_TRUE(BtData != nullptr);
}

/**
 * @tc.name: CheckBtRecord002
 * @tc.desc: Test NdefBtDataParserTest CheckBtRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefBtDataParserTest, CheckBtRecord002, TestSize.Level1)
{
    std::string msg = "CheckBtRecord";
    std::shared_ptr<NdefBtDataParser> ndefBtDataParser = std::make_shared<NdefBtDataParser>();
    std::shared_ptr<BtData> BtData = ndefBtDataParser->CheckBtRecord(msg);
    ASSERT_TRUE(BtData != nullptr);
}
}
}
}