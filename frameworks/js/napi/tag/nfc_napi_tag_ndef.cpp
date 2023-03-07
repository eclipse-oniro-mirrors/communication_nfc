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

#include "nfc_napi_tag_ndef.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
static const int32_t DEFAULT_REF_COUNT = 1;
const int INIT_REF = 1;
thread_local napi_ref ndefMessageRef_;       // for read and getNedfMessage NAPI

static bool CheckTagSessionAndThrow(const napi_env &env, const NdefTag *tagSession)
{
    if (tagSession == nullptr) {
        // object null is unexpected, unknown error.
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_TAG_STATE_INVALID,
            BuildErrorMessage(BUSI_ERR_TAG_STATE_INVALID, "", "", "", "")));
        return false;
    }
    return true;
}

std::shared_ptr<NdefRecord> ParseNdefParam(const napi_env &env, napi_value &args)
{
    std::shared_ptr<NdefRecord> ndefRecord = std::make_shared<NdefRecord>();
    napi_value elementValue = nullptr;

    napi_get_named_property(env, args, "tnf", &elementValue);
    if (!IsNumber(env, elementValue)) {
        ErrorLog("Wrong tnf argument type. Number expected.");
        ndefRecord->tnf_ = 0;
    }
    napi_get_value_uint32(env, elementValue, reinterpret_cast<uint32_t *>(&ndefRecord->tnf_));

    napi_get_named_property(env, args, "rtdType", &elementValue);
    if (!IsString(env, elementValue)) {
        ErrorLog("Wrong rtdType argument type. String expected.");
        ndefRecord->tagRtdType_ = "";
    }
    std::string rtdTypeStr;
    ParseString(env, rtdTypeStr, elementValue);
    ndefRecord->tagRtdType_ = rtdTypeStr;

    napi_get_named_property(env, args, "id", &elementValue);
    if (!IsString(env, elementValue)) {
        ErrorLog("Wrong id argument type. String expected.");
        ndefRecord->id_ = "";
    }
    std::string idStr;
    ParseString(env, idStr, elementValue);
    ndefRecord->id_ = idStr;

    napi_get_named_property(env, args, "payload", &elementValue);
    if (!IsString(env, elementValue)) {
        ErrorLog("Wrong payload argument type. String expected.");
        ndefRecord->payload_ = "";
    }
    std::string payloadStr;
    ParseString(env, payloadStr, elementValue);
    ndefRecord->payload_ = payloadStr;
    return ndefRecord;
}

std::vector<std::shared_ptr<NdefRecord>> ParseNdefRecords(const napi_env &env, napi_value &args)
{
    // already checked args is object array.
    uint32_t length = 0;
    napi_get_array_length(env, args, &length);
    std::vector<std::shared_ptr<NdefRecord>> params;
    std::shared_ptr<NdefRecord> ndefRecord = std::make_shared<NdefRecord>();

    for (size_t i = 0; i < length; i++) {
        napi_value ndefRecordValue;
        napi_get_element(env, args, i, &ndefRecordValue);
        if (!IsObject(env, ndefRecordValue)) {
            ErrorLog("ParseNdefRecords, Object expected.");
            continue;
        }
        ndefRecord = ParseNdefParam(env, ndefRecordValue);
        params.push_back(ndefRecord);
    }
    return params;
}

napi_value NdefMessage_Constructor(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    return thisVar;
}

napi_value NapiNdefTag::RegisterNdefMessageJSClass(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getNdefRecords", NapiNdefMessage::GetNdefRecords),
    };

    // NdefMessage_Constructor is for GetNdefMessage and Read NdefMessage NAPI with no params
    napi_value constructor = nullptr;
    NAPI_CALL(env,
        napi_define_class(env, "NdefMessage", NAPI_AUTO_LENGTH, NdefMessage_Constructor, nullptr,
            sizeof(desc) / sizeof(desc[0]), desc, &constructor));
    napi_create_reference(env, constructor, INIT_REF, &ndefMessageRef_);
    return exports;
}

