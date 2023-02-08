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

#include "nfc_napi_tag_isodep.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
static const int32_t DEFAULT_REF_COUNT = 1;

napi_value NapiIsoDepTag::GetHistoricalBytes(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiIsoDepTag *objectInfo = nullptr;
    napi_value result = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("GetHistoricalBytes, napi_unwrap failed, object is null.");
        ConvertStringToNumberArray(env, result, "");
        return result;
    }

    std::string historicalBytes = "";
    IsoDepTag *nfcIsoDepTagPtr = static_cast<IsoDepTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcIsoDepTagPtr == nullptr) {
        ErrorLog("GetHistoricalBytes, find objectInfo failed!");
    } else {
        historicalBytes = nfcIsoDepTagPtr->GetHistoricalBytes();
    }
    ConvertStringToNumberArray(env, result, historicalBytes);
    return result;
}

napi_value NapiIsoDepTag::GetHiLayerResponse(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiIsoDepTag *objectInfo = nullptr;
    napi_value result = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("GetHiLayerResponse, napi_unwrap failed, object is null.");
        ConvertStringToNumberArray(env, result, "");
        return result;
    }

    std::string hiLayerResponse = "";
    IsoDepTag *nfcIsoDepTagPtr = static_cast<IsoDepTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcIsoDepTagPtr == nullptr) {
        ErrorLog("GetHiLayerResponse, find objectInfo failed!");
    } else {
        hiLayerResponse = nfcIsoDepTagPtr->GetHiLayerResponse();
    }
    ConvertStringToNumberArray(env, result, hiLayerResponse);
    return result;
}

static bool CheckExtendedApduSupportedParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    // argments 0 or 1 is allowed.
    if (parameterCount == ARGV_NUM_1) {
        if (!CheckParametersAndThrow(env, parameters, {napi_function}, "callback", "function")) {
            return false;
        }
    } else if (parameterCount > ARGV_NUM_1) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return false;
    }
    return true;
}

static void NativeIsExtendedApduSupported(napi_env env, void *data)
{
    auto context = static_cast<CallBackContext<bool, NapiIsoDepTag> *>(data);
    context->value = false;
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;

    IsoDepTag *nfcIsoDepTagPtr = static_cast<IsoDepTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcIsoDepTagPtr != nullptr) {
        bool isSupported = false;
        context->errorCode = nfcIsoDepTagPtr->IsExtendedApduSupported(isSupported);
        context->value = isSupported;
    } else {
        ErrorLog("NativeIsExtendedApduSupported nfcIsoDepTagPtr failed!");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_TAG_STATE_INVALID,
            BuildErrorMessage(BUSI_ERR_TAG_STATE_INVALID, "", "", "", "")));
        return;
    }
    context->resolved = true;
}

static void IsExtendedApduSupportedCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<CallBackContext<bool, NapiIsoDepTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is boolean.
        napi_get_boolean(env, context->value, &callbackValue);
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string msg = BuildErrorMessage(errCode, "isExtendedApduSupported", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, msg);
    }
}

napi_value NapiIsoDepTag::IsExtendedApduSupported(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_1;
    napi_value params[ARGV_NUM_1] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiIsoDepTag *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckExtendedApduSupportedParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<CallBackContext<bool, NapiIsoDepTag>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }
    napi_create_reference(env, params[0], DEFAULT_REF_COUNT, &context->callbackRef);

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(
        env, context, "IsExtendedApduSupported", NativeIsExtendedApduSupported, IsExtendedApduSupportedCallback);
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS