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
    DebugLog("NapiIsoDepTag GetHistoricalBytes called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
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
        napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &ret);
    } else {
        std::string historicalBytes = nfcIsoDepTagPtr->GetHistoricalBytes();
        DebugLog("HistoricalBytes %{public}s", historicalBytes.c_str());
        napi_create_string_utf8(env, historicalBytes.c_str(), NAPI_AUTO_LENGTH, &ret);
    }
    return ret;
}

napi_value NapiIsoDepTag::GetHiLayerResponse(napi_env env, napi_callback_info info)
{
    DebugLog("NapiIsoDepTag GetHiLayerResponse called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
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
        napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &ret);
    } else {
        std::string hiLayerResponse = nfcIsoDepTagPtr->GetHiLayerResponse();
        DebugLog("HiLayerResponse %{public}s", hiLayerResponse.c_str());
        napi_create_string_utf8(env, hiLayerResponse.c_str(), NAPI_AUTO_LENGTH, &ret);
    }
    return ret;
}

static bool MatchIsExtendedApduSupportedParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount > 1) {
        return false;
    }
    if (parameterCount == 1) {
        return MatchParameters(env, parameters, {napi_function});
    }
    return true;
}

static void NativeIsExtendedApduSupported(napi_env env, void *data)
{
    DebugLog("NativeIsExtendedApduSupported called");
    auto context = static_cast<CallBackContext<bool, NapiIsoDepTag> *>(data);

    IsoDepTag *nfcIsoDepTagPtr = static_cast<IsoDepTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcIsoDepTagPtr == nullptr) {
        DebugLog("NativeIsExtendedApduSupported find objectInfo failed!");
        context->value = true;
    } else {
        context->value = nfcIsoDepTagPtr->IsExtendedApduSupported();
    }
    context->resolved = true;
}

static void IsExtendedApduSupportedCallback(napi_env env, napi_status status, void *data)
{
    DebugLog("IsExtendedApduSupportedCallback called");
    auto context = static_cast<CallBackContext<bool, NapiIsoDepTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_status status = napi_get_boolean(env, context->value, &callbackValue);
            if (status != napi_ok) {
                ErrorLog("get boolean failed");
            }
        } else {
            callbackValue = CreateErrorMessage(env, "IsExtendedApduSupported error by ipc");
        }
    } else {
        callbackValue =
            CreateErrorMessage(env, "IsExtendedApduSupported error,napi_status = " + std ::to_string(status));
    }
    Handle2ValueCallback(env, context, callbackValue);
}

napi_value NapiIsoDepTag::IsExtendedApduSupported(napi_env env, napi_callback_info info)
{
    DebugLog("GetIsoDepTag IsExtendedApduSupported called");
    size_t paramsCount = 1;
    napi_value params[1] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiIsoDepTag *objectInfoCb = nullptr;

    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    NAPI_ASSERT(env, MatchIsExtendedApduSupportedParameters(env, params, paramsCount),
        "IsExtendedApduSupported type mismatch");
    auto context = std::make_unique<CallBackContext<bool, NapiIsoDepTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "error at SingleValueContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    if (paramsCount == 1) {
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