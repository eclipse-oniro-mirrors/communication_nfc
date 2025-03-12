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
#include "ndefhardataparser_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "ndef_har_data_parser.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;
    using namespace OHOS::NFC::TAG;
    using namespace OHOS::NFC::NCI;

    constexpr const auto FUZZER_THRESHOLD = 4;
    const std::string msg1 =
        "910168550472656E6465722E616C697061792E636F6D2F702F732F756C696E6B2F6463303F733D646326736368656D653D616C69706179"
        "2533412532462532466E666325324661707025334669642533443230303032313533253236742533446E61303061723278366A3039140F"
        "1B616E64726F69642E636F6D3A706B67636F6D2E65672E616E64726F69642E416C697061794770686F6E65540C186F686F732E636F6D3A"
        "706B67636F6D2E616C697061792E6D6F62696C652E636C69656E74";
    const std::string msg2 =
        "D10157550077616C6C65743A2F2F636F6D2E6875617765692E77616C6C65742F6F70656E77616C6C65743F616374696F6E3D636F6D2E68"
        "75617765692E7061792E696E74656E742E616374696F6E2E4D41494E4143544956495459";
    const std::string msg3 =
        "D20F576170702F68772E66776C2E696E666F56323A533D4857384B5F5445535447325F35473B503D687561776569323031323B483D3137"
        "322E31362E33322E32323B493D343B43543D31303B49414F3D5555AAAA124C060001E0000000000000000000000000000000";

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    void FuzzTryNdef1(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> testPtr;
        std::weak_ptr<NFC::NCI::INciNfccInterface> testNfccInterface;
        std::weak_ptr<NFC::NfcService> nfcService;
        NdefHarDataParser::GetInstance().Initialize(nfcService, testPtr, testNfccInterface);
        std::string msg = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        NdefHarDataParser::GetInstance().TryNdef(msg, tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("DA060F01", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("D40F00616E64726F69642E636F6D3A706B67", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("D100023132", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("D1010055", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("D1010A550262616964752E636F6D", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef(
            "D10216537091010A550162616964752E636F6D51010451027A6861", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("D101015520", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("D10102550068", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("D101065500736D733A31", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("D101095506314071712E636F6D", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("D101045402656E31", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("D20A02746578742F76636172642021", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("D20301612F6231", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("D2000131", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("D400023231", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef("D40F01616E64726F69642E636F6D3A706B6763", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef(
            "D40F16616E64726F69642E636F6D3A706B67636F6D2E6875617765692E686D6F732E6865616C7468", tagInfo);
        NdefHarDataParser::GetInstance().TryNdef(msg1, tagInfo);
        msg =
            "D101A855046F70656E2E636D626368696E612E636F6D2F64697370617463682F676F3F75726C3D7765622676"
            "657273696F6E3D7632266E6578743D68747470732533412532462532467069616F2E6F326F2E636D62636869"
            "6E612E636F6D253246636D626C6966655F66616E7069616F25324673746F726544657461696C253346737472"
            "4E6F25334430353132303336373330303030323526646565706C696E6B49643D3230323431303131";
        NdefHarDataParser::GetInstance().TryNdef(msg, tagInfo);
        NdefHarDataParser::GetInstance().TryNdef(msg2, tagInfo);
        NdefHarDataParser::GetInstance().TryNdef(msg3, tagInfo);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzTryNdef1(data, size);
    return 0;
}