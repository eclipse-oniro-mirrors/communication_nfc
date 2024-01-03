/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "app_state_observer.h"

#include "app_mgr_constants.h"
#include "iservice_registry.h"
#include "ability_manager_client.h"
#include "system_ability_definition.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
TAG::TagSession *tagSession_ = nullptr;
AppStateObserver::AppStateObserver(TAG::TagSession *tagSession)
{
    tagSession_ = tagSession;
    SubscribeAppState();
}

AppStateObserver::~AppStateObserver()
{
    UnSubscribeAppState();
}

bool AppStateObserver::SubscribeAppState()
{
    InfoLog("SubscribeAppState start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (appStateAwareObserver_) {
        ErrorLog("SubscribeAppState: appStateAwareObserver_ has register");
        return false;
    }
    if (!Connect()) {
        return false;
    }
    appStateAwareObserver_ = new (std::nothrow)AppStateAwareObserver();
    auto err = appMgrProxy_->RegisterApplicationStateObserver(appStateAwareObserver_);
    if (err != 0) {
        ErrorLog("SubscribeAppState error, code = %{public}d", err);
        appStateAwareObserver_ = nullptr;
        return false;
    }
    InfoLog("SubscribeAppState end");
    return true;
}

bool AppStateObserver::UnSubscribeAppState()
{
    InfoLog("UnSubscribeAppState start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (!appStateAwareObserver_) {
        ErrorLog("UnSubscribeAppState: appStateAwareObserver_ is nullptr");
        return false;
    }
    if (appMgrProxy_) {
        appMgrProxy_->UnregisterApplicationStateObserver(appStateAwareObserver_);
        appMgrProxy_ = nullptr;
        appStateAwareObserver_ = nullptr;
    }
    InfoLog("UnSubscribeAppState end");
    return true;
}

bool AppStateObserver::Connect()
{
    if (appMgrProxy_ != nullptr) {
        InfoLog("already connect");
        return true;
    }

    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        ErrorLog("get SystemAbilityManager failed");
        return false;
    }

    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(APP_MGR_SERVICE_ID);
    if (remoteObject == nullptr) {
        ErrorLog("get App Manager Service failed");
        return false;
    }

    appMgrProxy_ = iface_cast<AppExecFwk::IAppMgr>(remoteObject);
    if (!appMgrProxy_ || !appMgrProxy_->AsObject()) {
        ErrorLog("get app mgr proxy failed!");
        return false;
    }
    return true;
}

void AppStateObserver::AppStateAwareObserver::OnAbilityStateChanged(
    const AppExecFwk::AbilityStateData &abilityStateData)
{
    DebugLog("OnAbilityStateChanged: bundleName = %{public}s, abilityName = %{public}s, abilityState = %{public}d",
        abilityStateData.bundleName.c_str(), abilityStateData.abilityName.c_str(), abilityStateData.abilityState);
    if (tagSession_->GetFgDataVecSize() == 0) {
        return;
    }
    tagSession_->HandleAppStateChanged(abilityStateData.bundleName, abilityStateData.abilityName,
        abilityStateData.abilityState);
}
} // namespace NFC
} // namespace OHOS