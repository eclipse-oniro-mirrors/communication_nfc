/*
* Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "app_data_parser.h"

#include "accesstoken_kit.h"
#include "common_event_manager.h"
#include "iservice_registry.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "system_ability_definition.h"
#include "taginfo.h"
#include "nfc_permission_checker.h"

namespace OHOS {
namespace NFC {
const int USER_ID = 100;
sptr<AppExecFwk::IBundleMgr> bundleMgrProxy_;
static AppDataParser g_appDataParser;
/** Tag type of tag app metadata name */
static const std::string KEY_TAG_TECH = "tag-tech";

AppDataParser::AppDataParser()
{
    g_tagAppAndTechMap.clear();
    g_hceAppAndAidMap.clear();
}

AppDataParser::~AppDataParser()
{
}

AppDataParser& AppDataParser::GetInstance()
{
    return g_appDataParser;
}

sptr<AppExecFwk::IBundleMgr> AppDataParser::GetBundleMgrProxy()
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

void AppDataParser::HandleAppAddOrChangedEvent(std::shared_ptr<EventFwk::CommonEventData> data)
{
    if (data == nullptr) {
        ErrorLog("HandleAppAddOrChangedEvent, invalid data.");
        return;
    }
    ElementName element = data->GetWant().GetElement();
    std::string bundleName = element.GetBundleName();
    if (bundleName.empty()) {
        ErrorLog("HandleAppAddOrChangedEvent, invaid bundleName.");
        return;
    }
    DebugLog("HandleAppAddOrChangedEvent bundlename: %{public}s", bundleName.c_str());
    UpdateAppListInfo(element, KITS::ACTION_TAG_FOUND);
    UpdateAppListInfo(element, KITS::ACTION_HOST_APDU_SERVICE);
}

void AppDataParser::HandleAppRemovedEvent(std::shared_ptr<EventFwk::CommonEventData> data)
{
    if (data == nullptr) {
        ErrorLog("HandleAppRemovedEvent, invalid data.");
        return;
    }
    ElementName element = data->GetWant().GetElement();
    std::string bundleName = element.GetBundleName();
    if (bundleName.empty()) {
        ErrorLog("HandleAppRemovedEvent, invalid bundleName.");
        return;
    }
    DebugLog("HandleAppRemovedEvent, bundleName %{public}s tag size %{public}zu, hce size %{public}zu",
        bundleName.c_str(),
        g_tagAppAndTechMap.size(),
        g_hceAppAndAidMap.size());
    RemoveTagAppInfo(element);
    RemoveHceAppInfo(element);
}

bool AppDataParser::VerifyHapPermission(const std::string bundleName, const std::string action)
{
    std::string permissionNfc;
    OHOS::Security::AccessToken::AccessTokenID tokenID;
    std::map<std::string, std::string> permissionMap = {
        {KITS::ACTION_TAG_FOUND, TAG_PERM},
        {KITS::ACTION_HOST_APDU_SERVICE, CARD_EMU_PERM}
    };
    std::map<std::string, std::string>::iterator it = permissionMap.find(action.c_str());
    if (it != permissionMap.end()) {
        permissionNfc = it->second;
    } else {
        ErrorLog("VerifyHapPermission, action no in map!");
        return false;
    }
    tokenID= OHOS::Security::AccessToken::AccessTokenKit::GetHapTokenID(USER_ID, bundleName, 0);
    int result = OHOS::Security::AccessToken::AccessTokenKit::VerifyAccessToken(tokenID, permissionNfc);
    if (result != OHOS::Security::AccessToken::PERMISSION_GRANTED) {
        ErrorLog("bundleName %{public}s no permission %{public}s", bundleName.c_str(), permissionNfc.c_str());
        return false;
    }
    return true;
}

