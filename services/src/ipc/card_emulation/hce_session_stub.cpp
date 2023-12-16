/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "hce_session_stub.h"

#include "hce_cmd_death_recipient.h"
#include "ipc_skeleton.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include "hce_cmd_callback_proxy.h"
#include "nfc_permission_checker.h"
#include "external_deps_proxy.h"
#include "ce_payment_services_parcelable.h"
#include "ability_info.h"

namespace OHOS {
namespace NFC {
namespace HCE {
using AppExecFwk::AbilityInfo;
int HceSessionStub::OnRemoteRequest(uint32_t code, OHOS::MessageParcel &data, OHOS::MessageParcel &reply,
                                    OHOS::MessageOption &option)
{
    DebugLog("hceSessionStub OnRemoteRequest occur, code is %d", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        ErrorLog("hceSessionStub OnRemoteRequest GetDescriptor failed");
        return KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }

    switch (code) {
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_ON):
            return HandleRegHceCmdCallback(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_TRANSMIT):
            return HandleSendRawFrame(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_GET_PAYMENT_SERVICES):
            return HandleGetPaymentServices(data, reply);
        default: return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int HceSessionStub::HandleRegHceCmdCallback(MessageParcel &data, MessageParcel &reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("HandleRegHceCmdCallback, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
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
        std::unique_ptr<HceCmdDeathRecipient> recipient =
            std::make_unique<HceCmdDeathRecipient>(this, IPCSkeleton::GetCallingTokenID());
        sptr<IRemoteObject::DeathRecipient> dr(recipient.release());
        if ((remote->IsProxyObject()) && (!remote->AddDeathRecipient(dr))) {
            ErrorLog("Failed to add death recipient");
            return ERR_NONE;
        }
        {
            std::lock_guard<std::mutex> guard(mutex_);
            deathRecipient_ = dr;
            hceCmdCallback_ = iface_cast<KITS::IHceCmdCallback>(remote);
            if (hceCmdCallback_ == nullptr) {
                hceCmdCallback_ = new (std::nothrow) HceCmdCallbackProxy(remote);
                DebugLog("create new `HceCmdCallbackProxy`!");
            }
            ret = RegHceCmdCallback(hceCmdCallback_, type);
        }
    } while (0);
    reply.WriteInt32(ret);
    return ERR_NONE;
}
int HceSessionStub::HandleSendRawFrame(OHOS::MessageParcel &data, OHOS::MessageParcel &reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("HandleRegHceCmdCallback, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }

    std::string hexCmdData = data.ReadString();
    bool raw = data.ReadBool();
    std::string hexRespData;
    int statusCode = SendRawFrame(hexCmdData, raw, hexRespData);
    reply.WriteString(hexRespData);
    return statusCode;
}
int HceSessionStub::HandleGetPaymentServices(MessageParcel &data, MessageParcel &reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::SYS_PERM)) {
        ErrorLog("HandleGetPaymentServices, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    int exception = data.ReadInt32();
    if (exception) {
        ErrorLog("HandleGetPaymentServices, exception");
        return KITS::ERR_NFC_PARAMETERS;
    }
    std::vector<AbilityInfo> abilityInfos;
    int result = GetPaymentServices(abilityInfos);
    if (result != NFC::KITS::ErrorCode::ERR_NONE) {
        ErrorLog("HandleGetPaymentServices, get payment service failed");
        return KITS::ErrorCode::ERR_HCE_NOT_GET_PAYMENT_SERVICES;
    }
    KITS::CePaymentServicesParcelable paymentServiceMsg;
    paymentServiceMsg.paymentAbilityInfos = abilityInfos;
    if (!reply.WriteParcelable(&paymentServiceMsg)) {
        ErrorLog("HandleGetPaymentServices, write payment service failed");
        return KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }
    return ERR_NONE;
}
} // namespace HCE
} // namespace NFC
} // namespace OHOS
