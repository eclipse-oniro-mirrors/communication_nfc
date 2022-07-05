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

void AppDataParser::PackageAddAndChangeEvent(const EventFwk::CommonEventData &data)
{
    InfoLog("Package add and change event");
    std::string bundlename = data.GetWant().GetElement().GetBundleName();
    DebugLog("PackageAddAndChangeEvent bundlename:%{public}s", bundlename.c_str());
    if (appDataParser_.QueryAbilityInfosByAction(bundlename, ACTION_TAG_FOUND)) {
        InfoLog("Add tag app is ok");
        return;
    }
    if (QueryAbilityInfosByAction(bundlename, ACTION_HOST_APDU_SERVICE)) {
        InfoLog("Add hce app is ok");
        return;
    }
    InfoLog("Query AbilityInfo failed.");
}

bool AppDataParser::QueryAbilityInfosByAction(const std::string bundlename, const std::string action)
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
    if (bundleMgrProxy_->QueryAllAbilityInfos(want, userId, abilityInfos)) {
        for (auto& abilityInfo : abilityInfos) {
            if (bundlename.compare(abilityInfo.bundleName) == 0) {
                if (action.compare(ACTION_TAG_FOUND) == 0) {
                    appDataParser_.ModifyAppTechList(abilityInfo);
                    auto deleiter = appDataParser_.g_hostApduService.find(bundlename);
                    if (deleiter != appDataParser_.g_hostApduService.end()) {
                        appDataParser_.g_hostApduService.erase(deleiter);
                        DebugLog("Delete apdu from host apdu service.");
                    }
                    return true;
                } else {
                    appDataParser_.ModifyHostApduService(abilityInfo);
                    auto deleiter = appDataParser_.g_appTechList.find(bundlename);
                    if (deleiter != appDataParser_.g_appTechList.end()) {
                        appDataParser_.g_appTechList.erase(deleiter);
                        DebugLog("Delete tech from app tech list.");
                    }
                    return true;
                }
            }
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
    if (bundleMgrProxy_->QueryAllAbilityInfos(want, userId, abilityInfos)) {
        if (action.compare(ACTION_TAG_FOUND) == 0) {
            for (auto& abilityInfo : abilityInfos) {
                appDataParser_.ModifyAppTechList(abilityInfo);
            }
        } else {
            for (auto& abilityInfo : abilityInfos) {
                appDataParser_.ModifyHostApduService(abilityInfo);
            }
        }
    }
    DebugLog("QueryAbilityInfosByAction finsih.");
    return true;
}

void AppDataParser::ModifyAppTechList(AppExecFwk::AbilityInfo &abilityInfo)
{
    std::vector<std::string> valuelist;
    for (auto& data : abilityInfo.metaData.customizeData) {
        if (data.name == "tech") {
            valuelist.emplace_back(data.value);
        }
    }
    AppTechList apptechlist;
    apptechlist.abilityinfo = abilityInfo;
    apptechlist.tech = valuelist;
    appDataParser_.g_appTechList.insert(make_pair(abilityInfo.bundleName, apptechlist));
    DebugLog("Finish modify APP tech list.");
}

void AppDataParser::ModifyHostApduService(AppExecFwk::AbilityInfo &abilityInfo)
{
    std::vector<AppDataParser::CustomDataAid> customDataAidlist;
    AppDataParser::CustomDataAid customdataAid;
    for (auto& data : abilityInfo.metaData.customizeData) {
        if ((data.name == "paymentAid") || (data.name == "otherAid")) {
            customdataAid.name = data.name;
            customdataAid.value = data.value;
            customDataAidlist.emplace_back(customdataAid);
        }
    }
    HostApduAid hostapduadi;
    hostapduadi.abilityinfo = abilityInfo;
    hostapduadi.customdataaid = customDataAidlist;
    appDataParser_.g_hostApduService.insert(make_pair(abilityInfo.bundleName, hostapduadi));
    DebugLog("Finish modify host apdu service.");
}

void AppDataParser::PackageRemoveEvent(const EventFwk::CommonEventData &data)
{
    InfoLog("NfcService::ProcessPackageRemoveEvent");
    std::string bundlename = data.GetWant().GetElement().GetBundleName();
    DebugLog("bundlename is:%{public}s",bundlename.c_str());
    if (bundlename.empty()) {
        DebugLog("Can not get bundlename.");
        return;
    }
    if ((!g_appTechList.empty()) || (!g_hostApduService.empty())) {
        auto appiter = appDataParser_.g_appTechList.find(bundlename);
        if (appiter != appDataParser_.g_appTechList.end()) {
            appDataParser_.g_appTechList.erase(appiter);
            DebugLog("Delete APP tech from app tech list.");
            return;
        }
        auto apduiter = appDataParser_.g_hostApduService.find(bundlename);
        if (apduiter != appDataParser_.g_hostApduService.end()) {
            appDataParser_.g_hostApduService.erase(apduiter);
            DebugLog("Delete AID from host apdu service.");
            return;
        }
    }
    DebugLog("Not need remove any record");
}

bool AppDataParser::UpdateTechListAndAidList()
{
    DebugLog("Update TechList and AidList");
    bundleMgrProxy_ = GetBundleMgrProxy();
    if (!bundleMgrProxy_) {
        DebugLog("bundleMgrProxy_ is nullptr.");
        return false;
    }
    if (!appDataParser_.QueryAbilityInfosByAction(ACTION_TAG_FOUND)) {
        DebugLog("Updaete app tech list failed.");
    }
    DebugLog("TechList update finish,tech list length=%{public}d", g_appTechList.size());
    if (!appDataParser_.QueryAbilityInfosByAction(ACTION_HOST_APDU_SERVICE)) {
        DebugLog("Updaete host apdu service failed.");
    }
    DebugLog("Host apdu aid servcie update finish,aid list length=%{public}d", g_hostApduService.size());
    InfoLog("Query AbilityInfos finish.");
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