napi_value NapiNdefTag::CreateNdefMessage(napi_env env, napi_callback_info info)
{
    size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    // check parameter number
    if (argc != ARGV_NUM_1) {
        ErrorLog("CreateNdefMessage, Invalid number of arguments");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM, BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return CreateUndefined(env);
    }

    // check parameter data type
    NapiNdefMessage *napiNdefMessage = new NapiNdefMessage();
    if (IsNumberArray(env, argv[ARGV_INDEX_0])) {
        // data: number[]
        std::vector<unsigned char> dataVec;
        ParseBytesVector(env, dataVec, argv[ARGV_INDEX_0]);
        std::string rawData = NfcSdkCommon::BytesVecToHexString(static_cast<unsigned char *>(dataVec.data()),
                                                                dataVec.size());
        napiNdefMessage->ndefMessage = NdefMessage::GetNdefMessage(rawData);
    } else if (IsObjectArray(env, argv[ARGV_INDEX_0])) {
        // ndefRecords: NdefRecord[]
        std::vector<std::shared_ptr<NdefRecord>> ndefRecords ;
        ndefRecords = ParseNdefRecords(env, argv[ARGV_INDEX_0]);
        napiNdefMessage->ndefMessage = NdefMessage::GetNdefMessage(ndefRecords);
    } else {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "data | ndefRecords", "number[] | NdefRecord[]")));
        delete napiNdefMessage;
        return CreateUndefined(env);
    }

    if (napiNdefMessage->ndefMessage == nullptr) {
        delete napiNdefMessage;
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_TAG_STATE_INVALID,
            BuildErrorMessage(BUSI_ERR_TAG_STATE_INVALID, "", "", "", "")));
        return CreateUndefined(env);
    }

    napi_value ndefMessage = nullptr;
    napi_value constructor = nullptr;
    napi_get_reference_value(env, ndefMessageRef_, &constructor);
    napi_new_instance(env, constructor, 0, nullptr, &ndefMessage);
    napi_status status = napi_wrap(
        env, ndefMessage, napiNdefMessage,
        [](napi_env env, void *data, void *hint) {
            if (data) {
                NapiNdefMessage *message = static_cast<NapiNdefMessage *>(data);
                delete message;
            }
        },
        nullptr, nullptr);
    NAPI_ASSERT(env, status == napi_ok, "CreateNdefMessage, failed to wrap ndefMessage");
    return ndefMessage;
}

napi_value NapiNdefTag::GetNdefTagType(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefTag *objectInfo = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("GetNdefTagType, napi_unwrap failed, object is null.");
        napi_create_int32(env, NdefTag::EmNfcForumType::NFC_FORUM_TYPE_UNKNOWN, &result);
        return result;
    }

    NdefTag::EmNfcForumType nfcForumType = NdefTag::EmNfcForumType::NFC_FORUM_TYPE_UNKNOWN;
    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcNdefTagPtr == nullptr) {
        ErrorLog("GetNdefTagType, find objectInfo failed!");
    } else {
        nfcForumType = nfcNdefTagPtr->GetNdefTagType();
    }
    napi_create_int32(env, nfcForumType, &result);
    return result;
}

napi_value NapiNdefTag::GetNdefMessage(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_value ndefMessage = nullptr;
    size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefTag *objectInfo = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("GetNdefMessage, napi_unwrap failed, object is null.");
        return CreateUndefined(env);
    }

    napi_value constructor = nullptr;
    napi_get_reference_value(env, ndefMessageRef_, &constructor);
    napi_new_instance(env, constructor, 0, nullptr, &ndefMessage);

    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcNdefTagPtr == nullptr) {
        ErrorLog("GetNdefMessage find objectInfo failed!");
        return CreateUndefined(env);
    }

    NapiNdefMessage *napiNdefMessage = new NapiNdefMessage();
    napiNdefMessage->ndefMessage = nfcNdefTagPtr->GetCachedNdefMsg();
    if (napiNdefMessage->ndefMessage == nullptr) {
        ErrorLog("GetNdefMessage ndefMessage failed!");
        delete napiNdefMessage;
        return CreateUndefined(env);
    }

    napi_status status2 = napi_wrap(
        env, ndefMessage, napiNdefMessage,
        [](napi_env env, void *data, void *hint) {
            if (data) {
                NapiNdefMessage *message = static_cast<NapiNdefMessage *>(data);
                delete message;
            }
        },
        nullptr, nullptr);
    if (status2 != napi_ok) {
        ErrorLog("GetNdefMessage, napi_wrap failed, object is null.");
        return CreateUndefined(env);
    }
    return ndefMessage;
}

