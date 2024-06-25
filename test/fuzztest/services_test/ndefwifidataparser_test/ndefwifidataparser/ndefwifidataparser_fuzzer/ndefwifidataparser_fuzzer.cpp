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
#include "ndefwifidataparser_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "ndef_wifi_data_parser.h"
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

    void FuzzCheckWifiRecord(const uint8_t* data, size_t size)
    {
        std::string msg = std::string(reinterpret_cast<const char*>(data), size);
        std::shared_ptr<WifiData> wifiData = NdefWifiDataParser::CheckWifiRecord(msg);
    }

    void FuzzCheckWifiRecord1(const uint8_t* data, size_t size)
    {
        std::string msg = "";
        std::shared_ptr<WifiData> wifiData = NdefWifiDataParser::CheckWifiRecord(msg);
    }

    void FuzzCheckWifiRecord2(const uint8_t* data, size_t size)
    {
        std::string msg = "CheckWifiRecord";
        std::shared_ptr<WifiData> wifiData = NdefWifiDataParser::CheckWifiRecord(msg);
    }

    void FuzzCheckWifiRecord3(const uint8_t* data, size_t size)
    {
        std::string msg = "DA1736016170706C69636174696F6E2F766E642E7766612E77736331100E0032"
                          "10260001011045000741646143393239100300020020100F0002000110270008"
                          "383838383838383810200006FFFFFFFFFFFF";
        std::shared_ptr<WifiData> wifiData = NdefWifiDataParser::CheckWifiRecord(msg);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzCheckWifiRecord(data, size);
    OHOS::FuzzCheckWifiRecord1(data, size);
    OHOS::FuzzCheckWifiRecord2(data, size);
    OHOS::FuzzCheckWifiRecord3(data, size);
    return 0;
}

