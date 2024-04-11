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

NdefHarDispatch::NdefHarDispatch()
{
}

NdefHarDispatch& NdefHarDispatch::GetInstance()
{
    static NdefHarDispatch instance;
    return instance;
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
    if (!GetBundleMgrProxy()->ImplicitQueryInfos(
        want, abilityInfoFlag, USER_ID, withDefault, abilityInfos, extensionInfos)) {
        ErrorLog("NdefHarDispatch::DispatchMimeType ImplicitQueryInfos false");
        return false;
    }
    int32_t errCode = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    if (errCode) {
        ErrorLog("NdefHarDispatch::DispatchMimeType call StartAbility fail. ret = %{public}d", errCode);
        return false;
    }
    return true;
}

/* Call GetLaunchWantForBundle through bundlename to obtain the want and pull up the app */
bool NdefHarDispatch::DispatchBundleAbility(
    const std::string &harPackage, std::shared_ptr<KITS::TagInfo> tagInfo, const std::string &mimeType)
{
    if (harPackage.empty()) {
        ErrorLog("NdefHarDispatch::DispatchBundleAbility harPackage is empty");
        return false;
    }
    std::string harPackageString = NfcSdkCommon::HexStringToAsciiString(harPackage);
    AAFwk::Want want;
    if (GetBundleMgrProxy() == nullptr) {
        ErrorLog("NdefHarDispatch::GetBundleMgrProxy is nullptr");
        return false;
    }
    int32_t errCode = GetBundleMgrProxy()->GetLaunchWantForBundle(harPackageString, want, USER_ID);
    if (errCode) {
        ErrorLog("NdefHarDispatch::GetLaunchWantForBundle fail. ret = %{public}d", errCode);
        return false;
    }
    if (!mimeType.empty() && tagInfo != nullptr) {
        want.SetType(mimeType);
        ExternalDepsProxy::GetInstance().SetWantExtraParam(tagInfo, want);
    }
    errCode = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    if (errCode) {
        ErrorLog("NdefHarDispatch::DispatchBundleAbility call StartAbility fail. ret = %{public}d", errCode);
        return false;
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

/* Pulling web page links through browser */
bool NdefHarDispatch::DispatchWebLink(const std::string &webAddress, const std::string &browserBundleName)
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    InfoLog("NdefHarDispatch::DispatchWebLink enter");
    if (webAddress.empty() || browserBundleName.empty()) {
        ErrorLog("NdefHarDispatch::DispatchWebLink is empty");
        return false;
    }
    uri_ = webAddress;
    browserBundleName_ = browserBundleName;
    ExternalDepsProxy::GetInstance().PublishNfcNotification(NFC_BROWSER_NOTIFICATION_ID, uri_, 0);
    return true;
}

void NdefHarDispatch::OnBrowserOpenLink()
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    InfoLog("NdefHarDispatch::OnBrowserOpenLink, %{public}s, %{public}s",
        NfcSdkCommon::CodeMiddlePart(browserBundleName_).c_str(), NfcSdkCommon::CodeMiddlePart(uri_).c_str());
    AAFwk::Want want;
    const std::string ABILITY_NAME = "MainAbility";
    const std::string ACTION_NAME = "ohos.want.action.viewData";
    const std::string ENTITY_NAME = "entity.system.browsable";
    want.SetElementName(browserBundleName_, ABILITY_NAME);
    want.SetAction(ACTION_NAME);
    want.SetUri(uri_);
    want.AddEntity(ENTITY_NAME);
    int32_t errCode = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    if (errCode) {
        ErrorLog("NdefHarDispatch::DispatchWebLink call StartAbility fail. ret = %{public}d", errCode);
    }
}
} // namespace TAG
} // namespace NFC
} // namespace OHOS