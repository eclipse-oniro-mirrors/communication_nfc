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
#include "ndef_har_data_parser.h"
#include "ndef_har_dispatch.h"
#include "nfc_sdk_common.h"
#include "uri.h"
#include "loghelper.h"
#include "nfc_hisysevent.h"
#include "external_deps_proxy.h"
#include "taginfo.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class NdefHarDataParserTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
};

void NdefHarDataParserTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NdefHarDataParserTest." << std::endl;
}

void NdefHarDataParserTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NdefHarDataParserTest." << std::endl;
}

void NdefHarDataParserTest::SetUp()
{
    std::cout << " SetUp NdefHarDataParserTest." << std::endl;
}

void NdefHarDataParserTest::TearDown()
{
    std::cout << " TearDown NdefHarDataParserTest." << std::endl;
}

/**
 * @tc.name: GetNdefHarDataParserTest001
 * @tc.desc: Test NdefHarDataParserTest GetNdefHarDataParserTest.
 * @tc.type: FUNC
 */
HWTEST_F(NdefHarDataParserTest, GetNdefHarDataParserTest001, TestSize.Level1)
{
    std::shared_ptr<NCI::INciTagInterface> testPtr = nullptr;
    std::shared_ptr<TAG::NdefHarDataParser> ndefHarDataParserTest = std::make_shared<TAG::NdefHarDataParser>(testPtr);
    std::shared_ptr<KITS::TagInfo> tagInfo = nullptr;

    std::string ndefMessage = "";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "DA060F01";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D400023231";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D40F00616E64726F69642E636F6D3A706B67";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D40F01616E64726F69642E636F6D3A706B6763";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D40F16616E64726F69642E636F6D3A706B67636F6D2E6875617765692E686D6F732E6865616C7468";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D100023132";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D1010055";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D1010A550262616964752E636F6D";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D10216537091010A550162616964752E636F6D51010451027A6861";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D101015520";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D10102550068";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D101065500736D733A31";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D101095506314071712E636F6D";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);

    ndefMessage = "D101045402656E31";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D20A02746578742F76636172642021";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D20301612F6231";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D2000131";
    ndefHarDataParserTest->TryNdef(ndefMessage, tagInfo);
    ASSERT_TRUE(ndefHarDataParserTest != nullptr);
}

}
}
}
