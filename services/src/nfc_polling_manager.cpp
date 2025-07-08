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
#include "nfc_polling_manager.h"
#include "common_event_support.h"
#include "loghelper.h"
#include "nfc_service.h"
#include "nfc_watch_dog.h"
#include "external_deps_proxy.h"

namespace OHOS {
namespace NFC {
NfcPollingManager::NfcPollingManager(std::weak_ptr<NfcService> nfcService,
                                     std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy,
                                     std::weak_ptr<NCI::INciTagInterface> nciTagProxy)
    : nfcService_(nfcService), nciNfccProxy_(nciNfccProxy), nciTagProxy_(nciTagProxy)
{
    foregroundData_ = std::make_shared<NfcPollingManager::ForegroundRegistryData>();
    readerModeData_ = std::make_shared<NfcPollingManager::ReaderModeRegistryData>();
    currPollingParams_ = NfcPollingParams::GetNfcOffParameters();
}

NfcPollingManager::~NfcPollingManager()
{
    foregroundData_ = nullptr;
    readerModeData_ = nullptr;
    currPollingParams_ = nullptr;
}

void NfcPollingManager::ResetCurrPollingParams()
{
    std::lock_guard<std::mutex> lock(mutex_);
    currPollingParams_ = std::make_shared<NfcPollingParams>();
}

std::shared_ptr<NfcPollingManager::ForegroundRegistryData> NfcPollingManager::GetForegroundData()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return foregroundData_;
}

std::shared_ptr<NfcPollingManager::ReaderModeRegistryData> NfcPollingManager::GetReaderModeData()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return readerModeData_;
}

std::shared_ptr<NfcPollingParams> NfcPollingManager::GetCurrentParameters()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return currPollingParams_;
}

std::string NfcPollingManager::GetForegroundAbility()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return fgAppAbilityName_;
}

std::shared_ptr<NfcPollingParams> NfcPollingManager::GetPollingParameters(int screenState)
{
    // Recompute polling parameters based on screen state
    std::shared_ptr<NfcPollingParams> params = std::make_shared<NfcPollingParams>();

    if (readerModeData_->isEnabled_) {
        params->SetTechMask(readerModeData_->techMask_);
        params->SetEnableReaderMode(true);
    } else {
        params->SetTechMask(NfcPollingParams::NFC_POLL_DEFAULT);
        params->SetEnableReaderMode(false);
    }
    return params;
}

