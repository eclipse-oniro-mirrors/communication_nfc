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
#include "tagsession_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "tag_session.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
    using namespace OHOS::NFC;
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

    void FuzzConnect(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[1]);
        int technology = static_cast<int>(data[2]);
        tagSession->Connect(tagRfDiscId, technology);
    }

    void FuzzIsConnected(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[1]);
        bool isConnected = data[0] % INT_TO_BOOL_DIVISOR;
        tagSession->IsConnected(tagRfDiscId, isConnected);
    }

    void FuzzReconnect(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[0]);
        tagSession->Reconnect(tagRfDiscId);
    }

    void FuzzDisconnect(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[0]);
        tagSession->Disconnect(tagRfDiscId);
    }

    void FuzzSetTimeout(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[0]);
        int timeout = static_cast<int>(data[1]);
        int technology = static_cast<int>(data[2]);
        tagSession->SetTimeout(tagRfDiscId, timeout, technology);
    }

    void FuzzGetTimeout(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[0]);
        int timeout = static_cast<int>(data[1]);
        int technology = static_cast<int>(data[2]);
        tagSession->GetTimeout(tagRfDiscId, technology, timeout);
    }

    void FuzzResetTimeout(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[0]);
        tagSession->ResetTimeout(tagRfDiscId);
    }

    void FuzzGetTechList(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[0]);
        std::vector<int32_t> techList = {};
        tagSession->GetTechList(tagRfDiscId, techList);
    }

    void FuzzIsTagFieldOn(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[0]);
        bool isTagFieldOn = false;
        tagSession->IsTagFieldOn(tagRfDiscId, isTagFieldOn);
    }

    void FuzzIsNdef(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[0]);
        bool isNdef = false;
        tagSession->IsNdef(tagRfDiscId, isNdef);
    }

    void FuzzSendRawFrame(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[0]);
        std::string hexCmdData = NfcSdkCommon::BytesVecToHexString(data, size);
        bool raw = data[0] % INT_TO_BOOL_DIVISOR;
        std::string hexRespData = NfcSdkCommon::BytesVecToHexString(data, size);
        tagSession->SendRawFrame(tagRfDiscId, hexCmdData, raw, hexRespData);
    }

    void FuzzNdefRead(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[0]);
        std::string ndefMessage = NfcSdkCommon::BytesVecToHexString(data, size);
        tagSession->NdefRead(tagRfDiscId, ndefMessage);
    }

    void FuzzNdefWrite(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[0]);
        std::string msg = NfcSdkCommon::BytesVecToHexString(data, size);
        tagSession->NdefWrite(tagRfDiscId, "");
        tagSession->NdefWrite(tagRfDiscId, msg);
    }

    void FuzzNdefMakeReadOnly(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[0]);
        tagSession->NdefMakeReadOnly(tagRfDiscId);
    }

    void FuzzFormatNdef(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[0]);
        std::string key = NfcSdkCommon::BytesVecToHexString(data, size);
        tagSession->FormatNdef(tagRfDiscId, key);
    }

    void FuzzCanMakeReadOnly(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[0]);
        bool canSetReadOnly = data[0] % INT_TO_BOOL_DIVISOR;
        tagSession->CanMakeReadOnly(tagRfDiscId, canSetReadOnly);
    }

    void FuzzGetMaxTransceiveLength(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int tagRfDiscId = static_cast<int>(data[0]);
        int maxSize = static_cast<int>(data[1]);
        tagSession->GetMaxTransceiveLength(tagRfDiscId, maxSize);
    }

    void FuzzIsSupportedApdusExtended(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        bool isSupported = data[0] % INT_TO_BOOL_DIVISOR;
        tagSession->IsSupportedApdusExtended(isSupported);
    }

    void FuzzIsSameAppAbility(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        ElementName fgElement1;
        ElementName fgElement2;
        tagSession->IsSameAppAbility(fgElement1, fgElement2);
        fgElement1.bundleName_ = std::string(reinterpret_cast<const char*>(data), size);
        fgElement1.bundleName_ = std::string(reinterpret_cast<const char*>(data), size);
        tagSession->IsSameAppAbility(fgElement1, fgElement2);
    }
 
    void FuzzRegForegroundDispatchInner(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
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
 
    void FuzzRegReaderModeInner(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
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
 
    void FuzzDump(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        int32_t fd = static_cast<int32_t>(data[0]);
        const std::vector<std::u16string> args;
        tagSession->Dump(fd, args);
    }

    void FuzzRegForegroundDispatch(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NfcService> nfcService = std::make_shared<NfcService>();
        nfcService->Initialize();
        std::shared_ptr<TAG::TagSession> tagSession = std::make_shared<TAG::TagSession>(nfcService);
        ElementName fgElement1;
        std::vector<uint32_t> discTech;
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        discTech.push_back(timeOutArray[0]);
        const sptr<KITS::IForegroundCallback> callback;
        tagSession->RegForegroundDispatch(fgElement1, discTech, callback);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzConnect(data, size);
    OHOS::FuzzIsConnected(data, size);
    OHOS::FuzzReconnect(data, size);
    OHOS::FuzzDisconnect(data, size);
    OHOS::FuzzSetTimeout(data, size);
    OHOS::FuzzGetTimeout(data, size);
    OHOS::FuzzResetTimeout(data, size);
    OHOS::FuzzGetTechList(data, size);
    OHOS::FuzzIsTagFieldOn(data, size);
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
    OHOS::FuzzRegForegroundDispatchInner(data, size);
    OHOS::FuzzRegReaderModeInner(data, size);
    OHOS::FuzzDump(data, size);
    OHOS::FuzzRegForegroundDispatch(data, size);
    return 0;
}