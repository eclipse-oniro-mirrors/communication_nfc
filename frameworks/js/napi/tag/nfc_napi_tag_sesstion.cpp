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
        ErrorLog("GetTag objectInfo nullptr!");
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
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, 0, argv);
    if (nfcTag == nullptr) {
        napi_get_boolean(env, false, &result);
    } else {
        int err = nfcTag->Connect();
        napi_get_boolean(env, err == ErrorCode::ERR_NONE, &result);
    }
    return result;
}

napi_value NapiNfcTagSession::Reset(napi_env env, napi_callback_info info)
{
    napi_value argv[] = {nullptr};
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, 0, argv);
    if (nfcTag != nullptr) {
        nfcTag->Close();
    }
    return CreateUndefined(env);
}

napi_value NapiNfcTagSession::IsTagConnected(napi_env env, napi_callback_info info)
{
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
    size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
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
    if (timeoutValue <= 0) {
        ErrorLog("SetSendDataTimeout, the arg must be positive.");
        napi_get_boolean(env, false, &result);
        return result;
    }
    bool succ = nfcTag->SetTimeout(timeoutValue) == ErrorCode::ERR_NONE;
    napi_get_boolean(env, succ, &result);
    return result;
}

napi_value NapiNfcTagSession::GetSendDataTimeout(napi_env env, napi_callback_info info)
{
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, 0, argv);
    if (nfcTag == nullptr) {
        ErrorLog("GetSendDataTimeout find objectInfo failed!");
        napi_create_int32(env, 0, &result);
    } else {
        int timeout = 0;
        nfcTag->GetTimeout(timeout);
        napi_create_int32(env, timeout, &result);
    }
    return result;
}

napi_value NapiNfcTagSession::GetMaxSendLength(napi_env env, napi_callback_info info)
{
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, 0, argv);
    if (nfcTag == nullptr) {
        ErrorLog("GetMaxSendLength find objectInfo failed!");
        napi_create_int32(env, 0, &result);
    } else {
        int maxsendlen = 0;
        nfcTag->GetMaxSendCommandLength(maxsendlen);
        napi_create_int32(env, maxsendlen, &result);
    }
    return result;
}

static bool MatchSendDataParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool isTypeMatched = false;
    switch (parameterCount) {
        case ARGV_NUM_1: {
            isTypeMatched = MatchParameters(env, parameters, {napi_object});
            break;
        }
        case ARGV_NUM_2:
            isTypeMatched = MatchParameters(env, parameters, {napi_object, napi_function});
            break;
        default: {
            return false;
        }
    }
    if (isTypeMatched) {
        isTypeMatched = IsNumberArray(env, parameters[ARGV_NUM_0]);
    }
    return isTypeMatched;
}

static void NativeSendData(napi_env env, void *data)
{
    auto context = static_cast<NfcTagSessionContext<std::string, NapiNfcTagSession> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    BasicTagSession *nfcTagSessionPtr =
        static_cast<BasicTagSession *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcTagSessionPtr != nullptr) {
        std::string hexRespData;
        context->errorCode = nfcTagSessionPtr->SendCommand(context->dataBytes, true, hexRespData);
        context->value = hexRespData;
    } else {
        ErrorLog("NativeSendData, nfcTagSessionPtr failed.");
    }
    context->resolved = true;
}

