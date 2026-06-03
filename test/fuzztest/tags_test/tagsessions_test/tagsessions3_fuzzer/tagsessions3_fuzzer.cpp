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
#define private public
#include "tagsessions_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "tag_session.h"
#include "nfc_sdk_common.h"
#include "app_state_observer.h"
#include "nfc_service_ipc_interface_code.h"
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS {
    using namespace OHOS::NFC::TAG;
    using namespace OHOS::NFC::KITS;

    constexpr const auto FUZZER_THRESHOLD = 4;
    constexpr const auto INT_TO_BOOL_DIVISOR = 2;

class IForegroundCallbackImpl : public IForegroundCallback {
public:
    IForegroundCallbackImpl() {}

    virtual ~IForegroundCallbackImpl() {}

public:
    void OnTagDiscovered(TagInfoParcelable* taginfo) override
    {
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    void FuzzNdefWrite(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        FuzzedDataProvider fdp(data, size);
        int32_t tagRfDiscId = fdp.ConsumeIntegral<int32_t>();
        std::string msg = NfcSdkCommon::BytesVecToHexString(data, size);
        tagSession->NdefWrite(tagRfDiscId, "");
        tagSession->NdefWrite(tagRfDiscId, msg);
    }

    void FuzzNdefMakeReadOnly(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        FuzzedDataProvider fdp(data, size);
        int32_t tagRfDiscId = fdp.ConsumeIntegral<int32_t>();
        tagSession->NdefMakeReadOnly(tagRfDiscId);
    }

    void FuzzFormatNdef(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        FuzzedDataProvider fdp(data, size);
        int32_t tagRfDiscId = fdp.ConsumeIntegral<int32_t>();
        std::string key = NfcSdkCommon::BytesVecToHexString(data, size);
        tagSession->FormatNdef(tagRfDiscId, key);
    }

    void FuzzCanMakeReadOnly(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        FuzzedDataProvider fdp(data, size);
        int32_t tagRfDiscId = fdp.ConsumeIntegral<int32_t>();
        bool canSetReadOnly = data[0] % INT_TO_BOOL_DIVISOR;
        tagSession->CanMakeReadOnly(tagRfDiscId, canSetReadOnly);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzNdefWrite(data, size);
    OHOS::FuzzNdefMakeReadOnly(data, size);
    OHOS::FuzzFormatNdef(data, size);
    OHOS::FuzzCanMakeReadOnly(data, size);
    return 0;
}