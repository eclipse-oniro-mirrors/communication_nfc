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
napi_value ndefMessageReadObject;
thread_local napi_ref ndefMessageRef_;
thread_local napi_ref ndefMessageReadRef_;
const int INIT_REF = 1;

std::vector<std::shared_ptr<NdefRecord>> ParseNdefRecords(const napi_env &env, napi_value &args)
{
    DebugLog("ParseNdefRecords called");
    uint32_t length = 0;
    napi_get_array_length(env, args, &length);
    DebugLog("ParseNdefRecords, ndef records length, %{public}d", length);

    std::vector<std::shared_ptr<NdefRecord>> params;
    std::shared_ptr<NdefRecord> param = std::make_shared<NdefRecord>();

    for (size_t i = 0; i < length; i++) {
        napi_value ndefRecord;
        napi_valuetype valueType = napi_undefined;
        napi_value result = nullptr;

        napi_get_element(env, args, i, &ndefRecord);
        napi_typeof(env, ndefRecord, &valueType);
        if (valueType != napi_object) {
            ErrorLog("Wrong ndefRecord argument type. Object expected.");
        }

        napi_get_named_property(env, ndefRecord, "tnf", &result);
        napi_typeof(env, result, &valueType);
        if (valueType != napi_number) {
            ErrorLog("Wrong tnf argument type. Number expected.");
            return params;
        }
        napi_get_value_uint32(env, result, reinterpret_cast<uint32_t *>(&param->tnf_));

        napi_get_named_property(env, ndefRecord, "rtdType", &result);
        napi_typeof(env, result, &valueType);
        if (valueType != napi_string) {
            ErrorLog("Wrong rtdType argument type. String expected.");
            return params;
        }
        ParseString(env, param->tagRtdType_, result);

        napi_get_named_property(env, ndefRecord, "id", &result);
        napi_typeof(env, result, &valueType);
        if (valueType != napi_string) {
            ErrorLog("Wrong id argument type. String expected.");
            return params;
        }
        ParseString(env, param->id_, result);

        napi_get_named_property(env, ndefRecord, "payload", &result);
        napi_typeof(env, result, &valueType);
        if (valueType != napi_string) {
            ErrorLog("Wrong payload argument type. String expected.");
            return params;
        }
        ParseString(env, param->payload_, result);

        params.push_back(param);
    }
    return params;
}

napi_value JS_Constructor(napi_env env, napi_callback_info cbinfo)
{
    DebugLog("ndef JS_Constructor in");
    std::shared_ptr<NdefMessage> ndefMessage;
    // nfcTag is defined as a native instance that will be wrapped in the JS object
    NapiNdefMessage *napiNdefMessage = new NapiNdefMessage();
    size_t argc = 1;
    napi_value argv[] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr));
    // check parameter number
    if (argc != ARGV_NUM_1) {
        ErrorLog("Invalid number of arguments");
        return nullptr;
    }
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[ARGV_NUM_0], &valueType));
    // check parameter data type
    bool isArray = false;
    napi_is_array(env, argv[ARGV_INDEX_0], &isArray);

    // parse  parameter from JS
    if (valueType == napi_string) {
        std::string data = GetStringFromValue(env, argv[ARGV_INDEX_0]);
        DebugLog("ndfe message parse data = %{public}s", data.c_str());
        napiNdefMessage->ndefMessage = NdefMessage::GetNdefMessage(data);
    } else if (isArray) {
        std::vector<std::shared_ptr<NdefRecord>> ndefRecords ;
        ndefRecords = ParseNdefRecords(env, argv[ARGV_INDEX_0]);
        DebugLog("NdefRecords Array parsed.");
        napiNdefMessage->ndefMessage = NdefMessage::GetNdefMessage(ndefRecords);
    } else {
        ErrorLog("invalid data type!");
        return nullptr;
    }

    // wrap  data into thisVar
    napi_status status = napi_wrap(
        env, thisVar, napiNdefMessage,
        [](napi_env env, void *data, void *hint) {
            if (data) {
                NapiNdefMessage *nfcTag = static_cast<NapiNdefMessage *>(data);
                delete nfcTag;
            }
        },
        nullptr, nullptr);
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    return thisVar;
}