void AppDataParser::QueryAbilityInfos(const std::string action, std::vector<AbilityInfo> &abilityInfos,
    std::vector<ExtensionAbilityInfo> &extensionInfos)
{
    if (bundleMgrProxy_ == nullptr) {
        bundleMgrProxy_ = GetBundleMgrProxy();
    }
    if (bundleMgrProxy_ == nullptr) {
        ErrorLog("QueryAbilityInfos, bundleMgrProxy_ is nullptr.");
        return;
    }
    AAFwk::Want want;
    want.SetAction(action);
    want.SetType("*/*"); // skip the type, matched action only.
    bool withDefault = false;
    auto abilityInfoFlag = AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_DEFAULT
        | AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_SKILL_URI
        | AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_METADATA;
    if (!bundleMgrProxy_->ImplicitQueryInfos(want, abilityInfoFlag, USER_ID, withDefault,
        abilityInfos, extensionInfos)) {
        WarnLog("QueryAbilityInfos, query none for action %{public}s", action.c_str());
        return;
    }
}

bool AppDataParser::UpdateAppListInfo(ElementName &element, const std::string action)
{
    if (action.compare(KITS::ACTION_TAG_FOUND) != 0 && action.compare(KITS::ACTION_HOST_APDU_SERVICE) != 0) {
        ErrorLog("UpdateAppListInfo, ignore action = %{public}s", action.c_str());
        return false;
    }
    std::string bundleName = element.GetBundleName();
    if (!VerifyHapPermission(bundleName, action)) {
        ErrorLog("Hap have no permission for action = %{public}s", action.c_str());
        return false;
    }

    // query the applications infos that're matched with the acitons.
    std::vector<AbilityInfo> abilityInfos;
    std::vector<ExtensionAbilityInfo> extensionAbilityInfos;
    QueryAbilityInfos(action, abilityInfos, extensionAbilityInfos);
    for (auto& abilityInfo : abilityInfos) {
        if (bundleName.empty() || bundleName.compare(abilityInfo.bundleName) != 0) {
            continue;
        }
        if (action.compare(KITS::ACTION_TAG_FOUND) == 0) {
            UpdateTagAppList(abilityInfo, element);
        }
        if (action.compare(KITS::ACTION_HOST_APDU_SERVICE) == 0) {
            UpdateHceAppList(abilityInfo, element);
        }
    }
    return true;
}

bool AppDataParser::InitAppListByAction(const std::string action)
{
    // query the applications infos that're matched with the acitons.
    std::vector<AbilityInfo> abilityInfos;
    std::vector<ExtensionAbilityInfo> extensionAbilityInfos;
    QueryAbilityInfos(action, abilityInfos, extensionAbilityInfos);
    if (KITS::ACTION_TAG_FOUND.compare(action) == 0) {
        for (auto& tagAbilityInfo : abilityInfos) {
            ElementName element(tagAbilityInfo.deviceId, tagAbilityInfo.bundleName, tagAbilityInfo.name,
                tagAbilityInfo.moduleName);
            UpdateTagAppList(tagAbilityInfo, element);
        }
    } else if (KITS::ACTION_HOST_APDU_SERVICE.compare(action) == 0) {
        for (auto& hceAbilityInfo : abilityInfos) {
            ElementName element(hceAbilityInfo.deviceId, hceAbilityInfo.bundleName, hceAbilityInfo.name,
                hceAbilityInfo.moduleName);
            UpdateHceAppList(hceAbilityInfo, element);
        }
    } else {
        WarnLog("InitAppListByAction,unknown action = %{public}s", action.c_str());
    }
    return true;
}

bool AppDataParser::IsMatchedByBundleName(ElementName &src, ElementName &target)
{
    if (src.GetBundleName().compare(target.GetBundleName()) == 0) {
        return true;
    }
    return false;
}

ElementName AppDataParser::GetMatchedTagKeyElement(ElementName &element)
{
    ElementName emptyElement;
    std::vector<TagAppTechInfo>::iterator iter;
    for (iter = g_tagAppAndTechMap.begin(); iter != g_tagAppAndTechMap.end(); ++iter) {
        if (IsMatchedByBundleName(element, (*iter).element)) {
            return (*iter).element;
        }
    }
    return emptyElement;
}

