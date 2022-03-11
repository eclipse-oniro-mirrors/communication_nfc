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
 #include <stdint.h>
 #include <string>
 #include "tag_session_proxy.h"
 #include "log.h"
 #include "ipc_cmd.h"
 #include "connected_tag_callback_stub.h"

namespace OHOS {
namespace ConnectedTag {
TagSessionProxy::TagSessionProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<ITagSession>(impl), mRemoteDied(false)
{
    HILOGI("TagSessionProxy() in!");
    if (impl) {
        if ((impl->IsProxyObject()) && (!impl->AddDeathRecipient(this))) {
            HILOGD("AddDeathRecipient!");
        } else {
            HILOGW("no recipient!");
        }
    }
}
TagSessionProxy::~TagSessionProxy()
{
}

ErrCode TagSessionProxy::Init()
{
    if (mRemoteDied) {
        HILOGD("failed to `%{public}s`,remote service is died!", __func__);
        return NFC_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    data.WriteInt32(NFC_OPT_SUCCESS);

    int error = Remote()->SendRequest(NFC_SVR_CMD_INIT, data, reply, option);
    if (error != ERR_NONE) {
        HILOGE("Init failed, error code is %{public}d", error);
        return NFC_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return NFC_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}
ErrCode TagSessionProxy::Uninit()
{
    if (mRemoteDied) {
        HILOGD("failed to `%{public}s`,remote service is died!", __func__);
        return NFC_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    data.WriteInt32(NFC_OPT_SUCCESS);

    int error = Remote()->SendRequest(NFC_SVR_CMD_UNINIT, data, reply, option);
    if (error != ERR_NONE) {
        HILOGE("Uninit failed, error code is %{public}d", error);
        return NFC_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return NFC_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}
ErrCode TagSessionProxy::ReadNdefTag(std::string &response)
{
    if (mRemoteDied) {
        HILOGD("failed to `%{public}s`,remote service is died!", __func__);
        return NFC_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    data.WriteInt32(NFC_OPT_SUCCESS);

    int error = Remote()->SendRequest(NFC_SVR_CMD_READ_NDEF_TAG, data, reply, option);
    if (error != ERR_NONE) {
        HILOGE("ReadNdefTag failed, error code is %{public}d", error);
        return NFC_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return NFC_OPT_FAILED;
    }
    response = reply.ReadString();
    return ErrCode(NFC_OPT_SUCCESS);
}
ErrCode TagSessionProxy::WriteNdefTag(std::string tagData)
{
    if (mRemoteDied) {
        HILOGD("failed to `%{public}s`,remote service is died!", __func__);
        return NFC_OPT_FAILED;
    }
    MessageOption option;
    MessageParcel data;
    MessageParcel reply;
    data.WriteInt32(NFC_OPT_SUCCESS);
    data.WriteString(tagData);
    HILOGE("TagSessionProxy WriteNdefTag tagData is %{public}s", tagData.c_str());

    int error = Remote()->SendRequest(NFC_SVR_CMD_WRITE_NDEF_TAG, data, reply, option);
    if (error != ERR_NONE) {
        HILOGE("WriteNdefTag failed,error code is %{public}d", error);
        return NFC_OPT_FAILED;
    }

    int exception = reply.ReadInt32();
    if (exception) {
        return NFC_OPT_FAILED;
    }
    return ErrCode(reply.ReadInt32());
}
ErrCode TagSessionProxy::RegListener(const sptr<IConnectedTagCallBack> &callback)
{
    return OHOS::ConnectedTag::ConnectedTagCallBackStub::GetInstance().RegisterUserCallBack(callback);
}
ErrCode TagSessionProxy::UnregListener(const sptr<IConnectedTagCallBack> &callback)
{
    return OHOS::ConnectedTag::ConnectedTagCallBackStub::GetInstance().RegisterUserCallBack(nullptr);
}

void TagSessionProxy::OnRemoteDied(const wptr<IRemoteObject> &remoteObject)
{
    HILOGD("Remote service is died!");
    mRemoteDied = true;
}
}  // namespace ConnectedTag
}  // namespace OHOS