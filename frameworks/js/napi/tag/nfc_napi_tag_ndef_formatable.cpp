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
    bool isTypeMatched = false;
    switch (parameterCount) {
        case ARGV_NUM_1: {
            isTypeMatched = MatchParameters(env, parameters, {napi_object});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                    BuildErrorMessage(BUSI_ERR_PARAM, "", "", "message", "NdefMessage")));
            }
            break;
        }
        case ARGV_NUM_2:
            isTypeMatched = MatchParameters(env, parameters, {napi_object, napi_function});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                    BuildErrorMessage(BUSI_ERR_PARAM, "", "", "message & callback", "NdefMessage & function")));
            }
            break;
        default: {
            napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
            return false;
        }
    }
    return isTypeMatched;
}

static void NativeFormat(napi_env env, void *data)
{
    auto context = static_cast<NdefFormatableContext<int, NapiNdefFormatableTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;

    NdefFormatableTag *ndefFormatableTagPtr =
        static_cast<NdefFormatableTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (ndefFormatableTagPtr != nullptr) {
        context->errorCode = ndefFormatableTagPtr->Format(context->msg);
    } else {
        ErrorLog("NativeFormat, ndefFormatableTagPtr failed.");
    }
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
        std::string errMessage = BuildErrorMessage(context->errorCode, "format", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, context->errorCode, errMessage);
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
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    if (!MatchFormatParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<NdefFormatableContext<int, NapiNdefFormatableTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), ERR_INIT_CONTEXT.c_str()));
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
    bool isTypeMatched = false;
    switch (parameterCount) {
        case ARGV_NUM_1: {
            isTypeMatched = MatchParameters(env, parameters, {napi_object});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                    BuildErrorMessage(BUSI_ERR_PARAM, "", "", "message", "NdefMessage")));
            }
            break;
        }
        case ARGV_NUM_2:
            isTypeMatched = MatchParameters(env, parameters, {napi_object, napi_function});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                    BuildErrorMessage(BUSI_ERR_PARAM, "", "", "message & callback", "NdefMessage & function")));
            }
            break;
        default: {
            napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
            return false;
        }
    }
    return isTypeMatched;
}

static void NativeFormatReadOnly(napi_env env, void *data)
{
    auto context = static_cast<NdefFormatableContext<int, NapiNdefFormatableTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    NdefFormatableTag *ndefFormatableTagPtr =
        static_cast<NdefFormatableTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (ndefFormatableTagPtr != nullptr) {
        context->errorCode = ndefFormatableTagPtr->FormatReadOnly(context->msg);
    } else {
        ErrorLog("NativeFormatReadOnly, ndefFormatableTagPtr failed.");
    }
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
        std::string errMessage = BuildErrorMessage(context->errorCode, "formatReadOnly", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, context->errorCode, errMessage);
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
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    if (!MatchFormatReadOnlyParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<NdefFormatableContext<int, NapiNdefFormatableTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), ERR_INIT_CONTEXT.c_str()));
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