ElementName AppDataParser::GetMatchedHceKeyElement(ElementName &element)
{
    ElementName emptyElement;
    std::vector<HceAppAidInfo>::iterator iter;
    for (iter = g_hceAppAndAidMap.begin(); iter != g_hceAppAndAidMap.end(); ++iter) {
        if (IsMatchedByBundleName(element, (*iter).element)) {
            return (*iter).element;
        }
    }
    return emptyElement;
}

void AppDataParser::UpdateTagAppList(AbilityInfo &abilityInfo, ElementName &element)
{
    if (!GetMatchedTagKeyElement(element).GetBundleName().empty()) {
        WarnLog("UpdateTagAppList, rm duplicated app %{public}s", element.GetBundleName().c_str());
        RemoveTagAppInfo(element);
    }
    std::vector<std::string> valueList;
    for (auto& data : abilityInfo.metadata) {
        if (KEY_TAG_TECH.compare(data.name) == 0) {
            valueList.emplace_back(data.value);
            DebugLog("UpdateTagAppList from metadata, push tech %{public}s", data.value.c_str());
        }
    }
    for (auto& data : abilityInfo.metaData.customizeData) {
        if (KEY_TAG_TECH.compare(data.name) == 0) {
            valueList.emplace_back(data.value);
            DebugLog("UpdateTagAppList from customizeData, push tech %{public}s", data.value.c_str());
        }
    }
    for (auto& uri : abilityInfo.skillUri) {
        if (uri.type.empty()) {
            continue;
        }
        // format example: "type": "tag-tech/NfcA"
        auto pos = uri.type.find("/");
        if (pos == std::string::npos) {
            ErrorLog("UpdateTagAppList from skillUri, separator not found %{public}s", uri.type.c_str());
            continue;
        }
        std::string tech = uri.type.substr(0, pos);
        if (KEY_TAG_TECH.compare(tech) != 0) {
            ErrorLog("UpdateTagAppList KEY_TAG_TECH for %{public}s", tech.c_str());
            continue;
        }
        std::string nfcType = uri.type.substr(pos + 1, uri.type.size());
        if (std::find(valueList.begin(), valueList.end(), nfcType) == valueList.end()) {
            valueList.emplace_back(nfcType);
            DebugLog("UpdateTagAppList from skillUri, push tech %{public}s", nfcType.c_str());
        }
    }

    if (valueList.empty()) {
        DebugLog("UpdateTagAppList, ignore for app %{public}s %{public}s", element.GetBundleName().c_str(),
            element.GetAbilityName().c_str());
        return;
    }

    TagAppTechInfo tagAppTechInfo;
    tagAppTechInfo.element = element;
    tagAppTechInfo.tech = valueList;
    g_tagAppAndTechMap.push_back(tagAppTechInfo);
    DebugLog("UpdateTagAppList, push for app %{public}s %{public}s", element.GetBundleName().c_str(),
        element.GetAbilityName().c_str());
}

void AppDataParser::UpdateHceAppList(AbilityInfo &abilityInfo, ElementName &element)
{
    if (!GetMatchedHceKeyElement(element).GetBundleName().empty()) {
        WarnLog("UpdateHceAppList, rm duplicated app %{public}s", element.GetBundleName().c_str());
        RemoveHceAppInfo(element);
    }
    std::vector<AppDataParser::AidInfo> customDataAidList;
    AppDataParser::AidInfo customDataAid;
    for (auto& data : abilityInfo.metadata) {
        if ((KITS::KEY_PAYMENT_AID.compare(data.name) == 0) || (KITS::KEY_OHTER_AID.compare(data.name) == 0)) {
            customDataAid.name = data.name;
            customDataAid.value = data.value;
            customDataAidList.emplace_back(customDataAid);
            DebugLog("UpdateHceAppList from metadata, push aid %{public}s", data.value.c_str());
        }
    }
    for (auto& data : abilityInfo.metaData.customizeData) {
        if ((KITS::KEY_PAYMENT_AID.compare(data.name) == 0) || (KITS::KEY_OHTER_AID.compare(data.name) == 0)) {
            customDataAid.name = data.name;
            customDataAid.value = data.value;
            customDataAidList.emplace_back(customDataAid);
            DebugLog("UpdateHceAppList from customizeData, push aid %{public}s", data.value.c_str());
        }
    }
    if (customDataAidList.empty()) {
        DebugLog("UpdateHceAppList, ignore for app %{public}s %{public}s", element.GetBundleName().c_str(),
            element.GetAbilityName().c_str());
        return;
    }
    HceAppAidInfo hceAppAidInfo;
    hceAppAidInfo.element = element;
    hceAppAidInfo.iconId = abilityInfo.iconId;
    hceAppAidInfo.labelId = abilityInfo.labelId;
    hceAppAidInfo.customDataAid = customDataAidList;
    g_hceAppAndAidMap.push_back(hceAppAidInfo);
    DebugLog("UpdateHceAppList, push for app %{public}s %{public}s", element.GetBundleName().c_str(),
        element.GetAbilityName().c_str());
}

