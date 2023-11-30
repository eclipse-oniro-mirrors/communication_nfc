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
#include "hce_session_proxy.h"

#include "element_name.h"
#include "hce_cmd_callback_stub.h"
#include "loghelper.h"
#include "message_option.h"
#include "message_parcel.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
namespace NFC {
namespace HCE {
using OHOS::AppExecFwk::ElementName;
static HceCmdCallbackStub *g_hceCmdCallbackStub = new HceCmdCallbackStub;

KITS::ErrorCode HceSessionProxy::RegHceCmdCallback(
    const sptr<KITS::IHceCmdCallback> &callback, const std::string &type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (g_hceCmdCallbackStub == nullptr) {
        ErrorLog("%{public}s:g_hceCmdCallbackStub is nullptr", __func__);
        return KITS::ERR_NFC_PARAMETERS;
    }
    g_hceCmdCallbackStub->RegHceCmdCallback(callback, type);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("Write interface token error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!data.WriteString(type)) {
        ErrorLog("Write type error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    data.WriteInt32(0);
    if (!data.WriteRemoteObject(g_hceCmdCallbackStub->AsObject())) {
        ErrorLog("RegHceCmdCallback WriteRemoteObject failed!");
        return KITS::ERR_NFC_PARAMETERS;
    }

    int error = SendRequestExpectReplyNone(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_REG_HCE_CMD),
        data, option);
    if (error != ERR_NONE) {
        ErrorLog("RegHceCmdCallback failed, error code is %{public}d", error);
        return KITS::ERR_NFC_PARAMETERS;
    }
    return KITS::ERR_NONE;
}

int HceSessionProxy::SendRawFrame(std::string hexCmdData, bool raw,
                                  std::string &hexRespData)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteString(hexCmdData);
    data.WriteBool(raw);
    int statusCode = Remote()->SendRequest(
        static_cast<uint32_t>(
            NfcServiceIpcInterfaceCode::COMMAND_HCE_SEND_RAW_FRAME),
        data, reply, option);
    if (statusCode == ERR_NONE) {
        hexRespData = reply.ReadString();
    }
    return statusCode;
}
} // namespace HCE
} // namespace NFC
} // namespace OHOS
