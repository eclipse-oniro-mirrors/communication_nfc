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
#include "nfc_control_death_recipient.h"
#include "permission_tools.h"

namespace OHOS {
namespace NFC {
int NfcControllerStub::OnRemoteRequest(uint32_t code,         /* [in] */
                                       MessageParcel& data,   /* [in] */
                                       MessageParcel& reply,  /* [out] */
                                       MessageOption& option) /* [in] */
{
    InfoLog("OnRemoteRequest occur, code is %{public}d", code);
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
    DebugLog("NfcControllerStub::HandleTurnOn");
    bool result = TurnOn();
    reply.WriteInt32(result);
    return ERR_NONE;
}

int NfcControllerStub::HandleTurnOff(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::SYS_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    bool saveState = data.ReadBool();

    saveState = TurnOff(saveState);
    reply.WriteInt32(saveState);
    return ERR_NONE;
}

int NfcControllerStub::HandleRegisterCallBack(MessageParcel &data, MessageParcel &reply)
{
    InfoLog("datasize %{public}zu", data.GetRawDataSize());
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        return KITS::NFC_FAILED;
    }
    std::string type = data.ReadString();
    int exception = data.ReadInt32();
    if (exception) {
        return KITS::NFC_FAILED;
    }
    KITS::NfcErrorCode ret = KITS::NFC_FAILED;
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
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        return KITS::NFC_FAILED;
    }
    std::string type = data.ReadString();
    int exception = data.ReadInt32();
    if (exception) {
        return KITS::NFC_FAILED;
    }
    KITS::NfcErrorCode ret = KITS::NFC_FAILED;
    ret = UnRegisterCallBack(type);
    DebugLog("OnUnRegisterCallBack::OnUnRegisterCallBack end##ret=%{public}d\n", ret);
    reply.WriteInt32(ret);
    return ERR_NONE;
}

KITS::NfcErrorCode NfcControllerStub::RegisterCallBack(const sptr<INfcControllerCallback> &callback,
    const std::string& type)
{
    return RegisterCallBack(callback_, type, IPCSkeleton::GetCallingTokenID());
}

KITS::NfcErrorCode NfcControllerStub::UnRegisterCallBack(const std::string& type)
{
    return UnRegisterCallBack(type, IPCSkeleton::GetCallingTokenID());
}
}  // namespace NFC
}  // namespace OHOS
