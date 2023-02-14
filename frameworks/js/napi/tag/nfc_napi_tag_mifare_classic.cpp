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
#include "nfc_napi_tag_mifare_classic.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace KITS {
static const int32_t DEFAULT_REF_COUNT = 1;

static bool CheckTagSessionAndThrow(const napi_env &env, MifareClassicTag *tagSession)
{
    if (tagSession == nullptr) {
        // object null is unexpected, unknown error.
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_TAG_STATE_INVALID,
            BuildErrorMessage(BUSI_ERR_TAG_STATE_INVALID, "", "", "", "")));
        return false;
    }
    return true;
}

napi_value NapiMifareClassicTag::GetSectorCount(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiMifareClassicTag *objectInfo = nullptr;
    napi_value result = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("GetSectorCount, napi_unwrap failed, object is null.");
        napi_create_int32(env, 0, &result);
        return result;
    }

    int sectorCount = 0;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("GetSectorCount, find objectInfo failed!");
    } else {
        sectorCount = nfcMifareClassicTagPtr->GetSectorCount();
    }
    napi_create_int32(env, sectorCount, &result);
    return result;
}

napi_value NapiMifareClassicTag::GetBlockCountInSector(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t expectedArgsCount = ARGV_NUM_1;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGV_NUM_1] = {0};
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));

    // check parameter valid.
    if (!CheckArgCountAndThrow(env, argc, expectedArgsCount) ||
        !CheckNumberAndThrow(env, argv[ARGV_INDEX_0], "sectorIndex", "number")) {
        return CreateUndefined(env);
    }

    NapiMifareClassicTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_PARAM)) {
        return CreateUndefined(env);
    }

    int32_t sectorIndex;
    ParseInt32(env, sectorIndex, argv[ARGV_INDEX_0]);

    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcMifareClassicTagPtr)) {
        return CreateUndefined(env);
    }
    int blockCountInSector = nfcMifareClassicTagPtr->GetBlockCountInSector(sectorIndex);
    napi_create_int32(env, blockCountInSector, &result);
    return result;
}

napi_value NapiMifareClassicTag::GetType(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiMifareClassicTag *objectInfo = nullptr;
    napi_value result = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("GetType, napi_unwrap failed, object is null.");
        napi_create_int32(env, static_cast<int>(MifareClassicTag::EmType::TYPE_UNKNOWN), &result);
        return result;
    }

    MifareClassicTag::EmType mifareType = MifareClassicTag::EmType::TYPE_UNKNOWN;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("GetType, find objectInfo failed!");
    } else {
        mifareType = nfcMifareClassicTagPtr->GetMifareTagType();
    }
    napi_create_int32(env, static_cast<int>(mifareType), &result);
    return result;
}

napi_value NapiMifareClassicTag::GetTagSize(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiMifareClassicTag *objectInfo = nullptr;
    napi_value result = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("GetTagSize, napi_unwrap failed, object is null.");
        napi_create_int32(env, 0, &result);
        return result;
    }

    int tagSize = 0;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("GetTagSize, find objectInfo failed!");
    } else {
        tagSize = nfcMifareClassicTagPtr->GetSize();
    }
    napi_create_int32(env, tagSize, &result);
    return result;
}

napi_value NapiMifareClassicTag::IsEmulatedTag(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiMifareClassicTag *objectInfo = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("IsEmulatedTag, napi_unwrap failed, object is null.");
        napi_get_boolean(env, false, &result);
        return result;
    }

    bool isEmulated = false;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("IsEmulatedTag, find objectInfo failed!");
    } else {
        isEmulated = nfcMifareClassicTagPtr->IsEmulated();
    }
    napi_get_boolean(env, isEmulated, &result);
    return result;
}

