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
#include "run_on_demaind_proxy.h"

namespace OHOS {
namespace NFC {
void RunOnDemaindProxy::HandleAppAddOrChangedEvent(std::shared_ptr<EventFwk::CommonEventData> data)
{
    AppDataParser::GetInstance().HandleAppAddOrChangedEvent(data);
}

void RunOnDemaindProxy::HandleAppRemovedEvent(std::shared_ptr<EventFwk::CommonEventData> data)
{
    AppDataParser::GetInstance().HandleAppRemovedEvent(data);
}

void RunOnDemaindProxy::InitAppList()
{
    AppDataParser::GetInstance().InitAppList();
}

std::vector<ElementName> RunOnDemaindProxy::GetDispatchTagAppsByTech(std::vector<int> discTechList)
{
    return AppDataParser::GetInstance().GetDispatchTagAppsByTech(discTechList);
}

KITS::ErrorCode RunOnDemaindProxy::NfcDataGetValue(Uri &uri, const std::string &column, int32_t &value)
{
    return NfcDataShareImpl::GetInstance()->GetValue(uri, column, value);
}

KITS::ErrorCode RunOnDemaindProxy::NfcDataSetValue(Uri &uri, const std::string &column, int &value)
{
    return NfcDataShareImpl::GetInstance()->SetValue(uri, column, value);
}

void RunOnDemaindProxy::NfcDataSetString(const std::string& key, const std::string& value)
{
    NfcDatabaseHelper::GetInstance().SetString(key, value);
}

std::string RunOnDemaindProxy::NfcDataGetString(const std::string& key)
{
    return NfcDatabaseHelper::GetInstance().GetString(key);
}

void RunOnDemaindProxy::NfcDataSetInt(const std::string& key, const int value)
{
    NfcDatabaseHelper::GetInstance().SetInt(key, value);
}

int RunOnDemaindProxy::NfcDataGetInt(const std::string& key)
{
    return NfcDatabaseHelper::GetInstance().GetInt(key);
}

void RunOnDemaindProxy::NfcDataClear()
{
    NfcDatabaseHelper::GetInstance().Clear();
}

void RunOnDemaindProxy::NfcDataDelete(const std::string& key)
{
    NfcDatabaseHelper::GetInstance().Delete(key);
}

void RunOnDemaindProxy::UpdateNfcState(int newState)
{
    NfcDatabaseHelper::GetInstance().UpdateNfcState(newState);
}

void RunOnDemaindProxy::PublishNfcStateChanged(int newState)
{
    NfcEventPublisher::PublishNfcStateChanged(newState);
}

void RunOnDemaindProxy::PublishNfcFieldStateChanged(bool isFieldOn)
{
    NfcEventPublisher::PublishNfcFieldStateChanged(isFieldOn);
}

void RunOnDemaindProxy::WriteNfcFailedHiSysEvent(const NfcFailedParams* failedParams)
{
    NfcHisysEvent::WriteNfcFailedHiSysEvent(failedParams);
}

void RunOnDemaindProxy::WriteOpenAndCloseHiSysEvent(int openRequestCnt, int openFailCnt,
                                                    int closeRequestCnt, int closeFailCnt)
{
    NfcHisysEvent::WriteOpenAndCloseHiSysEvent(openRequestCnt, openFailCnt,
        closeRequestCnt, closeFailCnt);
}

void RunOnDemaindProxy::WriteTagFoundHiSysEvent(int tagFoundCnt, int typeACnt,
                                                int typeBCnt, int typeFCnt, int typeVCnt)
{
    NfcHisysEvent::WriteTagFoundHiSysEvent(tagFoundCnt, typeACnt, typeBCnt, typeFCnt, typeVCnt);
}

void RunOnDemaindProxy::WritePassiveListenHiSysEvent(int requestCnt, int failCnt)
{
    NfcHisysEvent::WritePassiveListenHiSysEvent(requestCnt, failCnt);
}

void RunOnDemaindProxy::WriteFirmwareUpdateHiSysEvent(int requestCnt, int failCnt)
{
    NfcHisysEvent::WriteFirmwareUpdateHiSysEvent(requestCnt, failCnt);
}

void RunOnDemaindProxy::BuildFailedParams(NfcFailedParams &nfcFailedParams,
                                          MainErrorCode mainErrorCode,
                                          SubErrorCode subErrorCode)
{
    NfcHisysEvent::BuildFailedParams(nfcFailedParams, mainErrorCode, subErrorCode);
}

bool RunOnDemaindProxy::IsGranted(std::string permission)
{
    return PermissionTools::IsGranted(permission);
}

void RunOnDemaindProxy::DispatchTagAbility(std::shared_ptr<KITS::TagInfo> tagInfo,
                                           OHOS::sptr<IRemoteObject> tagServiceIface)
{
    TAG::TagAbilityDispatcher::DispatchTagAbility(tagInfo, tagServiceIface);
}

void RunOnDemaindProxy::StartVibratorOnce()
{
    TAG::TagAbilityDispatcher::StartVibratorOnce();
}
} // NFC
} // OHOS