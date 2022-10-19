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
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace KITS {
static const int32_t DEFAULT_REF_COUNT = 1;
const std::string VAR_UID = "uid";
const std::string VAR_TECH = "technology";
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

napi_value NapiNfcTagSession::GetTagInfo(napi_env env, napi_callback_info info)
{
    napi_value argv[] = {nullptr};
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, 0, argv);
    if (nfcTag == nullptr) {
        return CreateUndefined(env);
    }
    std::weak_ptr<TagInfo> tagInfo = nfcTag->GetTagInfo();
    if (tagInfo.expired()) {
        return CreateUndefined(env);
    }
    std::string uid = tagInfo.lock()->GetTagUid();
    std::vector<int> techList = tagInfo.lock()->GetTagTechList();

    // build tagInfo Js Object, with menber uid and technology only.
    napi_value tagInfoObj = nullptr;
    napi_value uidValue;
    napi_value techValue;
    napi_create_object(env, &tagInfoObj);
    std::vector<unsigned char> uidBytes;
    NfcSdkCommon::HexStringToBytes(uid, uidBytes);
    BytesVectorToJS(env, uidValue, uidBytes);
    napi_create_array_with_length(env, techList.size(), &techValue);
    for (uint32_t i = 0; i < techList.size(); i++) {
        napi_value tech;
        napi_create_uint32(env, techList[i], &tech);
        napi_set_element(env, techValue, i, tech);
    }
    napi_set_named_property(env, tagInfoObj, VAR_UID.c_str(), uidValue);
    napi_set_named_property(env, tagInfoObj, VAR_TECH.c_str(), techValue);
    return tagInfoObj;
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

std::string HexArrayToString(const std::vector<unsigned char >& data)
{
    const std::string hexKeys = "0123456789ABCDEF";
    std::string ret = "";
    size_t shift = 4;
    for (auto it : data) {
        ret.push_back(hexKeys[(it & 0xF0) >> shift]);
        ret.push_back(hexKeys[it & 0x0F]);
    }
    return ret;
}

static bool MatchSendDataParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool typeMatch = false;
    switch (parameterCount) {
        case ARGV_NUM_1: {
            typeMatch = MatchParameters(env, parameters, {napi_object});
            break;
        }
        case ARGV_NUM_2:
            typeMatch = MatchParameters(env, parameters, {napi_object, napi_function});
            break;
        default: {
            return false;
        }
    }
    return typeMatch;
}

static void NativeSendData(napi_env env, void *data)
{
    auto context = static_cast<NfcTagSessionContext<std::string, NapiNfcTagSession> *>(data);
    BasicTagSession *nfcTagSessionPtr =
        static_cast<BasicTagSession *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcTagSessionPtr == nullptr) {
        ErrorLog("NativeSendData find objectInfo failed!");
    } else {
        int status = TAG::TagRwResponse::Status::STATUS_FAILURE;
        context->value = nfcTagSessionPtr->SendCommand(context->dataBytes, true, status);
        DebugLog("NativeSendData context value = %{public}s", context->value.c_str());
    }
    context->resolved = true;
}

static void SendDataCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<NfcTagSessionContext<std::string, NapiNfcTagSession> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            ConvertStringToNumberArray(env, callbackValue, context->value.c_str());
        } else {
            callbackValue = CreateErrorMessage(env, "SendData error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "SendData error,napi_status = " + std ::to_string(status));
    }

    Handle2ValueCallback(env, context, callbackValue);
}

napi_value NapiNfcTagSession::SendData(napi_env env, napi_callback_info info)
{
    // JS API define1: sendData(data: number[]): Promise<number[]>
    // JS API define2: sendData(data: number[], callback: AsyncCallback<number[]>): void
    DebugLog("TagSession SendData called");
    size_t paramsCount = ARGV_NUM_2;
    napi_value params[ARGV_NUM_2] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiNfcTagSession *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    NAPI_ASSERT(env, MatchSendDataParameters(env, params, paramsCount), "SendData type mismatch");
    auto context = std::make_unique<NfcTagSessionContext<std::string, NapiNfcTagSession>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), "NfcTagSessionContext is nullptr"));
        return CreateUndefined(env);
    }
    // parse the params
    napi_value sendDatas = params[ARGV_INDEX_0];

    bool isArray = false;
    status = napi_is_array(env, sendDatas, &isArray);
    if (status != napi_ok || !isArray) {
        ErrorLog("NapiNfcTagSession::SendData, not array");
        return CreateUndefined(env);
    }

    int32_t sendData = 0;
    napi_value dataValue = nullptr;
    uint32_t arrayLength = 0;
    std::vector<unsigned char> dataBytes = {};
    NAPI_CALL(env, napi_get_array_length(env, params[ARGV_INDEX_0], &arrayLength));
    for (uint32_t i = 0; i < arrayLength; ++i) {
        NAPI_CALL(env, napi_get_element(env, sendDatas, i, &dataValue));
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, dataValue, &valueType);
        if (valueType != napi_number) {
            ErrorLog("NapiNfcTagSession::SendData, send data not number!");
            return CreateUndefined(env);
        }
        NAPI_CALL(env, napi_get_value_int32(env, dataValue, &sendData));
        dataBytes.push_back(sendData);
    }
    context->dataBytes = HexArrayToString(dataBytes);
    DebugLog("NapiNfcTagSession::SendData dataBytes %{public}s", context->dataBytes.c_str());

    if (paramsCount == ARGV_NUM_2) {
        napi_create_reference(env, params[ARGV_INDEX_1], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "SendData", NativeSendData, SendDataCallback);
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS