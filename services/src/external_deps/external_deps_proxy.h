/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#ifndef EXTERNAL_DEPS_PROXY_H
#define EXTERNAL_DEPS_PROXY_H
#include <vector>

#include "app_data_parser.h"
#include "common_event_manager.h"
#include "nfc_sdk_common.h"
#include "nfc_data_share_impl.h"
#include "nfc_preferences.h"
#include "nfc_event_publisher.h"
#include "nfc_hisysevent.h"
#include "nfc_permission_checker.h"
#include "nfc_notification_publisher.h"
#include "tag_ability_dispatcher.h"
#include "taginfo.h"
#include "ability_info.h"
#include "want.h"

namespace OHOS {
namespace NFC {
using OHOS::AppExecFwk::ElementName;
using AppExecFwk::AbilityInfo;
class ExternalDepsProxy {
public:
    static ExternalDepsProxy &GetInstance()
    {
        static ExternalDepsProxy instance;
        return instance;
    }

    bool HandleAppAddOrChangedEvent(std::shared_ptr<EventFwk::CommonEventData> data);
    bool HandleAppRemovedEvent(std::shared_ptr<EventFwk::CommonEventData> data);
    void InitAppList();
    std::vector<ElementName> GetDispatchTagAppsByTech(std::vector<int> discTechList);
#ifdef VENDOR_APPLICATIONS_ENABLED
    std::vector<ElementName> GetVendorDispatchTagAppsByTech(std::vector<int> discTechList);
    void RegQueryApplicationCb(sptr<IQueryAppInfoCallback> callback);
    void RegCardEmulationNotifyCb(sptr<IOnCardEmulationNotifyCb> callback);
    sptr<IOnCardEmulationNotifyCb> GetNotifyCardEmulationCallback();
#endif

    KITS::ErrorCode NfcDataGetValue(Uri &uri, const std::string &column, int32_t &value);
    KITS::ErrorCode NfcDataSetValue(Uri &uri, const std::string &column, int &value);

    void NfcDataSetString(const std::string& key, const std::string& value);
    std::string NfcDataGetString(const std::string& key);
    void NfcDataSetInt(const std::string& key, const int value);
    int NfcDataGetInt(const std::string& key);
    void NfcDataClear();
    void NfcDataDelete(const std::string& key);
    void UpdateNfcState(int newState);

    void PublishNfcStateChanged(int newState);
    void PublishNfcFieldStateChanged(bool isFieldOn);

    void WriteNfcFailedHiSysEvent(const NfcFailedParams* failedParams);
    void WriteOpenAndCloseHiSysEvent(int openRequestCnt, int openFailCnt,
                                     int closeRequestCnt, int closeFailCnt);
    void WriteHceSwipeResultHiSysEvent(const std::string &appPackageName, int hceSwipeCnt);
    void WriteDefaultPaymentAppChangeHiSysEvent(const std::string &oldAppPackageName,
                                                const std::string &newAppPackageName);
    void WriteForegroundAppChangeHiSysEvent(const std::string &appPackageName);
    void WriteTagFoundHiSysEvent(const std::vector<int> &techList);
    void WritePassiveListenHiSysEvent(int requestCnt, int failCnt);
    void WriteFirmwareUpdateHiSysEvent(int requestCnt, int failCnt);
    void BuildFailedParams(NfcFailedParams &nfcFailedParams, MainErrorCode mainErrorCode, SubErrorCode subErrorCode);

    bool IsGranted(std::string permission);

    void DispatchTagAbility(std::shared_ptr<KITS::TagInfo> tagInfo, OHOS::sptr<IRemoteObject> tagServiceIface);
    void StartVibratorOnce();
    void GetPaymentAbilityInfos(std::vector<AbilityInfo> &paymentAbilityInfos);
    void GetHceAppsByAid(const std::string &aid, std::vector<AppDataParser::HceAppAidInfo>& hceApps);
    void GetHceApps(std::vector<AppDataParser::HceAppAidInfo> &hceApps);
    bool IsSystemApp(uint32_t uid);
    bool IsHceApp(const ElementName &elementName);
    bool IsBundleInstalled(const std::string &bundleName);
    bool GetBundleInfo(AppExecFwk::BundleInfo &bundleInfo, const std::string &bundleName);
    void SetWantExtraParam(std::shared_ptr<KITS::TagInfo> &tagInfo, AAFwk::Want &want);

    void PublishNfcNotification(int notificationId, const std::string &name, int balance);
    void RegNotificationCallback(std::weak_ptr<NfcService> nfcService);
};
} // NFC
} // OHOS
#endif // EXTERNAL_DEPS_PROXY_H