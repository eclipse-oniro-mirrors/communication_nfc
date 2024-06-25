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
#include "ndefbtdataparser_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "ndef_bt_data_parser.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;
    using namespace OHOS::NFC::TAG;

    constexpr const auto FUZZER_THRESHOLD = 4;

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    void FuzzCheckBtRecord(const uint8_t* data, size_t size)
    {
        std::string msg = std::string(reinterpret_cast<const char*>(data), size);
        std::shared_ptr<BtData> btData = NdefBtDataParser::CheckBtRecord(msg);
    }

    void FuzzCheckBtRecord1(const uint8_t* data, size_t size)
    {
        std::string msg = "";
        std::shared_ptr<BtData> btData = NdefBtDataParser::CheckBtRecord(msg);
    }

    void FuzzCheckBtRecord2(const uint8_t* data, size_t size)
    {
        std::string msg = "CheckbtRecord";
        std::shared_ptr<BtData> btData = NdefBtDataParser::CheckBtRecord(msg);
    }

    void FuzzCheckBtRecord3(const uint8_t* data, size_t size)
    {
        std::string msg = "D220566170706C69636174696F6E2F766E642E626C7565746F6F74682E65"
                          "702E6F6F625600BE17010E7F04050949435341040D14042C0B030B110C11"
                          "0E111E11001236FF027D0320010240005A45303031810800113000190103"
                          "021901010101020306047F0E0117BE020E52726364687A5238363739393532";
        std::shared_ptr<BtData> btData = NdefBtDataParser::CheckBtRecord(msg);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzCheckBtRecord(data, size);
    OHOS::FuzzCheckBtRecord1(data, size);
    OHOS::FuzzCheckBtRecord2(data, size);
    OHOS::FuzzCheckBtRecord3(data, size);
    return 0;
}

