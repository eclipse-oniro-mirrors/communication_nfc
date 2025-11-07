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
    std::shared_ptr<NCI::INciNfccInterface> testNfccInterface = nullptr;
    std::weak_ptr<NfcService> nfcService;
    TAG::NdefHarDataParser::GetInstance().Initialize(nfcService, testPtr, testNfccInterface);
    std::shared_ptr<KITS::TagInfo> tagInfo = nullptr;

    std::string ndefMessage = "";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "DA060F01";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D400023231";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D40F00616E64726F69642E636F6D3A706B67";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D40F01616E64726F69642E636F6D3A706B6763";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D40F16616E64726F69642E636F6D3A706B67636F6D2E6875617765692E686D6F732E6865616C7468";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D100023132";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D1010055";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D1010A550262616964752E636F6D";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D10216537091010A550162616964752E636F6D51010451027A6861";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D101015520";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D10102550068";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D101065500736D733A31";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D101095506314071712E636F6D";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);

    ndefMessage = "D101045402656E31";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D20A02746578742F76636172642021";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D20301612F6231";
    TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ndefMessage = "D2000131";
    uint16_t ret = TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ASSERT_TRUE(ret == TAG::DISPATCH_UNKNOWN);
}

HWTEST_F(NdefHarDataParserTest, GetNdefHarDataParserTest002, TestSize.Level1)
{
    std::shared_ptr<NCI::INciTagInterface> testPtr = nullptr;
    std::shared_ptr<NCI::INciNfccInterface> testNfccInterface = nullptr;
    std::weak_ptr<NfcService> nfcService;
    TAG::NdefHarDataParser::GetInstance().Initialize(nfcService, testPtr, testNfccInterface);
    std::shared_ptr<KITS::TagInfo> tagInfo = nullptr;

    // Alipay
    std::string ndefMessage =
        "910168550472656E6465722E616C697061792E636F6D2F702F732F756C696E6B2F6463303F733D646326736368656D653D616C69706179"
        "2533412532462532466E666325324661707025334669642533443230303032313533253236742533446E61303061723278366A3039140F"
        "1B616E64726F69642E636F6D3A706B67636F6D2E65672E616E64726F69642E416C697061794770686F6E65540C186F686F732E636F6D3A"
        "706B67636F6D2E616C697061792E6D6F62696C652E636C69656E74";
    uint16_t ret = TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ASSERT_TRUE(ret == TAG::DISPATCH_UNKNOWN);
}

HWTEST_F(NdefHarDataParserTest, GetNdefHarDataParserTest003, TestSize.Level1)
{
    std::shared_ptr<NCI::INciTagInterface> testPtr = nullptr;
    std::shared_ptr<NCI::INciNfccInterface> testNfccInterface = nullptr;
    std::weak_ptr<NfcService> nfcService;
    TAG::NdefHarDataParser::GetInstance().Initialize(nfcService, testPtr, testNfccInterface);
    std::shared_ptr<KITS::TagInfo> tagInfo = nullptr;

    // China Merchants Bank's Palm Life
    std::string ndefMessage = "D101A855046F70656E2E636D626368696E612E636F6D2F64697370617463682F676F3F75726C3D7765622676"
                              "657273696F6E3D7632266E6578743D68747470732533412532462532467069616F2E6F326F2E636D62636869"
                              "6E612E636F6D253246636D626C6966655F66616E7069616F25324673746F726544657461696C253346737472"
                              "4E6F25334430353132303336373330303030323526646565706C696E6B49643D3230323431303131";
    uint16_t ret = TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ASSERT_TRUE(ret == TAG::DISPATCH_UNKNOWN);
}

HWTEST_F(NdefHarDataParserTest, GetNdefHarDataParserTest004, TestSize.Level1)
{
    std::shared_ptr<NCI::INciTagInterface> testPtr = nullptr;
    std::shared_ptr<NCI::INciNfccInterface> testNfccInterface = nullptr;
    std::weak_ptr<NfcService> nfcService;
    TAG::NdefHarDataParser::GetInstance().Initialize(nfcService, testPtr, testNfccInterface);
    std::shared_ptr<KITS::TagInfo> tagInfo = nullptr;

    // scheme is wallet
    std::string ndefMessage =
        "D10157550077616C6C65743A2F2F636F6D2E6875617765692E77616C6C65742F6F70656E77616C6C65743F616374696F6E3D636F6D2E68"
        "75617765692E7061792E696E74656E742E616374696F6E2E4D41494E4143544956495459";
    uint16_t ret = TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ASSERT_TRUE(ret == TAG::DISPATCH_UNKNOWN);
}

HWTEST_F(NdefHarDataParserTest, GetNdefHarDataParserTest005, TestSize.Level1)
{
    std::shared_ptr<NCI::INciTagInterface> testPtr = nullptr;
    std::shared_ptr<NCI::INciNfccInterface> testNfccInterface = nullptr;
    std::weak_ptr<NfcService> nfcService;
    TAG::NdefHarDataParser::GetInstance().Initialize(nfcService, testPtr, testNfccInterface);
    std::shared_ptr<KITS::TagInfo> tagInfo = nullptr;

    // mime type
    std::string ndefMessage =
        "D20F576170702F68772E66776C2E696E666F56323A533D4857384B5F5445535447325F35473B503D687561776569323031323B483D3137"
        "322E31362E33322E32323B493D343B43543D31303B49414F3D5555AAAA124C060001E0000000000000000000000000000000";
    uint16_t ret = TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ASSERT_TRUE(ret == TAG::DISPATCH_UNKNOWN);
}

HWTEST_F(NdefHarDataParserTest, GetNdefHarDataParserTest006, TestSize.Level1)
{
    std::shared_ptr<NCI::INciTagInterface> testPtr = nullptr;
    std::shared_ptr<NCI::INciNfccInterface> testNfccInterface = nullptr;
    std::weak_ptr<NfcService> nfcService;
    TAG::NdefHarDataParser::GetInstance().Initialize(nfcService, testPtr, testNfccInterface);
    std::shared_ptr<KITS::TagInfo> tagInfo = nullptr;

    // multi-records mime type
    std::string ndefMessage =
        "91014E5402656E3034324336323532414135323830783030303042413030303230323031303233313930333868747470733A2F2F71722E"
        "606373772E636E2F6C6373772F77787061792F3130393637313135521A056170706C69636174696F6E2F636F6D2E7461672E7461677061"
        "7948656C6C6F";
    uint16_t ret = TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ASSERT_TRUE(ret != TAG::DISPATCH_UNKNOWN);
}

HWTEST_F(NdefHarDataParserTest, GetNdefHarDataParserTest007, TestSize.Level1)
{
    std::shared_ptr<NCI::INciTagInterface> testPtr = nullptr;
    std::shared_ptr<NCI::INciNfccInterface> testNfccInterface = nullptr;
    std::weak_ptr<NfcService> nfcService;
    TAG::NdefHarDataParser::GetInstance().Initialize(nfcService, testPtr, testNfccInterface);
    std::shared_ptr<KITS::TagInfo> tagInfo = nullptr;

    // v-card
    std::string ndefMessage =
        "D20A47746578742F7663617264424547494E3A56434152440A56455253494F4E3A332E300A464E3AE5B7ABE994900A4F52473A636F6D70"
        "616E790A54454C3A31383630303234303330320A454E443A5643415244";
    uint16_t ret = TAG::NdefHarDataParser::GetInstance().TryNdef(ndefMessage, tagInfo);
    ASSERT_TRUE(ret != TAG::DISPATCH_UNKNOWN);
}
}
}
}
