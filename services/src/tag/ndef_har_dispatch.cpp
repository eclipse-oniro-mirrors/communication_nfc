/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "ndef_har_dispatch.h"

#include "external_deps_proxy.h"
#include "iservice_registry.h"
#include "ndef_har_data_parser.h"
#include "tag_ability_dispatcher.h"
#include "ability_manager_client.h"
#include "loghelper.h"
#include "bundle_mgr_interface.h"
#include "if_system_ability_manager.h"

namespace OHOS {
namespace NFC {
namespace TAG {
const int USER_ID = 100;
const int BUNDLE_MGR_SERVICE_SYS_ABILITY_ID = 401;
using namespace OHOS::NFC::KITS;

std::string uri_ {};
std::string browserBundleName_ {};

NdefHarDispatch::NdefHarDispatch(std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy)
    : nciNfccProxy_(nciNfccProxy)
{
}

sptr<AppExecFwk::IBundleMgr> NdefHarDispatch::GetBundleMgrProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        ErrorLog("GetBundleMgrProxy, systemAbilityManager is null");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        ErrorLog("GetBundleMgrProxy, remoteObject is null");
        return nullptr;
    }
    return iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
}

/* Implicit matching, using mimetype to pull up app */
bool NdefHarDispatch::DispatchMimeType(const std::string &type, std::shared_ptr<KITS::TagInfo> tagInfo)
{
    if (type.empty() || tagInfo == nullptr) {
        ErrorLog("NdefHarDispatch::DispatchMimeType type is empty");
        return false;
    }
    AAFwk::Want want;
    want.SetType(type);
    ExternalDepsProxy::GetInstance().SetWantExtraParam(tagInfo, want);
    if (GetBundleMgrProxy() == nullptr) {
        ErrorLog("NdefHarDispatch::DispatchMimeType GetBundleMgrProxy is nullptr");
        return false;
    }
    bool withDefault = false;
    auto abilityInfoFlag = AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_DEFAULT
        | AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_SKILL_URI
        | AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_METADATA;
    std::vector<AbilityInfo> abilityInfos;
    std::vector<ExtensionAbilityInfo> extensionInfos;
    bool findDefaultApp = false;
    if (!GetBundleMgrProxy()->ImplicitQueryInfos(
        want, abilityInfoFlag, USER_ID, withDefault, abilityInfos, extensionInfos, findDefaultApp)) {
        ErrorLog("NdefHarDispatch::DispatchMimeType ImplicitQueryInfos false");
        return false;
    }
    int32_t errCode = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    if (errCode) {
        ErrorLog("NdefHarDispatch::DispatchMimeType call StartAbility fail. ret = %{public}d", errCode);
        return false;
    }
    ExternalDepsProxy::GetInstance().WriteDispatchToAppHiSysEvent(want.GetElement().GetBundleName(),
        SubErrorCode::NDEF_HAR_DISPATCH);
    return true;
}

/* Verify harPackageString as BundleName/ServiceName and call StartExtensionAbility to pull up app */
bool NdefHarDispatch::DispatchBundleExtensionAbility(const std::string &harPackageString,
    const std::shared_ptr<KITS::TagInfo> &tagInfo, const std::string &mimeType, const std::string &uri)
{
    std::istringstream iss(harPackageString);
    std::string bundleName, serviceName;
    if (!getline(iss, bundleName, '/')) {
        InfoLog("harPackageString bundleName invalid");
        return false;
    }
    if (!getline(iss, serviceName, '/')) {
        InfoLog("harPackageString serviceName invalid");
        return false;
    }
    AAFwk::Want want;
    want.SetElementName(bundleName, serviceName);
    if (!mimeType.empty()) {
        want.SetType(mimeType);
    }
    if (uri.size() > 0) {
        want.SetUri(uri);
    }
    if (tagInfo != nullptr) {
        ExternalDepsProxy::GetInstance().SetWantExtraParam(tagInfo, want);
    }
    int errCode = AAFwk::AbilityManagerClient::GetInstance()->StartExtensionAbility(want, nullptr);
    InfoLog("StartExtensionAbility ret = %{public}d, bundleName = %{public}s, serviceName = %{public}s", errCode,
        bundleName.c_str(), serviceName.c_str());
    return (errCode == 0);
}

/* Call GetLaunchWantForBundle through bundlename to obtain the want and pull up the app */
bool NdefHarDispatch::DispatchBundleAbility(const std::string &harPackage,
    const std::shared_ptr<KITS::TagInfo> &tagInfo, const std::string &mimeType, const std::string &uri)
{
    if (harPackage.empty()) {
        ErrorLog("NdefHarDispatch::DispatchBundleAbility harPackage is empty");
        return false;
    }
    std::string harPackageString = NfcSdkCommon::HexStringToAsciiString(harPackage);
    AAFwk::Want want;
    if (GetBundleMgrProxy() == nullptr) {
        ErrorLog("GetBundleMgrProxy is nullptr");
        return false;
    }
    int32_t errCode = GetBundleMgrProxy()->GetLaunchWantForBundle(harPackageString, want, USER_ID);
    if (errCode) {
        InfoLog("GetLaunchWantForBundle fail. ret = %{public}d, harPackage = %{public}s, try ExtensionAbility instead",
            errCode, harPackageString.c_str());
        if (DispatchBundleExtensionAbility(harPackageString, tagInfo, mimeType, uri)) {
            return true;
        }
        return false;
    }
    if (!mimeType.empty()) {
        want.SetType(mimeType);
    }
    if (tagInfo != nullptr) {
        ExternalDepsProxy::GetInstance().SetWantExtraParam(tagInfo, want);
    }
    if (uri.size() > 0) {
        want.SetUri(uri);
    }
    errCode = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    if (errCode) {
        ErrorLog("StartAbility fail. ret = %{public}d, harPackage = %{public}s",
            errCode, harPackageString.c_str());
        return false;
    }
    ExternalDepsProxy::GetInstance().WriteDispatchToAppHiSysEvent(want.GetElement().GetBundleName(),
        SubErrorCode::NDEF_HAR_DISPATCH);
    if (!nciNfccProxy_.expired()) {
        nciNfccProxy_.lock()->NotifyMessageToVendor(KITS::TAG_DISPATCH_HAR_PACKAGE, harPackageString);
    }
    return true;
}

bool NdefHarDispatch::DispatchUriToBundleAbility(const std::string &uri)
{
    if (uri.empty()) {
        ErrorLog("NdefHarDispatch::DispatchUriToBundleAbility uri is empty");
        return false;
    }
    bool canOpen = false;
    if (GetBundleMgrProxy() == nullptr) {
        ErrorLog("NdefHarDispatch::DispatchUriToBundleAbility GetBundleMgrProxy is nullptr");
        return false;
    }
    int32_t errCode = GetBundleMgrProxy()->CanOpenLink(uri, canOpen);
    if (!errCode && canOpen) {
        InfoLog("NdefHarDispatch::DispatchUriToBundleAbility CanOpenLink");
    }
    ErrorLog("CanOpenLink fail. errCode = %{public}d, canOpen = %{public}d", errCode, canOpen);
    return false;
}

/* If the corresponding app has been installed, the system jumps to the corresponding app and starts it.
 * If the corresponding app is not installed, the default browser is used to open the corresponding page.
 */
bool NdefHarDispatch::DispatchHttpWebLink(const std::string &webLink)
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    InfoLog("enter");
    if (webLink.empty()) {
        ErrorLog("webLink is empty");
        return false;
    }
    AAFwk::Want want;
    const std::string PARAM_KEY = "appLinkingOnly"; // Use App Linking Mode
    want.SetUri(webLink);
    want.SetParam(PARAM_KEY, false);
    int32_t errCode = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    if (errCode) {
        ErrorLog("call StartAbility fail. ret = %{public}d", errCode);
        return false;
    }
    ExternalDepsProxy::GetInstance().WriteDispatchToAppHiSysEvent(want.GetElement().GetBundleName(),
        SubErrorCode::NDEF_URI_BROWSER_DISPATCH);
    return true;
}
} // namespace TAG
} // namespace NFC
} // namespace OHOS