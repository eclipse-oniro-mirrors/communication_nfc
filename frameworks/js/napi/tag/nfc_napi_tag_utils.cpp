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

#include "nfc_napi_tag_utils.h"
#include <cstring>
#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "securec.h"

namespace OHOS {
namespace NFC {
namespace KITS {
bool ParseString(napi_env env, std::string &param, napi_value args)
{
    napi_valuetype valuetype;
    napi_typeof(env, args, &valuetype);

    DebugLog("param=%{public}d.", valuetype);
    if (valuetype != napi_string) {
        DebugLog("Wrong argument type. String expected.");
        return false;
    }
    size_t size = 0;

    if (napi_get_value_string_utf8(env, args, nullptr, 0, &size) != napi_ok) {
        ErrorLog("can not get string size");
        param = "";
        return false;
    }
    param.reserve(size + 1);
    param.resize(size);
    if (napi_get_value_string_utf8(env, args, param.data(), (size + 1), &size) != napi_ok) {
        ErrorLog("can not get string value");
        param = "";
        return false;
    }
    return true;
}
bool ParseInt32(napi_env env, int32_t &param, napi_value args)
{
    napi_valuetype valuetype;
    napi_typeof(env, args, &valuetype);

    DebugLog("ParseInt32, valuetype %{public}d.", valuetype);
    if (valuetype != napi_number) {
        ErrorLog("Wrong argument type. Int32 expected.");
        return false;
    }
    napi_get_value_int32(env, args, &param);
    return true;
}

bool ParseBool(napi_env env, bool &param, napi_value args)
{
    napi_valuetype valuetype;
    napi_typeof(env, args, &valuetype);

    DebugLog("param=%{public}d.", valuetype);
    if (valuetype != napi_boolean) {
        ErrorLog("Wrong argument type. bool expected.");
        return false;
    }
    napi_get_value_bool(env, args, &param);
    return true;
}

bool ParseBytesVector(napi_env env, std::vector<unsigned char> &vec, napi_value args)
{
    bool isArray = false;
    napi_status status = napi_is_array(env, args, &isArray);
    if (status != napi_ok || !isArray) {
        ErrorLog("ParseBytesVector, not array");
        return false;
    }
    uint32_t arrayLength = 0;
    napi_get_array_length(env, args, &arrayLength);
    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_value element = nullptr;
        napi_get_element(env, args, i, &element);

        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, element, &valueType);
        if (valueType != napi_number) {
            ErrorLog("ParseBytesVector, not number!");
            return false;
        }

        uint32_t byteValue = 0x0;
        napi_get_value_uint32(env, element, &byteValue);
        vec.push_back(static_cast<unsigned char>(byteValue));
    }
    return true;
}

bool ParseArrayBuffer(napi_env env, uint8_t **data, size_t &size, napi_value args)
{
    napi_status status;
    napi_valuetype valuetype;
    napi_typeof(env, args, &valuetype);

    DebugLog("param=%{public}d.", valuetype);
    if (valuetype != napi_object) {
        ErrorLog("Wrong argument type. object expected.");
        return false;
    }

    status = napi_get_arraybuffer_info(env, args, reinterpret_cast<void **>(data), &size);
    if (status != napi_ok) {
        ErrorLog("can not get arraybuffer, error is %{public}d", status);
        (*data)[0] = 0;
        return false;
    }
    DebugLog("arraybuffer size is %{public}zu,buffer is %{public}d", size, (*data)[0]);
    return true;
}

napi_value UndefinedNapiValue(const napi_env &env)
{
    napi_value result;
    napi_get_undefined(env, &result);
    return result;
}

std::vector<std::string> ConvertStringVector(napi_env env, napi_value jsValue)
{
    bool isTypedArray = false;
    napi_status status = napi_is_typedarray(env, jsValue, &isTypedArray);
    if (status != napi_ok || !isTypedArray) {
        ErrorLog("%{public}s called, napi_is_typedarray error", __func__);
        return {};
    }

    napi_typedarray_type type;
    size_t length = 0;
    napi_value buffer = nullptr;
    size_t offset = 0;
    NAPI_CALL_BASE(env, napi_get_typedarray_info(env, jsValue, &type, &length, nullptr, &buffer, &offset), {});
    if (type != napi_uint8_array) {
        ErrorLog("%{public}s called, napi_uint8_array is null", __func__);
        return {};
    }
    std::string *data = nullptr;
    size_t total = 0;
    NAPI_CALL_BASE(env, napi_get_arraybuffer_info(env, buffer, reinterpret_cast<void **>(&data), &total), {});
    length = std::min<size_t>(length, total - offset);
    std::vector<std::string> result(sizeof(std::string) + length);
    int retCode = memcpy_s(result.data(), result.size(), &data[offset], length);
    if (retCode != 0) {
        return {};
    }
    return result;
}

napi_value CreateErrorMessage(napi_env env, const std::string &msg, int32_t errorCode)
{
    napi_value result = nullptr;
    napi_value message = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, msg.c_str(), msg.length(), &message));
    napi_value codeValue = nullptr;
    std::string errCode = std::to_string(errorCode);
    NAPI_CALL(env, napi_create_string_utf8(env, errCode.c_str(), errCode.length(), &codeValue));
    NAPI_CALL(env, napi_create_error(env, codeValue, message, &result));
    return result;
}

