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
#include "tagsessionstub_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "tag_session_stub.h"
#include "tag_session.h"
#include "nfc_sdk_common.h"
#include "nfc_service_fuzz.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;

    static constexpr const auto TAGSESSION_DESCRIPTOR = u"ohos.nfc.TAG.ITagSession";
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
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        uint32_t rfDiscId = timeOutArray[0];
        tagSession->OnRemoteRequest(rfDiscId);
    }

    void FuzzHandleTagLost(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        uint32_t rfDiscId = timeOutArray[0];
        tagSession->HandleTagLost(rfDiscId);
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

