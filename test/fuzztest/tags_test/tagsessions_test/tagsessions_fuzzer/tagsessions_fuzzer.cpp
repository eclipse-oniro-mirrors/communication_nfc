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

namespace OHOS {
    using namespace OHOS::NFC::TAG;
    using namespace OHOS::NFC::KITS;

    constexpr const auto FUZZER_THRESHOLD = 4;
    constexpr const auto INT_TO_BOOL_DIVISOR = 2;
    std::shared_ptr<NFC::AppStateObserver> g_appStateObserver = nullptr;

    const uint8_t *g_baseFuzzData = nullptr;
    size_t g_baseFuzzSize = 0;
    size_t g_baseFuzzPos = 0;

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

    void FuzzCallbackEnter(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t code = GetData<uint32_t>();
        tagSession->CallbackEnter(code);
    }

    void FuzzCallbackExit(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t code = GetData<uint32_t>();
        int32_t result = GetData<int32_t>();
        tagSession->CallbackExit(code, result);
    }

    void FuzzConnect(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        int32_t tagRfDiscId = GetData<int32_t>();
        int32_t technology = GetData<int32_t>();
        tagSession->Connect(tagRfDiscId, technology);
        service->Initialize();
        sptr<NFC::TAG::TagSession> tagSession1 = new NFC::TAG::TagSession(service);
        tagSession1->Connect(tagRfDiscId, technology);
    }

    void FuzzReconnect(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        int32_t tagRfDiscId = GetData<int32_t>();
        tagSession->Reconnect(tagRfDiscId);
        service->Initialize();
        sptr<NFC::TAG::TagSession> tagSession1 = new NFC::TAG::TagSession(service);
        tagSession1->Reconnect(tagRfDiscId);
    }

    void FuzzDisconnect(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        int32_t tagRfDiscId = GetData<int32_t>();
        tagSession->Disconnect(tagRfDiscId);
        service->Initialize();
        sptr<NFC::TAG::TagSession> tagSession1 = new NFC::TAG::TagSession(service);
        tagSession1->Disconnect(tagRfDiscId);
    }

    void FuzzGetTechList(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        int32_t tagRfDiscId = GetData<int32_t>();
        std::vector<int32_t> techList = {};
        tagSession->GetTechList(tagRfDiscId, techList);
        service->Initialize();
        sptr<NFC::TAG::TagSession> tagSession1 = new NFC::TAG::TagSession(service);
        tagSession1->GetTechList(tagRfDiscId, techList);
    }

    void FuzzIsNdef(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        int32_t tagRfDiscId = GetData<int32_t>();
        bool isNdef = false;
        tagSession->IsNdef(tagRfDiscId, isNdef);
        service->Initialize();
        sptr<NFC::TAG::TagSession> tagSession1 = new NFC::TAG::TagSession(service);
        tagSession1->IsNdef(tagRfDiscId, isNdef);
    }

    void FuzzSendRawFrame(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        int32_t tagRfDiscId = GetData<int32_t>();
        std::string hexCmdData = NfcSdkCommon::BytesVecToHexString(data, size);
        bool raw = data[0] % INT_TO_BOOL_DIVISOR;
        std::string hexRespData = NfcSdkCommon::BytesVecToHexString(data, size);
        tagSession->SendRawFrame(tagRfDiscId, hexCmdData, raw, hexRespData);
        service->Initialize();
        sptr<NFC::TAG::TagSession> tagSession1 = new NFC::TAG::TagSession(service);
        tagSession1->SendRawFrame(tagRfDiscId, hexCmdData, raw, hexRespData);
    }

    void FuzzNdefRead(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        int32_t tagRfDiscId = GetData<int32_t>();
        std::string ndefMessage = NfcSdkCommon::BytesVecToHexString(data, size);
        tagSession->NdefRead(tagRfDiscId, ndefMessage);
        service->Initialize();
        sptr<NFC::TAG::TagSession> tagSession1 = new NFC::TAG::TagSession(service);
        tagSession1->NdefRead(tagRfDiscId, ndefMessage);
    }

    void FuzzNdefWrite(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        int32_t tagRfDiscId = GetData<int32_t>();
        std::string msg = NfcSdkCommon::BytesVecToHexString(data, size);
        tagSession->NdefWrite(tagRfDiscId, "");
        tagSession->NdefWrite(tagRfDiscId, msg);
        service->Initialize();
        sptr<NFC::TAG::TagSession> tagSession1 = new NFC::TAG::TagSession(service);
        tagSession1->NdefWrite(tagRfDiscId, "");
        tagSession1->NdefWrite(tagRfDiscId, msg);
    }

    void FuzzNdefMakeReadOnly(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        int32_t tagRfDiscId = GetData<int32_t>();
        tagSession->NdefMakeReadOnly(tagRfDiscId);
        service->Initialize();
        sptr<NFC::TAG::TagSession> tagSession1 = new NFC::TAG::TagSession(service);
        tagSession1->NdefMakeReadOnly(tagRfDiscId);
    }

    void FuzzFormatNdef(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        int32_t tagRfDiscId = GetData<int32_t>();
        std::string key = NfcSdkCommon::BytesVecToHexString(data, size);
        tagSession->FormatNdef(tagRfDiscId, key);
        service->Initialize();
        sptr<NFC::TAG::TagSession> tagSession1 = new NFC::TAG::TagSession(service);
        tagSession1->FormatNdef(tagRfDiscId, key);
    }