napi_value NdefMessageCallbackObject(napi_env env, napi_callback_info cbinfo)
{
    DebugLog("ndef NdefMessageCallbackObject in");
    size_t argc = 0;
    napi_value argv[] = {nullptr};
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    return thisVar;
}

napi_value NapiNdefTag::RegisterNdefMessageJSClass(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getNdefRecords", NapiNdefMessage::GetNdefRecords),
        DECLARE_NAPI_FUNCTION("makeUriRecord", NapiNdefMessage::MakeUriRecord),
        DECLARE_NAPI_FUNCTION("makeTextRecord", NapiNdefMessage::MakeTextRecord),
        DECLARE_NAPI_FUNCTION("makeMimeRecord", NapiNdefMessage::MakeMimeRecord),
        DECLARE_NAPI_FUNCTION("makeExternalRecord", NapiNdefMessage::MakeExternalRecord),
        DECLARE_NAPI_FUNCTION("messageToString", NapiNdefMessage::MessageToString),
    };
    // define JS class NdefMessage, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    NAPI_CALL(env,
        napi_define_class(env, "NdefMessage", NAPI_AUTO_LENGTH, JS_Constructor, nullptr,
            sizeof(desc) / sizeof(desc[0]), desc, &constructor));
    
    NAPI_CALL(env,
        napi_define_class(env, "NdefMessage", NAPI_AUTO_LENGTH, NdefMessageCallbackObject, nullptr,
            sizeof(desc) / sizeof(desc[0]), desc, &constructor));
    
    napi_create_reference(env, constructor, INIT_REF, &ndefMessageRef_);
    napi_create_reference(env, constructor, INIT_REF, &ndefMessageReadRef_);
    return exports;
}

napi_value NapiNdefTag::CreateNdefMessage(napi_env env, napi_callback_info info)
{
    DebugLog("Ndef CreateNdefMessage begin");
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    NapiNdefTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    // transfer
    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcNdefTagPtr == nullptr) {
        ErrorLog("GetType find objectInfo failed!");
        return nullptr;
    } else {
        // to-do
        napi_value result = nullptr;
        return result;
    }
}

napi_value NapiNdefTag::GetNdefTagType(napi_env env, napi_callback_info info)
{
    DebugLog("Ndef GetType called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    // transfer
    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcNdefTagPtr == nullptr) {
        ErrorLog("GetType find objectInfo failed!");
        napi_create_int32(env, NdefTag::EmNfcForumType::NFC_FORUM_TYPE_1, &result);
        return result;
    } else {
        NdefTag::EmNfcForumType nfcForumType = nfcNdefTagPtr->GetNdefTagType();
        DebugLog("nfcForumType %{public}d", nfcForumType);
        napi_create_int32(env, nfcForumType, &result);
        return result;
    }
}

napi_value NapiNdefTag::GetNdefMessage(napi_env env, napi_callback_info info)
{
    DebugLog("Ndef GetNdefMessage called");
    napi_value thisVar = nullptr;
    napi_value ndefMessage = nullptr;
    NapiNdefMessage *napiNdefMessage = new NapiNdefMessage();
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    // transfer
    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcNdefTagPtr == nullptr) {
        ErrorLog("GetType find objectInfo failed!");
        return CreateUndefined(env);
    } else {
        napiNdefMessage->ndefMessage = nfcNdefTagPtr->GetCachedNdefMsg();

        napi_status status1 = napi_wrap(
            env, ndefMessage, napiNdefMessage,
            [](napi_env env, void *data, void *hint) {
                if (data) {
                    NapiNdefMessage *message = (NapiNdefMessage *)data;
                    delete message;
                }
            },
            nullptr, nullptr);
        NAPI_ASSERT(env, status1 == napi_ok, "failed to wrap ndefMessage");
        return ndefMessage;
    }
}

static bool MatchIsNdefWritableParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount > ARGV_NUM_1) {
        return false;
    }
    if (parameterCount == ARGV_NUM_1) {
        return MatchParameters(env, parameters, {napi_function});
    }
    return true;
}

