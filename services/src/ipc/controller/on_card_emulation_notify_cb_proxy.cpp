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

#include "on_card_emulation_notify_cb_proxy.h"

#include "nfc_service_ipc_interface_code.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
OnCardEmulationNotifyCbProxy::OnCardEmulationNotifyCbProxy(const sptr<IRemoteObject> &remote)
    : IRemoteProxy<IOnCardEmulationNotifyCb>(remote)
{}

bool OnCardEmulationNotifyCbProxy::OnCardEmulationNotify(uint32_t eventType, std::string apduData)
{
    MessageOption option = {MessageOption::TF_SYNC};
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("OnCardEmulationNotifyCbProxy:WriteInterfaceToken token error");
        return false;
    }
    data.WriteInt32(0);
    data.WriteUint32(eventType);
    data.WriteString(apduData);
    int error = Remote()->SendRequest(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_ON_CARD_EMULATION_NOTIFY), data, reply, option);
    if (error != ERR_NONE) {
        ErrorLog("OnCardEmulationNotifyCbProxy::OnCardEmulationNotify, Set Attr %{public}d error: %{public}d",
            NfcServiceIpcInterfaceCode::COMMAND_ON_CARD_EMULATION_NOTIFY, error);
        return false;
    }
    return true;
}
}  // namespace NFC
}  // namespace OHOS
