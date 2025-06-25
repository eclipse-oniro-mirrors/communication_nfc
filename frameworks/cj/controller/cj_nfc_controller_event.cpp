/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "cj_nfc_controller_event.h"

#include "cj_lambda.h"
#include "loghelper.h"
#include "nfc_controller.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace KITS {
const std::string EVENT_NFC_STATE_CHANGE = "nfcStateChange";

static std::set<std::string> g_supportEventList = {
    EVENT_NFC_STATE_CHANGE,
};

static std::mutex g_regInfoMutex;
static std::map<std::string, std::vector<std::tuple<int64_t, std::function<void(int32_t nfcState)>>>>
    g_eventRegisterInfo;

class NfcStateListenerEvent : public INfcControllerCallback {
public:
    NfcStateListenerEvent() {}

    virtual ~NfcStateListenerEvent() {}

public:
    void OnNfcStateChanged(int nfcState) override
    {
        InfoLog("OnNotify rcvd nfcRfState: %{public}d", nfcState);
        std::unique_lock<std::mutex> guard(g_regInfoMutex);
        auto iter = g_eventRegisterInfo.find(EVENT_NFC_STATE_CHANGE);
        if (iter == g_eventRegisterInfo.end()) {
            return;
        }
        for (auto cb : iter->second) {
            std::get<1>(cb)(nfcState);
        }
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

sptr<NfcStateListenerEvent> nfcStateListenerEvent = sptr<NfcStateListenerEvent>(new NfcStateListenerEvent());

void OnStateChange(int64_t callbackId)
{
    OHOS::DelayedSingleton<EventRegister>::GetInstance()->Register(EVENT_NFC_STATE_CHANGE, callbackId);
}

void OffStateChange(int64_t callbackId)
{
    OHOS::DelayedSingleton<EventRegister>::GetInstance()->Unregister(EVENT_NFC_STATE_CHANGE, callbackId);
}

void OffAllStateChange()
{
    OHOS::DelayedSingleton<EventRegister>::GetInstance()->Unregister(EVENT_NFC_STATE_CHANGE);
}

ErrorCode EventRegister::RegisterNfcStateChangedEvents(const std::string& type)
{
    NfcController nfcCtrl = NfcController::GetInstance();
    ErrorCode ret = nfcCtrl.RegListener(nfcStateListenerEvent, type);
    if (ret != KITS::ERR_NONE) {
        ErrorLog("RegisterNfcStateChangedEvents nfcListenerEvent failed!");
    }
    return ret;
}

ErrorCode EventRegister::UnRegisterNfcEvents(const std::string& type)
{
    NfcController nfcCtrl = OHOS::NFC::KITS::NfcController::GetInstance();
    ErrorCode ret = nfcCtrl.UnregListener(type);
    if (ret != KITS::ERR_NONE) {
        ErrorLog("UnRegisterNfcEvents nfcListenerEvent failed!");
    }
    return ret;
}

bool EventRegister::IsEventSupport(const std::string& type)
{
    return g_supportEventList.find(type) != g_supportEventList.end();
}

void EventRegister::Register(const std::string& type, int64_t callbackId)
{
    InfoLog("Register event: %{public}s", type.c_str());
    if (!IsEventSupport(type)) {
        ErrorLog("Register type error or not support!");
        return;
    }
    std::unique_lock<std::mutex> guard(g_regInfoMutex);
    RegisterNfcStateChangedEvents(type);
    auto func = reinterpret_cast<void (*)(int32_t nfcState)>(callbackId);
    auto cFunc = [lambda = CJLambda::Create(func)](int32_t nfcState) -> void { lambda(nfcState); };
    auto iter = g_eventRegisterInfo.find(type);
    if (iter == g_eventRegisterInfo.end()) {
        g_eventRegisterInfo[type] = std::vector<std::tuple<int64_t, std::function<void(int32_t nfcState)>>> {
            std::make_tuple(callbackId, cFunc)
        };
        return;
    }
    bool hasSameObj = false;
    for (auto value : iter->second) {
        if (callbackId == std::get<0>(value)) {
            WarnLog("handler function is same");
            hasSameObj = true;
            break;
        }
    }
    if (!hasSameObj) {
        iter->second.emplace_back(std::make_tuple(callbackId, cFunc));
    }
}

void EventRegister::Unregister(const std::string& type, int64_t callbackId)
{
    InfoLog("Unregister event: %{public}s", type.c_str());
    if (!IsEventSupport(type)) {
        ErrorLog("Unregister type error or not support!");
        return;
    }
    std::unique_lock<std::mutex> guard(g_regInfoMutex);
    auto iter = g_eventRegisterInfo.find(type);
    if (iter == g_eventRegisterInfo.end()) {
        WarnLog("Unregister type not registered!");
        if (UnRegisterNfcEvents(type) != KITS::ERR_NONE) {
            ErrorLog("UnRegisterNfcEvents failed.");
        }
        return;
    }
    iter->second.erase(std::remove_if(iter->second.begin(), iter->second.end(),
                           [callbackId](const std::tuple<int64_t, std::function<void(int32_t)>>& item) {
                               return std::get<0>(item) == callbackId;
                           }),
        iter->second.end());
    if (iter->second.empty()) {
        g_eventRegisterInfo.erase(iter);
        if (UnRegisterNfcEvents(type) != KITS::ERR_NONE) {
            ErrorLog("UnRegisterNfcEvents failed.");
        }
    }
}

void EventRegister::Unregister(const std::string& type)
{
    InfoLog("Unregister event: %{public}s", type.c_str());
    if (!IsEventSupport(type)) {
        ErrorLog("Unregister type error or not support!");
        return;
    }
    std::unique_lock<std::mutex> guard(g_regInfoMutex);
    auto iter = g_eventRegisterInfo.find(type);
    if (iter == g_eventRegisterInfo.end()) {
        WarnLog("Unregister type not registered!");
        if (UnRegisterNfcEvents(type) != KITS::ERR_NONE) {
            ErrorLog("UnRegisterNfcEvents failed.");
        }
        return;
    }
    g_eventRegisterInfo.erase(iter);
    if (UnRegisterNfcEvents(type) != KITS::ERR_NONE) {
        ErrorLog("UnRegisterNfcEvents failed.");
    }
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
