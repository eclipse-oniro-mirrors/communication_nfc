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
#define protected public
#include "isodepcardhandler_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "isodep_card_handler.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include <securec.h>

namespace OHOS {
    using namespace OHOS::NFC::NCI;
    using namespace OHOS::NFC::TAG;
    using namespace OHOS::NFC;

    constexpr const auto FUZZER_THRESHOLD = 4;

    uint8_t *g_baseFuzzData = nullptr;
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

    void FuzzInitTransportCardInfo(const uint8_t* data, size_t size)
    {
        std::weak_ptr<INciTagInterface> nciTagProxy;
        std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
        isodepCardHandler->InitTransportCardInfo();
    }

    void FuzzGetCardName(const uint8_t* data, size_t size)
    {
        uint8_t cardIndex = static_cast<uint8_t>(data[0]);
        std::string cardName = std::string(reinterpret_cast<const char*>(data), size);
        std::weak_ptr<INciTagInterface> nciTagProxy;
        std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
        isodepCardHandler->GetCardName(cardIndex, cardName);
    }

    void FuzzIsSupportedTransportCard(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        uint32_t rfDiscId = GetData<uint32_t>();
        uint8_t cardIndex = GetData<uint8_t>();
        std::weak_ptr<INciTagInterface> nciTagProxy;
        std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
        isodepCardHandler->IsSupportedTransportCard(rfDiscId, cardIndex);
    }

    void FuzzGetBalance(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        uint32_t rfDiscId = GetData<uint32_t>();
        uint8_t cardIndex = GetData<uint8_t>();
        int balance = GetData<int>();
        std::weak_ptr<INciTagInterface> nciTagProxy;
        std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
        isodepCardHandler->GetBalance(rfDiscId, cardIndex, balance);
    }

    void FuzzCheckApduResponse(const uint8_t* data, size_t size)
    {
        std::string response = std::string(reinterpret_cast<const char*>(data), size);
        std::weak_ptr<INciTagInterface> nciTagProxy;
        std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
        isodepCardHandler->CheckApduResponse(response);
    }

    void FuzzCheckApduResponsedata(const uint8_t* data, size_t size)
    {
        std::string response = std::string(reinterpret_cast<const char*>(data), size);
        uint8_t cardIndex = static_cast<uint8_t>(data[0]);
        std::weak_ptr<INciTagInterface> nciTagProxy;
        std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
        isodepCardHandler->CheckApduResponse(response, cardIndex);
    }

    void FuzzGetBalanceValue(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::string balanceStr = std::string(reinterpret_cast<const char*>(data), size);
        int balanceValue = GetData<int>();
        std::weak_ptr<INciTagInterface> nciTagProxy;
        std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
        isodepCardHandler->GetBalanceValue(balanceStr, balanceValue);
    }

    void FuzzMatchCity(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<NCI::INciTagInterface> nciTagProxy = nullptr;
        uint32_t rfDiscId = GetData<uint32_t>();
        uint8_t cardIndex = GetData<uint8_t>();
        std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
        isodepCardHandler->MatchCity(rfDiscId, cardIndex);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzInitTransportCardInfo(data, size);
    OHOS::FuzzGetCardName(data, size);
    OHOS::FuzzIsSupportedTransportCard(data, size);
    OHOS::FuzzGetBalance(data, size);
    OHOS::FuzzCheckApduResponse(data, size);
    OHOS::FuzzCheckApduResponsedata(data, size);
    OHOS::FuzzGetBalanceValue(data, size);
    OHOS::FuzzMatchCity(data, size);
    return 0;
}

