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

#include "nfc_napi_tag_ndef_formatable.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
static const int32_t DEFAULT_REF_COUNT = 1;

static bool MatchFormatParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
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

static void NativeFormat(napi_env env, void *data)
{
    DebugLog("NativeFormat called");
    auto context = static_cast<NdefFormatableContext<int, NapiNdefFormatableTag> *>(data);

    NdefFormatableTag *ndefFormatableTagPtr =
        static_cast<NdefFormatableTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (ndefFormatableTagPtr == nullptr) {
        ErrorLog("NativeFormat find objectInfo failed!");
        context->value = true;
    } else {
        context->value = ndefFormatableTagPtr->Format(context->msg);
        DebugLog("Format %{public}d", context->value);
    }
    context->resolved = true;
}

static void FormatCallback(napi_env env, napi_status status, void *data)
{
    DebugLog("FormatCallback called");
    auto context = static_cast<NdefFormatableContext<int, NapiNdefFormatableTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_int32(env, context->value, &callbackValue);
        } else {
            callbackValue = CreateErrorMessage(env, "Format error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "Format error,napi_status = " + std ::to_string(status));
    }
    Handle2ValueCallback(env, context, callbackValue);
}

napi_value NapiNdefFormatableTag::Format(napi_env env, napi_callback_info info)
{
    DebugLog("GetNdefFormatableTag Format called");
    size_t paramsCount = ARGV_NUM_2;
    napi_value params[ARGV_NUM_2] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiNdefFormatableTag *objectInfoCb = nullptr;

    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    NAPI_ASSERT(env, MatchFormatParameters(env, params, paramsCount), "Format type mismatch");
    auto context = std::make_unique<NdefFormatableContext<int, NapiNdefFormatableTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "error at CallBackContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }

    // parse the params
    napi_status status1 = napi_unwrap(env, params[ARGV_INDEX_0], reinterpret_cast<void **>(&context->msg));
    NAPI_ASSERT(env, status1 == napi_ok, "failed to get ndefMessage");

    if (paramsCount == ARGV_NUM_2) {
        napi_create_reference(env, params[ARGV_INDEX_1], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "Format", NativeFormat, FormatCallback);
    return result;
}

static bool MatchFormatReadOnlyParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
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

static void NativeFormatReadOnly(napi_env env, void *data)
{
    DebugLog("NativeFormatReadOnly called");
    auto context = static_cast<NdefFormatableContext<int, NapiNdefFormatableTag> *>(data);
    NdefFormatableTag *ndefFormatableTagPtr =
        static_cast<NdefFormatableTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (ndefFormatableTagPtr == nullptr) {
        ErrorLog("NativeFormatReadOnly find objectInfo failed!");
        context->value = true;
    } else {
        context->value = ndefFormatableTagPtr->FormatReadOnly(context->msg);
        DebugLog("FormatReadOnly %{public}d", context->value);
    }
    context->resolved = true;
}

static void FormatReadOnlyCallback(napi_env env, napi_status status, void *data)
{
    DebugLog("FormatReadOnlyCallback called");
    auto context = static_cast<NdefFormatableContext<int, NapiNdefFormatableTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_int32(env, context->value, &callbackValue);
        } else {
            callbackValue = CreateErrorMessage(env, "FormatReadOnly error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "FormatReadOnly error,napi_status = " + std ::to_string(status));
    }
    Handle2ValueCallback(env, context, callbackValue);
}

napi_value NapiNdefFormatableTag::FormatReadOnly(napi_env env, napi_callback_info info)
{
    DebugLog("GetNdefFormatableTag FormatReadOnly called");
    size_t paramsCount = ARGV_NUM_2;
    napi_value params[ARGV_NUM_2] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiNdefFormatableTag *objectInfoCb = nullptr;

    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    NAPI_ASSERT(env, MatchFormatReadOnlyParameters(env, params, paramsCount), "FormatReadOnly type mismatch");
    auto context = std::make_unique<NdefFormatableContext<int, NapiNdefFormatableTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "error at CallBackContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }

    // parse the params
    napi_status status1 = napi_unwrap(env, params[ARGV_INDEX_0], reinterpret_cast<void **>(&context->msg));
    NAPI_ASSERT(env, status1 == napi_ok, "failed to get ndefMessage");

    if (paramsCount == ARGV_NUM_2) {
        napi_create_reference(env, params[ARGV_INDEX_1], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "FormatReadOnly", NativeFormatReadOnly, FormatReadOnlyCallback);
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
