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
#include "ce_service.h"
#include "nfc_event_publisher.h"
#include "nfc_event_handler.h"
#include "external_deps_proxy.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "setting_data_share_impl.h"
#include "accesstoken_kit.h"
#include "hap_token_info.h"

namespace OHOS {
namespace NFC {
const int FIELD_COMMON_EVENT_INTERVAL = 1000;
const int DEACTIVATE_TIMEOUT = 6000;
static const int DEFAULT_HOST_ROUTE_DEST = 0x00;
static const int PWR_STA_SWTCH_ON_SCRN_UNLCK = 0x01;
static const int DEFAULT_PWR_STA_HOST = PWR_STA_SWTCH_ON_SCRN_UNLCK;
const std::string APP_REMOVED = "app_removed";
const std::string APP_ADDED = "app_added";

CeService::CeService(std::weak_ptr<NfcService> nfcService, std::weak_ptr<NCI::INciCeInterface> nciCeProxy)
    : nfcService_(nfcService), nciCeProxy_(nciCeProxy)

{
    Uri nfcDefaultPaymentApp(KITS::NFC_DATA_URI_PAYMENT_DEFAULT_APP);
    DelayedSingleton<SettingDataShareImpl>::GetInstance()->GetElementName(
        nfcDefaultPaymentApp, KITS::DATA_SHARE_KEY_NFC_PAYMENT_DEFAULT_APP, defaultPaymentElement_);

    appStateObserver_ = std::make_shared<AppStateObserver>(this);
    DebugLog("CeService constructor end");
}

CeService::~CeService()
{
    hostCardEmulationManager_ = nullptr;
    aidToAidEntry_.clear();
    foregroundElement_.SetBundleName("");
    foregroundElement_.SetAbilityName("");
    foregroundElement_.SetDeviceID("");
    foregroundElement_.SetModuleName("");
    defaultPaymentElement_.SetBundleName("");
    defaultPaymentElement_.SetAbilityName("");
    defaultPaymentElement_.SetDeviceID("");
    defaultPaymentElement_.SetModuleName("");
    dynamicAids_.clear();
    DebugLog("CeService deconstructor end");
}

void CeService::PublishFieldOnOrOffCommonEvent(bool isFieldOn)
{
    ExternalDepsProxy::GetInstance().PublishNfcFieldStateChanged(isFieldOn);
}

bool CeService::RegHceCmdCallback(const sptr<KITS::IHceCmdCallback> &callback, const std::string &type,
                                  Security::AccessToken::AccessTokenID callerToken)
{
    if (hostCardEmulationManager_ == nullptr) {
        ErrorLog("hce is null");
        return false;
    }
    return hostCardEmulationManager_->RegHceCmdCallback(callback, type, callerToken);
}

bool CeService::UnRegHceCmdCallback(const std::string &type, Security::AccessToken::AccessTokenID callerToken)
{
    if (hostCardEmulationManager_ == nullptr) {
        ErrorLog("hce is null");
        return false;
    }
    return hostCardEmulationManager_->UnRegHceCmdCallback(type, callerToken);
}

bool CeService::UnRegAllCallback(Security::AccessToken::AccessTokenID callerToken)
{
    if (hostCardEmulationManager_ == nullptr) {
        ErrorLog("hce is null");
        return false;
    }
    return hostCardEmulationManager_->UnRegAllCallback(callerToken);
}

bool CeService::IsDefaultService(ElementName &element, const std::string &type)
{
    return type == KITS::TYPE_PAYMENT && element.GetBundleName() == defaultPaymentElement_.GetBundleName() &&
           element.GetAbilityName() == defaultPaymentElement_.GetAbilityName();
}

bool CeService::SendHostApduData(const std::string &hexCmdData, bool raw, std::string &hexRespData,
                                 Security::AccessToken::AccessTokenID callerToken)
{
    if (hostCardEmulationManager_ == nullptr) {
        ErrorLog("hce is null");
        return false;
    }
    return hostCardEmulationManager_->SendHostApduData(hexCmdData, raw, hexRespData, callerToken);
}

bool CeService::InitConfigAidRouting()
{
    DebugLog("AddAidRoutingHceAids: start");
    std::lock_guard<std::mutex> lock(configRoutingMutex_);
    std::map<std::string, AidEntry> aidEntries;
    BuildAidEntries(aidEntries);
    InfoLog("AddAidRoutingHceAids, aid entries cache size %{public}zu,aid entries newly builded size %{public}zu",
            aidToAidEntry_.size(), aidEntries.size());
    if (aidEntries == aidToAidEntry_) {
        InfoLog("aid entries do not change.");
        return false;
    }

    nciCeProxy_.lock()->ClearAidTable();
    aidToAidEntry_.clear();
    bool addAllResult = true;
    for (const auto &pair : aidEntries) {
        AidEntry entry = pair.second;
        std::string aid = entry.aid;
        int aidInfo = entry.aidInfo;
        int power = entry.power;
        int route = entry.route;
        InfoLog("AddAidRoutingHceAids: aid= %{public}s, aidInfo= "
                "0x%{public}x, route=0x%{public}x, power=0x%{public}x",
                aid.c_str(), aidInfo, route, power);
        bool addResult = nciCeProxy_.lock()->AddAidRouting(aid, route, aidInfo, power);
        if (!addResult) {
            ErrorLog("AddAidRoutingHceAids: add aid failed aid= %{public}s", aid.c_str());
            addAllResult = false;
        }
    }
    if (addAllResult) {
        InfoLog("AddAidRoutingHceAids: add aids success, update the aid entries cache");
        aidToAidEntry_ = std::move(aidEntries);
    }
    DebugLog("AddAidRoutingHceAids: end");
    return true;
}

void CeService::HandleAppStateChanged(const std::string &bundleName, const std::string &abilityName,
                                      int abilityState)
{
    if (bundleName.empty()) {
        ErrorLog("OnForegroundApplicationChanged bundle name is empty.");
        return;
    }

    if (bundleName != foregroundElement_.GetBundleName()) {
        InfoLog("OnForegroundApplicationChanged not equal to the foreground element, no need to handle.");
        return;
    }
    if (abilityState == static_cast<int32_t>(AppExecFwk::ApplicationState::APP_STATE_FOREGROUND)) {
        InfoLog("OnForegroundApplicationChanged foreground state, no need to handle.");
        return;
    }

    ClearHceInfo();
    InfoLog("foreground app state change: refresh route table");
    ConfigRoutingAndCommit();
}

void CeService::BuildAidEntries(std::map<std::string, AidEntry> &aidEntries)
{
    std::vector<AppDataParser::HceAppAidInfo> hceApps;
    ExternalDepsProxy::GetInstance().GetHceApps(hceApps);
    InfoLog("AddAidRoutingHceOtherAids: hce apps size %{public}zu", hceApps.size());
    for (const AppDataParser::HceAppAidInfo &appAidInfo : hceApps) {
        bool isForeground = appAidInfo.element.GetBundleName() == foregroundElement_.GetBundleName() &&
                           appAidInfo.element.GetAbilityName() == foregroundElement_.GetAbilityName();
        bool isDefaultPayment = appAidInfo.element.GetBundleName() == defaultPaymentElement_.GetBundleName() &&
                                appAidInfo.element.GetAbilityName() == defaultPaymentElement_.GetAbilityName();
        for (const AppDataParser::AidInfo &aidInfo : appAidInfo.customDataAid) {
            // add payment aid of default payment app and foreground app
            // add other aid of all apps
            bool shouldAdd = KITS::KEY_OHTER_AID == aidInfo.name || isForeground || isDefaultPayment;
            if (shouldAdd) {
                AidEntry aidEntry;
                aidEntry.aid = aidInfo.value;
                aidEntry.aidInfo = 0;
                aidEntry.power = DEFAULT_PWR_STA_HOST;
                aidEntry.route = DEFAULT_HOST_ROUTE_DEST;
                aidEntries[aidInfo.value] = aidEntry;
            }
        }
    }
    for (const std::string &aid : dynamicAids_) {
        AidEntry aidEntry;
        aidEntry.aid = aid;
        aidEntry.aidInfo = 0;
        aidEntry.power = DEFAULT_PWR_STA_HOST;
        aidEntry.route = DEFAULT_HOST_ROUTE_DEST;
        aidEntries[aid] = aidEntry;
    }
}

void CeService::ClearAidEntriesCache()
{
    std::lock_guard<std::mutex> lock(configRoutingMutex_);
    aidToAidEntry_.clear();
    DebugLog("ClearAidEntriesCache end");
}

bool CeService::IsDynamicAid(const std::string &targetAid)
{
    for (const std::string &aid : dynamicAids_) {
        if (aid == targetAid) {
            return true;
        }
    }
    return false;
}

void CeService::OnDefaultPaymentServiceChange()
{
    ElementName newElement;
    Uri nfcDefaultPaymentApp(KITS::NFC_DATA_URI_PAYMENT_DEFAULT_APP);
    DelayedSingleton<SettingDataShareImpl>::GetInstance()->GetElementName(
        nfcDefaultPaymentApp, KITS::DATA_SHARE_KEY_NFC_PAYMENT_DEFAULT_APP, newElement);
    if (newElement.GetURI() == defaultPaymentElement_.GetURI()) {
        InfoLog("OnDefaultPaymentServiceChange: payment service not change");
        return;
    }

    if (nfcService_.expired()) {
        ErrorLog("nfcService_ is nullptr.");
        return;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("NFC not enabled, should not happen.The default payment app is be set when nfc is enabled.");
        return;
    }
    ExternalDepsProxy::GetInstance().WriteDefaultPaymentAppChangeHiSysEvent(defaultPaymentElement_.GetBundleName(),
                                                                            newElement.GetBundleName());
    UpdateDefaultPaymentElement(newElement);
    InfoLog("OnDefaultPaymentServiceChange: refresh route table");
    ConfigRoutingAndCommit();
}
void CeService::OnAppAddOrChangeOrRemove(std::shared_ptr<EventFwk::CommonEventData> data)
{
    DebugLog("OnAppAddOrChangeOrRemove start");

    if (!AppEventCheckValid(data)) {
        return;
    }

    std::string action = data->GetWant().GetAction();
    ElementName element = data->GetWant().GetElement();
    std::string bundleName = element.GetBundleName();

    InfoLog("OnAppAddOrChangeOrRemove: change bundleName %{public}s, default payment bundle name %{public}s, "
            "installed status %{public}d",
            bundleName.c_str(), defaultPaymentElement_.GetBundleName().c_str(),
            defaultPaymentBundleInstalled_);

    if (bundleName == defaultPaymentElement_.GetBundleName() &&
        action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED) {
        UpdateDefaultPaymentBundleInstalledStatus(false);
        ExternalDepsProxy::GetInstance().WriteDefaultPaymentAppChangeHiSysEvent(
            defaultPaymentElement_.GetBundleName(), APP_REMOVED);
    }

    if (bundleName == defaultPaymentElement_.GetBundleName() &&
        action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED) {
        UpdateDefaultPaymentBundleInstalledStatus(true);
        ExternalDepsProxy::GetInstance().WriteDefaultPaymentAppChangeHiSysEvent(
            defaultPaymentElement_.GetBundleName(), APP_ADDED);
    }

    if (nfcService_.expired()) {
        ErrorLog("nfcService_ is nullptr.");
        return;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog(" NFC not enabled, not need to update routing entry ");
        return;
    }
    InfoLog("OnAppAddOrChangeOrRemove: refresh route table");
    ConfigRoutingAndCommit();
    DebugLog("OnAppAddOrChangeOrRemove end");
}

bool CeService::AppEventCheckValid(std::shared_ptr<EventFwk::CommonEventData> data)
{
    if (data == nullptr) {
        ErrorLog("invalid event data");
        return false;
    }
    std::string action = data->GetWant().GetAction();
    if (action.empty()) {
        ErrorLog("action is empty");
        return false;
    }
    if ((action != EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED) &&
        (action != EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED) &&
        (action != EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED)) {
        InfoLog("not the interested action");
        return false;
    }

    ElementName element = data->GetWant().GetElement();
    std::string bundleName = element.GetBundleName();
    if (bundleName.empty()) {
        ErrorLog("invalid bundleName.");
        return false;
    }
    return true;
}
void CeService::UpdateDefaultPaymentBundleInstalledStatus(bool installed)
{
    InfoLog("UpdateDefaultPaymentBundleInstalledStatus: bundleName %{public}d", installed);
    std::lock_guard<std::mutex> lock(configRoutingMutex_);
    defaultPaymentBundleInstalled_ = installed;
}

void CeService::UpdateDefaultPaymentElement(const ElementName &element)
{
    InfoLog("UpdateDefaultPaymentElement: bundleName %{public}s", element.GetURI().c_str());
    std::lock_guard<std::mutex> lock(configRoutingMutex_);
    defaultPaymentElement_ = element;
    defaultPaymentBundleInstalled_ = true;
}
KITS::DefaultPaymentType CeService::GetDefaultPaymentType()
{
    InfoLog("GetDefaultPaymentType: default payment bundle name %{public}s, "
            "installed status %{public}d",
            defaultPaymentElement_.GetBundleName().c_str(), defaultPaymentBundleInstalled_);

    if (defaultPaymentElement_.GetBundleName().empty()) {
        return KITS::DefaultPaymentType::TYPE_EMPTY;
    }
    if (!defaultPaymentBundleInstalled_) {
        return KITS::DefaultPaymentType::TYPE_UNINSTALLED;
    }
    if (defaultPaymentElement_.GetBundleName() ==  nciCeProxy_.lock()->GetSimVendorBundleName()) {
        return KITS::DefaultPaymentType::TYPE_UICC;
    }
    if (ExternalDepsProxy::GetInstance().IsHceApp(defaultPaymentElement_)) {
        return KITS::DefaultPaymentType::TYPE_HCE;
    }

    return KITS::DefaultPaymentType::TYPE_ESE;
}

void CeService::ConfigRoutingAndCommit()
{
    if (nfcService_.expired()) {
        ErrorLog("ConfigRoutingAndCommit: nfc service is null");
        return;
    }
    std::weak_ptr<NfcRoutingManager> routingManager = nfcService_.lock()->GetNfcRoutingManager();
    if (routingManager.expired()) {
        ErrorLog("ConfigRoutingAndCommit: routing manager is null");
        return;
    }

    bool updateAids = InitConfigAidRouting();
    bool updatePaymentType = UpdateDefaultPaymentType();
    InfoLog(
        "ConfigRoutingAndCommit: aids updated status %{public}d, default payment type updated status %{public}d.",
        updateAids, updatePaymentType);
    if (updateAids || updatePaymentType) {
        routingManager.lock()->ComputeRoutingParams(defaultPaymentType_);
        routingManager.lock()->CommitRouting();
    }
}

void CeService::SearchElementByAid(const std::string &aid, ElementName &aidElement)
{
    if (aid.empty()) {
        InfoLog("aid is empty");
        return;
    }
    // find dynamic aid
    if (IsDynamicAid(aid) && !foregroundElement_.GetBundleName().empty()) {
        InfoLog("is foreground element");
        aidElement.SetBundleName(foregroundElement_.GetBundleName());
        aidElement.SetAbilityName(foregroundElement_.GetAbilityName());
        return;
    }
    std::vector<AppDataParser::HceAppAidInfo> hceApps;
    ExternalDepsProxy::GetInstance().GetHceAppsByAid(aid, hceApps);
    if (hceApps.empty()) {
        InfoLog("No applications found");
        return;
    }
    // only one element, resolved
    if (hceApps.size() == 1) {
        ElementName element = hceApps[0].element;
        aidElement.SetBundleName(element.GetBundleName());
        aidElement.SetAbilityName(element.GetAbilityName());
        return;
    }
    InfoLog("Found too many applications");
    for (const AppDataParser::HceAppAidInfo &hceApp : hceApps) {
        ElementName elementName = hceApp.element;
        InfoLog("ElementName: %{public}s", elementName.GetBundleName().c_str());
        InfoLog("ElementValue: %{public}s", elementName.GetAbilityName().c_str());

        bool isForeground = elementName.GetBundleName() == foregroundElement_.GetBundleName() &&
                           elementName.GetAbilityName() == foregroundElement_.GetAbilityName();
        bool isDefaultPayment = elementName.GetBundleName() == defaultPaymentElement_.GetBundleName() &&
                                elementName.GetAbilityName() == defaultPaymentElement_.GetAbilityName();
        if (isForeground) {
            // is foregroud, resolved
            InfoLog("is foreground element");
            aidElement.SetBundleName(elementName.GetBundleName());
            aidElement.SetAbilityName(elementName.GetAbilityName());
            return;
        } else if (isDefaultPayment && IsPaymentAid(aid, hceApp)) {
            // is default payment, resolved
            InfoLog("is default payment element");
            aidElement.SetBundleName(elementName.GetBundleName());
            aidElement.SetAbilityName(elementName.GetAbilityName());
            return;
        }
    }
    
    HandleOtherAidConflicted(hceApps);
    InfoLog("SearchElementByAid end.");
}
void CeService::HandleOtherAidConflicted(const std::vector<AppDataParser::HceAppAidInfo> &hceApps)
{
    InfoLog("too many applications found, let user decide.");
    TAG::NfcNotificationId notificationId = TAG::NFC_HCE_AID_CONFLICTED_ID;
    ExternalDepsProxy::GetInstance().PublishNfcNotification(notificationId, "", 0);
}

bool CeService::UpdateDefaultPaymentType()
{
    KITS::DefaultPaymentType defaultPaymentType = GetDefaultPaymentType();
    InfoLog("The last default payment type %{public}d, the new one %{public}d.", defaultPaymentType_,
            defaultPaymentType);
    if (defaultPaymentType == defaultPaymentType_) {
        return false;
    }
    std::lock_guard<std::mutex> lock(configRoutingMutex_);
    defaultPaymentType_ = defaultPaymentType;
    return true;
}

bool CeService::IsPaymentAid(const std::string &aid, const AppDataParser::HceAppAidInfo &hceApp)
{
    for (const AppDataParser::AidInfo &aidInfo : hceApp.customDataAid) {
        if (KITS::KEY_PAYMENT_AID == aidInfo.name && aid == aidInfo.value) {
            return true;
        }
    }
    return false;
}

void CeService::HandleFieldActivated()
{
    if (nfcService_.expired() || nfcService_.lock()->eventHandler_ == nullptr) {
        return;
    }
    nfcService_.lock()->eventHandler_->RemoveEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF));
    nfcService_.lock()->eventHandler_->RemoveEvent(
        static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF_TIMEOUT));
    nfcService_.lock()->eventHandler_->SendEvent(
        static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF_TIMEOUT), DEACTIVATE_TIMEOUT);

    uint64_t currentTime = KITS::NfcSdkCommon::GetRelativeTime();
    if (currentTime < lastFieldOnTime_) {
        WarnLog("currentTime = %{public}lu, lastFieldOnTime_ = %{public}lu", currentTime, lastFieldOnTime_);
        lastFieldOnTime_ = 0;
        return;
    }
    if (currentTime - lastFieldOnTime_ > FIELD_COMMON_EVENT_INTERVAL) {
        lastFieldOnTime_ = currentTime;
        nfcService_.lock()->eventHandler_->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_ON));
    }
}

