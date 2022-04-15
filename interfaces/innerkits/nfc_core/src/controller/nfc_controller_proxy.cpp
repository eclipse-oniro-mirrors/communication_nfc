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
#include "nfc_controller_proxy.h"

#include "loghelper.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
const std::string NFC_INTERFACE_TOKEN = "ohos.nfc.INfcController";
NfcControllerProxy ::~NfcControllerProxy() {}
bool NfcControllerProxy::TurnOn()
{
    DebugLog("NfcControllerProxy::TurnOn in.");
    bool result = false;
    MessageParcel data;
    MessageOption option(MessageOption::TF_ASYNC);
    int32_t res = ProcessBoolRes(COMMAND_TURN_ON, data, option, result);
    if (res != ERR_NONE) {
        DebugLog("NfcControllerProxy::TurnOn error.");
        return false;
    }
    return result;
}

bool NfcControllerProxy::TurnOff(bool saveState)
{
    DebugLog("NfcControllerProxy::TurnOff in.");
    bool result = false;
    MessageParcel data;
    data.WriteBool(saveState);
    MessageOption option(MessageOption::TF_ASYNC);
    int res = ProcessBoolRes(COMMAND_TURN_OFF, data, option, result);
    if (res != ERR_NONE) {
        DebugLog("NfcControllerProxy::TurnOff error.");
        return false;
    }
    return result;
}

int NfcControllerProxy::GetState()
{
    int state = NFC::KITS::STATE_OFF;
    MessageParcel data;
    MessageOption option;
    int res = ProcessIntRes(COMMAND_GET_STATE, data, option, state);
    if (res != ERR_NONE) {
        InfoLog("It is failed To Get State with Res(%d).", res);
        return NFC::KITS::STATE_OFF;
    }
    return state;
}
}  // namespace NFC
}  // namespace OHOS
