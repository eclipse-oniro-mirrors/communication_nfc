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
#include "nfcsdkcommon_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "nfc_sdk_common.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;

    constexpr const auto FUZZER_THRESHOLD = 4;
    constexpr const auto INT_TO_BOOL_DIVISOR = 2;

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    void FuzzIsLittleEndian(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcSdkCommon> nfcSdkCommon = std::make_shared<NfcSdkCommon>();
        nfcSdkCommon->IsLittleEndian();
    }

    void FuzzStringToInt(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcSdkCommon> nfcSdkCommon = std::make_shared<NfcSdkCommon>();
        std::string src = NfcSdkCommon::BytesVecToHexString(data, size);
        bool bLittleEndian = data[0] % INT_TO_BOOL_DIVISOR;
        nfcSdkCommon->StringToInt(src, bLittleEndian);
    }

    void FuzzHexStringToAsciiString(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcSdkCommon> nfcSdkCommon = std::make_shared<NfcSdkCommon>();
        std::string src = NfcSdkCommon::BytesVecToHexString(data, size);
        nfcSdkCommon->HexStringToAsciiString(src);
    }

    void FuzzGetCurrentTime(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcSdkCommon> nfcSdkCommon = std::make_shared<NfcSdkCommon>();
        nfcSdkCommon->GetCurrentTime();
    }

    void FuzzGetRelativeTime(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcSdkCommon> nfcSdkCommon = std::make_shared<NfcSdkCommon>();
        nfcSdkCommon->GetRelativeTime();
    }

    void FuzzCodeMiddlePart(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcSdkCommon> nfcSdkCommon = std::make_shared<NfcSdkCommon>();
        std::string src = NfcSdkCommon::BytesVecToHexString(data, size);
        nfcSdkCommon->CodeMiddlePart(src);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzIsLittleEndian(data, size);
    OHOS::FuzzStringToInt(data, size);
    OHOS::FuzzHexStringToAsciiString(data, size);
    OHOS::FuzzGetCurrentTime(data, size);
    OHOS::FuzzGetRelativeTime(data, size);
    OHOS::FuzzCodeMiddlePart(data, size);
    return 0;
}

