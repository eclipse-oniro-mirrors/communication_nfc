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
void ExternalDepsProxy::HandleAppAddOrChangedEvent(std::shared_ptr<EventFwk::CommonEventData> data)
{
    AppDataParser::GetInstance().HandleAppAddOrChangedEvent(data);
}

void ExternalDepsProxy::HandleAppRemovedEvent(std::shared_ptr<EventFwk::CommonEventData> data)
{
    AppDataParser::GetInstance().HandleAppRemovedEvent(data);
}

void ExternalDepsProxy::InitAppList()
{
    AppDataParser::GetInstance().InitAppList();
}

std::vector<ElementName> ExternalDepsProxy::GetDispatchTagAppsByTech(std::vector<int> discTechList)
{
    return AppDataParser::GetInstance().GetDispatchTagAppsByTech(discTechList);
}

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

void ExternalDepsProxy::WriteTagFoundHiSysEvent(int tagFoundCnt, int typeACnt,
                                                int typeBCnt, int typeFCnt, int typeVCnt)
{
    NfcHisysEvent::WriteTagFoundHiSysEvent(tagFoundCnt, typeACnt, typeBCnt, typeFCnt, typeVCnt);
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
} // namespace NFC
} // OHOS