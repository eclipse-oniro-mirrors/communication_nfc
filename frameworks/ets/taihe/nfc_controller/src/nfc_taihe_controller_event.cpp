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

#include "nfc_taihe_controller_event.h"

#include <mutex>
#include <thread>

#include "iservice_registry.h"
#include "loghelper.h"
#include "nfc_controller.h"

namespace OHOS {
namespace NFC {
namespace KITS {
constexpr const char* EVENT_NFC_STATE_CHANGE = "nfcStateChange";
constexpr uint32_t WAIT_ON_REMOTE_DIED_MS = 20;
static std::mutex g_callbackMutex {};
static std::shared_ptr<
    taihe::callback_view<void(ohos::nfc::controller::nfcController::NfcState)>> g_stateCallback = nullptr;
sptr<NfcStateListenerEvent> nfcStateListenerEvent = sptr<NfcStateListenerEvent>(new NfcStateListenerEvent());

void NfcStateListenerEvent::OnNfcStateChanged(int nfcState)
{
    InfoLog("OnNotify rcvd nfcRfState: %{public}d", nfcState);
    std::lock_guard<std::mutex> lock(g_callbackMutex);
    if (g_stateCallback) {
        (*g_stateCallback)(static_cast<ohos::nfc::controller::nfcController::NfcState::key_t>(nfcState));
    }
}

OHOS::sptr<OHOS::IRemoteObject> NfcStateListenerEvent::AsObject()
{
    return nullptr;
}

NfcStateEventRegister& NfcStateEventRegister::GetInstance()
{
    static NfcStateEventRegister instance;
    return instance;
}

void NfcStateEventRegister::Register(
    taihe::callback_view<void(ohos::nfc::controller::nfcController::NfcState)> callback)
{
    std::lock_guard<std::mutex> lock(g_callbackMutex);
    if (g_stateCallback) {
        WarnLog("callback already registered.");
        return;
    }
    ErrorCode ret = NfcController::GetInstance().RegListener(nfcStateListenerEvent, EVENT_NFC_STATE_CHANGE);
    if (ret != KITS::ERR_NONE) {
        ErrorLog("Register failed!");
        return;
    }
    g_stateCallback =
        std::make_shared<taihe::callback_view<void(ohos::nfc::controller::nfcController::NfcState)>>(callback);
}

void NfcStateEventRegister::Unregister()
{
    ErrorCode ret = NfcController::GetInstance().UnregListener(EVENT_NFC_STATE_CHANGE);
    if (ret != KITS::ERR_NONE) {
        ErrorLog("Unregister failed!");
        return;
    }
    std::lock_guard<std::mutex> lock(g_callbackMutex);
    g_stateCallback = nullptr;
}

void NfcTaiheSAStatusChange::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    InfoLog("OnAddSystemAbility, systemAbilityId = %{public}d", systemAbilityId);
    std::lock_guard<std::mutex> lock(g_callbackMutex);
    if (g_stateCallback) {
        InfoLog("OnAddSystemAbility g_stateCallback is not null");
        // sleep 20ms for waitting recv OnRemoteDied msg, to reset nfc proxy.
        std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_ON_REMOTE_DIED_MS));
        NfcController::GetInstance().RegListener(nfcStateListenerEvent, EVENT_NFC_STATE_CHANGE);
    } else {
        WarnLog("OnAddSystemAbility g_stateCallback is null");
    }
}

void NfcTaiheSAStatusChange::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    InfoLog("OnRemoveSystemAbility, systemAbilityId = %{public}d", systemAbilityId);
}

void NfcTaiheSAStatusChange::Init(int32_t systemAbilityId)
{
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgrProxy) {
        ErrorLog("samgrProxy is nullptr");
        return;
    }
    int32_t ret = samgrProxy->SubscribeSystemAbility(systemAbilityId, this);
    InfoLog("SubscribeSystemAbility, systemAbilityId = %{public}d, ret = %{public}d.", systemAbilityId, ret);
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