static void SendDataCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<NfcTagSessionContext<std::string, NapiNfcTagSession> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is number[].
        ConvertStringToNumberArray(env, callbackValue, context->value.c_str());
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "sendData", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiNfcTagSession::SendData(napi_env env, napi_callback_info info)
{
    // JS API define1: sendData(data: number[]): Promise<number[]>
    // JS API define2: sendData(data: number[], callback: AsyncCallback<number[]>): void
    size_t paramsCount = ARGV_NUM_2;
    napi_value params[ARGV_NUM_2] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiNfcTagSession *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    if (!MatchSendDataParameters(env, params, paramsCount)) {
        ErrorLog("SendData, invalid parameters!");
        return CreateUndefined(env);
    }
    auto context = std::make_unique<NfcTagSessionContext<std::string, NapiNfcTagSession>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), ERR_INIT_CONTEXT.c_str()));
        return CreateUndefined(env);
    }

    // parse the params
    int32_t hexCmdData = 0;
    napi_value hexCmdDataValue = nullptr;
    uint32_t arrayLength = 0;
    std::vector<unsigned char> dataBytes = {};
    NAPI_CALL(env, napi_get_array_length(env, params[ARGV_INDEX_0], &arrayLength));
    for (uint32_t i = 0; i < arrayLength; ++i) {
        NAPI_CALL(env, napi_get_element(env, params[ARGV_INDEX_0], i, &hexCmdDataValue));
        NAPI_CALL(env, napi_get_value_int32(env, hexCmdDataValue, &hexCmdData));
        dataBytes.push_back(hexCmdData);
    }
    context->dataBytes = NfcSdkCommon::BytesVecToHexString(static_cast<unsigned char *>(dataBytes.data()),
        dataBytes.size());
    if (paramsCount == ARGV_NUM_2) {
        napi_create_reference(env, params[ARGV_INDEX_1], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "SendData", NativeSendData, SendDataCallback);
    return result;
}

static bool CheckTagSessionAndThrow(napi_env env, std::shared_ptr<BasicTagSession> nfcTag)
{
    if (nfcTag == nullptr) {
        // object null is unexpected, unknown error.
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_TAG_STATE_INVALID,
            BuildErrorMessage(BUSI_ERR_TAG_STATE_INVALID, "", "", "", "")));
        return false;
    }
    return true;
}

napi_value NapiNfcTagSession::Connect(napi_env env, napi_callback_info info)
{
    napi_value argv[] = {nullptr};
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, 0, argv);
    if (!CheckTagSessionAndThrow(env, nfcTag)) {
        return CreateUndefined(env);
    }
    int statusCode = nfcTag->Connect();
    CheckTagStatusCodeAndThrow(env, statusCode, "connect");
    return CreateUndefined(env);
}

napi_value NapiNfcTagSession::ResetConnection(napi_env env, napi_callback_info info)
{
    napi_value argv[] = {nullptr};
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, 0, argv);
    if (!CheckTagSessionAndThrow(env, nfcTag)) {
        return nullptr;
    }
    int statusCode = nfcTag->Close();
    CheckTagStatusCodeAndThrow(env, statusCode, "resetConnection");
    return CreateUndefined(env);
}

napi_value NapiNfcTagSession::IsConnected(napi_env env, napi_callback_info info)
{
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, 0, argv);

    // IsConnected is returned by 'inner_api', not by 'service', no need to check permission.
    bool isConnected = nfcTag != nullptr && nfcTag->IsConnected();
    napi_get_boolean(env, isConnected, &result);
    return result;
}

napi_value NapiNfcTagSession::SetTimeout(napi_env env, napi_callback_info info)
{
    size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, argc, argv);
    if (!CheckTagSessionAndThrow(env, nfcTag)) {
        return CreateUndefined(env);
    }

    // check the arguments valid in napi.
    if (!CheckArgCountAndThrow(env, argc, ARGV_NUM_1) ||
        !CheckNumberAndThrow(env, argv[ARGV_INDEX_0], "timeout", "number")) {
        return CreateUndefined(env);
    }

    int32_t timeoutValue = 0;
    ParseInt32(env, timeoutValue, argv[0]);
    if (timeoutValue <= 0) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "timeout", "positive number")));
        return CreateUndefined(env);
    }
    int statusCode = nfcTag->SetTimeout(timeoutValue);
    CheckTagStatusCodeAndThrow(env, statusCode, "setTimeout");
    return CreateUndefined(env);
}

napi_value NapiNfcTagSession::GetTimeout(napi_env env, napi_callback_info info)
{
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, 0, argv);
    if (!CheckTagSessionAndThrow(env, nfcTag)) {
        return CreateUndefined(env);
    }
    int timeout = 0;
    int statusCode = nfcTag->GetTimeout(timeout);
    if (!CheckTagStatusCodeAndThrow(env, statusCode, "getTimeout")) {
        return CreateUndefined(env);
    }
    napi_create_int32(env, timeout, &result);
    return result;
}

