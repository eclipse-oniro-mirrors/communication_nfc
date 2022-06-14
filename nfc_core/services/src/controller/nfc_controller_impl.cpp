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

bool NfcControllerImpl::TurnOn()
{
    nfcService_.lock()->ExecuteTask(KITS::TASK_TURN_ON, true);
    return true;
}

bool NfcControllerImpl::TurnOff(bool saveState)
{
    nfcService_.lock()->ExecuteTask(KITS::TASK_TURN_OFF, saveState);
    return true;
}

bool NfcControllerImpl::IsNfcOpen()
{
    return nfcService_.lock()->IsNfcEnabled();
}

KITS::NfcErrorCode NfcControllerImpl::RegisterCallBack(const sptr<INfcControllerCallback> &callback,
    const std::string& type, Security::AccessToken::AccessTokenID callerToken)
{
    DebugLog("RegisterCallBack NfcControllerImpl");
    if (!nfcService_.lock()->SetRegisterCallBack(callback, type, callerToken)) {
        return KITS::NFC_SUCCESS;
    }
    return KITS::NFC_FAILED;
}

KITS::NfcErrorCode NfcControllerImpl::UnRegisterCallBack(const std::string& type,
    Security::AccessToken::AccessTokenID callerToken)
{
    DebugLog("UnRegisterCallBack NfcControllerImpl");
    if (!nfcService_.lock()->RemoveRegisterCallBack(type, callerToken)) {
        return KITS::NFC_SUCCESS;
    }
    return KITS::NFC_FAILED;
}

KITS::NfcErrorCode NfcControllerImpl::UnRegisterAllCallBack(Security::AccessToken::AccessTokenID callerToken)
{
    DebugLog("UnRegisterAllCallBack NfcControllerImpl");
    if (!nfcService_.lock()->RemoveAllRegisterCallBack(callerToken)) {
        return KITS::NFC_SUCCESS;
    }
    return KITS::NFC_FAILED;
}
}  // namespace NFC
}  // namespace OHOS