napi_value NapiNdefTag::IsNdefWritable(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefTag *objectInfo = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("IsNdefWritable, napi_unwrap failed, object is null.");
        napi_get_boolean(env, false, &result);
        return result;
    }

    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcNdefTagPtr == nullptr) {
        ErrorLog("IsNdefWritable find objectInfo failed!");
        napi_get_boolean(env, false, &result);
        return result;
    }

    bool isWritable = nfcNdefTagPtr->IsNdefWritable();
    napi_get_boolean(env, isWritable, &result);
    return result;
}

static bool CheckReadNdefParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    // argments 0 or 1 is allowed.
    if (parameterCount == ARGV_NUM_1) {
        if (!CheckParametersAndThrow(env, parameters, {napi_function}, "callback", "function")) {
            return false;
        }
    } else if (parameterCount > ARGV_NUM_1) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return false;
    }
    return true;
}

static void NativeReadNdef(napi_env env, void *data)
{
    auto context = static_cast<NdefContext<std::shared_ptr<NdefMessage>, NapiNdefTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;

    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcNdefTagPtr)) {
        return;
    }
    std::shared_ptr<NdefMessage> ndefMessage = nullptr;
    context->errorCode = nfcNdefTagPtr->ReadNdef(ndefMessage);
    context->value = ndefMessage;
    context->resolved = true;
}

static void ReadNdefCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<NdefContext<std::shared_ptr<NdefMessage>, NapiNdefTag> *>(data);
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is NdefMessage
        NapiNdefMessage *napiNdefMessage = new NapiNdefMessage();
        napiNdefMessage->ndefMessage = context->value;
        napi_value callbackValue = nullptr;
        napi_value constructor = nullptr;

        napi_get_reference_value(env, ndefMessageRef_, &constructor);
        napi_new_instance(env, constructor, 0, nullptr, &callbackValue);
        napi_status status = napi_wrap(
            env, callbackValue, napiNdefMessage,
            [](napi_env env, void *data, void *hint) {
                if (data) {
                    NapiNdefMessage *message = static_cast<NapiNdefMessage *>(data);
                    delete message;
                }
            },
            nullptr, nullptr);
        if (status == napi_ok) {
            DoAsyncCallbackOrPromise(env, context, callbackValue);
        } else {
            ErrorLog("ReadNdefCallback, napi_wrap failed!");
            if (!CheckTagSessionAndThrow(env, nullptr)) {
                return;
            }
        }
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "readNdef", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiNdefTag::ReadNdef(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_1;
    napi_value params[ARGV_NUM_1] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiNdefTag *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckReadNdefParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }
    auto context = std::make_unique<NdefContext<std::shared_ptr<NdefMessage>, NapiNdefTag>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }
    if (paramsCount == ARGV_NUM_1) {
        napi_create_reference(env, params[ARGV_INDEX_0], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "ReadNdef", NativeReadNdef, ReadNdefCallback);
    return result;
}

static bool CheckWriteNdefParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount == ARGV_NUM_1) {
        if (!CheckParametersAndThrow(env, parameters, {napi_object}, "msg", "NdefMessage")) {
            return false;
        }
        return true;
    } else if (parameterCount == ARGV_NUM_2) {
        if (!CheckParametersAndThrow(env, parameters, {napi_object, napi_function},
            "msg & callback", "NdefMessage & function")) {
            return false;
        }
        return true;
    } else {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return false;
    }
}

