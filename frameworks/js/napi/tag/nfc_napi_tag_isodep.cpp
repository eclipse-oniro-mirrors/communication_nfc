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
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    // transfer
    IsoDepTag *nfcIsoDepTagPtr = static_cast<IsoDepTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    napi_value ret = nullptr;
    if (nfcIsoDepTagPtr == nullptr) {
        ErrorLog("GetHistoricalBytes find objectInfo failed!");
        ConvertStringToNumberArray(env, ret, "");
    } else {
        std::string historicalBytes = nfcIsoDepTagPtr->GetHistoricalBytes();
        ConvertStringToNumberArray(env, ret, historicalBytes);
    }
    return ret;
}

napi_value NapiIsoDepTag::GetHiLayerResponse(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiIsoDepTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    // transfer
    IsoDepTag *nfcIsoDepTagPtr = static_cast<IsoDepTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    napi_value ret = nullptr;
    if (nfcIsoDepTagPtr == nullptr) {
        ErrorLog("GetHiLayerResponse find objectInfo failed!");
        ConvertStringToNumberArray(env, ret, "");
    } else {
        std::string hiLayerResponse = nfcIsoDepTagPtr->GetHiLayerResponse();
        ConvertStringToNumberArray(env, ret, hiLayerResponse);
    }
    return ret;
}

static bool MatchIsExtendedApduSupportedParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount > ARGV_NUM_1) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return false;
    }
    if (parameterCount == ARGV_NUM_1) {
        bool isTypeMatched = MatchParameters(env, parameters, {napi_function});
        if (!isTypeMatched) {
            napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                BuildErrorMessage(BUSI_ERR_PARAM, "", "", "callback", "function")));
        }
        return isTypeMatched;
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
        std::string msg = BuildErrorMessage(context->errorCode, "isExtendedApduSupported", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, context->errorCode, msg);
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
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    if (!MatchIsExtendedApduSupportedParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<CallBackContext<bool, NapiIsoDepTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), ERR_INIT_CONTEXT.c_str()));
        return CreateUndefined(env);
    }
    if (paramsCount == ARGV_NUM_1) {
        napi_create_reference(env, params[0], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(
        env, context, "IsExtendedApduSupported", NativeIsExtendedApduSupported, IsExtendedApduSupportedCallback);
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS