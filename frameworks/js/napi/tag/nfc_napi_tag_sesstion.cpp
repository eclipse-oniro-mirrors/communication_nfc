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

#include "nfc_napi_tag_sesstion.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
std::shared_ptr<BasicTagSession> NapiNfcTagSession::GetTag(napi_env env, napi_callback_info info,
    size_t argc, napi_value argv[])
{
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));

    // unwrap from thisVar to retrieve the native instance
    NapiNfcTagSession *objectInfo = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    if (objectInfo == nullptr) {
        ErrorLog("ConnectTag objectInfo nullptr!");
        return nullptr;
    }
    if (objectInfo->tagSession == nullptr) {
        ErrorLog("ConnectTag tagSession nullptr!");
        return nullptr;
    }
    return objectInfo->tagSession;
}

napi_value NapiNfcTagSession::ConnectTag(napi_env env, napi_callback_info info)
{
    DebugLog("GetTagSession ConnectTag called");
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, 0, argv);
    if (nfcTag == nullptr) {
        napi_get_boolean(env, false, &result);
    } else {
        int err = nfcTag->Connect();
        napi_get_boolean(env, err == NfcErrorCode::NFC_SUCCESS, &result);
    }
    return result;
}

napi_value NapiNfcTagSession::Reset(napi_env env, napi_callback_info info)
{
    DebugLog("TagSession Reset called");
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, 0, argv);
    if (nfcTag != nullptr) {
        nfcTag->Close();
    }
    return result;
}

napi_value NapiNfcTagSession::IsTagConnected(napi_env env, napi_callback_info info)
{
    DebugLog("GetTagSession IsTagConnected called");
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, 0, argv);
    if (nfcTag == nullptr) {
        ErrorLog("IsTagConnected find objectInfo failed!");
        napi_get_boolean(env, false, &result);
    } else {
        bool isConnected = nfcTag->IsConnected();
        napi_get_boolean(env, isConnected, &result);
    }
    return result;
}

napi_value NapiNfcTagSession::SetSendDataTimeout(napi_env env, napi_callback_info info)
{
    DebugLog("GetTagSession SetSendDataTimeout called");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value result = nullptr;
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, argc, argv);
    if (nfcTag == nullptr) {
        ErrorLog("SetSendDataTimeout find objectInfo failed!");
        napi_get_boolean(env, false, &result);
        return result;
    }

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_number) {
        ErrorLog("SetSendDataTimeout, the arg is not number");
        napi_get_boolean(env, false, &result);
        return result;
    }
    int32_t timeoutValue = 0;
    ParseInt32(env, timeoutValue, argv[0]);
    ErrorLog("SetSendDataTimeout, timeoutValue = %{public}d", timeoutValue);
    bool succ = nfcTag->SetTimeout(timeoutValue);
    napi_get_boolean(env, succ, &result);
    return result;
}

napi_value NapiNfcTagSession::GetSendDataTimeout(napi_env env, napi_callback_info info)
{
    DebugLog("TagSession GetSendDataTimeout called");
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, 0, argv);
    if (nfcTag == nullptr) {
        ErrorLog("GetSendDataTimeout find objectInfo failed!");
        napi_create_int32(env, 0, &result);
    } else {
        uint32_t timeout = nfcTag->GetTimeout();
        napi_create_int32(env, timeout, &result);
    }
    return result;
}

napi_value NapiNfcTagSession::GetMaxSendLength(napi_env env, napi_callback_info info)
{
    DebugLog("TagSession GetMaxSendLength called");
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, 0, argv);
    if (nfcTag == nullptr) {
        ErrorLog("GetMaxSendLength find objectInfo failed!");
        napi_create_int32(env, 0, &result);
    } else {
        int maxsendlen = nfcTag->GetMaxSendCommandLength();
        napi_create_int32(env, maxsendlen, &result);
    }
    return result;
}

napi_value NapiNfcTagSession::SendData(napi_env env, napi_callback_info info)
{
    //JS API define1: sendData(data: number[]): Promise<number[]>
    //JS API define2: sendData(data: number[], callback: AsyncCallback<number[]>): void
    DebugLog("TagSession SendData called");
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value result = nullptr;
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, argc, argv);
    if (nfcTag == nullptr) {
        ErrorLog("SendData find objectInfo failed!");
        ConvertStringToNumberArray(env, result, "");
        return result;
    }

    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (valueType != napi_string) {
        ErrorLog("SendData, the 1st arg is not string");
        ConvertStringToNumberArray(env, result, "");
        return result;
    }
    std::string dataBytes;
    ParseString(env, dataBytes, argv[0]);
    int status = TAG::TagRwResponse::Status::STATUS_FAILURE;
    std::string responseData = nfcTag->SendCommand(dataBytes, true, status);
    if (status != TAG::TagRwResponse::Status::STATUS_SUCCESS) {
        ErrorLog("SendData, rcv remote status err = %{public}d", status);
    } else {
        DebugLog("SendData, responseData = %{public}s", responseData.c_str());
        ConvertStringToNumberArray(env, result, responseData);
    }
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS