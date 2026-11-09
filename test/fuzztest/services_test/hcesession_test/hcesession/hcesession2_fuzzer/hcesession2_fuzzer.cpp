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
#include "hcesession2_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string>

#include "hce_session.h"
#include "nfc_sdk_common.h"
#include "hce_cmd_callback_stub.h"
#include "nfc_service_ipc_interface_code.h"
#include "loghelper.h"
#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS {
    using namespace OHOS::NFC;
    using namespace OHOS::NFC::HCE;
    using namespace OHOS::NFC::KITS;

    constexpr const auto FUZZER_THRESHOLD = 4;
    constexpr const size_t MAX_STRING_LEN = 256;

    class HceCmdListener : public IHceCmdCallback {
    public:
        HceCmdListener() {}

        virtual ~HceCmdListener() {}

    public:
        void OnCeApduData(const std::vector<uint8_t>& data) override
        {
            std::cout << "OnCeApduData" << std::endl;
        }

        OHOS::sptr<OHOS::IRemoteObject> AsObject() override
        {
            return nullptr;
        }
    };

    void FuzzRegHceCmdCallbackByToken(FuzzedDataProvider& fdp)
    {
        std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
        std::shared_ptr<HceSession> hceSession = std::make_shared<HceSession>(service);
        sptr<KITS::IHceCmdCallback> callback = nullptr;
        std::string type = fdp.ConsumeRandomLengthString(MAX_STRING_LEN);
        hceSession->RegHceCmdCallback(callback, type);
        std::weak_ptr<NCI::INciCeInterface> nciCeProxy;
        std::shared_ptr<CeService> ceService = std::make_shared<CeService>(service, nciCeProxy);
        hceSession->ceService_ = ceService;
        hceSession->RegHceCmdCallback(callback, type);
    }

    void FuzzRegHceCmdCallbackByToken2(FuzzedDataProvider& fdp)
    {
        std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
        std::shared_ptr<HceSession> hceSession = std::make_shared<HceSession>(service);
        sptr<KITS::IHceCmdCallback> callback = sptr<HCE::HceCmdCallbackStub>(new HCE::HceCmdCallbackStub);
        std::string type = fdp.ConsumeRandomLengthString(MAX_STRING_LEN);
        hceSession->RegHceCmdCallback(callback, type);
    }

    void FuzzUnRegHceCmdCallback(FuzzedDataProvider& fdp)
    {
        std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
        std::shared_ptr<HceSession> hceSession = std::make_shared<HceSession>(service);
        sptr<KITS::IHceCmdCallback> callback = nullptr;
        std::string type = fdp.ConsumeRandomLengthString(MAX_STRING_LEN);
        hceSession->UnregHceCmdCallback(callback, type);
        std::weak_ptr<NCI::INciCeInterface> nciCeProxy;
        std::shared_ptr<CeService> ceService = std::make_shared<CeService>(service, nciCeProxy);
        hceSession->ceService_ = ceService;
        hceSession->UnregHceCmdCallback(callback, type);
    }

    void FuzzUnRegHceCmdCallback2(FuzzedDataProvider& fdp)
    {
        std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
        std::shared_ptr<HceSession> hceSession = std::make_shared<HceSession>(service);
        sptr<KITS::IHceCmdCallback> callback = sptr<HCE::HceCmdCallbackStub>(new HCE::HceCmdCallbackStub);
        std::string type = fdp.ConsumeRandomLengthString(MAX_STRING_LEN);
        hceSession->UnregHceCmdCallback(callback, type);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    FuzzedDataProvider fdp(data, size);
    fdp.ConsumeIntegral<uint8_t>();
    OHOS::FuzzRegHceCmdCallbackByToken(fdp);
    OHOS::FuzzRegHceCmdCallbackByToken2(fdp);
    OHOS::FuzzUnRegHceCmdCallback(fdp);
    OHOS::FuzzUnRegHceCmdCallback2(fdp);
    return 0;
}
