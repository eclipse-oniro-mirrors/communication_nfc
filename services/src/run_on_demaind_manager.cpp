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
#include "run_on_demaind_manager.h"

namespace OHOS {
namespace NFC {
RunOnDemaindManager::RunOnDemaindManager()
{}

RunOnDemaindManager::~RunOnDemaindManager()
{}

void RunOnDemaindManager::HandleAppAddOrChangedEvent(std::shared_ptr<EventFwk::CommonEventData> data)
{
    RunOnDemaindProxy::GetInstance().HandleAppAddOrChangedEvent(data);
}

void RunOnDemaindManager::HandleAppRemovedEvent(std::shared_ptr<EventFwk::CommonEventData> data)
{
    RunOnDemaindProxy::GetInstance().HandleAppRemovedEvent(data);
}

void RunOnDemaindManager::InitAppList()
{
    RunOnDemaindProxy::GetInstance().InitAppList();
}

std::vector<ElementName> RunOnDemaindManager::GetDispatchTagAppsByTech(std::vector<int> discTechList)
{
    return RunOnDemaindProxy::GetInstance().GetDispatchTagAppsByTech(discTechList);
}

std::vector<ElementName> RunOnDemaindManager::GetVendorDispatchTagAppsByTech(std::vector<int> discTechList)
{
    return RunOnDemaindProxy::GetInstance().GetVendorDispatchTagAppsByTech(discTechList);
}

void RunOnDemaindManager::RegQueryApplicationCb(QueryApplicationByVendor callback)
{
    RunOnDemaindProxy::GetInstance().RegQueryApplicationCb(callback);
}

KITS::ErrorCode RunOnDemaindManager::NfcDataGetValue(Uri &uri, const std::string &column, int32_t &value)
{
    return RunOnDemaindProxy::GetInstance().NfcDataGetValue(uri, column, value);
}

KITS::ErrorCode RunOnDemaindManager::NfcDataSetValue(Uri &uri, const std::string &column, int &value)
{
    return RunOnDemaindProxy::GetInstance().NfcDataSetValue(uri, column, value);
}

void RunOnDemaindManager::NfcDataSetString(const std::string& key, const std::string& value)
{
    RunOnDemaindProxy::GetInstance().NfcDataSetString(key, value);
}

std::string RunOnDemaindManager::NfcDataGetString(const std::string& key)
{
    return RunOnDemaindProxy::GetInstance().NfcDataGetString(key);
}

void RunOnDemaindManager::NfcDataSetInt(const std::string& key, const int value)
{
    RunOnDemaindProxy::GetInstance().NfcDataSetInt(key, value);
}

int RunOnDemaindManager::NfcDataGetInt(const std::string& key)
{
    return RunOnDemaindProxy::GetInstance().NfcDataGetInt(key);
}

void RunOnDemaindManager::NfcDataClear()
{
    RunOnDemaindProxy::GetInstance().NfcDataClear();
}

void RunOnDemaindManager::NfcDataDelete(const std::string& key)
{
    RunOnDemaindProxy::GetInstance().NfcDataDelete(key);
}

void RunOnDemaindManager::UpdateNfcState(int newState)
{
    RunOnDemaindProxy::GetInstance().UpdateNfcState(newState);
}

void RunOnDemaindManager::PublishNfcStateChanged(int newState)
{
    RunOnDemaindProxy::GetInstance().PublishNfcStateChanged(newState);
}

void RunOnDemaindManager::PublishNfcFieldStateChanged(bool isFieldOn)
{
    RunOnDemaindProxy::GetInstance().PublishNfcFieldStateChanged(isFieldOn);
}

void RunOnDemaindManager::WriteNfcFailedHiSysEvent(const NfcFailedParams* failedParams)
{
    RunOnDemaindProxy::GetInstance().WriteNfcFailedHiSysEvent(failedParams);
}

void RunOnDemaindManager::WriteOpenAndCloseHiSysEvent(int openRequestCnt, int openFailCnt,
                                                      int closeRequestCnt, int closeFailCnt)
{
    RunOnDemaindProxy::GetInstance().WriteOpenAndCloseHiSysEvent(openRequestCnt, openFailCnt,
        closeRequestCnt, closeFailCnt);
}

void RunOnDemaindManager::WriteTagFoundHiSysEvent(int tagFoundCnt, int typeACnt,
                                                  int typeBCnt, int typeFCnt, int typeVCnt)
{
    RunOnDemaindProxy::GetInstance().WriteTagFoundHiSysEvent(tagFoundCnt, typeACnt,
        typeBCnt, typeFCnt, typeVCnt);
}

void RunOnDemaindManager::WritePassiveListenHiSysEvent(int requestCnt, int failCnt)
{
    RunOnDemaindProxy::GetInstance().WritePassiveListenHiSysEvent(requestCnt, failCnt);
}

void RunOnDemaindManager::WriteFirmwareUpdateHiSysEvent(int requestCnt, int failCnt)
{
    RunOnDemaindProxy::GetInstance().WriteFirmwareUpdateHiSysEvent(requestCnt, failCnt);
}

void RunOnDemaindManager::BuildFailedParams(NfcFailedParams &nfcFailedParams,
                                            MainErrorCode mainErrorCode,
                                            SubErrorCode subErrorCode)
{
    RunOnDemaindProxy::GetInstance().BuildFailedParams(nfcFailedParams, mainErrorCode, subErrorCode);
}

bool RunOnDemaindManager::IsGranted(std::string permission)
{
    return RunOnDemaindProxy::GetInstance().IsGranted(permission);
}

void RunOnDemaindManager::DispatchTagAbility(std::shared_ptr<KITS::TagInfo> tagInfo,
                                             OHOS::sptr<IRemoteObject> tagServiceIface)
{
    RunOnDemaindProxy::GetInstance().DispatchTagAbility(tagInfo, tagServiceIface);
}

void RunOnDemaindManager::StartVibratorOnce()
{
    return RunOnDemaindProxy::GetInstance().StartVibratorOnce();
}
} // NFC
} // OHOS