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

#include "nfc_taihe_card_emulation_event.h"

#include <mutex>
#include <thread>

#include "hce_service.h"
#include "iservice_registry.h"
#include "loghelper.h"
#include "nfc_controller.h"

namespace OHOS {
namespace NFC {
namespace KITS {
constexpr const char* EVENT_TYPE_HCE_CMD = "hceCmd";
const uint16_t WAIT_SA_START_TIME = 3;

static std::mutex g_callbackMutex {};
static std::shared_ptr<::taihe::callback_view<void(uintptr_t err, ::taihe::array_view<uint8_t> data)>>
    g_hceCmdCallback = nullptr;
sptr<HceCmdListenerEvent> g_hceCmdListenerEvent = sptr<HceCmdListenerEvent>(new HceCmdListenerEvent());

void HceCmdListenerEvent::OnCeApduData(const std::vector<uint8_t>& data)
{
    InfoLog("data Length = %{public}zu", data.size());
    std::lock_guard<std::mutex> lock(g_callbackMutex);
    if (!g_hceCmdCallback) {
        ErrorLog("callback nullptr");
        return;
    }
    std::vector<uint8_t> apduData(data);
    taihe::array<uint8_t> taiheData(apduData); // initialize without "const"
    uintptr_t err = 0;
    (*g_hceCmdCallback)(err, taiheData);
}

OHOS::sptr<OHOS::IRemoteObject> HceCmdListenerEvent::AsObject()
{
    return nullptr;
}

NfcHceEventRegister& NfcHceEventRegister::GetInstance()
{
    static NfcHceEventRegister instance;
    return instance;
}

void NfcHceEventRegister::Register(
    std::string type, ::taihe::callback_view<void(uintptr_t err, ::taihe::array_view<uint8_t> data)> callback)
{
    if (type.c_str() != EVENT_TYPE_HCE_CMD) {
        ErrorLog("wrong type: %{public}s", type.c_str());
        return;
    }
    std::lock_guard<std::mutex> lock(g_callbackMutex);
    if (g_hceCmdCallback) {
        WarnLog("callback already registered.");
        return;
    }
    KITS::ErrorCode ret = KITS::HceService::GetInstance().RegHceCmdCallback(g_hceCmdListenerEvent, EVENT_TYPE_HCE_CMD);
    if (ret != KITS::ERR_NONE) {
        ErrorLog("Register failed!");
        return;
    }
    g_hceCmdCallback = std::make_shared<
        ::taihe::callback_view<void(uintptr_t err, ::taihe::array_view<uint8_t> data)>>(callback);
}

void NfcHceEventRegister::Unregister(std::string type)
{
    if (type.c_str() != EVENT_TYPE_HCE_CMD) {
        ErrorLog("wrong type: %{public}s", type.c_str());
        return;
    }
    KITS::ErrorCode ret = KITS::HceService::GetInstance().UnRegHceCmdCallback(
        g_hceCmdListenerEvent, EVENT_TYPE_HCE_CMD);
    if (ret != KITS::ERR_NONE) {
        ErrorLog("Unregister failed!");
        return;
    }
    std::lock_guard<std::mutex> lock(g_callbackMutex);
    g_hceCmdCallback = nullptr;
}

void NfcTaiheHceSAStatusChange::OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    // sleep 3s to wait Nfc turn on
    if (!g_hceCmdCallback) {
        return;
    }
    std::this_thread::sleep_for(std::chrono::seconds(WAIT_SA_START_TIME));
    std::lock_guard<std::mutex> guard(g_callbackMutex);
    bool isNfcOpen = false;
    NfcController::GetInstance().IsNfcOpen(isNfcOpen);
    if (isNfcOpen) {
        ErrorCode ret = HceService::GetInstance().RegHceCmdCallback(g_hceCmdListenerEvent, EVENT_TYPE_HCE_CMD);
        InfoLog("RegHceCmdCallback, statusCode = %{public}d", ret);
    }
}

void NfcTaiheHceSAStatusChange::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    InfoLog("systemAbilityId = %{public}d, ClearHceSessionProxy", systemAbilityId);
}

void NfcTaiheHceSAStatusChange::Init(int32_t systemAbilityId)
{
    sptr<ISystemAbilityManager> samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!samgrProxy) {
        ErrorLog("samgrProxy is nullptr");
        return;
    }
    int32_t ret = samgrProxy->SubscribeSystemAbility(systemAbilityId, this);
    InfoLog("systemAbilityId = %{public}d, ret = %{public}d", systemAbilityId, ret);
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
