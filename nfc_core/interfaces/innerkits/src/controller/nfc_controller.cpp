/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "nfc_controller.h"

#include "loghelper.h"
#include "nfc_controller_callback_stub.h"
#include "nfc_sdk_common.h"
#include "infc_controller_callback.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace NFC {
namespace KITS {
std::shared_ptr<OHOS::NFC::NfcControllerProxy> NfcController::nfcControllerProxy_;
std::weak_ptr<INfcControllerService> NfcController::nfcControllerService_;
bool NfcController::initialized_ = false;
std::mutex NfcController::mutex_;

NfcController::NfcController()
{
    DebugLog("[NfcController::NfcController] new ability manager");
}

NfcController::~NfcController()
{
    DebugLog("destruct NfcController");
}

void NfcController::InitNfcController()
{
    DebugLog("NfcController::InitNfcController in.");
    std::lock_guard<std::mutex> guard(mutex_);
    if (!initialized_ || nfcControllerService_.expired()) {
        sptr<ISystemAbilityManager> systemAbilityMgr =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        OHOS::sptr<OHOS::IRemoteObject> obj = systemAbilityMgr->GetSystemAbility(NFC_MANAGER_SYS_ABILITY_ID);
        if (obj == nullptr) {
            DebugLog("Nfc Controller Is Unexist.");
            return;
        }
        nfcControllerProxy_ = std::make_shared<NfcControllerProxy>(obj);
        nfcControllerService_ = nfcControllerProxy_;

        initialized_ = true;
    }

    DebugLog("NfcController::InitNfcController success.");
}

NfcController &NfcController::GetInstance()
{
    InfoLog("NfcController::GetInstance in.");
    InitNfcController();
    static NfcController instance;
    return instance;
}

// Open NFC
int NfcController::TurnOn()
{
    DebugLog("NfcController::TurnOn in.");
    if (nfcControllerService_.expired()) {
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    return nfcControllerService_.lock()->TurnOn();
}

// Close NFC
int NfcController::TurnOff()
{
    DebugLog("NfcController::TurnOff in.");
    if (nfcControllerService_.expired()) {
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    return nfcControllerService_.lock()->TurnOff();
}

// get NFC state
int NfcController::GetNfcState()
{
    if (nfcControllerService_.expired()) {
        return NfcState::STATE_OFF;
    }
    return nfcControllerService_.lock()->GetState();
}

// check whether NFC is supported
int NfcController::IsNfcAvailable()
{
    DebugLog("NfcController::IsNfcAvailable");
    return true;
}

// check whether NFC is enabled
int NfcController::IsNfcOpen()
{
    if (nfcControllerService_.expired()) {
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    DebugLog("NfcController::IsNfcOpen");
    return nfcControllerService_.lock()->IsNfcOpen();
}

// register NFC state change callback
NfcErrorCode NfcController::RegListener(const sptr<INfcControllerCallback> &callback,
    const std::string& type)
{
    DebugLog("NfcController::RegListener");
    return nfcControllerService_.lock()->RegisterCallBack(callback, type);
}

// unregister NFC state change
NfcErrorCode NfcController::UnregListener(const std::string& type)
{
    DebugLog("NfcController::UnregListener");
    return nfcControllerService_.lock()->UnRegisterCallBack(type);
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS