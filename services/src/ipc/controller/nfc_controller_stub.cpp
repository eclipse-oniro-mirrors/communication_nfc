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
#include "nfc_controller_stub.h"

#include "ipc_skeleton.h"
#include "loghelper.h"
#include "ndef_msg_callback_proxy.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include "nfc_controller_death_recipient.h"
#include "nfc_permission_checker.h"
#ifdef VENDOR_APPLICATIONS_ENABLED
#include "on_card_emulation_notify_cb_proxy.h"
#include "query_app_info_callback_proxy.h"
#endif
#include "external_deps_proxy.h"

namespace OHOS {
namespace NFC {
int NfcControllerStub::OnRemoteRequest(uint32_t code,         /* [in] */
                                       MessageParcel& data,   /* [in] */
                                       MessageParcel& reply,  /* [out] */
                                       MessageOption& option) /* [in] */
{
    InfoLog("NfcControllerStub OnRemoteRequest occur, code is %{public}d", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        ErrorLog("NfcControllerStub OnRemoteRequest GetDescriptor failed");
        return KITS::ERR_NFC_PARAMETERS;
    }
    switch (code) {
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_GET_STATE):
            return HandleGetState(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_TURN_ON):
            return HandleTurnOn(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_TURN_OFF):
            return HandleTurnOff(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_REGISTER_CALLBACK):
            return HandleRegisterCallBack(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_UNREGISTER_CALLBACK):
            return HandleUnRegisterCallBack(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_IS_NFC_OPEN):
            return HandleIsNfcOpen(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_GET_TAG_INTERFACE):
            return HandleGetNfcTagInterface(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_REG_NDEF_MSG_CALLBACK):
            return HandleRegNdefMsgCb(data, reply);
#ifdef VENDOR_APPLICATIONS_ENABLED
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_QUERY_APP_INFO_MSG_CALLBACK):
            return HandleRegQueryApplicationCb(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_ON_CARD_EMULATION_NOTIFY):
            return HandleRegCardEmulationNotifyCb(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_VENDOR_NOTIFY):
            return HandleNotifyEventStatus(data, reply);
#endif
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_GET_HCE_INTERFACE):
            return HandleGetNfcHceInterface(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int NfcControllerStub::HandleGetState(MessageParcel& data, MessageParcel& reply)
{
    int state = GetState();
    reply.WriteInt32(state);
    return ERR_NONE;
}

int NfcControllerStub::HandleTurnOn(MessageParcel& data, MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::SYS_PERM)) {
        ErrorLog("HandleTurnOn no permission");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    return TurnOn();
}

int NfcControllerStub::HandleTurnOff(MessageParcel& data, MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::SYS_PERM)) {
        ErrorLog("HandleTurnOff no permission");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    return TurnOff();
}

int NfcControllerStub::HandleIsNfcOpen(MessageParcel& data, MessageParcel& reply)
{
    bool isOpen = false;
    int statusCode = IsNfcOpen(isOpen);
    reply.WriteBool(isOpen);
    return statusCode;
}

int NfcControllerStub::HandleRegisterCallBack(MessageParcel &data, MessageParcel &reply)
{
    InfoLog("datasize %{public}zu", data.GetRawDataSize());
    std::string type = data.ReadString();
    int exception = data.ReadInt32();
    if (exception) {
        return KITS::ERR_NFC_PARAMETERS;
    }
    KITS::ErrorCode ret = KITS::ERR_NFC_PARAMETERS;
    do {
        sptr<IRemoteObject> remote = data.ReadRemoteObject();
        if (remote == nullptr) {
            DebugLog("Failed to readRemoteObject!");
            break;
        }

        std::unique_ptr<NfcControllerDeathRecipient> recipient
            = std::make_unique<NfcControllerDeathRecipient>(this, IPCSkeleton::GetCallingTokenID());
        if (recipient == nullptr) {
            ErrorLog("recipient is null");
            return ERR_NONE;
        }
        sptr<IRemoteObject::DeathRecipient> dr(recipient.release());
        if ((remote->IsProxyObject()) && (!remote->AddDeathRecipient(dr))) {
            ErrorLog("Failed to add death recipient");
            return ERR_NONE;
        }

        {
            std::lock_guard<std::mutex> guard(mutex_);
            deathRecipient_ = dr;
            callback_ = iface_cast<INfcControllerCallback>(remote);
            if (callback_ == nullptr) {
                callback_ = new (std::nothrow) NfcControllerCallBackProxy(remote);
                DebugLog("create new `NfcControllerCallBackProxy`!");
            }
            ret = RegisterCallBack(callback_, type);
        }
    } while (0);

    reply.WriteInt32(ret);
    return ERR_NONE;
}

void NfcControllerStub::RemoveNfcDeathRecipient(const wptr<IRemoteObject> &remote)
{
    std::lock_guard<std::mutex> guard(mutex_);
    if (callback_ == nullptr) {
        ErrorLog("OnRemoteDied callback_ is nullptr");
        return;
    }
    auto serviceRemote = callback_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
        callback_ = nullptr;
        ErrorLog("on remote died");
    }
}

int NfcControllerStub::HandleUnRegisterCallBack(MessageParcel &data, MessageParcel &reply)
{
    InfoLog("OnUnRegisterCallBack");
    std::string type = data.ReadString();
    int exception = data.ReadInt32();
    if (exception) {
        return KITS::ERR_NFC_PARAMETERS;
    }
    KITS::ErrorCode ret = UnRegisterCallBack(type);
    DebugLog("OnUnRegisterCallBack::OnUnRegisterCallBack end##ret=%{public}d\n", ret);
    reply.WriteInt32(ret);
    return ERR_NONE;
}

