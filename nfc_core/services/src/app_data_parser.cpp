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

#include "common_event_handler.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace NFC {
const std::string ACTION_TAG_FOUND = "ohos.nfc.tag.action.TAG_FOUND";
const std::string ACTION_HOST_APDU_SERVICE = "ohos.nfc.cardemulation.action.HOST_APDU_SERVICE";
const std::string APP_TECH = "tag-tech";
const std::string PAY_AID = "payment-aid";
const std::string OTHER_AID = "other-aid";
sptr<AppExecFwk::IBundleMgr> bundleMgrProxy_;
static AppDataParser appDataParser_;

AppDataParser::AppDataParser()
{
}

AppDataParser::~AppDataParser()
{
}

AppDataParser& AppDataParser::GetInstance()
{
    return appDataParser_;
}

void AppDataParser::PackageAddAndChangeEvent(std::shared_ptr<EventFwk::CommonEventData> data)
{
    InfoLog("Package add and change event");
    std::string bundleName = data->GetWant().GetElement().GetBundleName();
    DebugLog("PackageAddAndChangeEvent bundlename:%{public}s", bundleName.c_str());
    if (appDataParser_.QueryAbilityInfosByAction(bundleName, ACTION_TAG_FOUND)) {
        InfoLog("Add tag app is ok");
        return;
    }
    if (QueryAbilityInfosByAction(bundleName, ACTION_HOST_APDU_SERVICE)) {
        InfoLog("Add hce app is ok");
        return;
    }
    InfoLog("Query AbilityInfo failed.");
}

bool AppDataParser::QueryAbilityInfosByAction(const std::string bundleName, const std::string action)
{
    if (!bundleMgrProxy_) {
        DebugLog("bundleMgrProxy_ is nullptr.");
        return false;
    }
    if ((action != ACTION_TAG_FOUND) && (action != ACTION_HOST_APDU_SERVICE)) {
        DebugLog("Action is not right.");
        return false;
    }
    AAFwk::Want want;
    want.SetAction(action);
    int32_t userId = AppExecFwk::Constants::START_USERID;
    std::vector<AppExecFwk::AbilityInfo> abilityInfos;
    if (!(bundleMgrProxy_->QueryAllAbilityInfos(want, userId, abilityInfos))) {
        DebugLog("Unable to get corresponding abilityInfos.");
        return false;
    }
    for (auto& abilityInfo : abilityInfos) {
        if ((bundleName.compare(abilityInfo.bundleName) == 0) &&
            (action.compare(ACTION_TAG_FOUND) == 0)) {
            appDataParser_.ModifyAppTechList(abilityInfo);
            DeleteHostApduService(bundleName);
            return true;
        } else if ((bundleName.compare(abilityInfo.bundleName) == 0) &&
            (action.compare(ACTION_HOST_APDU_SERVICE) == 0)) {
            appDataParser_.ModifyHostApduService(abilityInfo);
            DeleteAppTechList(bundleName);
            return true;
        }
    }
    DebugLog("There is not suitable abilityinfo.");
    return false;
}

bool AppDataParser::QueryAbilityInfosByAction(const std::string action)
{
    if (!bundleMgrProxy_) {
        DebugLog("bundleMgrProxy_ is nullptr.");
        return false;
    }
    if ((action != ACTION_TAG_FOUND) && (action != ACTION_HOST_APDU_SERVICE)) {
        DebugLog("Action is not right.");
        return false;
    }
    AAFwk::Want want;
    want.SetAction(action);
    int32_t userId = AppExecFwk::Constants::START_USERID;
    std::vector<AppExecFwk::AbilityInfo> abilityInfos;
    if (!(bundleMgrProxy_->QueryAllAbilityInfos(want, userId, abilityInfos))) {
        DebugLog("Unable to get corresponding abilityInfos.");
        return false;
    }
    if (action.compare(ACTION_TAG_FOUND) == 0) {
        for (auto& abilityInfo : abilityInfos) {
            appDataParser_.ModifyAppTechList(abilityInfo);
        }
    } else {
        for (auto& abilityInfo : abilityInfos) {
            appDataParser_.ModifyHostApduService(abilityInfo);
        }
    }
    DebugLog("QueryAbilityInfosByAction finsih.");
    return true;
}

