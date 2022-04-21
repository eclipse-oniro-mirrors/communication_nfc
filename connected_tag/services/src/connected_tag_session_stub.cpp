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
#include "i_tag_session.h"
#include "log.h"
#include "error_code.h"
#include "ipc_cmd.h"
#include "connected_tag_session_stub.h"

namespace OHOS {
namespace ConnectedTag {
ConnectedTagSessionStub::ConnectedTagSessionStub()
{
    HILOGI("ConnectedTagSessionStub: ConnectedTagSessionStub called.");
    InitHandleMap();
}
ConnectedTagSessionStub::~ConnectedTagSessionStub()
{}
void ConnectedTagSessionStub::InitHandleMap()
{
    handleFuncMap[NFC_SVR_CMD_INIT] = &ConnectedTagSessionStub::OnInit;
    handleFuncMap[NFC_SVR_CMD_UNINIT] = &ConnectedTagSessionStub::OnUninit;
    handleFuncMap[NFC_SVR_CMD_READ_NDEF_TAG] = &ConnectedTagSessionStub::OnReadNdefTag;
    handleFuncMap[NFC_SVR_CMD_WRITE_NDEF_TAG] = &ConnectedTagSessionStub::OnWriteNdefTag;
}
int ConnectedTagSessionStub::OnRemoteRequest(uint32_t code, MessageParcel &data,
                                             MessageParcel &reply, MessageOption &option)
{
    int exception = data.ReadInt32();
    if (exception) {
        return NFC_OPT_FAILED;
    }

    HandleFuncMap::iterator iter = handleFuncMap.find(code);
    if (iter == handleFuncMap.end()) {
        HILOGW("not find function to deal, code %{public}u", code);
        reply.WriteInt32(0);
        reply.WriteInt32(NFC_OPT_NOT_SUPPORTED);
    } else {
        (this->*(iter->second))(code, data, reply);
        return NFC_OPT_SUCCESS;
    }
    HILOGW("ConnectedTagSessionStub::OnRemoteRequest, default case, need check.");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}
void ConnectedTagSessionStub::OnInit(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    HILOGI("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = Init();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
}
void ConnectedTagSessionStub::OnUninit(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    HILOGI("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = Uninit();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
}
void ConnectedTagSessionStub::OnReadNdefTag(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    HILOGI("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    std::string response;
    ReadNdefTag(response);
    reply.WriteInt32(0);
    reply.WriteString(response);
}
void ConnectedTagSessionStub::OnWriteNdefTag(uint32_t code, MessageParcel &data, MessageParcel &reply)
{
    std::string dataToWrite = data.ReadString();
    HILOGI("run %{public}s datasize %{public}zu, str = %{public}s, len = %{public}d",
        __func__, data.GetRawDataSize(), dataToWrite.c_str(), dataToWrite.length());

    ErrCode ret = WriteNdefTag(dataToWrite);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
}
}  // namespace ConnectedTag
}  // namespace OHOS