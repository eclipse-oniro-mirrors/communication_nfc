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

#include "hce_cmd_callback_proxy.h"

#include "nfc_service_ipc_interface_code.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace HCE {
HceCmdCallbackProxy::HceCmdCallbackProxy(const sptr<IRemoteObject> &remote)
    : IRemoteProxy<KITS::IHceCmdCallback>(remote)
{
}

void HceCmdCallbackProxy::OnCeApduData(const std::vector<uint8_t> &apduData)
{
    DebugLog("HceCmdCallbackProxy::OnNotify");
    MessageOption option = {MessageOption::TF_ASYNC};
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DebugLog("Write interface token error: %{public}s", __func__);
        return;
    }
    data.WriteInt32(0);
    data.WriteUInt8Vector(apduData);

    int error = Remote()->SendRequest(
        static_cast<uint32_t>(
            NfcServiceIpcInterfaceCode::COMMAND_ON_CE_APDU_DATA),
        data, reply, option);
    if (error != ERR_NONE) {
        InfoLog("Set Attr %{public}d failed,error code is %{public}d",
                NfcServiceIpcInterfaceCode::COMMAND_ON_CE_APDU_DATA, error);
        return;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        DebugLog("notify COMMAND_ON_CE_APDU_DATA failed!");
    }
    return;
}

} // namespace HCE
} // namespace NFC
} // namespace OHOS
