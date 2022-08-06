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
#include "nfc_controller_callback_stub.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
const std::string NFC_INTERFACE_TOKEN = "ohos.nfc.INfcController";
static NfcControllerCallBackStub* g_nfcControllerCallbackStub = new NfcControllerCallBackStub;

NfcControllerProxy ::~NfcControllerProxy() {}

bool NfcControllerProxy::TurnOn()
{
    DebugLog("NfcControllerProxy::TurnOn in.");
    bool result = false;
    MessageParcel data;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DebugLog("Write interface token error");
        return KITS::NFC_FAILED;
    }
    int32_t res = ProcessBoolRes(KITS::COMMAND_TURN_ON, data, option, result);
    DebugLog("NfcControllerProxy::TurnOn res=%{public}d", res);
    if (res != ERR_NONE) {
        DebugLog("NfcControllerProxy::TurnOn error.");
        return false;
    }
    DebugLog("NfcControllerProxy::TurnOn result=%{public}d", result);
    return result;
}

bool NfcControllerProxy::TurnOff()
{
    DebugLog("NfcControllerProxy::TurnOff in.");
    bool result = false;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DebugLog("Write interface token error");
        return KITS::NFC_FAILED;
    }
    MessageOption option;
    int res = ProcessBoolRes(KITS::COMMAND_TURN_OFF, data, option, result);
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
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DebugLog("Write interface token error");
        return KITS::NFC_FAILED;
    }
    int res = ProcessIntRes(KITS::COMMAND_GET_STATE, data, option, state);
    if (res != ERR_NONE) {
        InfoLog("It is failed To Get State with Res(%d).", res);
        return NFC::KITS::STATE_OFF;
    }
    return state;
}

bool NfcControllerProxy::IsNfcOpen()
{
    DebugLog("NfcControllerProxy::IsNfcOpen.");
    bool result = true;
    MessageParcel data;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DebugLog("Write interface token error");
        return KITS::NFC_FAILED;
    }
    data.WriteInt32(0);
    int res = ProcessBoolRes(KITS::COMMAND_IS_NFC_OPEN, data, option, result);
    if (res != ERR_NONE) {
        DebugLog("NfcControllerProxy::IsNfcOpen error.");
        return false;
    }
    DebugLog("NfcControllerProxy::IsNfcOpen result=%{public}d", result);
    return result;
}

KITS::NfcErrorCode NfcControllerProxy::RegisterCallBack(
    const sptr<INfcControllerCallback> &callback,
    const std::string& type)
{
    DebugLog("RegisterCallBack start!");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);

    g_nfcControllerCallbackStub->RegisterCallBack(callback);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DebugLog("Write interface token error");
        return KITS::NFC_FAILED;
    }
    if (!data.WriteString(type)) {
        DebugLog("Write type error");
        return KITS::NFC_FAILED;
    }
    data.WriteInt32(0);
    if (!data.WriteRemoteObject(g_nfcControllerCallbackStub->AsObject())) {
        DebugLog("RegisterCallBack WriteRemoteObject failed!");
        return KITS::NFC_FAILED;
    }

    int error = ProcessCallBackCommand(KITS::COMMAND_REGISTER_CALLBACK, data, reply, option);
    if (error != ERR_NONE) {
        InfoLog("RegisterCallBack failed, error code is %{public}d", error);
        return KITS::NFC_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return KITS::NFC_FAILED;
    }
    return KITS::NFC_SUCCESS;
}

KITS::NfcErrorCode NfcControllerProxy::UnRegisterCallBack(const std::string& type)
{
    DebugLog("UnRegisterCallBack start!");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DebugLog("Write interface token error");
        return KITS::NFC_FAILED;
    }
    if (!data.WriteString(type)) {
        DebugLog("Write type error");
        return KITS::NFC_FAILED;
    }
    data.WriteInt32(0);
    int error = ProcessCallBackCommand(KITS::COMMAND_UNREGISTER_CALLBACK, data, reply, option);
    if (error != ERR_NONE) {
        InfoLog("RegisterCallBack failed, error code is %{public}d", error);
        return KITS::NFC_FAILED;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        return KITS::NFC_FAILED;
    }
    return KITS::NFC_SUCCESS;
}
}  // namespace NFC
}  // namespace OHOS
