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
#include "nfc_sdk_common.h"
#include "nfc_controller_death_recipient.h"
#include "permission_tools.h"

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
        case KITS::COMMAND_GET_STATE:
            return HandleGetState(data, reply);
        case KITS::COMMAND_TURN_ON:
            return HandleTurnOn(data, reply);
        case KITS::COMMAND_TURN_OFF:
            return HandleTurnOff(data, reply);
        case KITS::COMMAND_REGISTER_CALLBACK:
            return HandleRegisterCallBack(data, reply);
        case KITS::COMMAND_UNREGISTER_CALLBACK:
            return HandleUnRegisterCallBack(data, reply);
        case KITS::COMMAND_IS_NFC_OPEN:
            return HandleIsNfcOpen(data, reply);
        case KITS::COMMAND_GET_TAG_INTERFACE:
            return HandleGetNfcTagInterface(data, reply);
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
    if (!PermissionTools::IsGranted(OHOS::NFC::SYS_PERM)) {
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    return TurnOn();
}

int NfcControllerStub::HandleTurnOff(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::SYS_PERM)) {
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
        deathRecipient_ = dr;

        callback_ = iface_cast<INfcControllerCallback>(remote);
        if (callback_ == nullptr) {
            callback_ = new (std::nothrow) NfcControllerCallBackProxy(remote);
            DebugLog("create new `NfcControllerCallBackProxy`!");
        }
        ret = RegisterCallBack(callback_, type);
    } while (0);
    
    reply.WriteInt32(ret);
    return ERR_NONE;
}

void NfcControllerStub::RemoveNfcDeathRecipient(const wptr<IRemoteObject> &remote)
{
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
    return RegisterCallBack(callback_, type, IPCSkeleton::GetCallingTokenID());
}

KITS::ErrorCode NfcControllerStub::UnRegisterCallBack(const std::string& type)
{
    return UnRegisterCallBack(type, IPCSkeleton::GetCallingTokenID());
}

int NfcControllerStub::HandleGetNfcTagInterface(MessageParcel& data, MessageParcel& reply)
{
    OHOS::sptr<IRemoteObject> remoteOjbect = GetTagServiceIface();
    if (remoteOjbect == nullptr) {
        ErrorLog("HandleGetNfcTagInterface remoteOjbect null!");
        return KITS::ERR_NFC_PARAMETERS;
    }

    reply.WriteRemoteObject(remoteOjbect);
    return ERR_NONE;
}
}  // namespace NFC
}  // namespace OHOS
