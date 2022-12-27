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
#include "loghelper.h"
#include "nfc_service.h"
#include "system_ability.h"
#include "want.h"

namespace OHOS {
namespace NFC {
using AppExecFwk::AbilityInfo;
using OHOS::AppExecFwk::ElementName;
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
private:
    static sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy();
    ElementName GetMatchedTagKeyElement(ElementName &element);
    ElementName GetMatchedHceKeyElement(ElementName &element);
    bool IsMatchedByBundleName(ElementName &src, ElementName &target);
    bool InitAppListByAction(const std::string action);
    bool VerifyHapPermission(const std::string bundleName, const std::string action);
    bool UpdateAppListInfo(ElementName &element, const std::string action);
    void UpdateTagAppList(AbilityInfo &abilityInfo, ElementName &element);
    void UpdateHceAppList(AbilityInfo &abilityInfo, ElementName &element);
    void RemoveTagAppInfo(ElementName &element);
    void RemoveHceAppInfo(ElementName &element);
};
}  // namespace NFC
}  // namespace OHOS
#endif  // COMMON_EVENT_HANDLER_H