    void FuzzCanMakeReadOnly(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        int32_t tagRfDiscId = GetData<int32_t>();
        bool canSetReadOnly = data[0] % INT_TO_BOOL_DIVISOR;
        tagSession->CanMakeReadOnly(tagRfDiscId, canSetReadOnly);
        service->Initialize();
        sptr<NFC::TAG::TagSession> tagSession1 = new NFC::TAG::TagSession(service);
        tagSession1->CanMakeReadOnly(tagRfDiscId, canSetReadOnly);
    }

    void FuzzGetMaxTransceiveLength(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        int32_t tagRfDiscId = GetData<int32_t>();
        int32_t maxSize = GetData<int32_t>();
        tagSession->GetMaxTransceiveLength(tagRfDiscId, maxSize);
    }

    void FuzzIsSupportedApdusExtended(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        bool isSupported = data[0] % INT_TO_BOOL_DIVISOR;
        tagSession->IsSupportedApdusExtended(isSupported);
        service->Initialize();
        sptr<NFC::TAG::TagSession> tagSession1 = new NFC::TAG::TagSession(service);
        tagSession1->IsSupportedApdusExtended(isSupported);
    }

    void FuzzIsSameAppAbility(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        ElementName fgElement1;
        ElementName fgElement2;
        tagSession->IsSameAppAbility(fgElement1, fgElement2);
        fgElement1.bundleName_ = std::string(reinterpret_cast<const char*>(data), size);
        fgElement1.bundleName_ = std::string(reinterpret_cast<const char*>(data), size);
        tagSession->IsSameAppAbility(fgElement1, fgElement2);
    }

    void FuzzRegForegroundDispatchInnerData(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        ElementName element;
        element.bundleName_ = "bundleName";
        element.abilityName_ = "abilityName";
        std::vector<uint32_t> discTech = {};
        tagSession->RegForegroundDispatchInner(element, discTech, nullptr);
        tagSession->RegForegroundDispatchInner(element, discTech, nullptr);
 
        int abilityState = static_cast<int>(data[0]);
        tagSession->CheckFgAppStateChanged("bundleName", "abilityName", abilityState);
        abilityState = static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND);
        tagSession->CheckFgAppStateChanged("bundleName", "abilityName", abilityState);
        abilityState = static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_TERMINATED);
        tagSession->CheckFgAppStateChanged("bundleName", "abilityName", abilityState);
 
        tagSession->UnregForegroundDispatch(element);
        abilityState = static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND);
        tagSession->CheckFgAppStateChanged("bundleName", "abilityName", abilityState);
        abilityState = static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND);
        tagSession->CheckFgAppStateChanged("bundleName", "abilityName", abilityState);
        abilityState = static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_TERMINATED);
        tagSession->CheckFgAppStateChanged("bundleName", "abilityName", abilityState);
    }
 
    void FuzzRegReaderModeInnerData(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        ElementName element;
        element.bundleName_ = "bundleName";
        element.abilityName_ = "abilityName";
        std::vector<uint32_t> discTech = {};
 
        tagSession->RegReaderModeInner(element, discTech, nullptr);
        tagSession->RegReaderModeInner(element, discTech, nullptr);
 
        int abilityState = static_cast<int>(data[0]);
        tagSession->CheckReaderAppStateChanged("bundleName", "abilityName", abilityState);
        abilityState = static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND);
        tagSession->CheckReaderAppStateChanged("bundleName", "abilityName", abilityState);
        abilityState = static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_TERMINATED);
        tagSession->CheckReaderAppStateChanged("bundleName", "abilityName", abilityState);
 
        tagSession->UnregReaderModeInner(element, false);
        abilityState = static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND);
        tagSession->CheckReaderAppStateChanged("bundleName", "abilityName", abilityState);
        abilityState = static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND);
        tagSession->CheckReaderAppStateChanged("bundleName", "abilityName", abilityState);
        abilityState = static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_TERMINATED);
        tagSession->CheckReaderAppStateChanged("bundleName", "abilityName", abilityState);
    }

    void FuzzIsReaderUnregistered(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        ElementName element;
        element.bundleName_ = "bundleName";
        element.abilityName_ = "abilityName";
        bool isAppUnregistered = data[0] % INT_TO_BOOL_DIVISOR;
        tagSession->IsReaderUnregistered(element, isAppUnregistered);
    }

    void FuzzIsVendorProcess(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        tagSession->IsVendorProcess();
    }

    void FuzzIsForegroundApp(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        ElementName element;
        element.bundleName_ = std::string(reinterpret_cast<const char*>(data), size);
        g_appStateObserver->IsForegroundApp(element.GetBundleName());
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
    OHOS::FuzzConnect(data, size);
    OHOS::FuzzReconnect(data, size);
    OHOS::FuzzDisconnect(data, size);
    OHOS::FuzzGetTechList(data, size);
    OHOS::FuzzIsNdef(data, size);
    OHOS::FuzzSendRawFrame(data, size);
    OHOS::FuzzNdefRead(data, size);
    OHOS::FuzzNdefWrite(data, size);
    OHOS::FuzzNdefMakeReadOnly(data, size);
    OHOS::FuzzFormatNdef(data, size);
    OHOS::FuzzCanMakeReadOnly(data, size);
    OHOS::FuzzGetMaxTransceiveLength(data, size);
    OHOS::FuzzIsSupportedApdusExtended(data, size);
    OHOS::FuzzIsSameAppAbility(data, size);
    OHOS::FuzzRegForegroundDispatchInnerData(data, size);
    OHOS::FuzzRegReaderModeInnerData(data, size);
    OHOS::FuzzIsReaderUnregistered(data, size);
    OHOS::FuzzIsVendorProcess(data, size);
    OHOS::FuzzIsForegroundApp(data, size);

    return 0;
}