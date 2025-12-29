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
#include "nfcabilityconnectioncallback_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "nfc_ability_connection_callback.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include <securec.h>

namespace OHOS {
    using namespace OHOS::NFC;

    constexpr const auto FUZZER_THRESHOLD = 4;

    const uint8_t *g_baseFuzzData_ = nullptr;
    size_t g_baseFuzzSize_ = 0;
    size_t g_baseFuzzPos_;

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    template <class T> T GetData()
    {
        T object{};
        size_t objectSize = sizeof(object);
        if (g_baseFuzzData_ == nullptr || objectSize > g_baseFuzzSize_ - g_baseFuzzPos_) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_baseFuzzData_ + g_baseFuzzPos_, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_baseFuzzPos_ += objectSize;
        return object;
    }

    void FuzzOnAbilityConnectDone(const uint8_t* data, size_t size)
    {
        g_baseFuzzData_ = data;
        g_baseFuzzSize_ = size;
        g_baseFuzzPos_ = 0;

        AppExecFwk::ElementName element;
        sptr<IRemoteObject> remoteObject = nullptr;
        int resultCode = GetData<int>();
        std::shared_ptr<NfcAbilityConnectionCallback> nfcAbilityConnectionCallback =
            std::make_shared<NfcAbilityConnectionCallback>();
        nfcAbilityConnectionCallback->OnAbilityConnectDone(element, remoteObject, resultCode);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzOnAbilityConnectDone(data, size);
    return 0;
}