void CeService::HandleFieldDeactivated()
{
    if (nfcService_.expired() || nfcService_.lock()->eventHandler_ == nullptr) {
        return;
    }
    nfcService_.lock()->eventHandler_->RemoveEvent(
        static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF_TIMEOUT));
    nfcService_.lock()->eventHandler_->RemoveEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF));

    uint64_t currentTime = KITS::NfcSdkCommon::GetRelativeTime();
    if (currentTime < lastFieldOffTime_) {
        WarnLog("currentTime = %{public}lu, lastFieldOffTime_ = %{public}lu", currentTime, lastFieldOffTime_);
        lastFieldOffTime_ = 0;
        return;
    }
    if (currentTime - lastFieldOffTime_ > FIELD_COMMON_EVENT_INTERVAL) {
        lastFieldOffTime_ = currentTime;
        nfcService_.lock()->eventHandler_->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF),
                                                     FIELD_COMMON_EVENT_INTERVAL);
    }
}
void CeService::OnCardEmulationData(const std::vector<uint8_t> &data)
{
    if (hostCardEmulationManager_ == nullptr) {
        ErrorLog("hce is null");
        return;
    }
    hostCardEmulationManager_->OnHostCardEmulationDataNfcA(data);
}
void CeService::OnCardEmulationActivated()
{
    if (hostCardEmulationManager_ == nullptr) {
        ErrorLog("hce is null");
        return;
    }
    hostCardEmulationManager_->OnCardEmulationActivated();
}
void CeService::OnCardEmulationDeactivated()
{
    if (hostCardEmulationManager_ == nullptr) {
        ErrorLog("hce is null");
        return;
    }
    hostCardEmulationManager_->OnCardEmulationDeactivated();
}
OHOS::sptr<OHOS::IRemoteObject> CeService::AsObject()
{
    return nullptr;
}
void CeService::Initialize()
{
    DebugLog("CeService Initialize start");
    dataRdbObserver_ = sptr<DefaultPaymentServiceChangeCallback>(
        new (std::nothrow) DefaultPaymentServiceChangeCallback(shared_from_this()));
    Uri nfcDefaultPaymentApp(KITS::NFC_DATA_URI_PAYMENT_DEFAULT_APP);
    DelayedSingleton<SettingDataShareImpl>::GetInstance()->RegisterDataObserver(nfcDefaultPaymentApp,
                                                                                dataRdbObserver_);
    DelayedSingleton<SettingDataShareImpl>::GetInstance()->GetElementName(
        nfcDefaultPaymentApp, KITS::DATA_SHARE_KEY_NFC_PAYMENT_DEFAULT_APP, defaultPaymentElement_);
    hostCardEmulationManager_ =
        std::make_shared<HostCardEmulationManager>(nfcService_, nciCeProxy_, shared_from_this());

    defaultPaymentBundleInstalled_ =
        ExternalDepsProxy::GetInstance().IsBundleInstalled(defaultPaymentElement_.GetBundleName());

    std::string appStatus = defaultPaymentBundleInstalled_ ? APP_ADDED : APP_REMOVED;
    ExternalDepsProxy::GetInstance().WriteDefaultPaymentAppChangeHiSysEvent(defaultPaymentElement_.GetBundleName(),
                                                                            appStatus);

    defaultPaymentType_ = GetDefaultPaymentType();
    DebugLog("CeService Initialize end");
}
void CeService::Deinitialize()
{
    DebugLog("CeService Deinitialize start");
    ClearAidEntriesCache();
    foregroundElement_.SetBundleName("");
    foregroundElement_.SetAbilityName("");
    foregroundElement_.SetDeviceID("");
    foregroundElement_.SetModuleName("");
    defaultPaymentElement_.SetBundleName("");
    defaultPaymentElement_.SetAbilityName("");
    defaultPaymentElement_.SetDeviceID("");
    defaultPaymentElement_.SetModuleName("");
    dynamicAids_.clear();
    Uri nfcDefaultPaymentApp(KITS::NFC_DATA_URI_PAYMENT_DEFAULT_APP);
    DelayedSingleton<SettingDataShareImpl>::GetInstance()->ReleaseDataObserver(nfcDefaultPaymentApp,
                                                                               dataRdbObserver_);
    DebugLog("CeService Deinitialize end");
}

