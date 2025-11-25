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
#include "tagsession_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "tag_session.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"

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

    void FuzzResetTimeout(const uint8_t* data, size_t size)
    {
        //std::shared_ptr<NFC::NfcService> service = nullptr;
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        int tagRfDiscId = timeOutArray[0];
        tagSession->ResetTimeout(tagRfDiscId);
        service->Initialize();
        sptr<NFC::TAG::TagSession> TagSession1 = new NFC::TAG::TagSession(service);
        tagSession1->ResetTimeout(tagRfDiscId);
    }

    void FuzzIsTagFieldOn(const uint8_t* data, size_t size)
    {
        // std::shared_ptr<NFC::NfcService> service = nullptr;
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        int tagRfDiscId = timeOutArray[0];
        bool isTagFieldOn = false;
        tagSession->IsTagFieldOn(tagRfDiscId, isTagFieldOn);
        service->Initialize();
        sptr<NFC::TAG::TagSession> TagSession1 = new NFC::TAG::TagSession(service);
        tagSession1->IsTagFieldOn(tagRfDiscId, isTagFieldOn);
    }

    void FuzzGetFgDataVecSize(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        tagSession->GetFgDataVecSize();
    }

    void FuzzGetReaderDataVecSize(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        tagSession->GetReaderDataVecSize();
    }

    void FuzzHandleAppStateChanged(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        std::string bundleName = NfcSdkCommon::BytesVecToHexString(data, size);
        std::string abilityName = NfcSdkCommon::BytesVecToHexString(data, size);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        int abilityState = timeOutArray[0];
        tagSession->HandleAppStateChanged(bundleName, abilityName, abilityState);
    }

    void FuzzRegForegroundDispatch(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        ElementName element;
        element.SetBundleName(NfcSdkCommon::BytesVecToHexString(data, size));
        element.SetAbilityName(NfcSdkCommon::BytesVecToHexString(data, size));
        std::vector<uint32_t> discTech;
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        discTech.push_back(timeOutArray[0]);
        sptr<IForegroundCallbackImpl> iForegroundCallbackImpl =
            sptr<IForegroundCallbackImpl>(new (std::nothrow) IForegroundCallbackImpl());
        tagSession->RegForegroundDispatch(element, discTech, iForegroundCallbackImpl);
    }

    void FuzzUnregForegroundDispatch(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        ElementName element;
        element.SetBundleName(NfcSdkCommon::BytesVecToHexString(data, size));
        element.SetAbilityName(NfcSdkCommon::BytesVecToHexString(data, size));
        tagSession->UnregForegroundDispatch(element);
    }

    void FuzzRegReaderMode(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        ElementName element;
        element.SetBundleName(NfcSdkCommon::BytesVecToHexString(data, size));
        element.SetAbilityName(NfcSdkCommon::BytesVecToHexString(data, size));
        std::vector<uint32_t> discTech;
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        discTech.push_back(timeOutArray[0]);
        sptr<IReaderModeCallback> callback = nullptr;
        tagSession->RegReaderMode(element, discTech, callback);
    }

    void FuzzUnregReaderMode(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        ElementName element;
        element.SetBundleName(NfcSdkCommon::BytesVecToHexString(data, size));
        element.SetAbilityName(NfcSdkCommon::BytesVecToHexString(data, size));
        tagSession->UnregReaderMode(element);
    }

    void FuzzIsConnected(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        int tagRfDiscId = static_cast<int>(data[0]);
        bool isConnected = data[0] % INT_TO_BOOL_DIVISOR;
        tagSession->IsConnected(tagRfDiscId, isConnected);
        service->Initialize();
        sptr<NFC::TAG::TagSession> TagSession1 = new NFC::TAG::TagSession(service);
        tagSession1->IsConnected(tagRfDiscId, isConnected);
    }

    void FuzzSetTimeout(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        int tagRfDiscId = static_cast<int>(data[0]);
        int timeout = static_cast<int>(data[1]);
        int technology = static_cast<int>(data[2]);
        tagSession->SetTimeout(tagRfDiscId, timeout, technology);
    }

    void FuzzGetTimeout(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        int tagRfDiscId = static_cast<int>(data[0]);
        int technology = static_cast<int>(data[1]);
        int timeout = static_cast<int>(data[2]);
        tagSession->GetTimeout(tagRfDiscId, technology, timeout);
    }

    void FuzzRegForegroundDispatchInner(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        ElementName element;
        element.SetBundleName(NfcSdkCommon::BytesVecToHexString(data, size));
        element.SetAbilityName(NfcSdkCommon::BytesVecToHexString(data, size));
        std::vector<uint32_t> discTech;
        sptr<NFC::KITS::IForegroundCallback> callback = nullptr;
        bool isVendorApp = true;
        tagSession->RegForegroundDispatchInner(element, discTech, callback, isVendorApp);
    }

    void FuzzIsFgRegistered(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        ElementName element;
        element.SetBundleName(NfcSdkCommon::BytesVecToHexString(data, size));
        element.SetAbilityName(NfcSdkCommon::BytesVecToHexString(data, size));
        std::vector<uint32_t> discTech;
        sptr<NFC::KITS::IForegroundCallback> callback = nullptr;
        tagSession->IsFgRegistered(element, discTech, callback);
    }

    void FuzzIsReaderRegistered(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        ElementName element;
        element.SetBundleName(NfcSdkCommon::BytesVecToHexString(data, size));
        element.SetAbilityName(NfcSdkCommon::BytesVecToHexString(data, size));
        std::vector<uint32_t> discTech;
        sptr<NFC::KITS::IReaderModeCallback> callback = nullptr;
        tagSession->IsReaderRegistered(element, discTech, callback);
    }

    void FuzzRegReaderModeInner(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        ElementName element;
        element.SetBundleName(NfcSdkCommon::BytesVecToHexString(data, size));
        element.SetAbilityName(NfcSdkCommon::BytesVecToHexString(data, size));
        std::vector<uint32_t> discTech;
        sptr<NFC::KITS::IReaderModeCallback> callback = nullptr;
        bool isVendorApp = true;
        tagSession->RegReaderModeInner(element, discTech, callback, isVendorApp);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzResetTimeout(data, size);
    OHOS::FuzzIsTagFieldOn(data, size);
    OHOS::FuzzGetFgDataVecSize(data, size);
    OHOS::FuzzGetReaderDataVecSize(data, size);
    OHOS::FuzzHandleAppStateChanged(data, size);
    OHOS::FuzzRegForegroundDispatch(data, size);
    OHOS::FuzzUnregForegroundDispatch(data, size);
    OHOS::FuzzRegReaderMode(data, size);
    OHOS::FuzzUnregReaderMode(data, size);
    OHOS::FuzzIsConnected(data, size);
    OHOS::FuzzSetTimeout(data, size);
    OHOS::FuzzGetTimeout(data, size);
    OHOS::FuzzRegForegroundDispatchInner(data, size);
    OHOS::FuzzIsFgRegistered(data, size);
    OHOS::FuzzIsReaderRegistered(data, size);
    OHOS::FuzzRegReaderModeInner(data, size);
    return 0;
}

