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