napi_value CreateUndefined(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

std::string GetNapiStringValue(
    napi_env env, napi_value napiValue, const std::string &name, const std::string &defValue)
{
    napi_value value = GetNamedProperty(env, napiValue, name);
    if (value != nullptr) {
        return GetStringFromValue(env, value);
    } else {
        return defValue;
    }
}
std::string GetStringFromValue(napi_env env, napi_value value)
{
    constexpr int32_t MAX_TEXT_LENGTH = 4096;
    char msgChars[MAX_TEXT_LENGTH] = {0};
    size_t msgLength = 0;
    NAPI_CALL_BASE(env, napi_get_value_string_utf8(env, value, msgChars, MAX_TEXT_LENGTH, &msgLength), "");
    if (msgLength > 0) {
        return std::string(msgChars, 0, msgLength);
    } else {
        return "";
    }
}

napi_value GetNamedProperty(napi_env env, napi_value object, const std::string &propertyName)
{
    napi_value value = nullptr;
    bool hasProperty = false;
    NAPI_CALL(env, napi_has_named_property(env, object, propertyName.data(), &hasProperty));
    if (hasProperty) {
        NAPI_CALL(env, napi_get_named_property(env, object, propertyName.data(), &value));
    }
    return value;
}

int32_t GetNapiInt32Value(napi_env env, napi_value napiValue, const std::string &name, const int32_t &defValue)
{
    napi_value value = GetNamedProperty(env, napiValue, name);
    if (value != nullptr) {
        int32_t intValue = 0;
        napi_status getIntStatus = napi_get_value_int32(env, value, &intValue);
        if (getIntStatus == napi_ok) {
            return intValue;
        }
    }
    return defValue;
}

std::string UnwrapStringFromJS(napi_env env, napi_value param)
{
    constexpr size_t MAX_TEXT_LENGTH = 1024;
    char msgChars[MAX_TEXT_LENGTH] = {0};
    size_t msgLength = 0;
    NAPI_CALL_BASE(env, napi_get_value_string_utf8(env, param, msgChars, MAX_TEXT_LENGTH, &msgLength), "");
    DebugLog("NapiUtil GetStringFromValue msgLength = %{public}zu", msgLength);
    if (msgLength > 0) {
        return std::string(msgChars, 0, msgLength);
    } else {
        return "";
    }
}

void JsStringToBytesVector(napi_env env, napi_value &src, std::vector<unsigned char> &values)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, src, &valueType);
    if (valueType != napi_string) {
        return;
    }
    
    std::string data;
    ParseString(env, data, src);
    NfcSdkCommon::HexStringToBytes(data, values);
}

