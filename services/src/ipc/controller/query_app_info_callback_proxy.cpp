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

#include "query_app_info_callback_proxy.h"

#include "nfc_service_ipc_interface_code.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
QueryAppInfoCallbackProxy::QueryAppInfoCallbackProxy(const sptr<IRemoteObject> &remote)
    : IRemoteProxy<IQueryAppInfoCallback>(remote)
{}

bool QueryAppInfoCallbackProxy::OnQueryAppInfo(std::string type, std::vector<int> techList,
    std::vector<AAFwk::Want> &hceAppList, std::vector<AppExecFwk::ElementName> &elementNameList)
{
    MessageOption option = {MessageOption::TF_SYNC};
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("OnQueryAppInfo:WriteInterfaceToken token error");
        return false;
    }
    data.WriteInt32(0);
    data.WriteString(type);
    if (type.compare(KEY_TAG_APP) == 0) {
        DebugLog("query tag app.");
        data.WriteInt32Vector(techList);
        int error = Remote()->SendRequest(
            static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_QUERY_APP_INFO_MSG_CALLBACK),
            data, reply, option);
        if (error != ERR_NONE) {
            ErrorLog("QueryAppInfoCallbackProxy::OnQueryAppInfo, Set Attr %{public}d error: %{public}d",
                NfcServiceIpcInterfaceCode::COMMAND_QUERY_APP_INFO_MSG_CALLBACK, error);
            return false;
        }
        int elementNameListLen = reply.ReadInt32();
        for (int i = 0; i < elementNameListLen; i++) {
            elementNameList.push_back(*AppExecFwk::ElementName::Unmarshalling(reply));
        }
        return true;
    } else if (type.compare(KEY_HCE_APP) == 0) {
        DebugLog("query hce app.");
        int error = Remote()->SendRequest(
            static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_QUERY_APP_INFO_MSG_CALLBACK),
            data, reply, option);
        if (error != ERR_NONE) {
            ErrorLog("QueryAppInfoCallbackProxy::OnQueryAppInfo, Set Attr %{public}d error: %{public}d",
                NfcServiceIpcInterfaceCode::COMMAND_QUERY_APP_INFO_MSG_CALLBACK, error);
            return false;
        }
        int appLen = reply.ReadInt32();
        InfoLog("QueryAppInfoCallbackProxy::OnQueryAppInfo recv %{public}d app need to add", appLen);
        for (int i = 0; i < appLen; i++) {
            hceAppList.push_back(*(AAFwk::Want::Unmarshalling(reply)));
        }
        return true;
    }
    return false;
}
}  // namespace NFC
}  // namespace OHOS
