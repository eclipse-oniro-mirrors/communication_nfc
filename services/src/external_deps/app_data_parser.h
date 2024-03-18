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
#ifdef VENDOR_APPLICATIONS_ENABLED
#include "ion_card_emulation_notify_cb.h"
#include "iquery_app_info_callback.h"
#endif
#include "nfc_sdk_common.h"
#include "want.h"

namespace OHOS {
namespace NFC {
using AppExecFwk::AbilityInfo;
using AppExecFwk::ExtensionAbilityInfo;
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
        std::vector<std::string> tech;
    };

    struct HceAppAidInfo {
        ElementName element;
        int32_t labelId;
        int32_t iconId;
        std::vector<AidInfo> customDataAid;
    };

    std::vector<TagAppTechInfo> g_tagAppAndTechMap;
    std::vector<HceAppAidInfo> g_hceAppAndAidMap;
    std::vector<HceAppAidInfo> g_offHostAppAndAidMap;

    static AppDataParser& GetInstance();

    bool HandleAppAddOrChangedEvent(std::shared_ptr<EventFwk::CommonEventData> data);
    bool HandleAppRemovedEvent(std::shared_ptr<EventFwk::CommonEventData> data);
    void InitAppList();
    std::vector<ElementName> GetDispatchTagAppsByTech(std::vector<int> discTechList);
#ifdef VENDOR_APPLICATIONS_ENABLED
    std::vector<ElementName> GetVendorDispatchTagAppsByTech(std::vector<int>& discTechList);
    void RegQueryApplicationCb(sptr<IQueryAppInfoCallback> callback);
    void RegCardEmulationNotifyCb(sptr<IOnCardEmulationNotifyCb> callback);
    sptr<IOnCardEmulationNotifyCb> GetNotifyCardEmulationCallback() const;
#endif
    void GetHceAppsByAid(const std::string &aid, std::vector<AppDataParser::HceAppAidInfo>& hceApps);
    bool IsBundleInstalled(const std::string &bundleName);
    void GetHceApps(std::vector<HceAppAidInfo> &hceApps);
    void GetPaymentAbilityInfos(std::vector<AbilityInfo> &paymentAbilityInfos);
    bool GetBundleInfo(AppExecFwk::BundleInfo &bundleInfo, const std::string &bundleName);
    bool IsSystemApp(uint32_t uid);
    bool IsHceApp(const ElementName &elementName);
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
    void UpdateOffHostAppList(AbilityInfo &abilityInfo, ElementName &element);
    bool HaveMatchedOffHostKeyElement(ElementName &element);
    bool RemoveTagAppInfo(ElementName &element);
    bool RemoveHceAppInfo(ElementName &element);
    bool RemoveOffHostAppInfo(ElementName &element);
    bool IsPaymentApp(const AppDataParser::HceAppAidInfo &hceAppInfo);
#ifdef VENDOR_APPLICATIONS_ENABLED
    void GetHceAppsFromVendor(std::vector<HceAppAidInfo> &hceApps);
    void GetPaymentAbilityInfosFromVendor(std::vector<AbilityInfo> &paymentAbilityInfos);
    sptr<IQueryAppInfoCallback> queryApplicationByVendor_ {};
    sptr<IOnCardEmulationNotifyCb> onCardEmulationNotify_ {};
#endif
};
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_EVENT_HANDLER_H
