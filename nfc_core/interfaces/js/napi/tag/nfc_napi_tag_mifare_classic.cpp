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

namespace OHOS {
namespace NFC {
namespace KITS {
static const int32_t DEFAULT_REF_COUNT = 1;

napi_value NapiMifareClassicTag::GetSectorCount(napi_env env, napi_callback_info info)
{
    DebugLog("GetSectorCount called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiMifareClassicTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    // transfer
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("GetSectorCount find objectInfo failed!");
        return nullptr;
    } else {
        int sectorCount = nfcMifareClassicTagPtr->GetSectorCount();
        DebugLog("sectorCount %{public}d", sectorCount);
        napi_value result = nullptr;
        napi_create_int32(env, sectorCount, &result);
        return result;
    }
}

napi_value NapiMifareClassicTag::GetBlockCountInSector(napi_env env, napi_callback_info info)
{
    DebugLog("GetBlockCountInSector called");
    napi_value thisVar = nullptr;
    size_t expectedArgsCount = ARGV_NUM_1;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGV_NUM_1] = {0};
    napi_value result = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));

    // check parameter number
    if (argc != expectedArgsCount) {
        ErrorLog("NapiMifareClassicTag::GetBlockCountInSector, Requires 1 argument.");
        return result;
    }
    // check parameter data type
    napi_valuetype valueType = napi_undefined;
    if (valueType != napi_number) {
        ErrorLog("NapiMifareClassicTag::GetBlockCountInSector, Invalid data type!");
        return nullptr;
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
        ErrorLog("GetSectorCount find objectInfo failed!");
        return nullptr;
    }
    int blockCountInSector = nfcMifareClassicTagPtr->GetBlockCountInSector(sectorIndex);
    DebugLog("blockCountInSector %{public}d", blockCountInSector);

    napi_create_int32(env, blockCountInSector, &result);
    return result;
}

napi_value NapiMifareClassicTag::GetType(napi_env env, napi_callback_info info)
{
    DebugLog("Mifare Classic GetType called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiMifareClassicTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    // transfer
    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("GetType find objectInfo failed!");
        return nullptr;
    } else {
        // MifareClassicTag::EmMifareTagType mifareType = nfcMifareClassicTagPtr->GetType();
        MifareClassicTag::EmMifareTagType mifareType = MifareClassicTag::TYPE_UNKNOWN;
        DebugLog("sectorCount %{public}d", mifareType);
        napi_value result = nullptr;
        napi_create_int32(env, mifareType, &result);
        return result;
    }
}

napi_value NapiMifareClassicTag::GetTagSize(napi_env env, napi_callback_info info)
{
    DebugLog("GetTagSize called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
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
        ErrorLog("GetSectorCount find objectInfo failed!");
        return result;
    }

    int tagSize = nfcMifareClassicTagPtr->GetSize();
    DebugLog("sectorCount %{public}d", tagSize);
    napi_create_int32(env, tagSize, &result);
    return result;
}

napi_value NapiMifareClassicTag::IsEmulatedTag(napi_env env, napi_callback_info info)
{
    DebugLog("IsEmulatedTag called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
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
        ErrorLog("GetSectorCount find objectInfo failed!");
        return result;
    }

    bool isEmulated = nfcMifareClassicTagPtr->IsEmulated();
    DebugLog("sectorCount %{public}d", isEmulated);
    napi_get_boolean(env, isEmulated, &result);
    return result;
}

napi_value NapiMifareClassicTag::GetBlockIndex(napi_env env, napi_callback_info info)
{
    DebugLog("GetBlockCountInSector called");
    napi_value thisVar = nullptr;
    size_t expectedArgsCount = ARGV_NUM_1;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGV_NUM_1] = {0};
    napi_value result = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));

    // check parameter number
    if (argc != expectedArgsCount) {
        ErrorLog("NapiMifareClassicTag::GetBlockIndex, Requires 1 argument.");
        return result;
    }
    // check parameter data type
    napi_valuetype valueType = napi_undefined;
    if (valueType != napi_number) {
        ErrorLog("NapiMifareClassicTag::GetBlockIndex, Invalid data type!");
        return nullptr;
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
        ErrorLog("GetSectorCount find objectInfo failed!");
        return nullptr;
    }
    int blockIndex = nfcMifareClassicTagPtr->GetBlockIndexFromSector(sectorIndex);
    DebugLog("BlockIndex %{public}d", blockIndex);

    napi_create_int32(env, blockIndex, &result);
    return result;
}

napi_value NapiMifareClassicTag::GetSectorIndex(napi_env env, napi_callback_info info)
{
    DebugLog("GetSectorIndex called");
    napi_value thisVar = nullptr;
    size_t expectedArgsCount = ARGV_NUM_1;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGV_NUM_1] = {0};
    napi_value result = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));

    // check parameter number
    if (argc != expectedArgsCount) {
        ErrorLog("NapiMifareClassicTag::GetSectorIndex, Requires 1 argument.");
        return result;
    }
    // check parameter data type
    napi_valuetype valueType = napi_undefined;
    if (valueType != napi_number) {
        ErrorLog("NapiMifareClassicTag::GetSectorIndex, Invalid data type!");
        return nullptr;
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
        ErrorLog("GetSectorCount find objectInfo failed!");
        return nullptr;
    }
    // int sectorIndex = nfcMifareClassicTagPtr->GetBlockIndexFromSector(blockIndex); // no cpp func
    int sectorIndex = 4;
    DebugLog("sectorIndex%{public}d", blockIndex);

    napi_create_int32(env, sectorIndex, &result);
    return result;
}

static bool MatchAuthenticateSectorParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool typeMatch = false;
    switch (parameterCount) {
        case ARGV_NUM_3: {
            typeMatch = MatchParameters(env, parameters, {napi_number, napi_object, napi_boolean});
            break;
        }
        case ARGV_NUM_4:
            typeMatch = MatchParameters(env, parameters, {napi_number, napi_object, napi_boolean, napi_function});
            break;
        default: {
            return false;
        }
    }
    if (typeMatch) {
        bool isArray = false;
        napi_is_array(env, parameters[1], &isArray);
        return isArray;
    }
    return false;
}

static void NativeAuthenticateSector(napi_env env, void *data)
{
    DebugLog("NativeAuthenticateSector called");
    auto context = static_cast<MifareClassicContext<bool, NapiMifareClassicTag> *>(data);
    DebugLog("NativeAuthenticateSector objInfo %{public}p", context->objectInfo);

    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("NativeAuthenticateSector find objectInfo failed!");
        context->value = true;
    } else {
        context->value = nfcMifareClassicTagPtr->AuthenticateSector(context->sectorIndex, "aaa", context->bIsKeyA);
    }
    context->resolved = true;
}

static void AuthenticateSectorCallback(napi_env env, napi_status status, void *data)
{
    DebugLog("AuthenticateSectorCallback called");
    auto context = static_cast<MifareClassicContext<bool, NapiMifareClassicTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_status status = napi_get_boolean(env, context->value, &callbackValue);
            if (status != napi_ok) {
                ErrorLog("get boolean failed");
            }
        } else {
            callbackValue = CreateErrorMessage(env, "AuthenticateSector error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "AuthenticateSector error,napi_status = " + std ::to_string(status));
    }
    Handle2ValueCallback(env, context, callbackValue);
}

napi_value NapiMifareClassicTag::AuthenticateSector(napi_env env, napi_callback_info info)
{
    DebugLog("AuthenticateSector called");
    size_t paramsCount = ARGV_NUM_4;
    napi_value params[ARGV_NUM_4] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiMifareClassicTag *objectInfoCb = nullptr;

    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    NAPI_ASSERT(env, MatchAuthenticateSectorParameters(env, params, paramsCount), "AuthenticateSector type mismatch");
    auto context = std::make_unique<MifareClassicContext<bool, NapiMifareClassicTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "MifareClassicContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->sectorIndex);
    DebugLog("AuthenticateSector sectorIndex = %{public}d", context->sectorIndex);

    napi_get_value_bool(env, params[ARGV_INDEX_2], &context->bIsKeyA);
    DebugLog("AuthenticateSector bIsKeyA = %{public}d", context->bIsKeyA);

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

static void NativeReadSingleBlock(napi_env env, void *data)
{
    DebugLog("NativeReadSingleBlock called");
    auto context = static_cast<MifareClassicContext<std::string, NapiMifareClassicTag> *>(data);

    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("NativeReadSingleBlock find objectInfo failed!");
        context->value = "";
    } else {
        context->value = nfcMifareClassicTagPtr->ReadSingleBlock(context->blockIndex);
        DebugLog("ReadSingleBlock context value = %{public}s", context->value.c_str());
    }
    context->resolved = true;
}

