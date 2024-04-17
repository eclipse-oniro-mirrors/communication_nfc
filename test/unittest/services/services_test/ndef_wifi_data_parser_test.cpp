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

#include "ndef_wifi_data_parser.h"

namespace OHOS {
namespace NFC {
namespace TAG {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class NdefWifiDataParserTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NdefWifiDataParserTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NdefWifiDataParserTest." << std::endl;
}

void NdefWifiDataParserTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NdefWifiDataParserTest." << std::endl;
}

void NdefWifiDataParserTest::SetUp()
{
    std::cout << " SetUp NdefWifiDataParserTest." << std::endl;
}

void NdefWifiDataParserTest::TearDown()
{
    std::cout << " TearDown NdefWifiDataParserTest." << std::endl;
}

/**
 * @tc.name: CheckWifiRecord001
 * @tc.desc: Test NdefWifiDataParserTest CheckWifiRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefWifiDataParserTest, CheckWifiRecord001, TestSize.Level1)
{
    std::string msg = "";
    std::shared_ptr<WifiData> wifiData = NdefWifiDataParser::CheckWifiRecord(msg);
    ASSERT_TRUE(wifiData != nullptr);
}

/**
 * @tc.name: CheckWifiRecord002
 * @tc.desc: Test NdefWifiDataParserTest CheckWifiRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefWifiDataParserTest, CheckWifiRecord002, TestSize.Level1)
{
    std::string msg = "CheckWifiRecord";
    std::shared_ptr<WifiData> wifiData = NdefWifiDataParser::CheckWifiRecord(msg);
    ASSERT_TRUE(wifiData != nullptr);
}

/**
 * @tc.name: CheckWifiRecord003
 * @tc.desc: Test NdefWifiDataParserTest CheckWifiRecord.
 * @tc.type: FUNC
 */
HWTEST_F(NdefWifiDataParserTest, CheckWifiRecord003, TestSize.Level1)
{
    std::string msg = "DA1736016170706C69636174696F6E2F766E642E7766612E77736331100E0032"
                      "10260001011045000741646143393239100300020020100F0002000110270008"
                      "383838383838383810200006FFFFFFFFFFFF";
    std::shared_ptr<WifiData> wifiData = NdefWifiDataParser::CheckWifiRecord(msg);
    ASSERT_TRUE(wifiData != nullptr);
}
} // namespace TEST
} // namespace TAG
} // namespace NFC
} // namespace OHOS