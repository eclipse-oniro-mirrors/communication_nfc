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

#include "ndef_msg_callback_stub.h"

#include "nfc_service_ipc_interface_code.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
NdefMsgCallbackStub::NdefMsgCallbackStub() : callback_(nullptr), isRemoteDied_(false)
{}

NdefMsgCallbackStub::~NdefMsgCallbackStub()
{}

NdefMsgCallbackStub& NdefMsgCallbackStub::GetInstance()
{
    static NdefMsgCallbackStub instance;
    return instance;
}

bool NdefMsgCallbackStub::OnNdefMsgDiscovered(const std::string &tagUid, const std::string &ndef, int ndefMsgType)
{
    if (callback_) {
        DebugLog("NdefMsgCallbackStub callback_");
        return callback_->OnNdefMsgDiscovered(tagUid, ndef, ndefMsgType);
    }
    return false;
}

int NdefMsgCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    DebugLog("NdefMsgCallbackStub::OnRemoteRequest,code = %{public}d", code);
    if (isRemoteDied_) {
        return KITS::ERR_NFC_STATE_UNBIND;
    }
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        ErrorLog("NdefMsgCallbackStub: token verification error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    int exception = data.ReadInt32();
    if (exception) {
        ErrorLog("NdefMsgCallbackStub::OnRemoteRequest, got exception: (%{public}d))", exception);
        return exception;
    }
    int ret = KITS::ERR_NFC_STATE_UNBIND;
    switch (code) {
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_ON_NDEF_MSG_NOTIFY): {
            ret = RemoteNdefMsgDiscovered(data, reply);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }
    return ret;
}

KITS::ErrorCode NdefMsgCallbackStub::RegisterCallback(const sptr<INdefMsgCallback> &callback)
{
    DebugLog("NdefMsgCallbackStub RegisterCallBack");
    std::unique_lock<std::shared_mutex> guard(mutex_);
    if (callback == nullptr) {
        ErrorLog("RegisterUserCallBack:callBack is nullptr!");
        callback_ = callback;
        return KITS::ERR_NFC_PARAMETERS;
    }
    callback_ = callback;
    return KITS::ERR_NONE;
}

int NdefMsgCallbackStub::RemoteNdefMsgDiscovered(MessageParcel &data, MessageParcel &reply)
{
    InfoLog("NdefMsgCallbackStub::RemoteNdefMsgDiscovered");
    std::string tagUid = data.ReadString();
    std::string ndef = data.ReadString();
    int type = data.ReadInt32();
    std::unique_lock<std::shared_mutex> guard(mutex_);
    bool res = OnNdefMsgDiscovered(tagUid, ndef, type);
    reply.WriteBool(res); // Reply for ndef parse result
    return KITS::ERR_NONE;
}
}  // namespace NFC
}  // namespace OHOS