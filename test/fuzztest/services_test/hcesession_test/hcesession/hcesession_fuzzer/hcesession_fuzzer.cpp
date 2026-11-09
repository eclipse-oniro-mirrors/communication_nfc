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
#include "HceSession_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

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

    std::shared_ptr<NFC::NfcService> g_nfcService = nullptr;
    std::shared_ptr<HCE::HceSession> g_hceSession = nullptr;

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

    void FuzzCallbackEnter(FuzzedDataProvider& fdp)
    {
        if (!g_nfcService || !g_hceSession) {
            return;
        }
        uint32_t code = fdp.ConsumeIntegral<uint32_t>();
        g_hceSession->CallbackEnter(code);
    }

    void FuzzCallbackExit(FuzzedDataProvider& fdp)
    {
        if (!g_nfcService || !g_hceSession) {
            return;
        }
        uint32_t code = fdp.ConsumeIntegral<uint32_t>();
        int32_t result = fdp.ConsumeIntegral<int32_t>();
        g_hceSession->CallbackExit(code, result);
    }

    void FuzzIsDefaultService(FuzzedDataProvider& fdp)
    {
        if (!g_nfcService || !g_hceSession) {
            return;
        }
        ElementName element;
        std::string type = fdp.ConsumeRandomLengthString(MAX_STRING_LEN);
        bool isDefaultService = fdp.ConsumeBool();
        g_hceSession->IsDefaultService(element, type, isDefaultService);
        std::weak_ptr<NCI::INciCeInterface> nciCeProxy;
        std::shared_ptr<CeService> ceService = std::make_shared<CeService>(g_nfcService, nciCeProxy);
        g_hceSession->ceService_ = ceService;
        g_hceSession->IsDefaultService(element, type, isDefaultService);
    }

    void FuzzStartHce(FuzzedDataProvider& fdp)
    {
        if (!g_nfcService || !g_hceSession) {
            return;
        }
        ElementName element;
        std::vector<std::string> aids;
        aids.push_back(fdp.ConsumeRandomLengthString(MAX_STRING_LEN));
        g_hceSession->StartHce(element, aids);
    }

    void FuzzStopHce(FuzzedDataProvider& fdp)
    {
        if (!g_nfcService || !g_hceSession) {
            return;
        }
        ElementName element;
        fdp.ConsumeIntegral<uint8_t>();
        g_hceSession->StopHce(element);
    }
}

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    OHOS::g_nfcService = std::make_shared<OHOS::NFC::NfcService>();
    OHOS::g_nfcService->Initialize();
    OHOS::g_hceSession = std::make_shared<OHOS::HCE::HceSession>(OHOS::g_nfcService);
    return 0;
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
    OHOS::FuzzCallbackEnter(fdp);
    OHOS::FuzzCallbackExit(fdp);
    OHOS::FuzzIsDefaultService(fdp);
    OHOS::FuzzStartHce(fdp);
    OHOS::FuzzStopHce(fdp);
    return 0;
}
