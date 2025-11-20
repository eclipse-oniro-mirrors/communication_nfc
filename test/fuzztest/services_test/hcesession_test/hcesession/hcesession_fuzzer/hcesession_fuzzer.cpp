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
#include "hcesession_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "hce_session.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include "loghelper.h"

namespace OHOS {
    using namespace OHOS::NFC;
    using namespace OHOS::NFC::HCE;
    using namespace OHOS::NFC::KITS;

    constexpr const auto FUZZER_THRESHOLD = 4;
    constexpr const auto INT_TO_BOOL_DIVISOR = 2;

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

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    void FuzzCallbackEnter(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
        service->Initialize();
        std::shared_ptr<HceSession> hceSession = std::make_shared<HceSession>(service);
        uint32_t code = static_cast<uint32_t>(data[0]);
        hceSession->CallbackEnter(code);
    }

    void FuzzCallbackExit(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
        service->Initialize();
        std::shared_ptr<HceSession> hceSession = std::make_shared<HceSession>(service);
        uint32_t code = static_cast<uint32_t>(data[0]);
        int32_t result = static_cast<int32_t>(data[1]);
        hceSession->CallbackExit(code, result);
    }

    void FuzzIsDefaultService(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
        std::shared_ptr<HceSession> hceSession = std::make_shared<HceSession>(service);
        ElementName element;
        std::string type = NfcSdkCommon::BytesVecToHexString(data, size);
        bool isDefaultService = data[0] % INT_TO_BOOL_DIVISOR;
        hceSession->IsDefaultService(element, type, isDefaultService);
    }

    void FuzzStartHce(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
        std::shared_ptr<HceSession> hceSession = std::make_shared<HceSession>(service);
        ElementName element;
        std::vector<std::string> aids;
        aids.push_back(NfcSdkCommon::BytesVecToHexString(data, size));
        hceSession->StartHce(element, aids);
    }

    void FuzzStopHce(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
        std::shared_ptr<HceSession> hceSession = std::make_shared<HceSession>(service);
        ElementName element;
        Security::AccessToken::AccessTokenID callerToken = static_cast<Security::AccessToken::AccessTokenID>(data[0]);
        hceSession->StopHce(element);
    }

    void FuzzRegHceCmdCallbackByToken(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
        std::shared_ptr<HceSession> hceSession = std::make_shared<HceSession>(service);
        sptr<KITS::IHceCmdCallback> callback = nullptr;
        std::string type = NfcSdkCommon::BytesVecToHexString(data, size);
        hceSession->RegHceCmdCallback(callback, type);
    }

    void FuzzUnRegHceCmdCallback(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
        std::shared_ptr<HceSession> hceSession = std::make_shared<HceSession>(service);
        sptr<KITS::IHceCmdCallback> callback = nullptr;
        std::string type = NfcSdkCommon::BytesVecToHexString(data, size);
        hceSession->UnregHceCmdCallback(callback, type);
    }

    void FuzzUnRegAllCallback(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
        std::shared_ptr<HceSession> hceSession1 = std::make_shared<HceSession>(nullptr);
        std::shared_ptr<HceSession> hceSession = std::make_shared<HceSession>(service);
        Security::AccessToken::AccessTokenID callerToken = static_cast<Security::AccessToken::AccessTokenID>(data[0]);
        hceSession->UnRegAllCallback(callerToken);
    }

    void FuzzHandleWhenRemoteDie(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
        std::shared_ptr<HceSession> hceSession = std::make_shared<HceSession>(service);
        Security::AccessToken::AccessTokenID callerToken = static_cast<Security::AccessToken::AccessTokenID>(data[0]);
        hceSession->HandleWhenRemoteDie(callerToken);
    }

    void FuzzGetPaymentServices(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
        service->Initialize();
        std::shared_ptr<HceSession> hceSession = std::make_shared<HceSession>(service);
        CePaymentServicesParcelable parcelable;
        hceSession->GetPaymentServices(parcelable);
    }

    void FuzzAppendSimBundle(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> service = std::make_shared<NfcService>();
        service->Initialize();
        std::shared_ptr<HceSession> hceSession = std::make_shared<HceSession>(service);
        std::vector<AbilityInfo> paymentAbilityInfos;
        hceSession->AppendSimBundle(paymentAbilityInfos);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzCallbackEnter(data, size);
    OHOS::FuzzCallbackExit(data, size);
    OHOS::FuzzIsDefaultService(data, size);
    OHOS::FuzzStartHce(data, size);
    OHOS::FuzzStopHce(data, size);
    OHOS::FuzzRegHceCmdCallbackByToken(data, size);
    OHOS::FuzzUnRegHceCmdCallback(data, size);
    OHOS::FuzzUnRegAllCallback(data, size);
    OHOS::FuzzHandleWhenRemoteDie(data, size);
    OHOS::FuzzGetPaymentServices(data, size);
    OHOS::FuzzAppendSimBundle(data, size);
    
    return 0;
}