napi_value NapiMifareClassicTag::GetBlockIndex(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t expectedArgsCount = ARGV_NUM_1;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGV_NUM_1] = {0};
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));

    // check parameter valid.
    if (!CheckArgCountAndThrow(env, argc, expectedArgsCount) ||
        !CheckNumberAndThrow(env, argv[ARGV_INDEX_0], "sectorIndex", "number")) {
        return CreateUndefined(env);
    }

    // unwrap from thisVar to retrieve the native instance
    NapiMifareClassicTag *objectInfo = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_PARAM)) {
        return CreateUndefined(env);
    }

    int32_t sectorIndex;
    ParseInt32(env, sectorIndex, argv[ARGV_INDEX_0]);
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcMifareClassicTagPtr)) {
        return CreateUndefined(env);
    }
    int blockIndex = nfcMifareClassicTagPtr->GetBlockIndexFromSector(sectorIndex);
    napi_create_int32(env, blockIndex, &result);
    return result;
}

napi_value NapiMifareClassicTag::GetSectorIndex(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t expectedArgsCount = ARGV_NUM_1;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGV_NUM_1] = {0};
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));

    // check parameter valid.
    if (!CheckArgCountAndThrow(env, argc, expectedArgsCount) ||
        !CheckNumberAndThrow(env, argv[ARGV_INDEX_0], "sectorIndex", "number")) {
        return CreateUndefined(env);
    }

    // unwrap from thisVar to retrieve the native instance
    NapiMifareClassicTag *objectInfo = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_PARAM)) {
        return CreateUndefined(env);
    }

    int32_t blockIndex;
    ParseInt32(env, blockIndex, argv[ARGV_INDEX_0]);
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcMifareClassicTagPtr)) {
        return CreateUndefined(env);
    }
    int sectorIndex = nfcMifareClassicTagPtr->GetSectorIndexFromBlock(blockIndex);
    napi_create_int32(env, sectorIndex, &result);
    return result;
}

static bool CheckAuthenticateSectorParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount == ARGV_NUM_3) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number, napi_object, napi_boolean},
            "sectorIndex & key & isKeyA", "number & number[] & boolean") ||
            !CheckArrayNumberAndThrow(env, parameters[ARGV_INDEX_1], "key", "number[]")) {
            return false;
        }
        return true;
    } else if (parameterCount == ARGV_NUM_4) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number, napi_object, napi_boolean, napi_function},
            "sectorIndex & key & isKeyA & callback", "number & number[] & boolean & function") ||
            !CheckArrayNumberAndThrow(env, parameters[ARGV_INDEX_1], "key", "number[]")) {
                return false;
            }
        return true;
    } else {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return false;
    }
}

static void NativeAuthenticateSector(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<bool, NapiMifareClassicTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcMifareClassicTagPtr)) {
        return;
    }
    context->errorCode = nfcMifareClassicTagPtr->AuthenticateSector(context->sectorIndex,
        context->key, context->bIsKeyA);
    context->resolved = true;
}

static void AuthenticateSectorCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MifareClassicContext<bool, NapiMifareClassicTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is void.
        napi_get_undefined(env, &callbackValue);
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "authenticateSector", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiMifareClassicTag::AuthenticateSector(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_4;
    napi_value params[ARGV_NUM_4] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiMifareClassicTag *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckAuthenticateSectorParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareClassicContext<bool, NapiMifareClassicTag>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->sectorIndex);
    std::vector<unsigned char> keyVec;
    ParseBytesVector(env, keyVec, params[ARGV_INDEX_1]);
    context->key = NfcSdkCommon::BytesVecToHexString(static_cast<unsigned char *>(keyVec.data()), keyVec.size());
    napi_get_value_bool(env, params[ARGV_INDEX_2], &context->bIsKeyA);
    if (paramsCount == ARGV_NUM_4) {
        napi_create_reference(env, params[ARGV_INDEX_3], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result =
        HandleAsyncWork(env, context, "AuthenticateSector", NativeAuthenticateSector, AuthenticateSectorCallback);
    return result;
}

static bool CheckReadSingleBlockParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount == ARGV_NUM_1) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number}, "blockIndex", "number")) {
            return false;
        }
        return true;
    } else if (parameterCount == ARGV_NUM_2) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number, napi_function},
            "blockIndex & callback", "number & function")) {
            return false;
        }
        return true;
    } else {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return false;
    }
}