napi_value NapiNfcTagSession::GetMaxTransmitSize(napi_env env, napi_callback_info info)
{
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    std::shared_ptr<BasicTagSession> nfcTag = GetTag(env, info, 0, argv);
    if (!CheckTagSessionAndThrow(env, nfcTag)) {
        return CreateUndefined(env);
    }
    int maxSize = 0;
    int statusCode = nfcTag->GetMaxSendCommandLength(maxSize);
    if (!CheckTagStatusCodeAndThrow(env, statusCode, "getMaxTransmitSize")) {
        return CreateUndefined(env);
    }
    napi_create_int32(env, maxSize, &result);
    return result;
}

// check arguments valid for 'transmit' and throw error if invalid.
static bool CheckTransmitParametersAndThrow(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount == ARGV_NUM_1) {
        if (!CheckParametersAndThrow(env, parameters, {napi_object}, "data", "number[]")) {
            return false;
        }
        return true;
    } else if (parameterCount == ARGV_NUM_2) {
        if (!CheckParametersAndThrow(env, parameters, {napi_object, napi_function},
            "data & callback", "number[] & function") ||
            !CheckArrayNumberAndThrow(env, parameters[ARGV_NUM_0], "data", "number[]")) {
            return false;
        }
        return true;
    } else {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return false;
    }
}

// native function called, add the 'inner_api' calling to request to service.
static void NativeTransmit(napi_env env, void *data)
{
    auto context = static_cast<NfcTagSessionContext<std::string, NapiNfcTagSession> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    BasicTagSession *nfcTagSessionPtr =
        static_cast<BasicTagSession *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcTagSessionPtr != nullptr) {
        std::string hexRespData;
        context->errorCode = nfcTagSessionPtr->SendCommand(context->dataBytes, true, hexRespData);
        context->value = hexRespData;
    } else {
        ErrorLog("NativeSendData, nfcTagSessionPtr failed.");
    }
    context->resolved = true;
}

// the aysnc callback to check the status and throw error.
static void TransmitCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<NfcTagSessionContext<std::string, NapiNfcTagSession> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is number[].
        ConvertStringToNumberArray(env, callbackValue, context->value.c_str());
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "transmit", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiNfcTagSession::Transmit(napi_env env, napi_callback_info info)
{
    // JS API define1: Transmit(data: number[]): Promise<number[]>
    // JS API define2: Transmit(data: number[], callback: AsyncCallback<number[]>): void
    size_t paramsCount = ARGV_NUM_2;
    napi_value params[ARGV_NUM_2] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiNfcTagSession *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckTransmitParametersAndThrow(env, params, paramsCount)) {
        return CreateUndefined(env);
    }

    auto context = std::make_unique<NfcTagSessionContext<std::string, NapiNfcTagSession>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    // parse the params
    int32_t hexCmdData = 0;
    napi_value hexCmdDataValue = nullptr;
    uint32_t arrayLength = 0;
    std::vector<unsigned char> dataBytes = {};
    NAPI_CALL(env, napi_get_array_length(env, params[ARGV_INDEX_0], &arrayLength));
    for (uint32_t i = 0; i < arrayLength; ++i) {
        NAPI_CALL(env, napi_get_element(env, params[ARGV_INDEX_0], i, &hexCmdDataValue));
        NAPI_CALL(env, napi_get_value_int32(env, hexCmdDataValue, &hexCmdData));
        dataBytes.push_back(hexCmdData);
    }
    context->dataBytes = NfcSdkCommon::BytesVecToHexString(static_cast<unsigned char *>(dataBytes.data()),
        dataBytes.size());
    if (paramsCount == ARGV_NUM_2) {
        napi_create_reference(env, params[ARGV_INDEX_1], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "Transmit", NativeTransmit, TransmitCallback);
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS