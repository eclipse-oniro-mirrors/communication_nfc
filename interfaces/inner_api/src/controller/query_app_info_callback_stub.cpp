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

#include "query_app_info_callback_stub.h"

#include "nfc_service_ipc_interface_code.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
const std::string KEY_TAG_TECH = "tag-tech";
QueryAppInfoCallbackStub::QueryAppInfoCallbackStub() : callback_(nullptr), isRemoteDied_(false)
{}

QueryAppInfoCallbackStub::~QueryAppInfoCallbackStub()
{}

QueryAppInfoCallbackStub& QueryAppInfoCallbackStub::GetInstance()
{
    static QueryAppInfoCallbackStub instance;
    return instance;
}

bool QueryAppInfoCallbackStub::OnQueryAppInfo(std::string type, std::vector<int> techList,
    std::vector<std::string> aidList, std::vector<AppExecFwk::ElementName> &elementNameList)
{
    if (callback_) {
        InfoLog("OnQueryAppInfo:call callback_");
        elementNameList = callback_(type, techList);
        return true;
    }
    return false;
}

int QueryAppInfoCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    InfoLog("OnRemoteRequest: code = %{public}d", code);
    if (isRemoteDied_) {
        ErrorLog("remote service is died.");
        return KITS::ERR_NFC_STATE_UNBIND;
    }
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        ErrorLog("OnRemoteRequest: token verification error.");
        return KITS::ERR_NFC_PARAMETERS;
    }
    int exception = data.ReadInt32();
    if (exception) {
        ErrorLog("OnRemoteRequest:got exception: (%{public}d).", exception);
        return exception;
    }

    int ret = KITS::ERR_NFC_STATE_UNBIND;
    switch (code) {
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_QUERY_APP_INFO_MSG_CALLBACK): {
            ret = RemoteQueryAppInfo(data, reply);
            break;
        }

        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }
    return ret;
}

KITS::ErrorCode QueryAppInfoCallbackStub::RegisterCallback(const QueryApplicationByVendor callback)
{
    if (callback_ != nullptr) {
        InfoLog("RegisterCallback::callback_ has registered!");
        return KITS::ERR_NFC_PARAMETERS;
    }
    std::shared_lock<std::shared_mutex> guard(mutex_);
    if (callback == nullptr) {
        InfoLog("RegisterCallback::callback is nullptr!");
        callback_ = callback;
        return KITS::ERR_NFC_PARAMETERS;
    }
    callback_ = callback;
    return KITS::ERR_NONE;
}

int QueryAppInfoCallbackStub::RemoteQueryAppInfo(MessageParcel &data, MessageParcel &reply)
{
    std::shared_lock<std::shared_mutex> guard(mutex_);
    std::string type = data.ReadString();
    std::vector<AppExecFwk::ElementName> elementNameList;
    if (type.compare(KEY_TAG_TECH) == 0) {
        std::vector<int> techList;
        data.ReadInt32Vector(&techList);
        std::vector<std::string> aidList;
        OnQueryAppInfo(type, techList, aidList, elementNameList);
    }
    reply.WriteInt32(elementNameList.size());
    for (auto elementName : elementNameList) {
        elementName.Marshalling(reply);
    }
    return KITS::ERR_NONE;
}
}  // namespace NFC
}  // namespace OHOS