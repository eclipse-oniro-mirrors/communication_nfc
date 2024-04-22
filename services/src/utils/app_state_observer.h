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

#ifndef OHOS_NFC_APP_STATE_OBSERVER_H
#define OHOS_NFC_APP_STATE_OBSERVER_H

#include "app_mgr_interface.h"
#include "application_state_observer_stub.h"
#include "iremote_object.h"
#include "infc_app_state_observer.h"

namespace OHOS {
namespace NFC {
class AppStateObserver {
public:
    explicit AppStateObserver(INfcAppStateObserver *nfcAppStateChangeCallback);
    ~AppStateObserver();
    AppStateObserver(const AppStateObserver &) = delete;
    AppStateObserver &operator=(const AppStateObserver &) = delete;
    bool SubscribeAppState();
    bool UnSubscribeAppState();
    bool IsForegroundApp(const std::string &bundleName);

private:
    class AppStateAwareObserver : public AppExecFwk::ApplicationStateObserverStub {
    public:
        void OnProcessDied(const AppExecFwk::ProcessData &processData) override;
        void OnAbilityStateChanged(const AppExecFwk::AbilityStateData &abilityStateData) override;
        void OnForegroundApplicationChanged(const AppExecFwk::AppStateData &appStateData) override;
        bool RegisterAppStateChangeCallback(INfcAppStateObserver *nfcAppStateChangeCallback);

    private:
        INfcAppStateObserver *nfcAppStateChangeCallback_;
    };
    bool Connect();
    bool RegisterAppStateChangeCallback(INfcAppStateObserver *nfcAppStateChangeCallback);

private:
    std::mutex mutex_{};
    sptr<AppExecFwk::IAppMgr> appMgrProxy_{nullptr};
    sptr<AppStateAwareObserver> appStateAwareObserver_;
};
} //namespace NFC
} // namespace OHOS
#endif // OHOS_NFC_APP_STATE_OBSERVER_H