void ConvertStringVectorToJS(napi_env env, napi_value &result, std::vector<std::string>& stringVector)
{
    DebugLog("ConvertStringVectorToJS called");
    size_t idx = 0;

    if (stringVector.empty()) {
        WarnLog("ConvertStringVectorToJS stringVector empty");
        napi_create_array_with_length(env, 0, &result);
        return;
    }
    DebugLog("ConvertStringVectorToJS size is %{public}zu", stringVector.size());
    for (auto& str : stringVector) {
        napi_value obj = nullptr;
        napi_create_string_utf8(env, str.c_str(), NAPI_AUTO_LENGTH, &obj);
        napi_set_element(env, result, idx, obj);
        idx++;
    }
}

void BytesVectorToJS(napi_env env, napi_value &result, std::vector<unsigned char>& src)
{
    if (src.empty()) {
        WarnLog("BytesVectorToJS src empty");
        napi_create_array_with_length(env, 0, &result);
        return;
    }
    size_t idx = 0;
    DebugLog("BytesVectorToJS size is %{public}zu", src.size());
    napi_create_array_with_length(env, src.size(), &result);
    for (auto& num : src) {
        napi_value obj = nullptr;
        napi_create_uint32(env, num, &obj);
        napi_set_element(env, result, idx, obj);
        idx++;
    }
}

void ConvertStringToNumberArray(napi_env env, napi_value &result, std::string srcValue)
{
    if (srcValue.empty()) {
        WarnLog("ConvertStringToNumberArray srcValue empty");
        napi_create_array_with_length(env, 0, &result);
        return;
    }
    uint32_t strLength = srcValue.length();
    if (strLength % HEX_BYTE_LEN != 0) {
        srcValue = '0' + srcValue;
        strLength++;
    }

    napi_create_array_with_length(env, (strLength / HEX_BYTE_LEN), &result);
    unsigned int srcIntValue;
    for (uint32_t i = 0; i < strLength; i += HEX_BYTE_LEN) {
        // parse the hex string bytes into array.
        std::string oneByte = srcValue.substr(i, HEX_BYTE_LEN);
        if (sscanf_s(oneByte.c_str(), "%x", &srcIntValue) <= 0) {
            ErrorLog("ConvertStringToNumberArray, sscanf_s failed.");
            return;
        }
        unsigned char hexByte = static_cast<unsigned char>(srcIntValue & 0xFF);
        napi_value hexByteValue = nullptr;
        napi_create_int32(env, hexByte, &hexByteValue);
        napi_set_element(env, result, (i / HEX_BYTE_LEN), hexByteValue);
    }
}

void ConvertNdefRecordVectorToJS(napi_env env, napi_value &result,
                                 std::vector<std::shared_ptr<NdefRecord>> &ndefRecords)
{
    napi_create_array(env, &result);
    if (ndefRecords.empty()) {
        WarnLog("ConvertNdefRecordVectorToJS ndefRecords is empty.");
        return;
    }
    size_t idx = 0;
    for (auto& ndefRecord : ndefRecords) {
        napi_value obj = nullptr;
        ConvertNdefRecordToJS(env, obj, ndefRecord);
        napi_set_element(env, result, idx, obj);
        idx++;
    }
}

void ConvertNdefRecordToJS(napi_env env, napi_value &result, std::shared_ptr<NdefRecord> &ndefRecord)
{
    napi_create_object(env, &result);
    if (ndefRecord == nullptr) {
        WarnLog("ConvertNdefRecordToJS ndefRecord is null.");
        return;
    }

    napi_value tnf;
    napi_create_int32(env, ndefRecord->tnf_, &tnf);
    napi_set_named_property(env, result, "tnf", tnf);

    napi_value rtdType;
    napi_create_string_utf8(env, ndefRecord->tagRtdType_.c_str(), NAPI_AUTO_LENGTH, &rtdType);
    napi_set_named_property(env, result, "rtdType", rtdType);

    napi_value id;
    napi_create_string_utf8(env, ndefRecord->id_.c_str(), NAPI_AUTO_LENGTH, &id);
    napi_set_named_property(env, result, "id", id);

    napi_value payload;
    napi_create_string_utf8(env, ndefRecord->payload_.c_str(), NAPI_AUTO_LENGTH, &payload);
    napi_set_named_property(env, result, "payload", payload);
}