static void ReadSingleBlockCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MifareClassicContext<std::string, NapiMifareClassicTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_string_utf8(env, context->value.c_str(), context->value.size(), &callbackValue);
        } else {
            callbackValue = CreateErrorMessage(env, "ReadSingleBlock error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "ReadSingleBlock error,napi_status = " + std ::to_string(status));
    }

    Handle2ValueCallback(env, context, callbackValue);
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

    NAPI_ASSERT(env, MatchReadSingleBlockParameters(env, params, paramsCount), "ReadSingleBlock type mismatch");
    auto context = std::make_unique<MifareClassicContext<std::string, NapiMifareClassicTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "MifareClassicContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
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
    bool typeMatch = false;
    switch (parameterCount) {
        case ARGV_NUM_2: {
            typeMatch = MatchParameters(env, parameters, {napi_number, napi_string});
            break;
        }
        case ARGV_NUM_3:
            typeMatch = MatchParameters(env, parameters, {napi_number, napi_string, napi_function});
            break;
        default: {
            return false;
        }
    }
    return typeMatch;
}

static void NativeWriteSingleBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    DebugLog("NativeWriteSingleBlock objInfo %{public}p", context->objectInfo);

    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("NativeWriteSingleBlock find objectInfo failed!");
    } else {
        context->value = nfcMifareClassicTagPtr->WriteSingleBlock(context->blockIndex, context->data);
    }
    context->resolved = true;
}

static void WriteSingleBlockCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_int32(env, context->value, &callbackValue);
        } else {
            callbackValue = CreateErrorMessage(env, "WriteSingleBlock error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "WriteSingleBlock error,napi_status = " + std ::to_string(status));
    }

    Handle2ValueCallback(env, context, callbackValue);
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

    NAPI_ASSERT(env, MatchWriteSingleBlockParameters(env, params, paramsCount), "WriteSingleBlock type mismatch");
    auto context = std::make_unique<MifareClassicContext<int, NapiMifareClassicTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "MifareClassicContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->blockIndex);
    DebugLog("WriteSingleBlock blockIndex = %{public}d", context->blockIndex);

    context->data = GetStringFromValue(env, params[ARGV_INDEX_1]);
    DebugLog("WriteSingleBlock data = %{public}s", context->data.c_str());

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
    bool typeMatch = false;
    switch (parameterCount) {
        case ARGV_NUM_2: {
            typeMatch = MatchParameters(env, parameters, {napi_number, napi_number});
            break;
        }
        case ARGV_NUM_3:
            typeMatch = MatchParameters(env, parameters, {napi_number, napi_number, napi_function});
            break;
        default: {
            return false;
        }
    }
    return typeMatch;
}

static void NativeIncrementBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    DebugLog("NativeIncrementBlock objInfo %{public}p", context->objectInfo);

    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("NativeIncrementBlock find objectInfo failed!");
    } else {
        context->value = nfcMifareClassicTagPtr->IncrementBlock(context->blockIndex, context->incrementValue);
        DebugLog("IncrementBlock context value = %{public}d", context->value);
    }
    context->resolved = true;
}

static void IncrementBlockCallback(napi_env env, napi_status status, void *data)
{
    DebugLog("IncrementBlockCallback called");
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_int32(env, context->value, &callbackValue);
        } else {
            callbackValue = CreateErrorMessage(env, "IncrementBlock error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "IncrementBlock error,napi_status = " + std ::to_string(status));
    }

    Handle2ValueCallback(env, context, callbackValue);
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

    NAPI_ASSERT(env, MatchIncrementBlockParameters(env, params, paramsCount), "WriteSingleBlock type mismatch");
    auto context = std::make_unique<MifareClassicContext<int, NapiMifareClassicTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "MifareClassicContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->blockIndex);
    DebugLog("IncrementBlock blockIndex = %{public}d", context->blockIndex);

    napi_get_value_int32(env, params[ARGV_INDEX_1], &context->incrementValue);
    DebugLog("IncrementBlock data = %{public}d", context->incrementValue);

    if (paramsCount == ARGV_NUM_3) {
        napi_create_reference(env, params[ARGV_INDEX_2], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "IncrementBlock", NativeIncrementBlock, IncrementBlockCallback);
    return result;
}

static bool MatchDecrementBlockParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    bool typeMatch = false;
    switch (parameterCount) {
        case ARGV_NUM_2: {
            typeMatch = MatchParameters(env, parameters, {napi_number, napi_number});
            break;
        }
        case ARGV_NUM_3:
            typeMatch = MatchParameters(env, parameters, {napi_number, napi_number, napi_function});
            break;
        default: {
            return false;
        }
    }
    return typeMatch;
}