void AppDataParser::RemoveTagAppInfo(ElementName &element)
{
    ElementName keyElement = GetMatchedTagKeyElement(element);
    if (keyElement.GetBundleName().empty()) {
        WarnLog("RemoveTagAppInfo, keyElement is none, ignore it.");
        return;
    }
    DebugLog("RemoveTagAppInfo, request app %{public}s", keyElement.GetBundleName().c_str());
    std::vector<TagAppTechInfo>::iterator iter;
    for (iter = g_tagAppAndTechMap.begin(); iter != g_tagAppAndTechMap.end(); ++iter) {
        // compare only bundle name to remote the app.
        if (IsMatchedByBundleName(element, (*iter).element)) {
            DebugLog("RemoveTagAppInfo, erase app %{public}s", keyElement.GetBundleName().c_str());
            g_tagAppAndTechMap.erase(iter);
            break;
        }
    }
}

void AppDataParser::RemoveHceAppInfo(ElementName &element)
{
    ElementName keyElement = GetMatchedHceKeyElement(element);
    if (keyElement.GetBundleName().empty()) {
        WarnLog("RemoveHceAppInfo, keyElement is none, ignore it.");
        return;
    }
    DebugLog("RemoveHceAppInfo, app %{public}s", keyElement.GetBundleName().c_str());
    std::vector<HceAppAidInfo>::iterator iter;
    for (iter = g_hceAppAndAidMap.begin(); iter != g_hceAppAndAidMap.end(); ++iter) {
        // compare only bundle name to remote the app.
        if (IsMatchedByBundleName(element, (*iter).element)) {
            DebugLog("RemoveHceAppInfo, erase app %{public}s", keyElement.GetBundleName().c_str());
            g_hceAppAndAidMap.erase(iter);
            break;
        }
    }
}

void AppDataParser::InitAppList()
{
    bundleMgrProxy_ = GetBundleMgrProxy();
    if (!bundleMgrProxy_) {
        ErrorLog("InitAppList, bundleMgrProxy_ is nullptr.");
        return;
    }
    InitAppListByAction(KITS::ACTION_TAG_FOUND);
    InitAppListByAction(KITS::ACTION_HOST_APDU_SERVICE);
    DebugLog("InitAppList, tag size %{public}zu, hce size %{public}zu", g_tagAppAndTechMap.size(),
        g_hceAppAndAidMap.size());
}

std::vector<ElementName> AppDataParser::GetDispatchTagAppsByTech(std::vector<int> discTechList)
{
    std::vector<ElementName> elements;
    for (size_t i = 0; i < discTechList.size(); i++) {
        std::string discStrTech = KITS::TagInfo::GetStringTech(discTechList[i]);
        DebugLog("GetDispatchTagAppsByTech, tag size = %{public}zu", g_tagAppAndTechMap.size());
        if (discStrTech.empty()) {
            continue;
        }

        // parse for all installed app that can handle this technology.
        std::vector<TagAppTechInfo>::iterator iter;
        for (iter = g_tagAppAndTechMap.begin(); iter != g_tagAppAndTechMap.end(); ++iter) {
            bool appExisted = false;
            for (auto item : elements) {
                if (IsMatchedByBundleName(item, (*iter).element)) {
                    appExisted = true;
                    break;
                }
            }
            if (appExisted) {
                continue;
            }

            std::vector<std::string> vectorTech = (*iter).tech;
            for (size_t i = 0; i < vectorTech.size(); i++) {
                DebugLog("GetDispatchTagAppsByTech, cmp tech %{public}s vs %{public}s",
                    discStrTech.c_str(), vectorTech[i].c_str());
                if (discStrTech.compare(vectorTech[i]) == 0) {
                    elements.push_back((*iter).element);
                    break;
                }
            }
        }
    }
    return elements;
}

