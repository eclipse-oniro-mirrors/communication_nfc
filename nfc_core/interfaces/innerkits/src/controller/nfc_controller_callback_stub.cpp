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

#include "nfc_controller_callback_stub.h"

#include "nfc_sdk_common.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
NfcControllerCallBackStub::NfcControllerCallBackStub() : callback_(nullptr), mRemoteDied(false)
{}

NfcControllerCallBackStub::~NfcControllerCallBackStub()
{}

NfcControllerCallBackStub& NfcControllerCallBackStub::GetInstance()
{
    static NfcControllerCallBackStub NfcControllerCallBackStub;
    return NfcControllerCallBackStub;
}

void NfcControllerCallBackStub::OnNfcStateChanged(int nfcRfState)
{
    if (callback_) {
        DebugLog("NfcControllerCallBackStub callback_");
        callback_->OnNfcStateChanged(nfcRfState);
    }
}

int NfcControllerCallBackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    DebugLog("NfcControllerCallBackStub::OnRemoteRequest,code = %{public}d", code);
    if (mRemoteDied) {
        return KITS::NFC_FAILED;
    }
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        InfoLog("nfc callback stub token verification error");
        return KITS::NFC_FAILED;
    }
    int exception = data.ReadInt32();
    if (exception) {
        InfoLog("ConnectedTagCallBackStub::OnRemoteRequest, got exception: (%{public}d))", exception);
        return KITS::NFC_FAILED;
    }
    int ret = KITS::NFC_FAILED;
    switch (code) {
        case KITS::COMMAND_ON_NOTIFY: {
            ret = RemoteNfcStateChanged(data, reply);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }
    return ret;
}

KITS::NfcErrorCode NfcControllerCallBackStub::RegisterCallBack(const sptr<INfcControllerCallback> &callBack)
{
    DebugLog("NfcControllerCallBackStub RegisterCallBack");
    std::shared_lock<std::shared_mutex> guard(callbackMutex);
    if (callBack == nullptr) {
        DebugLog("RegisterUserCallBack:callBack is nullptr!");
        callback_ = callBack;
        return KITS::NFC_FAILED;
    }
    callback_ = callBack;
    return KITS::NFC_SUCCESS;
}

int NfcControllerCallBackStub::RemoteNfcStateChanged(MessageParcel &data, MessageParcel &reply)
{
    InfoLog("run %{public}zu datasize ", data.GetRawDataSize());
    int state = data.ReadInt32();
    std::shared_lock<std::shared_mutex> guard(callbackMutex);
    OnNfcStateChanged(state);
    reply.WriteInt32(KITS::NFC_SUCCESS); /* Reply 0 to indicate that no exception occurs. */
    return KITS::NFC_SUCCESS;
}
}  // namespace NFC
}  // namespace OHOS