static void NativeDecrementBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    DebugLog("NativeDecrementBlock objInfo %{public}p", context->objectInfo);

    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("NativeDecrementBlock find objectInfo failed!");
    } else {
        context->value = nfcMifareClassicTagPtr->DecrementBlock(context->blockIndex, context->decrementValue);
        DebugLog("DecrementBlock context value = %{public}d", context->value);
    }
    context->resolved = true;
}

static void DecrementBlockCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_int32(env, context->value, &callbackValue);
        } else {
            callbackValue = CreateErrorMessage(env, "DecrementBlock error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "DecrementBlock error,napi_status = " + std ::to_string(status));
    }

    Handle2ValueCallback(env, context, callbackValue);
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

    NAPI_ASSERT(env, MatchDecrementBlockParameters(env, params, paramsCount), "DecrementBlock type mismatch");
    auto context = std::make_unique<MifareClassicContext<int, NapiMifareClassicTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "MifareClassicContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->blockIndex);
    DebugLog("DecrementBlock blockIndex = %{public}d", context->blockIndex);

    napi_get_value_int32(env, params[ARGV_INDEX_1], &context->decrementValue);
    DebugLog("DecrementBlock data = %{public}d", context->decrementValue);

    if (paramsCount == ARGV_NUM_3) {
        napi_create_reference(env, params[ARGV_INDEX_2], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "DecrementBlock", NativeDecrementBlock, DecrementBlockCallback);
    return result;
}

static bool MatchTransferToBlockParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
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

static void NativeTransferToBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    DebugLog("NativeTransferToBlock objInfo %{public}p", context->objectInfo);

    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("NativeTransferToBlock find objectInfo failed!");
    } else {
        context->value = nfcMifareClassicTagPtr->TransferToBlock(context->blockIndex);
        DebugLog("TransferToBlock context value = %{public}d", context->value);
    }
    context->resolved = true;
}

static void TransferToBlockCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_int32(env, context->value, &callbackValue);
        } else {
            callbackValue = CreateErrorMessage(env, "TransferToBlock error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "TransferToBlock error,napi_status = " + std ::to_string(status));
    }

    Handle2ValueCallback(env, context, callbackValue);
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

    NAPI_ASSERT(env, MatchTransferToBlockParameters(env, params, paramsCount), "TransferToBlock type mismatch");
    auto context = std::make_unique<MifareClassicContext<int, NapiMifareClassicTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "MifareClassicContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->blockIndex);
    DebugLog("TransferToBlock blockIndex = %{public}d", context->blockIndex);

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

static void NativeRestoreFromBlock(napi_env env, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    DebugLog("NativeRestoreFromBlock objInfo %{public}p", context->objectInfo);

    MifareClassicTag *nfcMifareClassicTagPtr =
        static_cast<MifareClassicTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcMifareClassicTagPtr == nullptr) {
        ErrorLog("NativeRestoreFromBlock find objectInfo failed!");
    } else {
        context->value = nfcMifareClassicTagPtr->RestoreFromBlock(context->blockIndex);
        DebugLog("RestoreFromBlock context value = %{public}d", context->value);
    }
    context->resolved = true;
}

static void RestoreFromBlockCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<MifareClassicContext<int, NapiMifareClassicTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_int32(env, context->value, &callbackValue);
        } else {
            callbackValue = CreateErrorMessage(env, "RestoreFromBlock error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "RestoreFromBlock error,napi_status = " + std ::to_string(status));
    }

    Handle2ValueCallback(env, context, callbackValue);
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

    NAPI_ASSERT(env, MatchRestoreFromBlockParameters(env, params, paramsCount), "RestoreFromBlock type mismatch");
    auto context = std::make_unique<MifareClassicContext<int, NapiMifareClassicTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "MifareClassicContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return nullptr;
    }
    // parse the params
    napi_get_value_int32(env, params[ARGV_INDEX_0], &context->blockIndex);
    DebugLog("RestoreFromBlock blockIndex = %{public}d", context->blockIndex);

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