static void NativeIsNdefWritable(napi_env env, void *data)
{
    DebugLog("NativeIsNdefWritable called");
    auto context = static_cast<NdefContext<bool, NapiNdefTag> *>(data);

    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcNdefTagPtr == nullptr) {
        ErrorLog("NativeIsNdefWritable find objectInfo failed!");
        context->value = true;
    } else {
        context->value = nfcNdefTagPtr->IsNdefWritable();
        DebugLog("IsNdefWritable %{public}d", context->value);
    }
    context->resolved = true;
}

static void IsNdefWritableCallback(napi_env env, napi_status status, void *data)
{
    DebugLog("IsNdefWritableCallback called");
    auto context = static_cast<NdefContext<bool, NapiNdefTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_status status = napi_get_boolean(env, context->value, &callbackValue);
            if (status != napi_ok) {
                ErrorLog("get boolean failed");
            }
        } else {
            callbackValue = CreateErrorMessage(env, "IsNdefWritable error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "IsNdefWritable error,napi_status = " + std ::to_string(status));
    }
    Handle2ValueCallback(env, context, callbackValue);
}

napi_value NapiNdefTag::IsNdefWritable(napi_env env, napi_callback_info info)
{
    DebugLog("GetNdefTag IsNdefWritable called");
    size_t paramsCount = ARGV_NUM_1;
    napi_value params[ARGV_NUM_1] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiNdefTag *objectInfoCb = nullptr;

    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    NAPI_ASSERT(env, MatchIsNdefWritableParameters(env, params, paramsCount), "IsNdefWritable type mismatch");
    auto context = std::make_unique<NdefContext<bool, NapiNdefTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "error at CallBackContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return CreateUndefined(env);
    }
    if (paramsCount == ARGV_NUM_1) {
        napi_create_reference(env, params[ARGV_INDEX_0], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "IsNdefWritable", NativeIsNdefWritable, IsNdefWritableCallback);
    return result;
}

static bool MatchReadNdefParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount > ARGV_NUM_1) {
        return false;
    }
    if (parameterCount == ARGV_NUM_1) {
        return MatchParameters(env, parameters, {napi_function});
    }
    return true;
}

static void NativeReadNdef(napi_env env, void *data)
{
    DebugLog("NativeReadNdef called");
    auto context = static_cast<NdefContext<std::shared_ptr<NdefMessage>, NapiNdefTag> *>(data);

    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcNdefTagPtr == nullptr) {
        ErrorLog("NativeReadNdef find objectInfo failed!");
    } else {
        context->value = nfcNdefTagPtr->ReadNdef();
    }
    context->resolved = true;
}

static void ReadNdefCallback(napi_env env, napi_status status, void *data)
{
    DebugLog("ReadNdefCallback called");
    NapiNdefMessage *napiNdefMessage = new NapiNdefMessage();
    auto context = static_cast<NdefContext<std::shared_ptr<NdefMessage>, NapiNdefTag> *>(data);
    napi_value callbackValue = nullptr;
    napiNdefMessage->ndefMessage = context->value;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_new_instance(env, ndefMessageReadObject, 0, nullptr, &callbackValue);
            napi_status status1 = napi_wrap(
                env, callbackValue, napiNdefMessage,
                [](napi_env env, void *data, void *hint) {
                    if (data) {
                        NapiNdefMessage *message = (NapiNdefMessage *)data;
                        delete message;
                    }
                },
                nullptr, nullptr);
            if (status1 != napi_ok) {
                ErrorLog("wrap napiNdefMessage failed");
            }
        } else {
            callbackValue = CreateErrorMessage(env, "ReadNdef error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "ReadNdef error,napi_status = " + std ::to_string(status));
    }
    Handle2ValueCallback(env, context, callbackValue);
}

napi_value NapiNdefTag::ReadNdef(napi_env env, napi_callback_info info)
{
    DebugLog("GetNdefTag ReadNdef called");
    size_t paramsCount = ARGV_NUM_1;
    napi_value params[ARGV_NUM_1] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiNdefTag *objectInfoCb = nullptr;

    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    NAPI_ASSERT(env, MatchReadNdefParameters(env, params, paramsCount), "ReadNdef type mismatch");
    auto context = std::make_unique<NdefContext<std::shared_ptr<NdefMessage>, NapiNdefTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "error at CallBackContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return CreateUndefined(env);
    }
    if (paramsCount == ARGV_NUM_1) {
        napi_create_reference(env, params[ARGV_INDEX_0], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "ReadNdef", NativeReadNdef, ReadNdefCallback);
    return result;
}