bool MatchValueType(napi_env env, napi_value value, napi_valuetype targetType)
{
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL_BASE(env, napi_typeof(env, value, &valueType), false);
    return valueType == targetType;
}

bool MatchParameters(napi_env env, const napi_value parameters[], std::initializer_list<napi_valuetype> valueTypes)
{
    if (parameters == nullptr) {
        return false;
    }
    int i = 0;
    for (auto beg = valueTypes.begin(); beg != valueTypes.end(); ++beg) {
        if (!MatchValueType(env, parameters[i], *beg)) {
            return false;
        }
        ++i;
    }
    return true;
}

napi_value HandleAsyncWork(napi_env env, BaseContext *baseContext, const std::string &workName,
    napi_async_execute_callback execute, napi_async_complete_callback complete)
{
    DebugLog("NfcUtil HandleAsyncWork workName = %{public}s", workName.c_str());
    std::unique_ptr<BaseContext> context(baseContext);
    if (context == nullptr) {
        std::string errorCode = std::to_string(napi_invalid_arg);
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), ERR_INIT_CONTEXT.c_str()));
    }
    napi_value result = nullptr;
    if (context->callbackRef == nullptr) {
        NAPI_CALL(env, napi_create_promise(env, &context->deferred, &result));
    } else {
        NAPI_CALL(env, napi_get_undefined(env, &result));
    }
    napi_value resource = CreateUndefined(env);
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, workName.data(), NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env,
        napi_create_async_work(env, resource, resourceName, execute, complete, static_cast<void *>(context.get()),
            &context->work));
    napi_status queueWorkStatus = napi_queue_async_work(env, context->work);
    if (queueWorkStatus == napi_ok) {
        context.release();
        DebugLog("NapiUtil HandleAsyncWork napi_queue_async_work ok");
    } else {
        std::string errorCode = std::to_string(queueWorkStatus);
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), ERR_INIT_CONTEXT.c_str()));
    }
    DebugLog("NfcUtil HandleAsyncWork end");
    return result;
}

void DoAsyncCallbackOrPromise(const napi_env &env, BaseContext *baseContext, napi_value callbackValue)
{
    if (baseContext == nullptr) {
        ErrorLog("DoAsyncCallbackOrPromise serious error baseContext nullptr");
        return;
    }
    if (baseContext->callbackRef != nullptr) {
        DebugLog("DoAsyncCallbackOrPromise for callback");
        napi_value recv = CreateUndefined(env);
        napi_value callbackFunc = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, baseContext->callbackRef, &callbackFunc));
        napi_value callbackValues[] = {nullptr, nullptr};
        callbackValues[0] = baseContext->resolved ? CreateUndefined(env) : callbackValue;
        callbackValues[1] = baseContext->resolved ? callbackValue : CreateUndefined(env);
        napi_value result = nullptr;
        NAPI_CALL_RETURN_VOID(
            env, napi_call_function(env, recv, callbackFunc, std::size(callbackValues), callbackValues, &result));
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, baseContext->callbackRef));
    } else if (baseContext->deferred != nullptr) {
        DebugLog("DoAsyncCallbackOrPromise for promise");
        if (baseContext->resolved) {
            NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, baseContext->deferred, callbackValue));
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, baseContext->deferred, callbackValue));
        }
    }
    napi_delete_async_work(env, baseContext->work);
    delete baseContext;
    baseContext = nullptr;
}

