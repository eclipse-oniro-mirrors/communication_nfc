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
QueryAppInfoCallbackStub::QueryAppInfoCallbackStub()
    : queryTagAppByTechCallback_(nullptr), queryHceAppCallback_(nullptr), isRemoteDied_(false)
{}

QueryAppInfoCallbackStub::~QueryAppInfoCallbackStub()
{}

QueryAppInfoCallbackStub& QueryAppInfoCallbackStub::GetInstance()
{
    static QueryAppInfoCallbackStub instance;
    return instance;
}

bool QueryAppInfoCallbackStub::OnQueryAppInfo(std::string type, std::vector<int> techList,
    std::vector<AAFwk::Want> &hceAppList, std::vector<AppExecFwk::ElementName> &elementNameList)
{
    if (type.compare(KEY_TAG_APP) == 0) {
        if (queryTagAppByTechCallback_) {
            InfoLog("OnQueryAppInfo:call tag callback_");
            elementNameList = queryTagAppByTechCallback_(techList);
        }
        return true;
    } else if (type.compare(KEY_HCE_APP) == 0) {
        if (queryHceAppCallback_) {
            InfoLog("OnQueryAppInfo:call hce callback_");
            hceAppList = queryHceAppCallback_();
        }
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

KITS::ErrorCode QueryAppInfoCallbackStub::RegisterQueryTagAppCallback(const QueryApplicationByVendor tagCallback)
{
    if (queryTagAppByTechCallback_ != nullptr) {
        InfoLog("RegisterQueryTagAppCallback::queryTagAppByTechCallback_ has registered!");
        return KITS::ERR_NFC_PARAMETERS;
    }
    std::unique_lock<std::shared_mutex> guard(mutex_);
    if (tagCallback == nullptr) {
        InfoLog("RegisterQueryTagAppCallback::callback is nullptr!");
        queryTagAppByTechCallback_ = tagCallback;
        return KITS::ERR_NFC_PARAMETERS;
    }
    queryTagAppByTechCallback_ = tagCallback;
    return KITS::ERR_NONE;
}

KITS::ErrorCode QueryAppInfoCallbackStub::RegisterQueryHceAppCallback(const QueryHceAppByVendor hceCallback)
{
    if (queryHceAppCallback_ != nullptr) {
        InfoLog("RegisterQueryHceAppCallback::queryHceAppCallback_ has registered!");
        return KITS::ERR_NFC_PARAMETERS;
    }
    std::unique_lock<std::shared_mutex> guard(mutex_);
    if (hceCallback == nullptr) {
        InfoLog("RegisterQueryHceAppCallback::callback is nullptr!");
        queryHceAppCallback_ = hceCallback;
        return KITS::ERR_NFC_PARAMETERS;
    }
    queryHceAppCallback_ = hceCallback;
    return KITS::ERR_NONE;
}

int QueryAppInfoCallbackStub::RemoteQueryAppInfo(MessageParcel &data, MessageParcel &reply)
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    std::string type = data.ReadString();
    std::vector<AppExecFwk::ElementName> elementNameList;
    std::vector<AAFwk::Want> hceAppList;
    std::vector<int> techList;
    if (type.compare(KEY_TAG_APP) == 0) {
        data.ReadInt32Vector(&techList);
        OnQueryAppInfo(type, techList, hceAppList, elementNameList);
        reply.WriteInt32(elementNameList.size());
        for (AppExecFwk::ElementName elementName : elementNameList) {
            elementName.Marshalling(reply);
        }
    } else if (type.compare(KEY_HCE_APP) == 0) {
        OnQueryAppInfo(type, techList, hceAppList, elementNameList);
        int appLen = hceAppList.size();
        reply.WriteInt32(appLen);
        for (int i = 0; i < appLen; i++) {
            hceAppList[i].Marshalling(reply);
        }
    }
    return KITS::ERR_NONE;
}
}  // namespace NFC
}  // namespace OHOS