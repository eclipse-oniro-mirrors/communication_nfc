/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "nfccontroller_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "nfccontroller.h"
#include "nfc_sdk_common.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;
    //using namespace OHOS::NFC::TAG;

class INfcControllerCallbackImpl : public INfcControllerCallback {
public:
    INfcControllerCallbackImpl() {}

    virtual ~INfcControllerCallbackImpl() {}

public:
    void OnNfcStateChanged(int nfcState) override
    {
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

    constexpr const auto TEST_UID = "0102";
    constexpr const auto INT_TO_BOOL_DIVISOR = 2;
    constexpr const auto TEST_HISTORICAL_BYTES = "1015";
    constexpr const auto TEST_HILAYER_RESPONSE = "0106";

    void FuzzIsNfcOpen(const uint8_t* data, size_t size)
    {
        NfcController ctrl = NfcController::GetInstance();
        bool isOpen = data % INT_TO_BOOL_DIVISOR;
        if (ctrl == nullptr) {
            std::cout << "ctrl is nullptr." << std::endl;
            return;
        }
        ctrl->IsNfcOpen(isOpen);
    }

    void FuzzRegListener(const uint8_t* data, size_t size)
    {
        NfcController ctrl = NfcController::GetInstance();
        if (ctrl == nullptr) {
            std::cout << "ctrl is nullptr." << std::endl;
            return;
        }
        std::string type = NfcSdkCommon::BytesVecToHexString(data, size);
        sptr<INfcControllerCallbackImpl> iNfcControllerCallbackImpl =
        sptr<INfcControllerCallbackImpl>(new (std::nothrow) INfcControllerCallbackImpl());
        ctrl->RegListener(iNfcControllerCallbackImpl, type);
    }

    void FuzzUnregListener(const uint8_t* data, size_t size)
    {
        NfcController ctrl = NfcController::GetInstance();
        if (ctrl == nullptr) {
            std::cout << "ctrl is nullptr." << std::endl;
            return;
        }
        std::string type = NfcSdkCommon::BytesVecToHexString(data, size);
        ctrl->UnregListener(type);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::FuzzIsNfcOpen(data, size);
    OHOS::FuzzRegListener(data, size);
    OHOS::FuzzUnregListener(data, size);
    return 0;
}

