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

napi_value NapiMifareClassicTag::GetSectorCount(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiMifareClassicTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    // transfer
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    napi_value result = nullptr;
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("GetSectorCount find objectInfo failed!");
        napi_create_int32(env, 0, &result);
    } else {
        int sectorCount = nfcMifareClassicTagPtr->GetSectorCount();
        napi_create_int32(env, sectorCount, &result);
    }
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

    // check parameter number
    if (argc != expectedArgsCount) {
        ErrorLog("NapiMifareClassicTag::GetBlockCountInSector, Requires 1 argument.");
        napi_create_int32(env, 0, &result);
        return result;
    }
    // check parameter data type
    if (!IsNumber(env, argv[ARGV_INDEX_0])) {
        ErrorLog("NapiMifareClassicTag::GetBlockCountInSector, Invalid data type!");
        napi_create_int32(env, 0, &result);
        return result;
    }

    NapiMifareClassicTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    int32_t sectorIndex;
    ParseInt32(env, sectorIndex, argv[ARGV_INDEX_0]);

    // transfer
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("GetBlockCountInSector find objectInfo failed!");
        napi_create_int32(env, 0, &result);
        return result;
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
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    // transfer
    napi_value result = nullptr;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("GetType find objectInfo failed!");
        napi_create_int32(env, static_cast<int>(MifareClassicTag::EmType::TYPE_UNKNOWN), &result);
    } else {
        MifareClassicTag::EmType mifareType = nfcMifareClassicTagPtr->GetMifareTagType();
        napi_create_int32(env, static_cast<int>(mifareType), &result);
    }
    return result;
}

napi_value NapiMifareClassicTag::GetTagSize(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiMifareClassicTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    // transfer
    napi_value result = nullptr;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("GetTagSize find objectInfo failed!");
        napi_create_int32(env, 0, &result);
        return result;
    }

    int tagSize = nfcMifareClassicTagPtr->GetSize();
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
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    // transfer
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("IsEmulatedTag find objectInfo failed!");
        napi_get_boolean(env, false, &result);
        return result;
    }

    bool isEmulated = nfcMifareClassicTagPtr->IsEmulated();
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

    // check parameter number
    if (argc != expectedArgsCount) {
        ErrorLog("NapiMifareClassicTag::GetBlockIndex, Requires 1 argument.");
        napi_create_int32(env, 0, &result);
        return result;
    }
    // check parameter data type
    if (!IsNumber(env, argv[ARGV_INDEX_0])) {
        ErrorLog("NapiMifareClassicTag::GetBlockIndex, Invalid data type!");
        napi_create_int32(env, 0, &result);
        return result;
    }

    NapiMifareClassicTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    int32_t sectorIndex;
    ParseInt32(env, sectorIndex, argv[ARGV_INDEX_0]);

    // transfer
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("GetBlockIndex find objectInfo failed!");
        napi_create_int32(env, 0, &result);
        return result;
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

    // check parameter number
    if (argc != expectedArgsCount) {
        ErrorLog("NapiMifareClassicTag::GetSectorIndex, Requires 1 argument.");
        napi_create_int32(env, 0, &result);
        return result;
    }
    // check parameter data type
    if (!IsNumber(env, argv[ARGV_INDEX_0])) {
        ErrorLog("NapiMifareClassicTag::GetSectorIndex, Invalid data type!");
        napi_create_int32(env, 0, &result);
        return result;
    }

    NapiMifareClassicTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    int32_t blockIndex;
    ParseInt32(env, blockIndex, argv[ARGV_INDEX_0]);

    // transfer
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("GetSectorIndex find objectInfo failed!");
        napi_create_int32(env, 0, &result);
        return result;
    }
    int sectorIndex = nfcMifareClassicTagPtr->GetSectorIndexFromBlock(blockIndex);
    napi_create_int32(env, sectorIndex, &result);
    return result;
}

static bool MatchAuthenticateSectorParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool isTypeMatched = false;
    switch (parameterCount) {
        case ARGV_NUM_3: {
            isTypeMatched = MatchParameters(env, parameters, {napi_number, napi_object, napi_boolean});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM, BuildErrorMessage(BUSI_ERR_PARAM,
                    "", "", "sectorIndex & key & isKeyA", "number & number[] & boolean")));
            }
            break;
        }
        case ARGV_NUM_4:
            isTypeMatched = MatchParameters(env, parameters, {napi_number, napi_object, napi_boolean, napi_function});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM, BuildErrorMessage(BUSI_ERR_PARAM,
                    "", "", "sectorIndex & key & isKeyA & callback", "number & number[] & boolean & function")));
            }
            break;
        default: {
            napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
            return false;
        }
    }
    if (isTypeMatched) {
        // authenticateSector(sectorIndex: number, key: number[], isKeyA: boolean)
        isTypeMatched = IsNumberArray(env, parameters[ARGV_INDEX_1]);
        if (!isTypeMatched) {
            napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM, BuildErrorMessage(BUSI_ERR_PARAM,
                "", "", "key", "number[]")));
        }
    }
    return isTypeMatched;
}

static void NativeAuthenticateSector(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<bool, NapiMifareClassicTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr != nullptr) {
        context->errorCode = nfcMifareClassicTagPtr->AuthenticateSector(context->sectorIndex,
            context->key, context->bIsKeyA);
    } else {
        ErrorLog("NativeAuthenticateSector, nfcMifareClassicTagPtr failed.");
    }
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
        std::string errMessage = BuildErrorMessage(context->errorCode, "authenticateSector", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, context->errorCode, errMessage);
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
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    if (!MatchAuthenticateSectorParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareClassicContext<bool, NapiMifareClassicTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), ERR_INIT_CONTEXT.c_str()));
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

static bool MatchReadSingleBlockParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool isTypeMatched = false;
    switch (parameterCount) {
        case ARGV_NUM_1: {
            isTypeMatched = MatchParameters(env, parameters, {napi_number});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                    BuildErrorMessage(BUSI_ERR_PARAM, "", "", "blockIndex", "number")));
            }
            break;
        }
        case ARGV_NUM_2:
            isTypeMatched = MatchParameters(env, parameters, {napi_number, napi_function});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                    BuildErrorMessage(BUSI_ERR_PARAM, "", "", "blockIndex & callback", "number & function")));
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

static void NativeReadSingleBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<std::string, NapiMifareClassicTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;

    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr != nullptr) {
        std::string hexRespData = "";
        context->errorCode = nfcMifareClassicTagPtr->ReadSingleBlock(context->blockIndex, hexRespData);
        context->value = hexRespData;
    } else {
        ErrorLog("NativeReadSingleBlock, nfcMifareClassicTagPtr failed.");
    }
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
        std::string errMessage = BuildErrorMessage(context->errorCode, "readSingleBlock", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, context->errorCode, errMessage);
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
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    if (!MatchReadSingleBlockParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareClassicContext<std::string, NapiMifareClassicTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), ERR_INIT_CONTEXT.c_str()));
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

static bool MatchWriteSingleBlockParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool isTypeMatched = false;
    switch (parameterCount) {
        case ARGV_NUM_2: {
            isTypeMatched = MatchParameters(env, parameters, {napi_number, napi_object});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                    BuildErrorMessage(BUSI_ERR_PARAM, "", "", "blockIndex & data", "number & number[]")));
            }
            break;
        }
        case ARGV_NUM_3:
            isTypeMatched = MatchParameters(env, parameters, {napi_number, napi_object, napi_function});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM, BuildErrorMessage(BUSI_ERR_PARAM,
                    "", "", "blockIndex & data & callback", "number & number[] & function")));
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

static void NativeWriteSingleBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr != nullptr) {
        context->errorCode = nfcMifareClassicTagPtr->WriteSingleBlock(context->blockIndex, context->data);
    } else {
        ErrorLog("NativeWriteSingleBlock, nfcMifareClassicTagPtr failed.");
    }
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
        std::string errMessage = BuildErrorMessage(context->errorCode, "writeSingleBlock", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, context->errorCode, errMessage);
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
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    if (!MatchWriteSingleBlockParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareClassicContext<int, NapiMifareClassicTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), ERR_INIT_CONTEXT.c_str()));
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

static bool MatchIncrementBlockParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool isTypeMatched = false;
    switch (parameterCount) {
        case ARGV_NUM_2: {
            isTypeMatched = MatchParameters(env, parameters, {napi_number, napi_number});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                    BuildErrorMessage(BUSI_ERR_PARAM, "", "", "blockIndex & value", "number & number")));
            }
            break;
        }
        case ARGV_NUM_3:
            isTypeMatched = MatchParameters(env, parameters, {napi_number, napi_number, napi_function});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM, BuildErrorMessage(BUSI_ERR_PARAM,
                    "", "", "blockIndex & value & callback", "number & number & function")));
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

