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

static bool CheckTagSessionAndThrow(const napi_env &env, const NdefFormatableTag *tagSession)
{
    if (tagSession == nullptr) {
        // object null is unexpected, unknown error.
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_TAG_STATE_INVALID,
            BuildErrorMessage(BUSI_ERR_TAG_STATE_INVALID, "", "", "", "")));
        return false;
    }
    return true;
}

static bool CheckFormatParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount == ARGV_NUM_1) {
        if (!CheckParametersAndThrow(env, parameters, {napi_object}, "message", "NdefMessage")) {
            return false;
        }
        return true;
    } else if (parameterCount == ARGV_NUM_2) {
        if (!CheckParametersAndThrow(env, parameters, {napi_object, napi_function},
            "message & callback", "NdefMessage & function")) {
            return false;
        }
        return true;
    } else {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return false;
    }
}

static void NativeFormat(napi_env env, void *data)
{
    auto context = static_cast<NdefFormatableContext<int, NapiNdefFormatableTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;

    NdefFormatableTag *ndefFormatableTagPtr =
        static_cast<NdefFormatableTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, ndefFormatableTagPtr)) {
        return;
    }
    context->errorCode = ndefFormatableTagPtr->Format(context->msg);
    context->resolved = true;
}

static void FormatCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<NdefFormatableContext<int, NapiNdefFormatableTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is void.
        napi_get_undefined(env, &callbackValue);
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "format", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiNdefFormatableTag::Format(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_2;
    napi_value params[ARGV_NUM_2] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiNdefFormatableTag *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckFormatParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<NdefFormatableContext<int, NapiNdefFormatableTag>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    napi_status status2 = napi_unwrap(env, params[ARGV_INDEX_0], reinterpret_cast<void **>(&context->msg));
    if (!CheckUnwrapStatusAndThrow(env, status2, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }
    if (paramsCount == ARGV_NUM_2) {
        napi_create_reference(env, params[ARGV_INDEX_1], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "Format", NativeFormat, FormatCallback);
    return result;
}

static void NativeFormatReadOnly(napi_env env, void *data)
{
    auto context = static_cast<NdefFormatableContext<int, NapiNdefFormatableTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    NdefFormatableTag *ndefFormatableTagPtr =
        static_cast<NdefFormatableTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, ndefFormatableTagPtr)) {
        return;
    }
    context->errorCode = ndefFormatableTagPtr->FormatReadOnly(context->msg);
    context->resolved = true;
}

static void FormatReadOnlyCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<NdefFormatableContext<int, NapiNdefFormatableTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is void.
        napi_get_undefined(env, &callbackValue);
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "formatReadOnly", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiNdefFormatableTag::FormatReadOnly(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_2;
    napi_value params[ARGV_NUM_2] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiNdefFormatableTag *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckFormatParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<NdefFormatableContext<int, NapiNdefFormatableTag>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    napi_status status2 = napi_unwrap(env, params[ARGV_INDEX_0], reinterpret_cast<void **>(&context->msg));
    if (!CheckUnwrapStatusAndThrow(env, status2, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }
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
