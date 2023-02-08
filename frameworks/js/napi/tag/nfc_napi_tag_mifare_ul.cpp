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

static bool CheckTagSessionAndThrow(const napi_env &env, const MifareUltralightTag *tagSession)
{
    if (tagSession == nullptr) {
        // object null is unexpected, unknown error.
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_TAG_STATE_INVALID,
            BuildErrorMessage(BUSI_ERR_TAG_STATE_INVALID, "", "", "", "")));
        return false;
    }
    return true;
}

static bool CheckReadMultiplePagesParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount == ARGV_NUM_1) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number}, "pageIndex", "number")) {
            return false;
        }
        return true;
    } else if (parameterCount == ARGV_NUM_2) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number, napi_function},
            "pageIndex & callback", "number & function")) {
            return false;
        }
        return true;
    } else {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return false;
    }
}

static void NativeReadMultiplePages(napi_env env, void *data)
{
    auto context = static_cast<MifareUltralightContext<std::string, NapiMifareUltralightTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    MifareUltralightTag *nfcMifareUlTagPtr =
        static_cast<MifareUltralightTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcMifareUlTagPtr)) {
        return;
    }
    std::string hexRespData;
    context->errorCode = nfcMifareUlTagPtr->ReadMultiplePages(context->pageIndex, hexRespData);
    context->value = hexRespData;
    context->resolved = true;
}

static void ReadMultiplePagesCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MifareUltralightContext<std::string, NapiMifareUltralightTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is number[].
        ConvertStringToNumberArray(env, callbackValue, context->value);
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "readMultiplePages", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiMifareUltralightTag::ReadMultiplePages(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_2;
    napi_value params[ARGV_NUM_2] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiMifareUltralightTag *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckReadMultiplePagesParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareUltralightContext<std::string, NapiMifareUltralightTag>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->pageIndex);
    if (paramsCount == ARGV_NUM_2) {
        napi_create_reference(env, params[1], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result =
        HandleAsyncWork(env, context, "ReadMultiplePages", NativeReadMultiplePages, ReadMultiplePagesCallback);
    return result;
}

static bool CheckWriteSinglePagesParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount == ARGV_NUM_2) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number, napi_object}, "pageIndex & data",
            "number & number[]") ||
            !CheckArrayNumberAndThrow(env, parameters[ARGV_NUM_1], "data", "number[]")) {
            return false;
        }
        return true;
    } else if (parameterCount == ARGV_NUM_3) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number, napi_object, napi_function},
            "pageIndex & data & callback", "number & number[] & function") ||
            !CheckArrayNumberAndThrow(env, parameters[ARGV_NUM_1], "data", "number[]")) {
            return false;
        }
        return true;
    } else {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
            return false;
    }
}

static void NativeWriteSinglePages(napi_env env, void *data)
{
    auto context = static_cast<MifareUltralightContext<int, NapiMifareUltralightTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    MifareUltralightTag *nfcMifareUlTagPtr =
        static_cast<MifareUltralightTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcMifareUlTagPtr)) {
        return;
    }
    context->errorCode = nfcMifareUlTagPtr->WriteSinglePage(context->pageIndex, context->data);
    context->resolved = true;
}

static void WriteSinglePagesCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MifareUltralightContext<int, NapiMifareUltralightTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is void.
        napi_get_undefined(env, &callbackValue);
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "writeSinglePage", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiMifareUltralightTag::WriteSinglePage(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_3;
    napi_value params[ARGV_NUM_3] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiMifareUltralightTag *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckWriteSinglePagesParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareUltralightContext<int, NapiMifareUltralightTag>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->pageIndex);
    std::vector<unsigned char> dataVec;
    ParseBytesVector(env, dataVec, params[ARGV_INDEX_1]);
    context->data = NfcSdkCommon::BytesVecToHexString(static_cast<unsigned char *>(dataVec.data()), dataVec.size());
    if (paramsCount == ARGV_NUM_3) {
        napi_create_reference(env, params[ARGV_INDEX_2], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result =
        HandleAsyncWork(env, context, "WriteSinglePage", NativeWriteSinglePages, WriteSinglePagesCallback);
    return result;
}

napi_value NapiMifareUltralightTag::GetType(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiMifareUltralightTag *objectInfo = nullptr;
    napi_value result = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("GetType, napi_unwrap failed, object is null.");
        napi_create_int32(env, static_cast<int>(MifareUltralightTag::EmType::TYPE_UNKNOWN), &result);
        return result;
    }

    MifareUltralightTag *nfcMifareUlTagPtr =
        static_cast<MifareUltralightTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    MifareUltralightTag::EmType mifareUlType = MifareUltralightTag::EmType::TYPE_UNKNOWN;
    if (nfcMifareUlTagPtr != nullptr) {
        mifareUlType = nfcMifareUlTagPtr->GetType();
    }
    napi_create_int32(env, static_cast<int>(mifareUlType), &result);
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