static bool MatchWriteNdefParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
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

static void NativeWriteNdef(napi_env env, void *data)
{
    DebugLog("NativeWriteNdef called");
    auto context = static_cast<NdefContext<int, NapiNdefTag> *>(data);

    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcNdefTagPtr == nullptr) {
        ErrorLog("NativeWriteNdef find objectInfo failed!");
        context->value = true;
    } else {
        context->value = nfcNdefTagPtr->WriteNdef(context->msg);
        DebugLog("WriteNdef %{public}d", context->value);
    }
    context->resolved = true;
}

static void WriteNdefCallback(napi_env env, napi_status status, void *data)
{
    DebugLog("SetReadOnlyCallback called");
    auto context = static_cast<NdefContext<int, NapiNdefTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_int32(env, context->value, &callbackValue);
        } else {
            callbackValue = CreateErrorMessage(env, "WriteNdef error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "WriteNdef error,napi_status = " + std ::to_string(status));
    }
    Handle2ValueCallback(env, context, callbackValue);
}

napi_value NapiNdefTag::WriteNdef(napi_env env, napi_callback_info info)
{
    DebugLog("GetNdefTag WriteNdef called");
    size_t paramsCount = ARGV_NUM_2;
    napi_value params[ARGV_NUM_2] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiNdefTag *objectInfoCb = nullptr;

    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    NAPI_ASSERT(env, MatchWriteNdefParameters(env, params, paramsCount), "WriteNdef type mismatch");
    auto context = std::make_unique<NdefContext<int, NapiNdefTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "error at CallBackContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return CreateUndefined(env);
    }

    NapiNdefMessage *napiNdefMessage = nullptr;
    // parse the params
    napi_status status1 = napi_unwrap(env, params[ARGV_INDEX_0], reinterpret_cast<void **>(&napiNdefMessage));
    NAPI_ASSERT(env, status1 == napi_ok, "failed to get ndefMessage");

    context->msg = napiNdefMessage->ndefMessage;
    std::string ndefMessage = NdefMessage::MessageToString(context->msg);
    if (ndefMessage.empty()) {
        ErrorLog("WriteNdef ndefMessage is empty!");
    }

    if (paramsCount == ARGV_NUM_2) {
        napi_create_reference(env, params[ARGV_INDEX_1], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "WriteNdef", NativeWriteNdef, WriteNdefCallback);
    return result;
}

static bool MatchCanSetReadOnlyParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount > ARGV_NUM_1) {
        return false;
    }
    if (parameterCount == ARGV_NUM_1) {
        return MatchParameters(env, parameters, {napi_function});
    }
    return true;
}

static void NativeCanSetReadOnly(napi_env env, void *data)
{
    DebugLog("NativeCanSetReadOnly called");
    auto context = static_cast<NdefContext<bool, NapiNdefTag> *>(data);

    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcNdefTagPtr == nullptr) {
        ErrorLog("NativeCanSetReadOnly find objectInfo failed!");
        context->value = true;
    } else {
        context->value = nfcNdefTagPtr->IsEnableReadOnly();
        DebugLog("CanSetReadOnly %{public}d", context->value);
    }
    context->resolved = true;
}

static void CanSetReadOnlyCallback(napi_env env, napi_status status, void *data)
{
    DebugLog("CanSetReadOnlyCallback called");
    auto context = static_cast<NdefContext<bool, NapiNdefTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_status status = napi_get_boolean(env, context->value, &callbackValue);
            if (status != napi_ok) {
                ErrorLog("get boolean failed");
            }
        } else {
            callbackValue = CreateErrorMessage(env, "CanSetReadOnly error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "CanSetReadOnly error,napi_status = " + std ::to_string(status));
    }
    Handle2ValueCallback(env, context, callbackValue);
}

