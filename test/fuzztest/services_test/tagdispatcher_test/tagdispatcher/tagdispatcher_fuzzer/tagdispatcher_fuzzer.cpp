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
#include "tagdispatcher_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include "tag_dispatcher.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;

    constexpr const auto FUZZER_THRESHOLD = 4;

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    void FuzzHandleTagFound(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        tagDispatcher->HandleTagFound(timeOutArray[0]);
    }

    void FuzzHandleTagLost(const uint8_t* data, size_t size)
    {
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        std::shared_ptr<NFC::TAG::TagDispatcher> tagDispatcher = std::make_shared<NFC::TAG::TagDispatcher>(service);
        tagDispatcher->HandleTagFound(timeOutArray[0]);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzHandleTagFound(data, size);
    OHOS::FuzzHandleTagLost(data, size);

    return 0;
}

