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
#include "connected_tag_callback_stub.h"
#include "ipc_cmd.h"
#include "error_code.h"
#include "log.h"

namespace OHOS {
namespace ConnectedTag {
ConnectedTagCallBackStub::ConnectedTagCallBackStub() : callback_(nullptr), mRemoteDied(false)
{}

ConnectedTagCallBackStub::~ConnectedTagCallBackStub()
{}

ConnectedTagCallBackStub& ConnectedTagCallBackStub::GetInstance()
{
    static ConnectedTagCallBackStub sConnectedTagCallBackStub;
    return sConnectedTagCallBackStub;
}

void ConnectedTagCallBackStub::OnNotify(int nfcRfState)
{
    HILOGD("ConnectedTagCallBackStub::OnNotify");
    if (callback_) {
        callback_->OnNotify(nfcRfState);
    }
}

int ConnectedTagCallBackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    HILOGD("ConnectedTagCallBackStub::OnRemoteRequest!");
    if (mRemoteDied) {
        HILOGE("Failed to `%{public}s`,Remote service is died!", __func__);
        return NFC_OPT_FAILED;
    }
    int exception = data.ReadInt32();
    if (exception) {
        HILOGE("ConnectedTagCallBackStub::OnRemoteRequest, got exception: %{public}d!", exception);
        return NFC_OPT_FAILED;
    }
    int ret = NFC_OPT_FAILED;
    switch (code) {
        case CMD_ON_NOTIFY: {
            ret = RemoteOnNotify(data, reply);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }
    return ret;
}

ErrCode ConnectedTagCallBackStub::RegisterUserCallBack(const sptr<IConnectedTagCallBack> &callBack)
{
    std::shared_lock<std::shared_mutex> guard(callbackMutex);
    if (callBack == nullptr) {
        HILOGW("RegisterUserCallBack:callBack is nullptr!");
    }
    callback_ = callBack;
    return NFC_OPT_SUCCESS;
}

int ConnectedTagCallBackStub::RemoteOnNotify(MessageParcel &data, MessageParcel &reply)
{
    HILOGD("run %{public}s datasize %{public}zu", __func__, data.GetRawDataSize());
    int state = data.ReadInt32();
    std::shared_lock<std::shared_mutex> guard(callbackMutex);
    if (callback_) {
        callback_->OnNotify(state);
    }
    reply.WriteInt32(NFC_OPT_SUCCESS); /* Reply 0 to indicate that no exception occurs. */
    return NFC_OPT_SUCCESS;
}
}  // namespace ConnectedTag
}  // namespace OHOS