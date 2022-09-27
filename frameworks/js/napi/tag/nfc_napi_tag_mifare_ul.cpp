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
#include "nfc_napi_tag_mifare_ul.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace KITS {
static const int32_t DEFAULT_REF_COUNT = 1;

static bool MatchReadMultiplePagesParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool typeMatch = false;
    switch (parameterCount) {
        case ARGV_NUM_1: {
            typeMatch = MatchParameters(env, parameters, {napi_number});
            break;
        }
        case ARGV_NUM_2:
            typeMatch = MatchParameters(env, parameters, {napi_number, napi_function});
            break;
        default: {
            return false;
        }
    }
    return typeMatch;
}

static void NativeReadMultiplePages(napi_env env, void *data)
{
    DebugLog("NativeReadMultiplePages called");
    auto context = static_cast<MifareUltralightContext<std::string, NapiMifareUltralightTag> *>(data);
    MifareUltralightTag *nfcMifareUlTagPtr =
        static_cast<MifareUltralightTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareUlTagPtr == nullptr) {
        ErrorLog("NativeReadMultiplePages find objectInfo failed!");
    } else {
        context->value = nfcMifareUlTagPtr->ReadMultiplePages(context->pageIndex);
        DebugLog("NativeReadMultiplePages context value = %{public}s", context->value.c_str());
    }
    context->resolved = true;
}

static void ReadMultiplePagesCallback(napi_env env, napi_status status, void *data)
{
    DebugLog("ReadMultiplePagesCallback called");
    auto context = static_cast<MifareUltralightContext<std::string, NapiMifareUltralightTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            ConvertStringToNumberArray(env, callbackValue, context->value);
        } else {
            callbackValue = CreateErrorMessage(env, "ReadMultiplePages error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "ReadMultiplePages error,napi_status = " + std ::to_string(status));
    }

    Handle2ValueCallback(env, context, callbackValue);
}

napi_value NapiMifareUltralightTag::ReadMultiplePages(napi_env env, napi_callback_info info)
{
    DebugLog("ReadMultiplePages called");
    size_t paramsCount = ARGV_NUM_2;
    napi_value params[ARGV_NUM_2] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiMifareUltralightTag *objectInfoCb = nullptr;

    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    NAPI_ASSERT(env, MatchReadMultiplePagesParameters(env, params, paramsCount), "ReadMultiplePages type mismatch");
    auto context = std::make_unique<MifareUltralightContext<std::string, NapiMifareUltralightTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "MifareUltralightContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return CreateUndefined(env);
    }
    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->pageIndex);
    DebugLog("ReadMultiplePages sectorIndex = %{public}d", context->pageIndex);

    if (paramsCount == ARGV_NUM_2) {
        napi_create_reference(env, params[1], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result =
        HandleAsyncWork(env, context, "ReadMultiplePages", NativeReadMultiplePages, ReadMultiplePagesCallback);
    return result;
}

static bool MatchWriteSinglePagesParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool typeMatch = false;
    switch (parameterCount) {
        case ARGV_NUM_2: {
            typeMatch = MatchParameters(env, parameters, {napi_number, napi_object});
            break;
        }
        case ARGV_NUM_3:
            typeMatch = MatchParameters(env, parameters, {napi_number, napi_object, napi_function});
            break;
        default: {
            return false;
        }
    }
    return typeMatch;
}

static void NativeWriteSinglePages(napi_env env, void *data)
{
    DebugLog("NativeWriteSinglePages called");
    auto context = static_cast<MifareUltralightContext<int, NapiMifareUltralightTag> *>(data);
    MifareUltralightTag *nfcMifareUlTagPtr =
        static_cast<MifareUltralightTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareUlTagPtr == nullptr) {
        ErrorLog("NativeWriteSinglePages find objectInfo failed!");
    } else {
        context->value = nfcMifareUlTagPtr->WriteSinglePages(context->pageIndex, context->data);
        DebugLog("NativeWriteSinglePages context value = %{public}d", context->value);
    }
    context->resolved = true;
}

static void WriteSinglePagesCallback(napi_env env, napi_status status, void *data)
{
    DebugLog("WriteSinglePagesCallback called");
    auto context = static_cast<MifareUltralightContext<int, NapiMifareUltralightTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_int32(env, context->value, &callbackValue);
        } else {
            callbackValue = CreateErrorMessage(env, "WriteSinglePages error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "WriteSinglePages error,napi_status = " + std ::to_string(status));
    }

    Handle2ValueCallback(env, context, callbackValue);
}

napi_value NapiMifareUltralightTag::WriteSinglePages(napi_env env, napi_callback_info info)
{
    DebugLog("WriteSinglePages called");
    size_t paramsCount = ARGV_NUM_3;
    napi_value params[ARGV_NUM_3] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiMifareUltralightTag *objectInfoCb = nullptr;

    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    NAPI_ASSERT(env, MatchWriteSinglePagesParameters(env, params, paramsCount), "WriteSinglePages type mismatch");
    auto context = std::make_unique<MifareUltralightContext<int, NapiMifareUltralightTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "MifareUltralightContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return CreateUndefined(env);
    }
    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->pageIndex);
    DebugLog("WriteSinglePages blockIndex = %{public}d", context->pageIndex);

    std::vector<unsigned char> dataVec;
    ParseBytesVector(env, dataVec, params[ARGV_INDEX_1]);
    context->data = NfcSdkCommon::BytesVecToHexString(static_cast<unsigned char *>(dataVec.data()), dataVec.size());
    DebugLog("WriteSinglePages data = %{public}s", context->data.c_str());

    if (paramsCount == ARGV_NUM_3) {
        napi_create_reference(env, params[ARGV_INDEX_2], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result =
        HandleAsyncWork(env, context, "WriteSinglePages", NativeWriteSinglePages, WriteSinglePagesCallback);
    return result;
}

napi_value NapiMifareUltralightTag::GetType(napi_env env, napi_callback_info info)
{
    DebugLog("MifareUl GetType called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiMifareUltralightTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    // transfer
    napi_value result = nullptr;
    MifareUltralightTag *nfcMifareUlTagPtr =
        static_cast<MifareUltralightTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcMifareUlTagPtr == nullptr) {
        ErrorLog("GetType find objectInfo failed!");
        napi_create_int32(env, static_cast<int>(MifareUltralightTag::EmMifareUltralightType::TYPE_UNKOWN), &result);
    } else {
        MifareUltralightTag::EmMifareUltralightType mifareUlType = nfcMifareUlTagPtr->GetType();
        DebugLog("GetType mifareUlType %{public}d", mifareUlType);
        napi_create_int32(env, static_cast<int>(mifareUlType), &result);
    }
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