static void NativeReadSingleBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<std::string, NapiMifareClassicTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;

    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcMifareClassicTagPtr)) {
        return;
    }
    std::string hexRespData = "";
    context->errorCode = nfcMifareClassicTagPtr->ReadSingleBlock(context->blockIndex, hexRespData);
    context->value = hexRespData;
    context->resolved = true;
}

static void ReadSingleBlockCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MifareClassicContext<std::string, NapiMifareClassicTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is number[].
        ConvertStringToNumberArray(env, callbackValue, context->value);
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "readSingleBlock", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiMifareClassicTag::ReadSingleBlock(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_2;
    napi_value params[ARGV_NUM_2] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiMifareClassicTag *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckReadSingleBlockParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareClassicContext<std::string, NapiMifareClassicTag>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->blockIndex);
    if (paramsCount == ARGV_NUM_2) {
        napi_create_reference(env, params[ARGV_INDEX_1], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result =
        HandleAsyncWork(env, context, "ReadSingleBlock", NativeReadSingleBlock, ReadSingleBlockCallback);
    return result;
}

static bool CheckWriteSingleBlockParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount == ARGV_NUM_2) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number, napi_object},
            "blockIndex & data", "number & number[]") ||
            !CheckArrayNumberAndThrow(env, parameters[ARGV_NUM_1], "data", "number[]")) {
            return false;
        }
        return true;
    } else if (parameterCount == ARGV_NUM_3) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number, napi_object, napi_function},
            "blockIndex & data & callback", "number & number[] & function") ||
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

static void NativeWriteSingleBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcMifareClassicTagPtr)) {
        return;
    }
    context->errorCode = nfcMifareClassicTagPtr->WriteSingleBlock(context->blockIndex, context->data);
    context->resolved = true;
}

static void WriteSingleBlockCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is void.
        napi_get_undefined(env, &callbackValue);
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "writeSingleBlock", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiMifareClassicTag::WriteSingleBlock(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_3;
    napi_value params[ARGV_NUM_3] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiMifareClassicTag *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckWriteSingleBlockParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareClassicContext<int, NapiMifareClassicTag>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->blockIndex);
    std::vector<unsigned char> dataVec;
    ParseBytesVector(env, dataVec, params[ARGV_INDEX_1]);
    context->data = NfcSdkCommon::BytesVecToHexString(static_cast<unsigned char *>(dataVec.data()), dataVec.size());
    if (paramsCount == ARGV_NUM_3) {
        napi_create_reference(env, params[ARGV_INDEX_2], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result =
        HandleAsyncWork(env, context, "WriteSingleBlock", NativeWriteSingleBlock, WriteSingleBlockCallback);
    return result;
}

static bool CheckIncrementBlockParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount == ARGV_NUM_2) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number, napi_number},
            "blockIndex & value", "number & number")) {
            return false;
        }
        return true;
    } else if (parameterCount == ARGV_NUM_3) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number, napi_number, napi_function},
            "blockIndex & value & callback", "number & number & function")) {
            return false;
        }
        return false;
    } else {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return false;
    }
}

static void NativeIncrementBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcMifareClassicTagPtr)) {
        return;
    }
    context->errorCode = nfcMifareClassicTagPtr->IncrementBlock(context->blockIndex, context->incrementValue);
    context->resolved = true;
}

static void IncrementBlockCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is void.
        napi_get_undefined(env, &callbackValue);
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "incrementBlock", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiMifareClassicTag::IncrementBlock(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_3;
    napi_value params[ARGV_NUM_3] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiMifareClassicTag *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckIncrementBlockParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareClassicContext<int, NapiMifareClassicTag>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->blockIndex);
    napi_get_value_int32(env, params[ARGV_INDEX_1], &context->incrementValue);
    if (paramsCount == ARGV_NUM_3) {
        napi_create_reference(env, params[ARGV_INDEX_2], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "IncrementBlock", NativeIncrementBlock, IncrementBlockCallback);
    return result;
}

