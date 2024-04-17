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
#include "hceservice_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "hce_service.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;

class HceCmdListener : public IHceCmdCallback {
public:
    HceCmdListener() {}

    virtual ~HceCmdListener() {}

public:
    void OnCeApduData(const std::vector<uint8_t>& data) override
    {
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

    constexpr const auto FUZZER_THRESHOLD = 4;
    constexpr const auto INT_TO_BOOL_DIVISOR = 2;

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    void FuzzRegHceCmdCallback(const uint8_t* data, size_t size)
    {
        sptr<HceCmdListener> callback = sptr<HceCmdListener>(new (std::nothrow) HceCmdListener());
        std::string type = NfcSdkCommon::BytesVecToHexString(data, size);
        HceService::GetInstance().RegHceCmdCallback(callback, type);
    }

    void FuzzStopHce(const uint8_t* data, size_t size)
    {
        ElementName element;
        std::string bundleName = NfcSdkCommon::BytesVecToHexString(data, size);
        element.SetBundleName(bundleName);
        HceService::GetInstance().StopHce(element);
    }

    void FuzzIsDefaultService(const uint8_t* data, size_t size)
    {
        ElementName element;
        std::string bundleName = NfcSdkCommon::BytesVecToHexString(data, size);
        element.SetBundleName(bundleName);
        std::string type = NfcSdkCommon::BytesVecToHexString(data, size);
        bool isDefaultService = data[0] % INT_TO_BOOL_DIVISOR;
        HceService::GetInstance().IsDefaultService(element, type, isDefaultService);
    }

    void FuzzSendRawFrame(const uint8_t* data, size_t size)
    {
        std::string hexCmdData = NfcSdkCommon::BytesVecToHexString(data, size);
        bool raw = data[0] % INT_TO_BOOL_DIVISOR;
        std::string hexRespData = NfcSdkCommon::BytesVecToHexString(data, size);
        HceService::GetInstance().SendRawFrame(hexCmdData, raw, hexRespData);
    }

}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzRegHceCmdCallback(data, size);
    OHOS::FuzzStopHce(data, size);
    OHOS::FuzzIsDefaultService(data, size);
    OHOS::FuzzSendRawFrame(data, size);
    return 0;
}

