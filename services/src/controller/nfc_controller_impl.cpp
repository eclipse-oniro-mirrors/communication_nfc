/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "nfc_controller_impl.h"

#include "nfc_sdk_common.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
const std::string DUMP_LINE = "---------------------------";
const std::string DUMP_END = "\n";

NfcControllerImpl::NfcControllerImpl(std::weak_ptr<NfcService> nfcService)
    : NfcControllerStub(), nfcService_(nfcService)
{
}

NfcControllerImpl::~NfcControllerImpl()
{
}

int NfcControllerImpl::GetState()
{
    return nfcService_.lock()->GetNfcState();
}

int NfcControllerImpl::TurnOn()
{
    nfcService_.lock()->ExecuteTask(KITS::TASK_TURN_ON);
    return KITS::ERR_NONE;
}

int NfcControllerImpl::TurnOff()
{
    nfcService_.lock()->ExecuteTask(KITS::TASK_TURN_OFF);
    return KITS::ERR_NONE;
}

int NfcControllerImpl::IsNfcOpen(bool &isOpen)
{
    isOpen = nfcService_.lock()->IsNfcEnabled();
    return KITS::ERR_NONE;
}

KITS::ErrorCode NfcControllerImpl::RegisterCallBack(const sptr<INfcControllerCallback> &callback,
    const std::string& type, Security::AccessToken::AccessTokenID callerToken)
{
    if (!nfcService_.lock()->SetRegisterCallBack(callback, type, callerToken)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_NFC_PARAMETERS;
}

KITS::ErrorCode NfcControllerImpl::UnRegisterCallBack(const std::string& type,
    Security::AccessToken::AccessTokenID callerToken)
{
    if (!nfcService_.lock()->RemoveRegisterCallBack(type, callerToken)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_NFC_PARAMETERS;
}

KITS::ErrorCode NfcControllerImpl::UnRegisterAllCallBack(Security::AccessToken::AccessTokenID callerToken)
{
    if (!nfcService_.lock()->RemoveAllRegisterCallBack(callerToken)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_NFC_PARAMETERS;
}

OHOS::sptr<IRemoteObject> NfcControllerImpl::GetTagServiceIface()
{
    return nfcService_.lock()->GetTagServiceIface();
}

int32_t NfcControllerImpl::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    std::string info = GetDumpInfo();
    int ret = dprintf(fd, "%s\n", info.c_str());
    if (ret < 0) {
        ErrorLog("NfcControllerImpl Dump ret = %{public}d", ret);
        return KITS::ERR_NFC_PARAMETERS;
    }
    return KITS::ERR_NONE;
}

std::string NfcControllerImpl::GetDumpInfo()
{
    std::string info;
    return info.append(DUMP_LINE)
        .append(" NFC DUMP ")
        .append(DUMP_LINE)
        .append(DUMP_END)
        .append("NFC_STATE          : ")
        .append(std::to_string(nfcService_.lock()->GetNfcState()))
        .append(DUMP_END)
        .append("SCREEN_STATE       : ")
        .append(std::to_string(nfcService_.lock()->GetScreenState()))
        .append(DUMP_END)
        .append("NCI_VERSION        : ")
        .append(std::to_string(nfcService_.lock()->GetNciVersion()))
        .append(DUMP_END);
}
}  // namespace NFC
}  // namespace OHOS