void NfcPollingManager::StartPollingLoop(bool force)
{
    InfoLog("StartPollingLoop force = %{public}d", force);
    if (nfcService_.expired()) {
        ErrorLog("StartPollingLoop: nfcService_ is nullptr.");
        return;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("StartPollingLoop: NFC not enabled, do not Compute Routing Params.");
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    NfcWatchDog pollingWatchDog("StartPollingLoop", WAIT_MS_SET_ROUTE, nciNfccProxy_);
    pollingWatchDog.Run();
    // Compute new polling parameters
    std::shared_ptr<NfcPollingParams> newParams = GetPollingParameters(screenState_);
    InfoLog("newParams: %{public}s", newParams->ToString().c_str());
    InfoLog("currParams: %{public}s", currPollingParams_->ToString().c_str());
    if ((force || !(newParams == currPollingParams_)) && !nciNfccProxy_.expired()) {
        if (newParams->ShouldEnablePolling()) {
            bool shouldRestart = currPollingParams_->ShouldEnablePolling();
            InfoLog("StartPollingLoop shouldRestart = %{public}d", shouldRestart);

            nciNfccProxy_.lock()->EnableDiscovery(newParams->GetTechMask(),
                                                  newParams->ShouldEnableReaderMode(),
                                                  newParams->ShouldEnableHostRouting(),
                                                  shouldRestart || force);
        } else {
            nciNfccProxy_.lock()->DisableDiscovery();
        }
        currPollingParams_ = newParams;
    } else {
        InfoLog("StartPollingLoop: polling params equal, not updating");
    }
    pollingWatchDog.Cancel();
}

void NfcPollingManager::HandleScreenChanged(int screenState)
{
    std::lock_guard<std::mutex> lock(mutex_);
    screenState_ = screenState;
    InfoLog("Screen changed screenState %{public}d", screenState_);
    if (nciTagProxy_.expired() || nciNfccProxy_.expired()) {
        ErrorLog("nci proxy nullptr");
        return;
    }
    nciTagProxy_.lock()->StopFieldChecking();
    nciNfccProxy_.lock()->SetScreenStatus(screenState_);
}

bool NfcPollingManager::HandlePackageUpdated(std::shared_ptr<EventFwk::CommonEventData> data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (data == nullptr) {
        ErrorLog("data is null");
        return false;
    }
    std::string action = data->GetWant().GetAction();
    if (action.empty()) {
        ErrorLog("action is empty");
        return false;
    }
    if ((action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED) ||
        (action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED)) {
        return ExternalDepsProxy::GetInstance().HandleAppAddOrChangedEvent(data);
    } else if (action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED) {
        return ExternalDepsProxy::GetInstance().HandleAppRemovedEvent(data);
    } else {
        DebugLog("not need event.");
        return false;
    }
}

bool NfcPollingManager::EnableForegroundDispatch(AppExecFwk::ElementName &element,
    const std::vector<uint32_t> &discTech, const sptr<KITS::IForegroundCallback> &callback, bool isVendorApp)
{
    if (nfcService_.expired() || nciTagProxy_.expired()) {
        ErrorLog("EnableForegroundDispatch: nfcService_ is nullptr.");
        return false;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("EnableForegroundDispatch: NFC not enabled, do not set foreground");
        return false;
    }
    if (callback == nullptr) {
        ErrorLog("EnableForegroundDispatch: ForegroundCallback invalid");
        return false;
    }
    bool isDisablePolling = (discTech.size() == 0);
    DebugLog("EnableForegroundDispatch: element: %{public}s/%{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    if (!isDisablePolling) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            foregroundData_->isEnabled_ = true;
            foregroundData_->isVendorApp_ = isVendorApp;
            foregroundData_->techMask_ = nciTagProxy_.lock()->GetTechMaskFromTechList(discTech);
            foregroundData_->element_ = element;
            foregroundData_->callback_ = callback;
            if (fgAppBundleName_.empty()) {
                InfoLog("set fgAppBundleName");
                fgAppBundleName_ = element.GetBundleName();
            }
        }
        if (!nciNfccProxy_.expired()) {
            nciNfccProxy_.lock()->NotifyMessageToVendor(KITS::FOREGROUND_APP_KEY, element.GetBundleName());
        }
    }
    return true;
}

bool NfcPollingManager::DisableForegroundDispatch(const AppExecFwk::ElementName &element)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        DebugLog("DisableForegroundDispatch: element: %{public}s/%{public}s",
            element.GetBundleName().c_str(), element.GetAbilityName().c_str());
        foregroundData_->isEnabled_ = false;
        foregroundData_->isVendorApp_ = false;
        foregroundData_->techMask_ = 0xFFFF;
        foregroundData_->callerToken_ = 0;
        foregroundData_->callback_ = nullptr;
    }
    if (!nciNfccProxy_.expired()) {
        nciNfccProxy_.lock()->NotifyMessageToVendor(KITS::FOREGROUND_APP_KEY, "");
    }
    return true;
}

bool NfcPollingManager::DisableForegroundByDeathRcpt()
{
    return DisableForegroundDispatch(foregroundData_->element_);
}

bool NfcPollingManager::IsForegroundEnabled()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (foregroundData_ == nullptr) {
        ErrorLog("foregroundData_ nullptr");
        return false;
    }
    if (!foregroundData_->isEnabled_) {
        return false;
    }
    if (foregroundData_->isVendorApp_) {
        InfoLog("vendor app, skip foreground check");
        return true;
    }
    std::string bundleName = foregroundData_->element_.GetBundleName();
    if (bundleName != fgAppBundleName_) {
        WarnLog("IsForegroundEnabled %{public}s not foreground", bundleName.c_str());
        return false;
    }
    return true;
}

