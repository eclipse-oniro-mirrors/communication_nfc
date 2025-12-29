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
#include "nfcpollingmanager_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "nfc_polling_manager.h"
#include "nfc_service.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include <securec.h>

namespace OHOS {
    using namespace OHOS::NFC;

    constexpr const auto FUZZER_THRESHOLD = 4;
    constexpr const auto INT_TO_BOOL_DIVISOR = 2;

    const uint8_t *g_baseFuzzData = nullptr;
    size_t g_baseFuzzSize = 0;
    size_t g_baseFuzzPos = 0;

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
        if (g_baseFuzzData == nullptr || objectSize > g_baseFuzzSize - g_baseFuzzPos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_baseFuzzData + g_baseFuzzPos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_baseFuzzPos += objectSize;
        return object;
    }

    void FuzzStartPollingLoop(const uint8_t* data, size_t size)
    {
        std::weak_ptr<NfcService> nfcService;
        std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
        std::weak_ptr<NCI::INciTagInterface> nciTagProxy;
        std::shared_ptr<NfcPollingManager> nfcPollingManager =
            std::make_shared<NfcPollingManager>(nfcService, nciNfccProxy, nciTagProxy);
        bool force = data[0] % INT_TO_BOOL_DIVISOR;
        nfcPollingManager->StartPollingLoop(force);
    }

    void FuzzEnableForegroundDispatch(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::weak_ptr<NfcService> nfcService;
        std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
        std::weak_ptr<NCI::INciTagInterface> nciTagProxy;
        std::shared_ptr<NfcPollingManager> nfcPollingManager =
            std::make_shared<NfcPollingManager>(nfcService, nciNfccProxy, nciTagProxy);
        std::string deviceId = std::string(reinterpret_cast<const char*>(data), size);
        std::string bundleName = std::string(reinterpret_cast<const char*>(data), size);
        std::string abilityName = std::string(reinterpret_cast<const char*>(data), size);
        std::string moduleName = std::string(reinterpret_cast<const char*>(data), size);
        AppExecFwk::ElementName element(deviceId, bundleName, abilityName, moduleName);
        std::vector<uint32_t> discTech;
        discTech.push_back(GetData<uint32_t>());
        sptr<KITS::IForegroundCallback> callback = nullptr;
        nfcPollingManager->EnableForegroundDispatch(element, discTech, callback);
    }

    void FuzzDisableForegroundDispatch(const uint8_t* data, size_t size)
    {
        std::weak_ptr<NfcService> nfcService;
        std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
        std::weak_ptr<NCI::INciTagInterface> nciTagProxy;
        std::shared_ptr<NfcPollingManager> nfcPollingManager =
            std::make_shared<NfcPollingManager>(nfcService, nciNfccProxy, nciTagProxy);
        std::string deviceId = std::string(reinterpret_cast<const char*>(data), size);
        std::string bundleName = std::string(reinterpret_cast<const char*>(data), size);
        std::string abilityName = std::string(reinterpret_cast<const char*>(data), size);
        std::string moduleName = std::string(reinterpret_cast<const char*>(data), size);
        AppExecFwk::ElementName element(deviceId, bundleName, abilityName, moduleName);
        nfcPollingManager->DisableForegroundDispatch(element);
    }

    void FuzzDisableForegroundByDeathRcpt(const uint8_t* data, size_t size)
    {
        std::weak_ptr<NfcService> nfcService;
        std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
        std::weak_ptr<NCI::INciTagInterface> nciTagProxy;
        std::shared_ptr<NfcPollingManager> nfcPollingManager =
            std::make_shared<NfcPollingManager>(nfcService, nciNfccProxy, nciTagProxy);
        nfcPollingManager->DisableForegroundByDeathRcpt();
    }

    void FuzzIsForegroundEnabled(const uint8_t* data, size_t size)
    {
        std::weak_ptr<NfcService> nfcService;
        std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
        std::weak_ptr<NCI::INciTagInterface> nciTagProxy;
        std::shared_ptr<NfcPollingManager> nfcPollingManager =
            std::make_shared<NfcPollingManager>(nfcService, nciNfccProxy, nciTagProxy);
        nfcPollingManager->IsForegroundEnabled();
    }

    void FuzzSendTagToForeground(const uint8_t* data, size_t size)
    {
        std::weak_ptr<NfcService> nfcService;
        std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
        std::weak_ptr<NCI::INciTagInterface> nciTagProxy;
        std::shared_ptr<NfcPollingManager> nfcPollingManager =
            std::make_shared<NfcPollingManager>(nfcService, nciNfccProxy, nciTagProxy);
        nfcPollingManager->SendTagToForeground(nullptr);
    }

    void FuzzIsReaderModeEnabled(const uint8_t* data, size_t size)
    {
        std::weak_ptr<NfcService> nfcService;
        std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
        std::weak_ptr<NCI::INciTagInterface> nciTagProxy;
        std::shared_ptr<NfcPollingManager> nfcPollingManager =
            std::make_shared<NfcPollingManager>(nfcService, nciNfccProxy, nciTagProxy);
        nfcPollingManager->IsReaderModeEnabled();
    }

    void FuzzSendTagToReaderApp(const uint8_t* data, size_t size)
    {
        std::weak_ptr<NfcService> nfcService;
        std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy;
        std::weak_ptr<NCI::INciTagInterface> nciTagProxy;
        std::shared_ptr<NfcPollingManager> nfcPollingManager =
            std::make_shared<NfcPollingManager>(nfcService, nciNfccProxy, nciTagProxy);
        nfcPollingManager->SendTagToReaderApp(nullptr);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzStartPollingLoop(data, size);
    OHOS::FuzzEnableForegroundDispatch(data, size);
    OHOS::FuzzDisableForegroundDispatch(data, size);
    OHOS::FuzzDisableForegroundByDeathRcpt(data, size);
    OHOS::FuzzIsForegroundEnabled(data, size);
    OHOS::FuzzSendTagToForeground(data, size);
    OHOS::FuzzIsReaderModeEnabled(data, size);
    OHOS::FuzzSendTagToReaderApp(data, size);
    return 0;
}