static void NativeIncrementBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr != nullptr) {
        context->errorCode = nfcMifareClassicTagPtr->IncrementBlock(context->blockIndex, context->incrementValue);
    } else {
        ErrorLog("NativeIncrementBlock, nfcMifareClassicTagPtr failed.");
    }
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
        std::string errMessage = BuildErrorMessage(context->errorCode, "incrementBlock", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, context->errorCode, errMessage);
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
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    if(!MatchIncrementBlockParameters(env, params, paramsCount)){
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareClassicContext<int, NapiMifareClassicTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), ERR_INIT_CONTEXT.c_str()));
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

static bool MatchDecrementBlockParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool isTypeMatched = false;
    switch (parameterCount) {
        case ARGV_NUM_2: {
            isTypeMatched = MatchParameters(env, parameters, {napi_number, napi_number});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                    BuildErrorMessage(BUSI_ERR_PARAM, "", "", "blockIndex & value", "number & number")));
            }
            break;
        }
        case ARGV_NUM_3:
            isTypeMatched = MatchParameters(env, parameters, {napi_number, napi_number, napi_function});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM, BuildErrorMessage(BUSI_ERR_PARAM,
                    "", "", "blockIndex & value & callback", "number & number & function")));
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

static void NativeDecrementBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr != nullptr) {
        context->errorCode = nfcMifareClassicTagPtr->DecrementBlock(context->blockIndex, context->decrementValue);
    } else {
        ErrorLog("NativeDecrementBlock, nfcMifareClassicTagPtr failed.");
    }
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
        std::string errMessage = BuildErrorMessage(context->errorCode, "decrementBlock", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, context->errorCode, errMessage);
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
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    if (!MatchDecrementBlockParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareClassicContext<int, NapiMifareClassicTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), ERR_INIT_CONTEXT.c_str()));
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

static bool MatchTransferToBlockParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool isTypeMatched = false;
    switch (parameterCount) {
        case ARGV_NUM_1: {
            isTypeMatched = MatchParameters(env, parameters, {napi_number});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                    BuildErrorMessage(BUSI_ERR_PARAM, "", "", "blockIndex", "number")));
            }
            break;
        }
        case ARGV_NUM_2:
            isTypeMatched = MatchParameters(env, parameters, {napi_number, napi_function});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                    BuildErrorMessage(BUSI_ERR_PARAM, "", "", "blockIndex & callback", "number & function")));
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

static void NativeTransferToBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr != nullptr) {
        context->errorCode = nfcMifareClassicTagPtr->TransferToBlock(context->blockIndex);
    } else {
        ErrorLog("NativeTransferToBlock, nfcMifareClassicTagPtr failed.");
    }
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
        std::string errMessage = BuildErrorMessage(context->errorCode, "transferToBlock", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, context->errorCode, errMessage);
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
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    if (!MatchTransferToBlockParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareClassicContext<int, NapiMifareClassicTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), ERR_INIT_CONTEXT.c_str()));
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

static bool MatchRestoreFromBlockParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool isTypeMatched = false;
    switch (parameterCount) {
        case ARGV_NUM_1: {
            isTypeMatched = MatchParameters(env, parameters, {napi_number});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                    BuildErrorMessage(BUSI_ERR_PARAM, "", "", "blockIndex", "number")));
            }
            break;
        }
        case ARGV_NUM_2:
            isTypeMatched = MatchParameters(env, parameters, {napi_number, napi_function});
            if (!isTypeMatched) {
                napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                    BuildErrorMessage(BUSI_ERR_PARAM, "", "", "blockIndex & callback", "number & function")));
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

static void NativeRestoreFromBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr != nullptr) {
        context->errorCode = nfcMifareClassicTagPtr->RestoreFromBlock(context->blockIndex);
    } else {
        ErrorLog("NativeRestoreFromBlock, nfcMifareClassicTagPtr failed.");
    }
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
        std::string errMessage = BuildErrorMessage(context->errorCode, "restoreFromBlock", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, context->errorCode, errMessage);
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
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    if (!MatchRestoreFromBlockParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<MifareClassicContext<int, NapiMifareClassicTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), ERR_INIT_CONTEXT.c_str()));
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