static void NativeWriteNdef(napi_env env, void *data)
{
    auto context = static_cast<NdefContext<int, NapiNdefTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;

    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcNdefTagPtr)) {
        return;
    }
    context->errorCode = nfcNdefTagPtr->WriteNdef(context->msg);
    context->resolved = true;
}

static void WriteNdefCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<NdefContext<int, NapiNdefTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is void.
        napi_get_undefined(env, &callbackValue);
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "writeNdef", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiNdefTag::WriteNdef(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_2;
    napi_value params[ARGV_NUM_2] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiNdefTag *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckWriteNdefParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }

    auto context = std::make_unique<NdefContext<int, NapiNdefTag>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    NapiNdefMessage *napiNdefMessage = nullptr;
    napi_status status2 = napi_unwrap(env, params[ARGV_INDEX_0], reinterpret_cast<void **>(&napiNdefMessage));
    if (!CheckUnwrapStatusAndThrow(env, status2, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    context->msg = napiNdefMessage->ndefMessage;
    if (paramsCount == ARGV_NUM_2) {
        napi_create_reference(env, params[ARGV_INDEX_1], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "WriteNdef", NativeWriteNdef, WriteNdefCallback);
    return result;
}

napi_value NapiNdefTag::CanSetReadOnly(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefTag *objectInfo = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcNdefTagPtr)) {
        return CreateUndefined(env);
    }

    bool canSetReadOnly = false;
    int statusCode = nfcNdefTagPtr->IsEnableReadOnly(canSetReadOnly);
    if (!CheckTagStatusCodeAndThrow(env, statusCode, "canSetReadOnly")) {
        return CreateUndefined(env);
    }
    napi_get_boolean(env, canSetReadOnly, &result);
    return result;
}

static bool CheckSetReadOnlyParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    // argments 0 or 1 is allowed.
    if (parameterCount == ARGV_NUM_1) {
        if (!CheckParametersAndThrow(env, parameters, {napi_function}, "callback", "function")) {
            return false;
        }
    } else if (parameterCount > ARGV_NUM_1) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return false;
    }
    return true;
}

static void NativeSetReadOnly(napi_env env, void *data)
{
    auto context = static_cast<NdefContext<int, NapiNdefTag> *>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;

    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcNdefTagPtr)) {
        return;
    }
    context->errorCode = nfcNdefTagPtr->EnableReadOnly();
    context->resolved = true;
}

static void SetReadOnlyCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<NdefContext<int, NapiNdefTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        // the return is void.
        napi_get_undefined(env, &callbackValue);
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "setReadOnly", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NapiNdefTag::SetReadOnly(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_1;
    napi_value params[ARGV_NUM_1] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiNdefTag *objectInfoCb = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID) ||
        !CheckSetReadOnlyParameters(env, params, paramsCount)) {
        return CreateUndefined(env);
    }

    auto context = std::make_unique<NdefContext<int, NapiNdefTag>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    if (paramsCount == ARGV_NUM_1) {
        napi_create_reference(env, params[ARGV_INDEX_0], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "SetReadOnly", NativeSetReadOnly, SetReadOnlyCallback);
    return result;
}

napi_value NapiNdefTag::GetNdefTagTypeString(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    size_t expectedArgsCount = ARGV_NUM_1;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGV_NUM_1] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));

    // check parameter number
    if (!CheckArgCountAndThrow(env, argc, expectedArgsCount) ||
        !CheckNumberAndThrow(env, argv[ARGV_INDEX_0], "type", "number")) {
        return CreateUndefined(env);
    }

    // unwrap from thisVar to retrieve the native instance
    NapiNdefTag *objectInfo = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    int type = 0;
    ParseInt32(env, type, argv[ARGV_INDEX_0]);

    // transfer
    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (!CheckTagSessionAndThrow(env, nfcNdefTagPtr)) {
        return CreateUndefined(env);
    }
    napi_value ret = nullptr;
    std::string ndefTagType =
        nfcNdefTagPtr->GetNdefTagTypeString(static_cast<NdefTag::EmNfcForumType>(type));
    napi_create_string_utf8(env, ndefTagType.c_str(), NAPI_AUTO_LENGTH, &ret);
    return ret;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
