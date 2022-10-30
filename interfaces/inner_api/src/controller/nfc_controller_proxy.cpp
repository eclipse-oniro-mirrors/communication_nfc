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

int NfcControllerProxy::TurnOn()
{
    MessageParcel data;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("Write interface token error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    return SendRequestExpectReplyNone(KITS::COMMAND_TURN_ON, data, option);
}

int NfcControllerProxy::TurnOff()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("Write interface token error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    MessageOption option;
    return SendRequestExpectReplyNone(KITS::COMMAND_TURN_OFF, data, option);
}

int NfcControllerProxy::GetState()
{
    int state = NFC::KITS::STATE_OFF;
    MessageParcel data;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("Write interface token error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    int res = SendRequestExpectReplyInt(KITS::COMMAND_GET_STATE, data, option, state);
    if (res != ERR_NONE) {
        ErrorLog("It is failed To Get State with Res(%{public}d).", res);
        return NFC::KITS::STATE_OFF;
    }
    return state;
}

int NfcControllerProxy::IsNfcOpen(bool &isOpen)
{
    MessageParcel data;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("Write interface token error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    return SendRequestExpectReplyBool(KITS::COMMAND_IS_NFC_OPEN, data, option, isOpen);
}

KITS::ErrorCode NfcControllerProxy::RegisterCallBack(
    const sptr<INfcControllerCallback> &callback,
    const std::string& type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    g_nfcControllerCallbackStub->RegisterCallBack(callback);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("Write interface token error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!data.WriteString(type)) {
        ErrorLog("Write type error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    data.WriteInt32(0);
    if (!data.WriteRemoteObject(g_nfcControllerCallbackStub->AsObject())) {
        ErrorLog("RegisterCallBack WriteRemoteObject failed!");
        return KITS::ERR_NFC_PARAMETERS;
    }

    int error = SendRequestExpectReplyNone(KITS::COMMAND_REGISTER_CALLBACK, data, option);
    if (error != ERR_NONE) {
        ErrorLog("RegisterCallBack failed, error code is %{public}d", error);
        return KITS::ERR_NFC_PARAMETERS;
    }
    return KITS::ERR_NONE;
}

KITS::ErrorCode NfcControllerProxy::UnRegisterCallBack(const std::string& type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("Write interface token error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!data.WriteString(type)) {
        ErrorLog("Write type error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    data.WriteInt32(0);
    int error = SendRequestExpectReplyNone(KITS::COMMAND_UNREGISTER_CALLBACK, data, option);
    if (error != ERR_NONE) {
        ErrorLog("RegisterCallBack failed, error code is %{public}d", error);
        return KITS::ERR_NFC_PARAMETERS;
    }
    return KITS::ERR_NONE;
}

OHOS::sptr<IRemoteObject> NfcControllerProxy::GetTagServiceIface()
{
    DebugLog("GetTagServiceIface start!");
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("GetTagServiceIface, Write interface token error");
        return nullptr;
    }
    int32_t res = Remote()->SendRequest(KITS::COMMAND_GET_TAG_INTERFACE, data, reply, option);
    if (res != ERR_NONE) {
        ErrorLog("GetTagServiceIface SendRequest err %{public}d", res);
        return nullptr;
    }
    sptr<OHOS::IRemoteObject> remoteObject = reply.ReadRemoteObject();
    return remoteObject;
}
}  // namespace NFC
}  // namespace OHOS
