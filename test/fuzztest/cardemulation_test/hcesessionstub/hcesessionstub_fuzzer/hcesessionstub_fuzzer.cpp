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
#include "hcesessionstub_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "hce_session_stub.h"
#include "hce_session.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;

    static constexpr const auto HCESESSION_DESCRIPTOR = u"ohos.nfc.cardemulation.IHceSession";
    constexpr const auto FUZZER_THRESHOLD = 4;

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    void HandleRegHceCmdCallback(const uint8_t* data, size_t size)
    {
        std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
        std::shared_ptr<NFC::HCE::HceSession> hceSession = std::make_shared<NFC::HCE::HceSession>(nfcService);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(HCESESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        hceSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_ON),
            data2, reply, option);
    }

    void HandleSendRawFrame(const uint8_t* data, size_t size)
    {
        std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
        std::shared_ptr<NFC::HCE::HceSession> hceSession = std::make_shared<NFC::HCE::HceSession>(nfcService);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(HCESESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        hceSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_TRANSMIT),
            data2, reply, option);
    }

    void HandleGetPaymentServices(const uint8_t* data, size_t size)
    {
        std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
        std::shared_ptr<NFC::HCE::HceSession> hceSession = std::make_shared<NFC::HCE::HceSession>(nfcService);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(HCESESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        hceSession->OnRemoteRequest(static_cast<uint32_t>(
            NFC::NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_GET_PAYMENT_SERVICES), data2, reply, option);
    }

    void HandleStopHce(const uint8_t* data, size_t size)
    {
        std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
        std::shared_ptr<NFC::HCE::HceSession> hceSession = std::make_shared<NFC::HCE::HceSession>(nfcService);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(HCESESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        hceSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_STOP),
            data2, reply, option);
    }

    void HandleIsDefaultService(const uint8_t* data, size_t size)
    {
        std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
        std::shared_ptr<NFC::HCE::HceSession> hceSession = std::make_shared<NFC::HCE::HceSession>(nfcService);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(HCESESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        hceSession->OnRemoteRequest(static_cast<uint32_t>(
            NFC::NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_IS_DEFAULT_SERVICE), data2, reply, option);
    }

    void HandleStartHce(const uint8_t* data, size_t size)
    {
        std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
        std::shared_ptr<NFC::HCE::HceSession> hceSession = std::make_shared<NFC::HCE::HceSession>(nfcService);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(HCESESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        hceSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_START),
            data2, reply, option);
    }

}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::HandleRegHceCmdCallback(data, size);
    OHOS::HandleSendRawFrame(data, size);
    OHOS::HandleGetPaymentServices(data, size);
    OHOS::HandleStopHce(data, size);
    OHOS::HandleIsDefaultService(data, size);
    OHOS::HandleStartHce(data, size);
    return 0;
}

