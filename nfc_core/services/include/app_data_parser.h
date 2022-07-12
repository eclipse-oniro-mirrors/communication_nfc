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
#ifndef APP_DATA_PARSER_H
#define APP_DATA_PARSER_H

#include <future>
#include <map>
#include <mutex>
#include <vector>

#include "ability_info.h"
#include "access_token.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "nfc_service.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "loghelper.h"
#include "system_ability.h"
#include "want.h"

namespace OHOS {
namespace NFC {
class AppDataParser {
public:
    explicit AppDataParser();
    ~AppDataParser();

    struct CustomDataAid {
        std::string name;
        std::string value;
    };

    struct AppTechList {
        OHOS::AppExecFwk::AbilityInfo abilityInfo;
        std::vector<std::string> tech;
    };

    struct HostApduAid {
        OHOS::AppExecFwk::AbilityInfo abilityInfo;
        std::vector<AppDataParser::CustomDataAid> customDataAid;
    };

    std::map<std::string, AppDataParser::AppTechList> g_appTechList;
    std::map<std::string, AppDataParser::HostApduAid> g_hostApduService;

    static AppDataParser& GetInstance();
    static sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy();
    void PackageAddAndChangeEvent(std::shared_ptr<EventFwk::CommonEventData> data);
    void PackageRemoveEvent(std::shared_ptr<EventFwk::CommonEventData> data);
    bool UpdateTechList();
    bool UpdateAidList();
    bool DeleteAppTechList(std::string bundleName);
    bool DeleteHostApduService(std::string bundleName);
private:
    void ModifyAppTechList(AppExecFwk::AbilityInfo &abilityInfo);
    void ModifyHostApduService(AppExecFwk::AbilityInfo &abilityInfo);
    bool QueryAbilityInfosByAction(const std::string bundleName, const std::string action);
    bool QueryAbilityInfosByAction(const std::string action);
};
}  // namespace NFC
}  // namespace OHOS
#endif  // COMMON_EVENT_HANDLER_H
