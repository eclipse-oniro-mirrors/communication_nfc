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
#include "external_deps_proxy.h"

namespace OHOS {
namespace NFC {
bool ExternalDepsProxy::HandleAppAddOrChangedEvent(std::shared_ptr<EventFwk::CommonEventData> data)
{
    return AppDataParser::GetInstance().HandleAppAddOrChangedEvent(data);
}

bool ExternalDepsProxy::HandleAppRemovedEvent(std::shared_ptr<EventFwk::CommonEventData> data)
{
    return AppDataParser::GetInstance().HandleAppRemovedEvent(data);
}

void ExternalDepsProxy::InitAppList()
{
    AppDataParser::GetInstance().InitAppList();
}

std::vector<ElementName> ExternalDepsProxy::GetDispatchTagAppsByTech(std::vector<int> discTechList)
{
    return AppDataParser::GetInstance().GetDispatchTagAppsByTech(discTechList);
}

#ifdef VENDOR_APPLICATIONS_ENABLED
std::vector<ElementName> ExternalDepsProxy::GetVendorDispatchTagAppsByTech(std::vector<int> discTechList)
{
    return AppDataParser::GetInstance().GetVendorDispatchTagAppsByTech(discTechList);
}

void ExternalDepsProxy::RegQueryApplicationCb(sptr<IQueryAppInfoCallback> callback)
{
    AppDataParser::GetInstance().RegQueryApplicationCb(callback);
}

void ExternalDepsProxy::RegCardEmulationNotifyCb(sptr<IOnCardEmulationNotifyCb> callback)
{
    AppDataParser::GetInstance().RegCardEmulationNotifyCb(callback);
}

sptr<IOnCardEmulationNotifyCb> ExternalDepsProxy::GetNotifyCardEmulationCallback()
{
    return AppDataParser::GetInstance().GetNotifyCardEmulationCallback();
}
#endif

KITS::ErrorCode ExternalDepsProxy::NfcDataGetValue(Uri &uri, const std::string &column, int32_t &value)
{
    return NfcDataShareImpl::GetInstance()->GetValue(uri, column, value);
}

KITS::ErrorCode ExternalDepsProxy::NfcDataSetValue(Uri &uri, const std::string &column, int &value)
{
    return NfcDataShareImpl::GetInstance()->SetValue(uri, column, value);
}

void ExternalDepsProxy::NfcDataSetString(const std::string& key, const std::string& value)
{
    NfcPreferences::GetInstance().SetString(key, value);
}

std::string ExternalDepsProxy::NfcDataGetString(const std::string& key)
{
    return NfcPreferences::GetInstance().GetString(key);
}

void ExternalDepsProxy::NfcDataSetInt(const std::string& key, const int value)
{
    NfcPreferences::GetInstance().SetInt(key, value);
}

int ExternalDepsProxy::NfcDataGetInt(const std::string& key)
{
    return NfcPreferences::GetInstance().GetInt(key);
}

void ExternalDepsProxy::NfcDataClear()
{
    NfcPreferences::GetInstance().Clear();
}

void ExternalDepsProxy::NfcDataDelete(const std::string& key)
{
    NfcPreferences::GetInstance().Delete(key);
}

void ExternalDepsProxy::UpdateNfcState(int newState)
{
    NfcPreferences::GetInstance().UpdateNfcState(newState);
}

void ExternalDepsProxy::PublishNfcStateChanged(int newState)
{
    NfcEventPublisher::PublishNfcStateChanged(newState);
}

void ExternalDepsProxy::PublishNfcFieldStateChanged(bool isFieldOn)
{
    NfcEventPublisher::PublishNfcFieldStateChanged(isFieldOn);
}

void ExternalDepsProxy::WriteNfcFailedHiSysEvent(const NfcFailedParams* failedParams)
{
    NfcHisysEvent::WriteNfcFailedHiSysEvent(failedParams);
}

void ExternalDepsProxy::WriteOpenAndCloseHiSysEvent(int openRequestCnt, int openFailCnt,
                                                    int closeRequestCnt, int closeFailCnt)
{
    NfcHisysEvent::WriteOpenAndCloseHiSysEvent(openRequestCnt, openFailCnt,
        closeRequestCnt, closeFailCnt);
}

void ExternalDepsProxy::WriteHceSwipeResultHiSysEvent(const std::string &appPackageName, int hceSwipeCnt)
{
    NfcHisysEvent::WriteHceSwipeResultHiSysEvent(appPackageName, hceSwipeCnt);
}

void ExternalDepsProxy::WriteDefaultPaymentAppChangeHiSysEvent(const std::string &oldAppPackageName,
                                                               const std::string &newAppPackageName)
{
    NfcHisysEvent::WriteDefaultPaymentAppChangeHiSysEvent(oldAppPackageName, newAppPackageName);
}

void ExternalDepsProxy::WriteForegroundAppChangeHiSysEvent(const std::string &appPackageName)
{
    NfcHisysEvent::WriteForegroundAppChangeHiSysEvent(appPackageName);
}

void ExternalDepsProxy::WriteTagFoundHiSysEvent(const std::vector<int>& techList)
{
    NfcHisysEvent::WriteTagFoundHiSysEvent(techList);
}

void ExternalDepsProxy::WritePassiveListenHiSysEvent(int requestCnt, int failCnt)
{
    NfcHisysEvent::WritePassiveListenHiSysEvent(requestCnt, failCnt);
}

void ExternalDepsProxy::WriteFirmwareUpdateHiSysEvent(int requestCnt, int failCnt)
{
    NfcHisysEvent::WriteFirmwareUpdateHiSysEvent(requestCnt, failCnt);
}

void ExternalDepsProxy::BuildFailedParams(NfcFailedParams &nfcFailedParams,
                                          MainErrorCode mainErrorCode,
                                          SubErrorCode subErrorCode)
{
    NfcHisysEvent::BuildFailedParams(nfcFailedParams, mainErrorCode, subErrorCode);
}

bool ExternalDepsProxy::IsGranted(std::string permission)
{
    return NfcPermissionChecker::IsGranted(permission);
}

void ExternalDepsProxy::DispatchTagAbility(std::shared_ptr<KITS::TagInfo> tagInfo,
                                           OHOS::sptr<IRemoteObject> tagServiceIface)
{
    TAG::TagAbilityDispatcher::DispatchTagAbility(tagInfo, tagServiceIface);
}

void ExternalDepsProxy::StartVibratorOnce()
{
    TAG::TagAbilityDispatcher::StartVibratorOnce();
}

void ExternalDepsProxy::GetPaymentAbilityInfos(std::vector<AbilityInfo>& paymentAbilityInfos)
{
    AppDataParser::GetInstance().GetPaymentAbilityInfos(paymentAbilityInfos);
}

void ExternalDepsProxy::GetHceAppsByAid(const std::string& aid, std::vector<AppDataParser::HceAppAidInfo>& hceApps)
{
    AppDataParser::GetInstance().GetHceAppsByAid(aid, hceApps);
}

void ExternalDepsProxy::GetHceApps(std::vector<AppDataParser::HceAppAidInfo>& hceApps)
{
    AppDataParser::GetInstance().GetHceApps(hceApps);
}
bool ExternalDepsProxy::IsSystemApp(uint32_t uid)
{
    return AppDataParser::GetInstance().IsSystemApp(uid);
}

bool ExternalDepsProxy::IsHceApp(const ElementName& elementName)
{
    return AppDataParser::GetInstance().IsHceApp(elementName);
}

bool ExternalDepsProxy::IsBundleInstalled(const std::string& bundleName)
{
    return AppDataParser::GetInstance().IsBundleInstalled(bundleName);
}

bool ExternalDepsProxy::GetBundleInfo(AppExecFwk::BundleInfo& bundleInfo, const std::string& bundleName)
{
    return AppDataParser::GetInstance().GetBundleInfo(bundleInfo, bundleName);
}

void ExternalDepsProxy::SetWantExtraParam(std::shared_ptr<KITS::TagInfo> &tagInfo, AAFwk::Want &want)
{
    TAG::TagAbilityDispatcher::SetWantExtraParam(tagInfo, want);
}

void ExternalDepsProxy::PublishNfcNotification(int notificationId, const std::string &name, int balance)
{
    TAG::NfcNotificationPublisher::GetInstance().PublishNfcNotification(notificationId, name, balance);
}

void ExternalDepsProxy::RegNotificationCallback(std::weak_ptr<NfcService> nfcService)
{
    TAG::NfcNotificationPublisher::GetInstance().RegNotificationCallback(nfcService);
}
} // namespace NFC
} // OHOS