bool CeService::StartHce(const ElementName &element, const std::vector<std::string> &aids)
{
    if (nfcService_.expired()) {
        ErrorLog("nfcService_ is nullptr.");
        return false;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("NFC not enabled, should not happen.");
        return false;
    }
    SetHceInfo(element, aids);
    InfoLog("StartHce: refresh route table");
    ConfigRoutingAndCommit();
    return true;
}

void CeService::SetHceInfo(const ElementName &element, const std::vector<std::string> &aids)
{
    InfoLog("SetHceInfo start.");
    std::lock_guard<std::mutex> lock(configRoutingMutex_);
    foregroundElement_ = element;
    ExternalDepsProxy::GetInstance().WriteForegroundAppChangeHiSysEvent(foregroundElement_.GetBundleName());
    dynamicAids_.clear();
    dynamicAids_ = std::move(aids);
}

void CeService::ClearHceInfo()
{
    InfoLog("ClearHceInfo start.");
    std::lock_guard<std::mutex> lock(configRoutingMutex_);
    foregroundElement_.SetBundleName("");
    foregroundElement_.SetAbilityName("");
    foregroundElement_.SetDeviceID("");
    foregroundElement_.SetModuleName("");
    ExternalDepsProxy::GetInstance().WriteForegroundAppChangeHiSysEvent(foregroundElement_.GetBundleName());
    dynamicAids_.clear();
}