static bool CheckDecrementBlockParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount == ARGV_NUM_2) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number, napi_number}, "blockIndex & value",
            "number & number")) {
            return false;
        }
        return true;
    } else if (parameterCount == ARGV_NUM_3) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number, napi_number, napi_function},
            "blockIndex & value & callback", "number & number & function")) {
            return false;
        }
        return true;
    } else {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return false;
    }
}

static void NativeDecrementBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcMifareClassicTagPtr)) {
        return;
    }
    context->errorCode = nfcMifareClassicTagPtr->DecrementBlock(context->blockIndex, context->decrementValue);
    context->resolved = true;
}

static void DecrementBlockCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is void.
        napi_get_undefined(env, &callbackValue);
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "decrementBlock", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiMifareClassicTag::DecrementBlock(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_3;
    napi_value params[ARGV_NUM_3] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiMifareClassicTag *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckDecrementBlockParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareClassicContext<int, NapiMifareClassicTag>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->blockIndex);
    napi_get_value_int32(env, params[ARGV_INDEX_1], &context->decrementValue);
    if (paramsCount == ARGV_NUM_3) {
        napi_create_reference(env, params[ARGV_INDEX_2], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "DecrementBlock", NativeDecrementBlock, DecrementBlockCallback);
    return result;
}

static bool CheckTransferToBlockParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount == ARGV_NUM_1) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number}, "blockIndex", "number")) {
            return false;
        }
        return true;
    } else if (parameterCount == ARGV_NUM_2) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number, napi_function},
            "blockIndex & callback", "number & function")) {
            return false;
        }
        return true;
    } else {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return false;
    }
}

static void NativeTransferToBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcMifareClassicTagPtr)) {
        return;
    }
    context->errorCode = nfcMifareClassicTagPtr->TransferToBlock(context->blockIndex);
    context->resolved = true;
}

static void TransferToBlockCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is void.
        napi_get_undefined(env, &callbackValue);
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "transferToBlock", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiMifareClassicTag::TransferToBlock(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_2;
    napi_value params[ARGV_NUM_2] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiMifareClassicTag *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckTransferToBlockParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareClassicContext<int, NapiMifareClassicTag>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->blockIndex);
    if (paramsCount == ARGV_NUM_2) {
        napi_create_reference(env, params[ARGV_INDEX_1], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result =
        HandleAsyncWork(env, context, "TransferToBlock", NativeTransferToBlock, TransferToBlockCallback);
    return result;
}

static bool CheckRestoreFromBlockParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount == ARGV_NUM_1) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number}, "blockIndex", "number")) {
            return false;
        }
        return true;
    } else if (parameterCount == ARGV_NUM_2) {
        if (!CheckParametersAndThrow(env, parameters, {napi_number, napi_function},
            "blockIndex & callback", "number & function")) {
            return false;
        }
        return true;
    } else {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return false;
    }
}

static void NativeRestoreFromBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcMifareClassicTagPtr)) {
        return;
    }
    context->errorCode = nfcMifareClassicTagPtr->RestoreFromBlock(context->blockIndex);
    context->resolved = true;
}

static void RestoreFromBlockCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is void.
        napi_get_undefined(env, &callbackValue);
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "restoreFromBlock", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiMifareClassicTag::RestoreFromBlock(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_2;
    napi_value params[ARGV_NUM_2] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiMifareClassicTag *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckRestoreFromBlockParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareClassicContext<int, NapiMifareClassicTag>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->blockIndex);
    if (paramsCount == ARGV_NUM_2) {
        napi_create_reference(env, params[ARGV_INDEX_1], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result =
        HandleAsyncWork(env, context, "RestoreFromBlock", NativeRestoreFromBlock, RestoreFromBlockCallback);
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