void ThrowAsyncError(const napi_env &env, BaseContext *baseContext, int errCode, const std::string &errMsg)
{
    napi_value businessError = CreateErrorMessage(env, errMsg, errCode);
    if (baseContext->callbackRef != nullptr) {
        DebugLog("ThrowAsyncError for callback");
        napi_value recv = CreateUndefined(env);
        napi_value callbackFunc = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, baseContext->callbackRef, &callbackFunc));
        napi_value callbackValues[] = {nullptr, nullptr};
        callbackValues[0] = businessError; // parameter "error"
        callbackValues[1] = CreateUndefined(env); // parameter "callback"
        napi_value result = nullptr;
        NAPI_CALL_RETURN_VOID(
            env, napi_call_function(env, recv, callbackFunc, std::size(callbackValues), callbackValues, &result));
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, baseContext->callbackRef));
    } else if (baseContext->deferred != nullptr) {
        DebugLog("ThrowAsyncError for promise");
        NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, baseContext->deferred, businessError));
    }

    if (baseContext != nullptr) {
        if (baseContext->work != nullptr) {
            napi_delete_async_work(env, baseContext->work);
        }
        delete baseContext;
        baseContext = nullptr;
    }
}

bool IsNumberArray(const napi_env &env, const napi_value &param)
{
    if (!IsArray(env, param)) {
        return false;
    }

    uint32_t arrayLength = 0;
    napi_get_array_length(env, param, &arrayLength);
    napi_value elementValue = nullptr;
    for (uint32_t i = 0; i < arrayLength; ++i) {
        napi_get_element(env, param, i, &elementValue);
        napi_valuetype elementType = napi_undefined;
        napi_typeof(env, elementValue, &elementType);
        if (elementType != napi_number) {
            return false;
        }
    }
    return true;
}

bool IsObjectArray(const napi_env &env, const napi_value &param)
{
    if (!IsArray(env, param)) {
        return false;
    }

    uint32_t arrayLength = 0;
    napi_get_array_length(env, param, &arrayLength);
    napi_value elementValue = nullptr;
    for (uint32_t i = 0; i < arrayLength; ++i) {
        napi_get_element(env, param, i, &elementValue);
        napi_valuetype elementType = napi_undefined;
        napi_typeof(env, elementValue, &elementType);
        if (elementType != napi_object) {
            return false;
        }
    }
    return true;
}

bool IsArray(const napi_env &env, const napi_value &param)
{
    bool arrayType = false;
    napi_status status = napi_is_array(env, param, &arrayType);
    if (status != napi_ok || !arrayType) {
        return false;
    }
    
    uint32_t arrayLength = 0;
    napi_get_array_length(env, param, &arrayLength);
    if (arrayLength == 0) {
        return false;
    }
    return true;
}

bool IsNumber(const napi_env &env, const napi_value &param)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, param, &valueType);
    return valueType == napi_number;
}

bool IsString(const napi_env &env, const napi_value &param)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, param, &valueType);
    return valueType == napi_string;
}

bool IsObject(const napi_env &env, const napi_value &param)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, param, &valueType);
    return valueType == napi_object;
}

int BuildOutputErrorCode(int errCode)
{
    if (errCode == BUSI_ERR_PERM) {
        return BUSI_ERR_PERM;
    } else if (errCode == BUSI_ERR_PARAM) {
        return BUSI_ERR_PARAM;
    } else if (errCode >= ERR_TAG_BASE && errCode < ERR_CE_BASE) {
        return BUSI_ERR_TAG_STATE_INVALID;
    }
    return errCode;
}

std::string BuildErrorMessage(int errCode, std::string funcName, std::string forbiddenPerm,
    std::string paramName, std::string expertedType)
{
    std::string errMsg;
    if (errCode == BUSI_ERR_PERM) {
        return errMsg.append("Permission denied. An attempt was made to ${")
            .append(funcName)
            .append("} forbidden by permission: ${")
            .append(forbiddenPerm)
            .append("}.");
    } else if (errCode == BUSI_ERR_PARAM) {
        if (paramName.length() > 0) {
            return errMsg.append("Parameter error. The type of \"${")
                .append(paramName)
                .append("}\" must be ${")
                .append(expertedType)
                .append("}.");
        } else {
            return "Parameter error. The parameter number is invalid.";
        }
    } else if (errCode == BUSI_ERR_TAG_STATE_INVALID) {
        return "Tag running state is abnormal in service.";
    }
    return "Unknown error message";
}

