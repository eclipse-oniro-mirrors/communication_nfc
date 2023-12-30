/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "nfc_sa_manager.h"
#include "loghelper.h"
#include "system_ability_definition.h"
#include "external_deps_proxy.h"

namespace OHOS {
namespace NFC {
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(DelayedSingleton<NfcSaManager>::GetInstance().get());

NfcSaManager::NfcSaManager() : SystemAbility(NFC_MANAGER_SYS_ABILITY_ID, false) {}

NfcSaManager::~NfcSaManager()
{
    if (nfcService_) {
        nfcService_ = nullptr;
    }
}

void NfcSaManager::OnStart()
{
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        InfoLog("NfcSaManager has already started.");
        return;
    }

    if (!Init()) {
        InfoLog("failed to init NfcSaManager");
        // record init sa failed event.
        NfcFailedParams err;
        ExternalDepsProxy::GetInstance().BuildFailedParams(err, MainErrorCode::INIT_SA_FAILED,
            SubErrorCode::DEFAULT_ERR_DEF);
        ExternalDepsProxy::GetInstance().WriteNfcFailedHiSysEvent(&err);
        return;
    }
    state_ = ServiceRunningState::STATE_RUNNING;
    InfoLog("NfcSaManager::OnStart start service success.");
}

bool NfcSaManager::Init()
{
    std::lock_guard<std::mutex> guard(initMutex_);
    InfoLog("NfcSaManager::Init ready to init.");
    if (!registerToService_) {
        nfcService_ = std::make_shared<NfcService>();
        nfcService_->Initialize();
        bool ret = Publish(nfcService_->nfcControllerImpl_);
        if (ret) {
            InfoLog("NfcSaManager::Init Add System Ability SUCCESS!");
        } else {
            ErrorLog("NfcSaManager::Init Add System Ability FAILED!");
            return false;
        }
        AddSystemAbilityListener(COMMON_EVENT_SERVICE_ID);
        registerToService_ = true;
    }
    InfoLog("NfcSaManager::Init init success.");
    return true;
}

void NfcSaManager::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    InfoLog("OnAddSystemAbility systemAbilityId:%{public}d added!", systemAbilityId);
    if (systemAbilityId != COMMON_EVENT_SERVICE_ID) {
        InfoLog("OnAddSystemAbility systemAbilityId is not COMMON_EVENT_SERVICE_ID");
        return;
    }
    if (nfcService_ == nullptr) {
        ErrorLog("nfcService_ is nullptr");
        return;
    }
    if (nfcService_->eventHandler_ == nullptr) {
        ErrorLog("eventHandler_ is nullptr");
        return;
    }
    nfcService_->eventHandler_->SubscribePackageChangedEvent();
    nfcService_->eventHandler_->SubscribeScreenChangedEvent();
}

void NfcSaManager::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId)
{
    InfoLog("NfcSaManager OnRemoveSystemAbility finish");
}

void NfcSaManager::OnStop()
{
    InfoLog("NfcSaManager::OnStop ready to stop service.");
    state_ = ServiceRunningState::STATE_NOT_START;
    registerToService_ = false;
    InfoLog("NfcSaManager::OnStop stop service success.");
}
}  // namespace NFC
}  // namespace OHOS