bool CeService::StopHce(const ElementName &element, Security::AccessToken::AccessTokenID callerToken)
{
    bool isForegroud = element.GetBundleName() == foregroundElement_.GetBundleName() &&
                       element.GetAbilityName() == foregroundElement_.GetAbilityName();
    if (isForegroud) {
        ClearHceInfo();
        InfoLog("StopHce: refresh route table");
        ConfigRoutingAndCommit();
    }
    return hostCardEmulationManager_->UnRegAllCallback(callerToken);
}

bool CeService::HandleWhenRemoteDie(Security::AccessToken::AccessTokenID callerToken)
{
    Security::AccessToken::HapTokenInfo hapTokenInfo;
    int result = Security::AccessToken::AccessTokenKit::GetHapTokenInfo(callerToken, hapTokenInfo);

    InfoLog("get hap token info, result = %{public}d", result);
    if (result) {
        return false;
    }
    if (hapTokenInfo.bundleName.empty()) {
        ErrorLog("HandleWhenRemoteDie: not got bundle name");
        return false;
    }

    bool isForegroud = hapTokenInfo.bundleName == foregroundElement_.GetBundleName();
    if (isForegroud) {
        ClearHceInfo();
        InfoLog("remote die: refresh route table");
        ConfigRoutingAndCommit();
    }
    return hostCardEmulationManager_->UnRegAllCallback(callerToken);
}
} // namespace NFC
} // namespace OHOS