napi_value GenerateBusinessError(const napi_env &env, int errCode, const std::string &errMessage)
{
    napi_value code = nullptr;
    napi_create_uint32(env, errCode, &code);
    napi_value message = nullptr;
    napi_create_string_utf8(env, errMessage.c_str(), NAPI_AUTO_LENGTH, &message);
    napi_value businessError = nullptr;
    napi_create_error(env, nullptr, message, &businessError);
    napi_set_named_property(env, businessError, KEY_CODE.c_str(), code);
    return businessError;
}

bool CheckUnwrapStatusAndThrow(const napi_env &env, napi_status status, int errCode)
{
    if (status != napi_ok) {
        napi_throw(env, GenerateBusinessError(env, errCode, BuildErrorMessage(errCode, "", "", "", "")));
        return false;
    }
    return true;
}
bool CheckContextAndThrow(const napi_env &env, const BaseContext *context, int errCode)
{
    if (context == nullptr) {
        napi_throw(env, GenerateBusinessError(env, errCode, BuildErrorMessage(errCode, "", "", "", "")));
        return false;
    }
    return true;
}
bool CheckParametersAndThrow(const napi_env &env, const napi_value parameters[],
    std::initializer_list<napi_valuetype> types, const std::string &argName, const std::string &argType)
{
    if (!MatchParameters(env, parameters, types)) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM, BuildErrorMessage(BUSI_ERR_PARAM,
            "", "", argName, argType)));
        return false;
    }
    return true;
}
bool CheckArrayNumberAndThrow(const napi_env &env, const napi_value &param, const std::string &argName,
    const std::string &argType)
{
    if (!IsNumberArray(env, param)) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM, BuildErrorMessage(BUSI_ERR_PARAM,
            "", "", argName, argType)));
        return false;
    }
    return true;
}
bool CheckNumberAndThrow(const napi_env &env, const napi_value &param, const std::string &argName,
    const std::string &argType)
{
    if (!IsNumber(env, param)) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM, BuildErrorMessage(BUSI_ERR_PARAM,
            "", "", argName, argType)));
        return false;
    }
    return true;
}
bool CheckStringAndThrow(const napi_env &env, const napi_value &param, const std::string &argName,
    const std::string &argType)
{
    if (!IsString(env, param)) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM, BuildErrorMessage(BUSI_ERR_PARAM,
            "", "", argName, argType)));
        return false;
    }
    return true;
}
bool CheckObjectAndThrow(const napi_env &env, const napi_value &param, const std::string &argName,
    const std::string &argType)
{
    if (!IsObject(env, param)) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM, BuildErrorMessage(BUSI_ERR_PARAM,
            "", "", argName, argType)));
        return false;
    }
    return true;
}
bool CheckArgCountAndThrow(const napi_env &env, int argCount, int expCount)
{
    if (argCount != expCount) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM, BuildErrorMessage(BUSI_ERR_PARAM,
            "", "", "", "")));
        return false;
    }
    return true;
}
bool CheckTagStatusCodeAndThrow(const napi_env &env, int statusCode, const std::string &funcName)
{
    if (statusCode == BUSI_ERR_PERM) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PERM,
            BuildErrorMessage(BUSI_ERR_PERM, funcName, TAG_PERM_DESC, "", "")));
        return false;
    } else if (statusCode >= ErrorCode::ERR_TAG_BASE && statusCode < ErrorCode::ERR_CE_BASE) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_TAG_STATE_INVALID,
            BuildErrorMessage(BUSI_ERR_TAG_STATE_INVALID, "", "", "", "")));
        return false;
    }
    return true;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
