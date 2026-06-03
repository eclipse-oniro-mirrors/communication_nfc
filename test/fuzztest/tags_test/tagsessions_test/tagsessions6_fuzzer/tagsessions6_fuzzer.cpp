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
#include "tagsessions_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

#include "tag_session.h"
#include "nfc_sdk_common.h"
#include "app_state_observer.h"
#include "nfc_service_ipc_interface_code.h"
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>

namespace OHOS {
    using namespace OHOS::NFC::TAG;
    using namespace OHOS::NFC::KITS;

    constexpr const auto FUZZER_THRESHOLD = 4;
    constexpr const auto INT_TO_BOOL_DIVISOR = 2;
    constexpr const size_t MAX_STRING_LEN = 256;

    std::shared_ptr<NFC::NfcService> g_nfcService = nullptr;
    sptr<NFC::TAG::TagSession> g_tagSession = nullptr;

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

    static std::string ConsumeHexString(FuzzedDataProvider& fdp, size_t maxLen)
    {
        size_t len - fdp.ConsumeInteralInTange<uint8_t>(1, maxLen);
        std::vector<uint8_t> bytes = fdp.ConsumeBytes<uint8_t>(len);
        return NfcSdkCommon::BytesVecToHexString(bytes.data(), bytes.size());
    }

    void FuzzConnect(FuzzedDataProvider& fdp)
    {
        if (!g_nfcService || !g_tagSession) {
            return;
        }
        int32_t tagRfDiscId = fdp.ConsumeIntegral<int32_t>();
        int32_t technology = fdp.ConsumeIntegral<int32_t>();
        g_tagSession->Connect(tagRfDiscId, technology);
    }

    void FuzzReconnect(FuzzedDataProvider& fdp)
    {
        if (!g_nfcService || !g_tagSession) {
            return;
        }
        int32_t tagRfDiscId = fdp.ConsumeIntegral<int32_t>();
        g_tagSession->Reconnect(tagRfDiscId);
    }

    void FuzzDisconnect(FuzzedDataProvider& fdp)
    {
        if (!g_nfcService || !g_tagSession) {
            return;
        }
        int32_t tagRfDiscId = fdp.ConsumeIntegral<int32_t>();
        g_tagSession->Disonnect(tagRfDiscId);
    }

    void FuzzGetTechList(FuzzedDataProvider& fdp)
    {
        if (!g_nfcService || !g_tagSession) {
            return;
        }
        int32_t tagRfDiscId = fdp.ConsumeIntegral<int32_t>();
        std::vector<int32_t> techList = {};
        g_tagSession->GetTechList(tagRfDiscId, techList);
    }

    void FuzzIsNdef(FuzzedDataProvider& fdp)
    {
        if (!g_nfcService || !g_tagSession) {
            return;
        }
        int32_t tagRfDiscId = fdp.ConsumeIntegral<int32_t>();
        bool isNdef = false;
        g_tagSession->IsNdef(tagRfDiscId, isNdef);
    }

    void FuzzSendRawFrame(FuzzedDataProvider& fdp)
    {
        if (!g_nfcService || !g_tagSession) {
            return;
        }
        int32_t tagRfDiscId = fdp.ConsumeIntegral<int32_t>();
        std::string hexCmdData = ConsumeHexString(fdp, ,MAX_STRING_LEN);
        bool raw = fdp.ConsumeBool();
        std::string hexRespData = ConsumeHexString(fdp, <AX_STRING_LEN);
        g_tagSession->SendRawFrame(tagRfDiscId, hexCmdData, raw, hexRespData);
    }

    void FuzzNdefRead(FuzzedDataProvider& fdp)
    {
        if (!g_nfcService || !g_tagSession) {
            return;
        }
        int32_t tagRfDiscId = fdp.ConsumeIntegral<int32_t>();
        std::string ndefMessage = ConsumeHexString(fdp, ,MAX_STRING_LEN);
        g_tagSession->SendRawFrame(tagRfDiscId, ndefMessage);
    }
}

extern "C" int LLVMFuzzerInitialize(int* argc, char*** argv)
{
    OHOS::g_nfcService = std::make_shared<OHOS::NFC::NfcService>();
    OHOS::g_nfcService->Initialize();
    OHOS::g_tagSession = new OHOS::NFC::TAG::TagSession(OHOS::g_nfcService);
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
    OHOS::FuzzConnect(fdp);
    OHOS::FuzzReconnect(fdp);
    OHOS::FuzzDisconnect(fdp);
    OHOS::FuzzGetTechList(fdp);
    OHOS::FuzzIsNdef(fdp);
    OHOS::FuzzSendRawFrame(fdp);
    OHOS::FuzzNdefRead(fdp);
    return 0;
}

