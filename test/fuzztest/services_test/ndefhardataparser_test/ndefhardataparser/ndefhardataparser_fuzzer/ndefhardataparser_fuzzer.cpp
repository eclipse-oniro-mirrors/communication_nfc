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
        std::shared_ptr<NdefHarDataParser> ndefHarDataParser = std::make_shared<NdefHarDataParser>(testPtr);
        std::string msg = "";
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        ndefHarDataParser->TryNdef(msg, tagInfo);
    }

    void FuzzTryNdef2(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> testPtr;
        std::shared_ptr<NdefHarDataParser> ndefHarDataParser = std::make_shared<NdefHarDataParser>(testPtr);
        std::string msg = "DA060F01";
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        ndefHarDataParser->TryNdef(msg, tagInfo);
    }

    void FuzzTryNdef3(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> testPtr;
        std::shared_ptr<NdefHarDataParser> ndefHarDataParser = std::make_shared<NdefHarDataParser>(testPtr);
        std::string msg = "D40F00616E64726F69642E636F6D3A706B67";
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        ndefHarDataParser->TryNdef(msg, tagInfo);
    }

    void FuzzTryNdef4(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> testPtr;
        std::shared_ptr<NdefHarDataParser> ndefHarDataParser = std::make_shared<NdefHarDataParser>(testPtr);
        std::string msg = "D100023132";
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        ndefHarDataParser->TryNdef(msg, tagInfo);
    }

    void FuzzTryNdef5(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> testPtr;
        std::shared_ptr<NdefHarDataParser> ndefHarDataParser = std::make_shared<NdefHarDataParser>(testPtr);
        std::string msg = "D1010055";
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        ndefHarDataParser->TryNdef(msg, tagInfo);
    }

    void FuzzTryNdef6(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> testPtr;
        std::shared_ptr<NdefHarDataParser> ndefHarDataParser = std::make_shared<NdefHarDataParser>(testPtr);
        std::string msg = "D1010A550262616964752E636F6D";
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        ndefHarDataParser->TryNdef(msg, tagInfo);
    }

    void FuzzTryNdef7(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> testPtr;
        std::shared_ptr<NdefHarDataParser> ndefHarDataParser = std::make_shared<NdefHarDataParser>(testPtr);
        std::string msg = "D10216537091010A550162616964752E636F6D51010451027A6861";
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        ndefHarDataParser->TryNdef(msg, tagInfo);
    }

    void FuzzTryNdef8(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> testPtr;
        std::shared_ptr<NdefHarDataParser> ndefHarDataParser = std::make_shared<NdefHarDataParser>(testPtr);
        std::string msg = "D101015520";
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        ndefHarDataParser->TryNdef(msg, tagInfo);
    }

    void FuzzTryNdef9(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> testPtr;
        std::shared_ptr<NdefHarDataParser> ndefHarDataParser = std::make_shared<NdefHarDataParser>(testPtr);
        std::string msg = "D10102550068";
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        ndefHarDataParser->TryNdef(msg, tagInfo);
    }

    void FuzzTryNdef10(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> testPtr;
        std::shared_ptr<NdefHarDataParser> ndefHarDataParser = std::make_shared<NdefHarDataParser>(testPtr);
        std::string msg = "D101065500736D733A31";
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        ndefHarDataParser->TryNdef(msg, tagInfo);
    }

    void FuzzTryNdef11(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> testPtr;
        std::shared_ptr<NdefHarDataParser> ndefHarDataParser = std::make_shared<NdefHarDataParser>(testPtr);
        std::string msg = "D101095506314071712E636F6D";
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        ndefHarDataParser->TryNdef(msg, tagInfo);
    }

    void FuzzTryNdef12(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> testPtr;
        std::shared_ptr<NdefHarDataParser> ndefHarDataParser = std::make_shared<NdefHarDataParser>(testPtr);
        std::string msg = "D101045402656E31";
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        ndefHarDataParser->TryNdef(msg, tagInfo);
    }

    void FuzzTryNdef13(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> testPtr;
        std::shared_ptr<NdefHarDataParser> ndefHarDataParser = std::make_shared<NdefHarDataParser>(testPtr);
        std::string msg = "D20A02746578742F76636172642021";
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        ndefHarDataParser->TryNdef(msg, tagInfo);
    }

    void FuzzTryNdef14(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> testPtr;
        std::shared_ptr<NdefHarDataParser> ndefHarDataParser = std::make_shared<NdefHarDataParser>(testPtr);
        std::string msg = "D20301612F6231";
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        ndefHarDataParser->TryNdef(msg, tagInfo);
    }

    void FuzzTryNdef15(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> testPtr;
        std::shared_ptr<NdefHarDataParser> ndefHarDataParser = std::make_shared<NdefHarDataParser>(testPtr);
        std::string msg = "D2000131";
        std::shared_ptr<TagInfo> tagInfo = nullptr;
        ndefHarDataParser->TryNdef(msg, tagInfo);
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
    OHOS::FuzzTryNdef2(data, size);
    OHOS::FuzzTryNdef3(data, size);
    OHOS::FuzzTryNdef4(data, size);
    OHOS::FuzzTryNdef5(data, size);
    OHOS::FuzzTryNdef6(data, size);
    OHOS::FuzzTryNdef7(data, size);
    OHOS::FuzzTryNdef8(data, size);
    OHOS::FuzzTryNdef9(data, size);
    OHOS::FuzzTryNdef10(data, size);
    OHOS::FuzzTryNdef11(data, size);
    OHOS::FuzzTryNdef12(data, size);
    OHOS::FuzzTryNdef13(data, size);
    OHOS::FuzzTryNdef14(data, size);
    OHOS::FuzzTryNdef15(data, size);
    return 0;
}

