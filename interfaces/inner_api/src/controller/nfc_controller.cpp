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
            ErrorLog("Nfc Controller Is Unexist.");
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
    DebugLog("NfcController::GetInstance in.");
    InitNfcController();
    static NfcController instance;
    return instance;
}

// Open NFC
int NfcController::TurnOn()
{
    if (nfcControllerService_.expired()) {
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    return nfcControllerService_.lock()->TurnOn();
}

// Close NFC
int NfcController::TurnOff()
{
    if (nfcControllerService_.expired()) {
        return ErrorCode::ERR_NFC_STATE_UNBIND;
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
bool NfcController::IsNfcAvailable()
{
    return true;
}

// check whether NFC is enabled
int NfcController::IsNfcOpen(bool &isOpen)
{
    if (nfcControllerService_.expired()) {
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    return nfcControllerService_.lock()->IsNfcOpen(isOpen);
}

// register NFC state change callback
ErrorCode NfcController::RegListener(const sptr<INfcControllerCallback> &callback,
    const std::string& type)
{
    DebugLog("NfcController::RegListener");
    return nfcControllerService_.lock()->RegisterCallBack(callback, type);
}

// unregister NFC state change
ErrorCode NfcController::UnregListener(const std::string& type)
{
    DebugLog("NfcController::UnregListener");
    return nfcControllerService_.lock()->UnRegisterCallBack(type);
}

OHOS::sptr<IRemoteObject> NfcController::GetTagServiceIface()
{
    return nfcControllerService_.lock()->GetTagServiceIface();
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS