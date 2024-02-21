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
#include "hce_session_proxy.h"

#include "element_name.h"
#include "hce_cmd_callback_stub.h"
#include "loghelper.h"
#include "message_option.h"
#include "message_parcel.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include "ce_payment_services_parcelable.h"
#include "start_hce_info_parcelable.h"

namespace OHOS {
namespace NFC {
namespace HCE {
using OHOS::AppExecFwk::ElementName;
static sptr<HceCmdCallbackStub> g_hceCmdCallbackStub =
    sptr<HceCmdCallbackStub>(new (std::nothrow) HceCmdCallbackStub);

KITS::ErrorCode HceSessionProxy::RegHceCmdCallback(const sptr<KITS::IHceCmdCallback> &callback,
                                                   const std::string &type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (g_hceCmdCallbackStub == nullptr) {
        ErrorLog("%{public}s:g_hceCmdCallbackStub is nullptr", __func__);
        return KITS::ERR_HCE_PARAMETERS;
    }
    g_hceCmdCallbackStub->RegHceCmdCallback(callback, type);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("Write interface token error");
        return KITS::ERR_HCE_PARAMETERS;
    }
    if (!data.WriteString(type)) {
        ErrorLog("Write type error");
        return KITS::ERR_HCE_PARAMETERS;
    }
    data.WriteInt32(0);
    if (!data.WriteRemoteObject(g_hceCmdCallbackStub->AsObject())) {
        ErrorLog("RegHceCmdCallback WriteRemoteObject failed!");
        return KITS::ERR_HCE_PARAMETERS;
    }

    int error = SendRequestExpectReplyNone(static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_ON),
                                           data, option);
    if (error != ERR_NONE) {
        ErrorLog("RegHceCmdCallback failed, error code is %{public}d", error);
        return KITS::ERR_HCE_PARAMETERS;
    }
    return KITS::ERR_NONE;
}

int HceSessionProxy::SendRawFrame(std::string hexCmdData, bool raw, std::string &hexRespData)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }

    if (hexCmdData.size() > KITS::MAX_APDU_DATA_HEX_STR) {
        ErrorLog("raw frame too long");
        return KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }
    data.WriteString(hexCmdData);
    data.WriteBool(raw);
    int statusCode = Remote()->SendRequest(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_TRANSMIT), data, reply, option);
    if (statusCode == ERR_NONE) {
        hexRespData = reply.ReadString();
    }
    return statusCode;
}
int HceSessionProxy::GetPaymentServices(std::vector<AbilityInfo> &abilityInfos)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }
    data.WriteInt32(0);

    int statusCode = Remote()->SendRequest(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_GET_PAYMENT_SERVICES), data, reply,
        option);
    if (statusCode != ERR_NONE) {
        ErrorLog("GetPaymentServices failed, error code is %{public}d", statusCode);
        return statusCode;
    }

    std::shared_ptr<KITS::CePaymentServicesParcelable> paymentServices(
        reply.ReadParcelable<KITS::CePaymentServicesParcelable>());
    if (paymentServices == nullptr) {
        ErrorLog("paymentServices read failed.");
        return KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }
    std::vector<AbilityInfo> paymentAbilityInfos = paymentServices->paymentAbilityInfos;
    DebugLog("GetPaymentServices size %{public}zu", paymentAbilityInfos.size());
    abilityInfos = std::move(paymentAbilityInfos);
    return statusCode;
}
KITS::ErrorCode HceSessionProxy::StopHce(ElementName &element)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("Write interface token error");
        return KITS::ERR_HCE_PARAMETERS;
    }
    if (!element.Marshalling(data)) {
        ErrorLog("Write element error");
        return KITS::ERR_HCE_PARAMETERS;
    }

    data.WriteInt32(0);
    int error = SendRequestExpectReplyNone(static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_STOP),
                                           data, option);
    if (error != ERR_NONE) {
        ErrorLog("StopHce failed, error code is %{public}d", error);
        return KITS::ERR_HCE_PARAMETERS;
    }
    return KITS::ERR_NONE;
}
KITS::ErrorCode HceSessionProxy::IsDefaultService(ElementName &element, const std::string &type,
                                                  bool &isDefaultService)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("Write interface token error");
        return KITS::ERR_HCE_PARAMETERS;
    }
    if (!element.Marshalling(data)) {
        ErrorLog("Write element error");
        return KITS::ERR_HCE_PARAMETERS;
    }

    if (!data.WriteString(type)) {
        ErrorLog("Write type error");
        return KITS::ERR_HCE_PARAMETERS;
    }
    data.WriteInt32(0);
    int error = SendRequestExpectReplyBool(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_IS_DEFAULT_SERVICE), data, option,
        isDefaultService);
    if (error != ERR_NONE) {
        ErrorLog("IsDefaultService failed, error code is %{public}d", error);
        return KITS::ERR_HCE_PARAMETERS;
    }
    return KITS::ERR_NONE;
}

KITS::ErrorCode HceSessionProxy::StartHce(const ElementName &element, const std::vector<std::string> &aids)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("Write interface token error");
        return KITS::ERR_HCE_PARAMETERS;
    }

    KITS::StartHceInfoParcelable startHceInfo;
    startHceInfo.SetAids(aids);
    startHceInfo.SetElement(element);
    if (!startHceInfo.Marshalling(data)) {
        ErrorLog("Write start info error");
        return KITS::ERR_HCE_PARAMETERS;
    }

    data.WriteInt32(0);
    int error = SendRequestExpectReplyNone(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_CE_HCE_START), data, option);
    if (error != ERR_NONE) {
        ErrorLog("StartHce failed, error code is %{public}d", error);
        return KITS::ERR_HCE_PARAMETERS;
    }
    return KITS::ERR_NONE;
}
} // namespace HCE
} // namespace NFC
} // namespace OHOS
