/*
* Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "nfc_controller_callback_proxy.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
NfcControllerCallBackProxy::NfcControllerCallBackProxy(const sptr<IRemoteObject> &remote)
    : IRemoteProxy<INfcControllerCallback>(remote)
{}

void NfcControllerCallBackProxy::OnNfcStateChanged(int nfcRfState)
{
    DebugLog("NfcControllerCallBackProxy::OnNotify");
    MessageOption option = {MessageOption::TF_ASYNC};
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        DebugLog("Write interface token error: %{public}s", __func__);
        return;
    }
    data.WriteInt32(0);
    data.WriteInt32(nfcRfState);

    int error = Remote()->SendRequest(KITS::COMMAND_ON_NOTIFY, data, reply, option);
    if (error != ERR_NONE) {
        InfoLog("Set Attr %{public}d failed,error code is %{public}d", KITS::COMMAND_ON_NOTIFY, error);
        return;
    }
    int exception = reply.ReadInt32();
    if (exception) {
        DebugLog("notify COMMAND_ON_NOTIFY state change failed!");
    }
    return;
}
}  // namespace NFC
}  // namespace OHOS