void NfcPollingManager::SendTagToForeground(KITS::TagInfoParcelable* tagInfo)
{
    if (tagInfo == nullptr) {
        ErrorLog("SendTagToForeground: tagInfo is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (foregroundData_->callback_ == nullptr) {
        ErrorLog("SendTagToForeground: invalid foreground state");
        return;
    }
    DebugLog("SendTagToForeground: OnTagDiscovered, tagInfo = %{public}s", tagInfo->ToString().c_str());
    foregroundData_->callback_->OnTagDiscovered(tagInfo);
}

bool NfcPollingManager::EnableReaderMode(AppExecFwk::ElementName &element, std::vector<uint32_t> &discTech,
    const sptr<KITS::IReaderModeCallback> &callback, bool isVendorApp)
{
    if (nfcService_.expired() || nciTagProxy_.expired()) {
        ErrorLog("EnableReaderMode: nfcService_ or nciTagProxy_ is nullptr.");
        return false;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("EnableReaderMode: NFC not enabled, do not set reader mode");
        return false;
    }
    if (callback == nullptr) {
        ErrorLog("EnableReaderMode: ReaderModeCallback invalid");
        return false;
    }
    bool isDisablePolling = (discTech.size() == 0);
    DebugLog("EnableReaderMode: element: %{public}s/%{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    if (!isDisablePolling) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            readerModeData_->isEnabled_ = true;
            readerModeData_->isVendorApp_ = isVendorApp;
            readerModeData_->techMask_ = nciTagProxy_.lock()->GetTechMaskFromTechList(discTech);
            readerModeData_->element_ = element;
            readerModeData_->callback_ = callback;
            if (fgAppBundleName_.empty()) {
                InfoLog("set fgAppBundleName");
                fgAppBundleName_ = element.GetBundleName();
            }
        }
        if (!nciNfccProxy_.expired()) {
            nciNfccProxy_.lock()->NotifyMessageToVendor(KITS::READERMODE_APP_KEY, element.GetBundleName());
        }
    }
    nciTagProxy_.lock()->StopFieldChecking();
    StartPollingLoop(true);
    return true;
}

bool NfcPollingManager::DisableReaderMode(AppExecFwk::ElementName &element)
{
    DebugLog("DisableReaderMode: element: %{public}s/%{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    {
        std::lock_guard<std::mutex> lock(mutex_);
        readerModeData_->isEnabled_ = false;
        readerModeData_->isVendorApp_ = false;
        readerModeData_->techMask_ = 0xFFFF;
        readerModeData_->callerToken_ = 0;
        readerModeData_->callback_ = nullptr;
    }
    if (!nciTagProxy_.expired()) {
        nciTagProxy_.lock()->StopFieldChecking();
    }
    if (!nciNfccProxy_.expired()) {
        nciNfccProxy_.lock()->NotifyMessageToVendor(KITS::READERMODE_APP_KEY, "");
    }
    StartPollingLoop(true);
    return true;
}

bool NfcPollingManager::DisableReaderModeByDeathRcpt()
{
    return DisableReaderMode(readerModeData_->element_);
}

bool NfcPollingManager::IsReaderModeEnabled()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (readerModeData_ == nullptr) {
        ErrorLog("readerModeData_ nullptr");
        return false;
    }
    if (!readerModeData_->isEnabled_) {
        return false;
    }
    if (readerModeData_->isVendorApp_) {
        InfoLog("vendor app, skip foreground check");
        return true;
    }
    std::string bundleName = readerModeData_->element_.GetBundleName();
    if (bundleName != fgAppBundleName_) {
        WarnLog("IsReaderModeEnabled %{public}s not foreground", bundleName.c_str());
        return false;
    }
    return true;
}

void NfcPollingManager::SendTagToReaderApp(KITS::TagInfoParcelable* tagInfo)
{
    if (tagInfo == nullptr) {
        ErrorLog("SendTagToReaderApp: tagInfo is nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (readerModeData_->callback_ == nullptr) {
        ErrorLog("SendTagToReaderApp: invalid readermode state");
        return;
    }
    DebugLog("SendTagToReaderApp: OnTagDiscovered, tagInfo = %{public}s", tagInfo->ToString().c_str());
    readerModeData_->callback_->OnTagDiscovered(tagInfo);
}

void NfcPollingManager::SetForegroundAbility(const std::string &bundleName, const std::string &abilityName,
    int abilityState)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (abilityState == static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND)) {
        DebugLog("SetForegroundAbility element: %{public}s/%{public}s", bundleName.c_str(), abilityName.c_str());
        fgAppBundleName_ = bundleName;
        fgAppAbilityName_ = abilityName;
    } else if (abilityState == static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND)
        || abilityState == static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_TERMINATED)) {
        if (fgAppBundleName_ == bundleName && fgAppAbilityName_ == abilityName) {
            DebugLog("SetForegroundAbility element: %{public}s/%{public}s is background", bundleName.c_str(),
                abilityName.c_str());
            fgAppBundleName_ = "";
            fgAppAbilityName_ = "";
        }
    }
}
} // namespace NFC
} // namespace OHOS