std::vector<ElementName> AppDataParser::GetVendorDispatchTagAppsByTech(std::vector<int>& discTechList)
{
    std::vector<ElementName> elements;
    std::vector<std::string> aidList {};
    if (queryApplicationByVendor_ == nullptr) {
        ErrorLog("AppDataParser::GetVendorDispatchTagAppsByTech queryApplicationByVendor_ is nullptr.");
        return std::vector<ElementName>();
    }
    queryApplicationByVendor_->OnQueryAppInfo(KEY_TAG_APP, discTechList, aidList, elements);
    return elements;
}

void AppDataParser::RegQueryApplicationCb(sptr<IQueryAppInfoCallback> callback)
{
    queryApplicationByVendor_ = callback;
}

void AppDataParser::RegCardEmulationNotifyCb(sptr<IOnCardEmulationNotifyCb> callback)
{
    onCardEmulationNotify_ = callback;
}

sptr<IOnCardEmulationNotifyCb> AppDataParser::GetNotifyCardEmulationCallback()
{
    return onCardEmulationNotify_;
}

void AppDataParser::GetHceAppsByAid(const std::string& aid, std::vector<ElementName>& elementNames)
{
    for (const HceAppAidInfo& appAidInfo : g_hceAppAndAidMap) {
        for (const AidInfo& aidInfo : appAidInfo.customDataAid) {
            if (aid == aidInfo.value) {
                elementNames.push_back(appAidInfo.element);
                break;
            }
        }
    }
}
void AppDataParser::GetHceApps(std::vector<HceAppAidInfo> &hceApps)
{
    for (const AppDataParser::HceAppAidInfo &appAidInfo : g_hceAppAndAidMap) {
        hceApps.push_back(appAidInfo);
    }

    AppDataParser::HceAppAidInfo vendorAppAidInfo;
    std::vector<AppDataParser::AidInfo> vendorCustomDataAid;
    AppDataParser::AidInfo vendorAidInfo;
    ElementName vendorElementName;
    vendorElementName.SetDeviceID("");
    vendorElementName.SetAbilityName("com.nxp.cascaen.paymenthost");
    vendorElementName.SetBundleName("/com.nxp.cascaen.paymenthost.PaymentServiceHost");
    vendorAidInfo.name = "other-aid";
    vendorAidInfo.value = "A0000000041010";
    vendorCustomDataAid.push_back(vendorAidInfo);
    vendorAppAidInfo.element = vendorElementName;
    vendorAppAidInfo.customDataAid = vendorCustomDataAid;
    hceApps.push_back(vendorAppAidInfo);
}

bool AppDataParser::IsPaymentApp(const AppDataParser::HceAppAidInfo &hceAppInfo)
{
    for (const AppDataParser::AidInfo &aidInfo : hceAppInfo.customDataAid) {
        if (KITS::KEY_PAYMENT_AID == aidInfo.name) {
            return true;
        }
    }
    return false;
}
void AppDataParser::GetPaymentAbilityInfos(std::vector<AbilityInfo> &paymentAbilityInfos)
{
    for (const AppDataParser::HceAppAidInfo &appAidInfo : g_hceAppAndAidMap) {
        if (!isPaymentApp(appAidInfo)) {
            continue;
        }
        AbilityInfo ability;
        ability.name = appAidInfo.element.GetAbilityName();
        ability.bundleName = appAidInfo.element.GetBundleName();
        ability.label = appAidInfo.label;
        ability.iconPath = appAidInfo.iconPath;
        paymentAbilityInfos.push_back(ability);
    }
}
} // namespace NFC
} // namespace OHOS