void AppDataParser::ModifyAppTechList(AppExecFwk::AbilityInfo &abilityInfo)
{
    std::vector<std::string> valueList;
    for (auto& data : abilityInfo.metaData.customizeData) {
        if (data.name == APP_TECH) {
            valueList.emplace_back(data.value);
        }
    }
    AppTechList appTechList;
    appTechList.abilityInfo = abilityInfo;
    appTechList.tech = valueList;
    appDataParser_.g_appTechList.insert(make_pair(abilityInfo.bundleName, appTechList));
    DebugLog("Finish modify APP tech list.");
}

void AppDataParser::ModifyHostApduService(AppExecFwk::AbilityInfo &abilityInfo)
{
    std::vector<AppDataParser::CustomDataAid> customDataAidList;
    AppDataParser::CustomDataAid customDataAid;
    for (auto& data : abilityInfo.metaData.customizeData) {
        if ((data.name == PAY_AID) || (data.name == OTHER_AID)) {
            customDataAid.name = data.name;
            customDataAid.value = data.value;
            customDataAidList.emplace_back(customDataAid);
        }
    }
    HostApduAid hostApduAid;
    hostApduAid.abilityInfo = abilityInfo;
    hostApduAid.customDataAid = customDataAidList;
    appDataParser_.g_hostApduService.insert(make_pair(abilityInfo.bundleName, hostApduAid));
    DebugLog("Finish modify host apdu service.");
}

void AppDataParser::PackageRemoveEvent(std::shared_ptr<EventFwk::CommonEventData> data)
{
    InfoLog("NfcService::ProcessPackageRemoveEvent");
    std::string bundleName = data->GetWant().GetElement().GetBundleName();
    DebugLog("bundleName is:%{public}s", bundleName.c_str());
    if (bundleName.empty()) {
        DebugLog("Can not get bundleName.");
        return;
    }
    if ((!g_appTechList.empty()) || (!g_hostApduService.empty())) {
        if (DeleteAppTechList(bundleName)) {
            DebugLog("DeleteAppTechList success.");
            return;
        }
        if (DeleteHostApduService(bundleName)) {
            DebugLog("DeleteHostApduService success.");
            return;
        }
    }
    DebugLog("Not need remove any record");
}

bool AppDataParser::DeleteAppTechList(std::string bundleName)
{
    auto appIter = appDataParser_.g_appTechList.find(bundleName);
    if (appIter != appDataParser_.g_appTechList.end()) {
        appDataParser_.g_appTechList.erase(appIter);
        DebugLog("Delete APP tech from app tech list.");
        return true;
    }
    return false;
}

bool AppDataParser::DeleteHostApduService(std::string bundleName)
{
    auto apduIter = appDataParser_.g_hostApduService.find(bundleName);
    if (apduIter != appDataParser_.g_hostApduService.end()) {
        appDataParser_.g_hostApduService.erase(apduIter);
        DebugLog("Delete AID from host apdu service.");
        return true;
    }
    return false;
}

bool AppDataParser::UpdateTechList()
{
    DebugLog("Update TechList");
    bundleMgrProxy_ = GetBundleMgrProxy();
    if (!bundleMgrProxy_) {
        DebugLog("bundleMgrProxy_ is nullptr.");
        return false;
    }
    if (!appDataParser_.QueryAbilityInfosByAction(ACTION_TAG_FOUND)) {
        DebugLog("Updaete app tech list failed.");
        return false;
    }
    DebugLog("TechList update finish,tech list length=%{public}d",
        (int)appDataParser_.g_appTechList.size());
    return true;
}

bool AppDataParser::UpdateAidList()
{
    DebugLog("Update AidList");
    bundleMgrProxy_ = GetBundleMgrProxy();
    if (!bundleMgrProxy_) {
        DebugLog("bundleMgrProxy_ is nullptr.");
        return false;
    }
    if (!appDataParser_.QueryAbilityInfosByAction(ACTION_HOST_APDU_SERVICE)) {
        DebugLog("Updaete host apdu service failed.");
        return false;
    }
    DebugLog("Host apdu aid servcie update finish,aid list length=%{public}d",
        (int)appDataParser_.g_hostApduService.size());
    return true;
}

sptr<AppExecFwk::IBundleMgr> AppDataParser::GetBundleMgrProxy()
{
    InfoLog("Get bundle manager proxy.");
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        InfoLog("systemAbilityManager is null");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        InfoLog("remoteObject is null");
        return nullptr;
    }
    InfoLog("bundle manager proxy acquire");
    return iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
}
}  // namespace NFC
}  // namespace OHOS
