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
#include <vector>
#include "ability_info.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "element_name.h"
#include "iquery_app_info_callback.h"
#include "want.h"

namespace OHOS {
namespace NFC {
using AppExecFwk::AbilityInfo;
using AppExecFwk::ExtensionAbilityInfo;
using OHOS::AppExecFwk::ElementName;
using QueryApplicationByVendor = std::vector<ElementName> (*)(std::string, std::vector<int>);
class AppDataParser {
public:
    explicit AppDataParser();
    ~AppDataParser();

    struct AidInfo {
        std::string name; // the type, payment-aid, or other-aid
        std::string value; // the aid value
    };

    struct TagAppTechInfo {
        ElementName element;
        std::vector<std::string> tech; // technology, such as NfcA/NfcB/IsoDep.
    };

    struct HceAppAidInfo {
        ElementName element;
        std::vector<AidInfo> customDataAid;
    };

    std::vector<TagAppTechInfo> g_tagAppAndTechMap;
    std::vector<HceAppAidInfo> g_hceAppAndAidMap;

    static AppDataParser& GetInstance();

    void HandleAppAddOrChangedEvent(std::shared_ptr<EventFwk::CommonEventData> data);
    void HandleAppRemovedEvent(std::shared_ptr<EventFwk::CommonEventData> data);
    void InitAppList();
    std::vector<ElementName> GetDispatchTagAppsByTech(std::vector<int> discTechList);
    std::vector<ElementName> GetVendorDispatchTagAppsByTech(std::vector<int>& discTechList);
    void RegQueryApplicationCb(sptr<IQueryAppInfoCallback> callback);
    std::vector<ElementName> GetHceAppsByAid(const std::string &aid, std::vector<ElementName> elementNames);
    void GetHceApps(std::vector<HceAppAidInfo> &hceApps);
private:
    static sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy();
    ElementName GetMatchedTagKeyElement(ElementName &element);
    ElementName GetMatchedHceKeyElement(ElementName &element);
    bool IsMatchedByBundleName(ElementName &src, ElementName &target);
    bool InitAppListByAction(const std::string action);
    void QueryAbilityInfos(const std::string action, std::vector<AbilityInfo> &abilityInfos,
        std::vector<ExtensionAbilityInfo> &extensionInfos);
    bool VerifyHapPermission(const std::string bundleName, const std::string action);
    bool UpdateAppListInfo(ElementName &element, const std::string action);
    void UpdateTagAppList(AbilityInfo &abilityInfo, ElementName &element);
    void UpdateHceAppList(AbilityInfo &abilityInfo, ElementName &element);
    void RemoveTagAppInfo(ElementName &element);
    void RemoveHceAppInfo(ElementName &element);
    QueryApplicationByVendor queryApplicationByVendor = nullptr;
};
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_EVENT_HANDLER_H