napi_value NapiNdefTag::CanSetReadOnly(napi_env env, napi_callback_info info)
{
    DebugLog("GetNdefTag CanSetReadOnly called");
    size_t paramsCount = ARGV_NUM_1;
    napi_value params[ARGV_NUM_1] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiNdefTag *objectInfoCb = nullptr;

    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    NAPI_ASSERT(env, MatchCanSetReadOnlyParameters(env, params, paramsCount), "CanSetReadOnly type mismatch");
    auto context = std::make_unique<NdefContext<bool, NapiNdefTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "error at CallBackContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return CreateUndefined(env);
    }
    if (paramsCount == ARGV_NUM_1) {
        napi_create_reference(env, params[ARGV_INDEX_0], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    context->objectInfo = objectInfoCb;
    napi_value result = HandleAsyncWork(env, context, "CanSetReadOnly", NativeCanSetReadOnly, CanSetReadOnlyCallback);
    return result;
}

static bool MatchSetReadOnlyParameters(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    if (parameterCount > ARGV_NUM_1) {
        return false;
    }
    if (parameterCount == ARGV_NUM_1) {
        return MatchParameters(env, parameters, {napi_function});
    }
    return true;
}

static void NativeSetReadOnly(napi_env env, void *data)
{
    DebugLog("NativeSetReadOnly called");
    auto context = static_cast<NdefContext<int, NapiNdefTag> *>(data);

    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (nfcNdefTagPtr == nullptr) {
        ErrorLog("NativeSetReadOnly find objectInfo failed!");
        context->value = false;
    } else {
        context->value = nfcNdefTagPtr->EnableReadOnly();
        DebugLog("SetReadOnly %{public}d", context->value);
    }
    context->resolved = true;
}

static void SetReadOnlyCallback(napi_env env, napi_status status, void *data)
{
    DebugLog("SetReadOnlyCallback called");
    auto context = static_cast<NdefContext<int, NapiNdefTag> *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_int32(env, context->value, &callbackValue);
        } else {
            callbackValue = CreateErrorMessage(env, "SetReadOnly error by ipc");
        }
    } else {
        callbackValue = CreateErrorMessage(env, "SetReadOnly error,napi_status = " + std ::to_string(status));
    }
    Handle2ValueCallback(env, context, callbackValue);
}

napi_value NapiNdefTag::SetReadOnly(napi_env env, napi_callback_info info)
{
    DebugLog("GetNdefTag SetReadOnly called");
    size_t paramsCount = ARGV_NUM_1;
    napi_value params[ARGV_NUM_1] = {0};
    void *data = nullptr;
    napi_value thisVar = nullptr;
    NapiNdefTag *objectInfoCb = nullptr;

    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfoCb));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    NAPI_ASSERT(env, MatchSetReadOnlyParameters(env, params, paramsCount), "SetReadOnly type mismatch");
    auto context = std::make_unique<NdefContext<int, NapiNdefTag>>().release();
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_generic_failure);
        std::string errorMessage = "error at CallBackContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
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
    DebugLog("GetNdefTag GetNdefTagTypeString called");
    napi_value thisVar = nullptr;
    size_t expectedArgsCount = ARGV_NUM_1;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGV_NUM_1] = {0};
    napi_value result = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));

    // check parameter number
    if (argc != expectedArgsCount) {
        ErrorLog("NapiNdefTag::GetNdefTagTypeString, Requires 1 argument.");
        napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &result);
        return result;
    }
    // check parameter data type
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[ARGV_INDEX_0], &valueType));

    if (valueType != napi_number) {
        ErrorLog("NapiNdefTag::GetNdefTagTypeString, Invalid data type!");
        napi_create_string_utf8(env, "", NAPI_AUTO_LENGTH, &result);
        return result;
    }

    NapiNdefTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    int type;
    ParseInt32(env, type, argv[ARGV_INDEX_0]);

    // transfer
    NdefTag *nfcNdefTagPtr = static_cast<NdefTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcNdefTagPtr == nullptr) {
        ErrorLog("GetNdefTagTypeString find objectInfo failed!");
        return nullptr;
    } else {
        napi_value ret = nullptr;
        std::string ndefTagType =
            nfcNdefTagPtr->GetNdefTagTypeString(static_cast<NdefTag::EmNfcForumType>(type));
        DebugLog("ndefTagType %{public}s", ndefTagType.c_str());
        napi_create_string_utf8(env, ndefTagType.c_str(), NAPI_AUTO_LENGTH, &ret);
        return ret;
    }
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
