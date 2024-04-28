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

std::shared_ptr<NfcPollingParams> NfcPollingManager::GetPollingParameters(int screenState)
{
    // Recompute polling parameters based on screen state
    std::shared_ptr<NfcPollingParams> params = std::make_shared<NfcPollingParams>();

    if (readerModeData_->isEnabled_) {
        params->SetTechMask(readerModeData_->techMask_);
        params->SetEnableReaderMode(true);
    } else if (foregroundData_->isEnabled_) {
        params->SetTechMask(foregroundData_->techMask_);
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
    if (force || !(newParams == currPollingParams_)) {
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
    DebugLog("Screen changed screenState %{public}d", screenState_);
    nciNfccProxy_.lock()->SetScreenStatus(screenState_);
}

bool NfcPollingManager::HandlePackageUpdated(std::shared_ptr<EventFwk::CommonEventData> data)
{
    std::lock_guard<std::mutex> lock(mutex_);
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
    const std::vector<uint32_t> &discTech, const sptr<KITS::IForegroundCallback> &callback)
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
        foregroundData_->isEnabled_ = true;
        foregroundData_->techMask_ = nciTagProxy_.lock()->GetTechMaskFromTechList(discTech);
        foregroundData_->element_ = element;
        foregroundData_->callback_ = callback;
    }
    return true;
}

bool NfcPollingManager::DisableForegroundDispatch(const AppExecFwk::ElementName &element)
{
    DebugLog("DisableForegroundDispatch: element: %{public}s/%{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    foregroundData_->isEnabled_ = false;
    foregroundData_->techMask_ = 0xFFFF;
    foregroundData_->callerToken_ = 0;
    foregroundData_->callback_ = nullptr;
    return true;
}

bool NfcPollingManager::DisableForegroundByDeathRcpt()
{
    return DisableForegroundDispatch(foregroundData_->element_);
}

bool NfcPollingManager::IsForegroundEnabled()
{
    return foregroundData_->isEnabled_;
}

void NfcPollingManager::SendTagToForeground(KITS::TagInfoParcelable* tagInfo)
{
    if (!IsForegroundEnabled() || foregroundData_->callback_ == nullptr) {
        ErrorLog("SendTagToForeground: invalid foreground state");
        return;
    }
    DebugLog("SendTagToForeground: OnTagDiscovered, tagInfo = %{public}s", tagInfo->ToString().c_str());
    foregroundData_->callback_->OnTagDiscovered(tagInfo);
}

bool NfcPollingManager::EnableReaderMode(AppExecFwk::ElementName &element, std::vector<uint32_t> &discTech,
    const sptr<KITS::IReaderModeCallback> &callback)
{
    if (nfcService_.expired() || nciTagProxy_.expired()) {
        ErrorLog("EnableReaderMode: nfcService_ is nullptr.");
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
        readerModeData_->isEnabled_ = true;
        readerModeData_->techMask_ = nciTagProxy_.lock()->GetTechMaskFromTechList(discTech);
        readerModeData_->element_ = element;
        readerModeData_->callback_ = callback;
    }
    nciTagProxy_.lock()->StopFieldChecking();
    StartPollingLoop(true);
    return true;
}

bool NfcPollingManager::DisableReaderMode(AppExecFwk::ElementName &element)
{
    DebugLog("DisableReaderMode: element: %{public}s/%{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    readerModeData_->isEnabled_ = false;
    readerModeData_->techMask_ = 0xFFFF;
    readerModeData_->callerToken_ = 0;
    readerModeData_->callback_ = nullptr;
    nciTagProxy_.lock()->StopFieldChecking();
    StartPollingLoop(true);
    return true;
}

bool NfcPollingManager::DisableReaderModeByDeathRcpt()
{
    return DisableReaderMode(readerModeData_->element_);
}

bool NfcPollingManager::IsReaderModeEnabled()
{
    return readerModeData_->isEnabled_;
}

void NfcPollingManager::SendTagToReaderApp(KITS::TagInfoParcelable* tagInfo)
{
    if (!IsReaderModeEnabled() || readerModeData_->callback_ == nullptr) {
        ErrorLog("SendTagToReaderApp: invalid readermode state");
        return;
    }
    DebugLog("SendTagToReaderApp: OnTagDiscovered, tagInfo = %{public}s", tagInfo->ToString().c_str());
    readerModeData_->callback_->OnTagDiscovered(tagInfo);
}
} // namespace NFC
} // namespace OHOS