KITS::ErrorCode NfcControllerStub::RegisterCallBack(const sptr<INfcControllerCallback> &callback,
    const std::string& type)
{
    return RegisterCallBack(callback, type, IPCSkeleton::GetCallingTokenID());
}

KITS::ErrorCode NfcControllerStub::UnRegisterCallBack(const std::string& type)
{
    return UnRegisterCallBack(type, IPCSkeleton::GetCallingTokenID());
}

int NfcControllerStub::HandleGetNfcTagInterface(MessageParcel& data, MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleGetNfcTagInterface no permission");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    OHOS::sptr<IRemoteObject> remoteOjbect = GetTagServiceIface();
    if (remoteOjbect == nullptr) {
        ErrorLog("HandleGetNfcTagInterface remoteOjbect null!");
        return KITS::ERR_NFC_PARAMETERS;
    }

    reply.WriteRemoteObject(remoteOjbect);
    return ERR_NONE;
}

int NfcControllerStub::HandleGetNfcHceInterface(MessageParcel& data, MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("HandleGetNfcHceInterface no permission");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    OHOS::sptr<IRemoteObject> remoteOjbect = GetHceServiceIface();
    if (remoteOjbect == nullptr) {
        ErrorLog("HandleGetNfcHceInterface remoteOjbect null!");
        return KITS::ERR_NFC_PARAMETERS;
    }

    reply.WriteRemoteObject(remoteOjbect);
    return ERR_NONE;
}

int NfcControllerStub::HandleRegNdefMsgCb(MessageParcel& data, MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleRegNdefMsgCb no permission");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    InfoLog("NfcControllerStub::HandleRegNdefMsgCb");
    KITS::ErrorCode ret = KITS::ERR_NFC_PARAMETERS;
    do {
        sptr<IRemoteObject> remote = data.ReadRemoteObject();
        if (remote == nullptr) {
            DebugLog("Failed to readRemoteObject!");
            break;
        }
        {
            std::lock_guard<std::mutex> guard(mutex_);
            ndefCallback_ = iface_cast<INdefMsgCallback>(remote);
            if (ndefCallback_ == nullptr) {
                ndefCallback_ = new (std::nothrow) NdefMsgCallbackProxy(remote);
                DebugLog("NfcControllerStub::HandleRegNdefMsgCb, create new `NdefMsgCallbackProxy`!");
            }
            ret = RegNdefMsgCallback(ndefCallback_);
        }
    } while (0);
    reply.WriteInt32(ret);
    return ERR_NONE;
}

#ifdef VENDOR_APPLICATIONS_ENABLED
int NfcControllerStub::HandleRegQueryApplicationCb(MessageParcel& data, MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("HandleRegQueryApplicationCb no permission");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    InfoLog("NfcControllerStub::HandleRegQueryApplicationCb");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        ErrorLog("Failed to readRemoteObject!");
        return KITS::ERR_NFC_PARAMETERS;
    }
    {
        std::lock_guard<std::mutex> guard(mutex_);
        queryAppInfoCallback_ = iface_cast<IQueryAppInfoCallback>(remote);
        if (queryAppInfoCallback_ == nullptr) {
            queryAppInfoCallback_ = new (std::nothrow) QueryAppInfoCallbackProxy(remote);
            DebugLog("NfcControllerStub::HandleRegQueryApplicationCb, create new `QueryAppInfoCallbackProxy`!");
        }
        int ret = RegQueryApplicationCb(queryAppInfoCallback_);
        reply.WriteInt32(ret);
    }
    return ERR_NONE;
}

int NfcControllerStub::HandleRegCardEmulationNotifyCb(MessageParcel& data, MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("HandleRegCardEmulationNotifyCb no permission");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    InfoLog("NfcControllerStub::HandleRegCardEmulationNotifyCb");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        ErrorLog("Failed to readRemoteObject!");
        return KITS::ERR_NFC_PARAMETERS;
    }
    {
        std::lock_guard<std::mutex> guard(mutex_);
        onCardEmulationNotifyCb_ = iface_cast<IOnCardEmulationNotifyCb>(remote);
        if (onCardEmulationNotifyCb_ == nullptr) {
            onCardEmulationNotifyCb_ = new (std::nothrow) OnCardEmulationNotifyCbProxy(remote);
            DebugLog("NfcControllerStub::HandleRegCardEmulationNotifyCb, create new `OnCardEmulationNotifyCbProxy`!");
        }
        int ret = RegCardEmulationNotifyCb(onCardEmulationNotifyCb_);
        reply.WriteInt32(ret);
    }
    return ERR_NONE;
}
int NfcControllerStub::HandleNotifyEventStatus(MessageParcel& data, MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("HandleNotifyEventStatus no permission");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    int eventType = data.ReadInt32();
    int arg1 = data.ReadInt32();
    std::string arg2 = data.ReadString();
    int exception = data.ReadInt32();
    if (exception) {
        ErrorLog("HandleNotifyEventStatus::read param failed.");
        return KITS::ERR_NFC_PARAMETERS;
    }
    KITS::ErrorCode ret = NotifyEventStatus(eventType, arg1, arg2);
    reply.WriteInt32(ret);
    return ERR_NONE;
}
#endif

KITS::ErrorCode NfcControllerStub::RegNdefMsgCb(const sptr<INdefMsgCallback> &callback)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("RegNdefMsgCb no permission");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    InfoLog("NfcControllerStub::RegNdefMsgCb");
    return RegNdefMsgCallback(callback);
}
}  // namespace NFC
}  // namespace OHOS
