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
#include "hcesessionstub_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>
#include <string>

#include "hce_session_stub.h"
#include "hce_session.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include "nfc_access_token_mock.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;
    using namespace OHOS::NFC::HCE;

    constexpr uint32_t MESSAGE_SIZE = NFC::NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_SESSION_BOTTOM;
    constexpr const auto FUZZER_THRESHOLD = 6;

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    uint32_t GetU32Data(const uint8_t* data)
    {
        /*
        * Move the 0th digit to the left by 24 bits, the 1st digit to the left by 16 bits,
        * the 2nd digit to the left by 8 bits, and the 3rd digit not to the left
        */
        return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | (data[3]);
    }

    std::string BuildAddressString(const uint8_t* data)
    {
        std::string addr("00:00:00:00:00:00");
        char temp[18] = {0};
        int ret = sprintf_s(temp, sizeof(temp), "%02X:%02X:%02X:%02X:%02X:%02X",
            data[0], data[1], data[2], data[3], data[4], data[5]);
        if (ret != -1) {
            addr = std::string(temp);
        }
        return addr;
    }

    bool DoHceSessionStubFuzzTest(const uint8_t* data, size_t size)
    {
        uint32_t code = (GetU32Data(data) % MESSAGE_SIZE);
        auto addr = BuildAddressString(data);

        MessageParcel datas;
        std::u16string descriptor = NFC::HceSessionStub::GetDescriptor();
        datas.WriteInterfaceToken(descriptor);
        datas.WriteInt32(*(reinterpret_cast<const int32_t *>(data)));
        datas.WriteString(addr.c_str());
        datas.RewindRead(0);
        MessageParcel reply;
        MessageOption option;

        std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
        std::shared_ptr<NFC::HCE::HceSession> hceSession = std::make_shared<NFC::HCE::HceSession>(nfcService);
        hceSession->OnRemoteRequest(code, datas, reply, option);
        return true;
    }

    void StopHceFuzzTest(const uint8_t* data, size_t size)
    {
        ElementName element;
        Security::AccessToken::AccessTokenID callerToken = static_cast<Security::AccessToken::AccessTokenID>(data[0]);
        std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
        std::shared_ptr<NFC::HCE::HceSession> hceSession = std::make_shared<NFC::HCE::HceSession>(nfcService);
        hceSession->StopHce(element);
    }

    void RemoveHceDeathRecipientFuzzTest(const uint8_t* data, size_t size)
    {
        wptr<IRemoteObject> remote = nullptr;
        std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
        std::shared_ptr<NFC::HCE::HceSession> hceSession = std::make_shared<NFC::HCE::HceSession>(nfcService);
        hceSession->RemoveHceDeathRecipient(remote);
    }

    void FuzzHceSessionDump(const uint8_t* data, size_t size)
    {
        auto addr = BuildAddressString(data);
        std::shared_ptr<OHOS::NFC::NfcService> nfcService = std::make_shared<OHOS::NFC::NfcService>();
        int32_t fd = 0;
        std::vector<std::u16string> args;
        std::shared_ptr<NFC::HCE::HceSession> hceSession = std::make_shared<NFC::HCE::HceSession>(nfcService);
        (void)hceSession->Dump(fd, args);
    }

}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::NFC::NfcAccessTokenMock::SetNativeTokenInfo();
    OHOS::DoHceSessionStubFuzzTest(data, size);
    OHOS::StopHceFuzzTest(data, size);
    OHOS::RemoveHceDeathRecipientFuzzTest(data, size);
    OHOS::FuzzHceSessionDump(data, size);
    return 0;
}

