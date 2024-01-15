/*
 * Copyright (C) 2022 - 2023 Huawei Device Co., Ltd.
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

#ifdef VENDOR_APPLICATIONS_ENABLED
#include "iquery_app_info_callback.h"
#endif

#include "loghelper.h"
#include "ndef_msg_callback_stub.h"
#include "nfc_controller_callback_stub.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
namespace NFC {
const std::string NFC_INTERFACE_TOKEN = "ohos.nfc.INfcController";
static sptr<NfcControllerCallBackStub> g_nfcControllerCallbackStub =
    sptr<NfcControllerCallBackStub>(new (std::nothrow) NfcControllerCallBackStub());
static sptr<NdefMsgCallbackStub> g_ndefMsgCallbackStub =
    sptr<NdefMsgCallbackStub>(new (std::nothrow) NdefMsgCallbackStub());

NfcControllerProxy ::~NfcControllerProxy() {}

int NfcControllerProxy::TurnOn()
{
    MessageParcel data;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("Write interface token error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    return SendRequestExpectReplyNone(static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_TURN_ON),
        data, option);
}

int NfcControllerProxy::TurnOff()
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("Write interface token error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    MessageOption option;
    return SendRequestExpectReplyNone(static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_TURN_OFF),
        data, option);
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
    int res = SendRequestExpectReplyInt(static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_GET_STATE),
        data, option, state);
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
    return SendRequestExpectReplyBool(static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_IS_NFC_OPEN),
        data, option, isOpen);
}

KITS::ErrorCode NfcControllerProxy::RegisterCallBack(
    const sptr<INfcControllerCallback> &callback,
    const std::string& type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (g_nfcControllerCallbackStub == nullptr) {
        ErrorLog("%{public}s:g_nfcControllerCallbackStub is nullptr", __func__);
        return KITS::ERR_NFC_PARAMETERS;
    }
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

    int error = SendRequestExpectReplyNone(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_REGISTER_CALLBACK),
        data, option);
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
    int error = SendRequestExpectReplyNone(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_UNREGISTER_CALLBACK),
        data, option);
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
    int32_t res = Remote()->SendRequest(static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_GET_TAG_INTERFACE),
        data, reply, option);
    if (res != ERR_NONE) {
        ErrorLog("GetTagServiceIface SendRequest err %{public}d", res);
        return nullptr;
    }
    sptr<OHOS::IRemoteObject> remoteObject = reply.ReadRemoteObject();
    return remoteObject;
}

KITS::ErrorCode NfcControllerProxy::RegNdefMsgCb(const sptr<INdefMsgCallback> &callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (g_ndefMsgCallbackStub == nullptr) {
        ErrorLog("NfcControllerProxy::RegNdefMsgCb:g_ndefMsgCallbackStub is nullptr");
        return KITS::ERR_NFC_PARAMETERS;
    }
    g_ndefMsgCallbackStub->RegisterCallback(callback);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("NfcControllerProxy::RegNdefMsgCb Write interface token error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!data.WriteRemoteObject(g_ndefMsgCallbackStub->AsObject())) {
        ErrorLog("NfcControllerProxy::RegNdefMsgCb WriteRemoteObject failed!");
        return KITS::ERR_NFC_PARAMETERS;
    }

    int error = SendRequestExpectReplyNone(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_REG_NDEF_MSG_CALLBACK),
        data, option);
    if (error != ERR_NONE) {
        ErrorLog("NfcControllerProxy::RegNdefMsgCb failed, error code is %{public}d", error);
        return KITS::ERR_NFC_PARAMETERS;
    }
    return KITS::ERR_NONE;
}

#ifdef VENDOR_APPLICATIONS_ENABLED
KITS::ErrorCode NfcControllerProxy::RegQueryApplicationCb(sptr<IQueryAppInfoCallback> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (callback == nullptr) {
        ErrorLog("NfcControllerProxy::RegQueryApplicationCb failed, callback is null.");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("NfcControllerProxy::RegQueryApplicationCb failed, write interface token error.");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ErrorLog("NfcControllerProxy::RegQueryApplicationCb WriteRemoteObject failed!");
        return KITS::ERR_NFC_PARAMETERS;
    }
    int error = SendRequestExpectReplyNone(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_QUERY_APP_INFO_MSG_CALLBACK),
        data, option);
    if (error != ERR_NONE) {
        ErrorLog("NfcControllerProxy::RegQueryApplicationCb failed, error code: %{public}d.", error);
        return KITS::ERR_NFC_PARAMETERS;
    }
    return KITS::ERR_NONE;
}

KITS::ErrorCode NfcControllerProxy::RegCardEmulationNotifyCb(sptr<IOnCardEmulationNotifyCb> callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (callback == nullptr) {
        ErrorLog("NfcControllerProxy::RegCardEmulationNotifyCb failed, callback is null.");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("NfcControllerProxy::RegCardEmulationNotifyCb failed, write interface token error.");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        ErrorLog("NfcControllerProxy::RegCardEmulationNotifyCb WriteRemoteObject failed!");
        return KITS::ERR_NFC_PARAMETERS;
    }
    int error = SendRequestExpectReplyNone(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_ON_CARD_EMULATION_NOTIFY),
        data, option);
    if (error != ERR_NONE) {
        ErrorLog("NfcControllerProxy::RegCardEmulationNotifyCb failed, error code: %{public}d.", error);
        return KITS::ERR_NFC_PARAMETERS;
    }
    return KITS::ERR_NONE;
}
KITS::ErrorCode NfcControllerProxy::NotifyEventStatus(int eventType, int arg1, std::string arg2)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("NfcControllerProxy::NotifyEventStatus failed, write interface token error.");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!data.WriteInt32(eventType)) {
        ErrorLog("NfcControllerProxy::NotifyEventStatus Write event type failed!");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!data.WriteInt32(arg1)) {
        ErrorLog("NfcControllerProxy::NotifyEventStatus Write arg1 failed!");
        return KITS::ERR_NFC_PARAMETERS;
    }

    if (!data.WriteString(arg2)) {
        ErrorLog("NfcControllerProxy::NotifyEventStatus Write arg2 failed!");
        return KITS::ERR_NFC_PARAMETERS;
    }
    data.WriteInt32(0);
    int error = SendRequestExpectReplyNone(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_VENDOR_NOTIFY),
        data, option);
    if (error != ERR_NONE) {
        ErrorLog("NfcControllerProxy::NotifyEventStatus failed, error code: %{public}d.", error);
        return KITS::ERR_NFC_PARAMETERS;
    }
    return KITS::ERR_NONE;
}
#endif

OHOS::sptr<IRemoteObject> NfcControllerProxy::GetHceServiceIface()
{
    DebugLog("GetHceServiceIface start!");
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("GetHceServiceIface, Write interface token error");
        return nullptr;
    }
    int32_t res = Remote()->SendRequest(static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_GET_HCE_INTERFACE),
        data, reply, option);
    if (res != ERR_NONE) {
        ErrorLog("GetHceServiceIface SendRequest err %{public}d", res);
        return nullptr;
    }
    sptr<OHOS::IRemoteObject> remoteObject = reply.ReadRemoteObject();
    return remoteObject;
}
}  // namespace NFC
}  // namespace OHOS
