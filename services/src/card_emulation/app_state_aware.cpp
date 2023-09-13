/*
 * Copyright (C) 2023-2023 Huawei Device Co., Ltd.
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

#include "app_state_aware.h"

#include "app_mgr_constants.h"
#include "iservice_registry.h"
#include "ability_manager_client.h"
#include "system_ability_definition.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {

AppStateAware::AppStateAware()
{
    SubscribeAppState();
    foregroundAppBundleName_ = GetForegroundApp();
}

AppStateAware::~AppStateAware()
{
    UnSubscribeAppState();
}

AppStateAware *AppStateAware::GetInstance()
{
    static AppStateAware instance;
    return &instance;
}

bool AppStateAware::SubscribeAppState()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!Connect()) {
        return false;
    }
    appMgrProxy_->RegisterApplicationStateObserver(iface_cast<AppExecFwk::IApplicationStateObserver>(this));
    return true;
}

bool AppStateAware::UnSubscribeAppState()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!Connect()) {
        return false;
    }
    appMgrProxy_->UnregisterApplicationStateObserver(iface_cast<AppExecFwk::IApplicationStateObserver>(this));
    return true;
}

bool AppStateAware::Connect()
{
    if (appMgrProxy_ != nullptr) {
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

void AppStateAware::OnForegroundApplicationChanged(const AppExecFwk::AppStateData &appStateData)
{
    InfoLog("name = %{public}s, state = %{public}d", appStateData.bundleName.c_str(), appStateData.state);
    if (!ValidateAppStateData(appStateData)) {
        ErrorLog("%{public}s : validate app state data failed!", __func__);
        return;
    }

    if (appStateData.state == static_cast<int32_t>(AppExecFwk::ApplicationState::APP_STATE_FOREGROUND)) {
        foregroundAppBundleName_ = appStateData.bundleName;
        InfoLog("foregroundAppBundleName_ = %{public}s", foregroundAppBundleName_.c_str());
    } else if (appStateData.state == static_cast<int32_t>(AppExecFwk::ApplicationState::APP_STATE_BACKGROUND) &&
        foregroundAppBundleName_ == appStateData.bundleName) {
        foregroundAppBundleName_ = "";
        InfoLog("foregroundAppBundleName_ = %{public}s", foregroundAppBundleName_.c_str());
    } else {
        InfoLog("state = %{public}d, not handle", appStateData.state);
    }
}

std::string AppStateAware::GetForegroundApp()
{
    if (!Connect()) {
        return "";
    }
    std::vector<AppExecFwk::AppStateData> fgAppList;
    appMgrProxy_->GetForegroundApplications(fgAppList);
    if (fgAppList.size() > 0) {
        InfoLog("fgApp: %{public}s, state = %{public}d", fgAppList[0].bundleName.c_str(), fgAppList[0].state);
        return fgAppList[0].bundleName;
    }
    return "";
}

inline bool AppStateAware::ValidateAppStateData(const AppExecFwk::AppStateData &appStateData)
{
    return appStateData.uid > 0 && appStateData.bundleName.length() > 0;
}

bool AppStateAware::IsForegroundApp(std::string bundleName)
{
    return bundleName == foregroundAppBundleName_;
}
} // namespace NFC
} // namespace OHOS