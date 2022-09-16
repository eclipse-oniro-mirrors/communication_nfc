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

#include "nfc_napi_utils.h"

#include <cstring>
#include "loghelper.h"
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

    status = napi_get_arraybuffer_info(env, args, (void **)data, &size);
    if (status != napi_ok) {
        ErrorLog("can not get arraybuffer, error is %{public}d", status);
        (*data)[0] = ERROR_DEFAULT;
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

napi_value CreateErrorMessage(napi_env env, std::string msg, int32_t errorCode)
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

std::string UnwrapStringFromJS(napi_env env, napi_value arg)
{
    constexpr size_t MAX_TEXT_LENGTH = 1024;
    char msgChars[MAX_TEXT_LENGTH] = {0};
    size_t msgLength = 0;
    NAPI_CALL_BASE(env, napi_get_value_string_utf8(env, arg, msgChars, MAX_TEXT_LENGTH, &msgLength), "");
    DebugLog("NapiUtil GetStringFromValue msgLength = %{public}zu", msgLength);
    if (msgLength > 0) {
        return std::string(msgChars, 0, msgLength);
    } else {
        return "";
    }
}

void ConvertStringVectorToJS(napi_env env, napi_value result, std::vector<std::string>& stringVector)
{
    DebugLog("ConvertStringVectorToJS called");
    size_t idx = 0;

    if (stringVector.empty()) {
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

void ConvertIntVectorToJS(napi_env env, napi_value result, std::vector<int>& intVector)
{
    DebugLog("ConvertIntVectorToJS called");
    size_t idx = 0;

    if (intVector.empty()) {
        return;
    }
    DebugLog("ConvertIntVectorToJS size is %{public}zu", intVector.size());
    for (auto& num : intVector) {
        napi_value obj = nullptr;
        napi_create_int32(env, num, &obj);
        napi_set_element(env, result, idx, obj);
        idx++;
    }
}

void ConvertUsignedCharVectorToJS(napi_env env, napi_value result, std::vector<unsigned char> &unsignedCharVector)
{
    DebugLog("ConvertUsignedCharVectorToJS called");
    size_t idx = 0;

    if (unsignedCharVector.empty()) {
        return;
    }
    DebugLog("ConvertUsignedCharVectorToJS size is %{public}zu", unsignedCharVector.size());
    for (auto& num : unsignedCharVector) {
        napi_value obj = nullptr;
        napi_create_int32(env, num, &obj);
        napi_set_element(env, result, idx, obj);
        idx++;
    }
}

void ConvertNdefRecordVectorToJS(napi_env env, napi_value result, std::vector<std::shared_ptr<NdefRecord>> &ndefRecords)
{
    DebugLog("ConvertNdefRecordVectorToJS called");
    size_t idx = 0;

    if (ndefRecords.empty()) {
        DebugLog("ConvertNdefRecordVectorToJS ndefRecords is empty.");
        return;
    }
    DebugLog("ConvertNdefRecordVectorToJS size is %{public}zu", ndefRecords.size());
    for (auto& ndefRecord : ndefRecords) {
        napi_value obj = nullptr;
        napi_create_object(env, &obj);
        ConvertNdefRecordToJS(env, obj, ndefRecord);
        napi_set_element(env, result, idx, obj);
        idx++;
    }
}

void ConvertNdefRecordToJS(napi_env env, napi_value result, std::shared_ptr<NdefRecord> &ndefRecord)
{
    DebugLog("ConvertNdefRecordToJS called");

    ndefRecord = std::make_shared<NdefRecord>();

    napi_value tnf;
    napi_create_int32(env, ndefRecord->tnf_, &tnf);
    napi_set_named_property(env, result, "tnf", tnf);
    DebugLog("ConvertNdefRecordToJS tnf is %{public}zu", ndefRecord->tnf_);

    napi_value rtdType;
    napi_create_string_utf8(env, ndefRecord->payload_.c_str(), NAPI_AUTO_LENGTH, &rtdType);
    napi_set_named_property(env, result, "rtdType", rtdType);
    DebugLog("ConvertGattServiceToJS rtdType is %{public}s", ndefRecord->payload_.c_str());

    napi_value id;
    napi_create_string_utf8(env, ndefRecord->id_.c_str(), NAPI_AUTO_LENGTH, &id);
    napi_set_named_property(env, result, "id", id);
    DebugLog("ConvertGattServiceToJS id is %{public}s", ndefRecord->id_.c_str());

    napi_value payload;
    napi_create_string_utf8(env, ndefRecord->payload_.c_str(), NAPI_AUTO_LENGTH, &payload);
    napi_set_named_property(env, result, "payload", payload);
    DebugLog("ConvertGattServiceToJS payload is %{public}s", ndefRecord->payload_.c_str());
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
        std::string errorMessage = "error at baseContext is nullptr";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
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
        napi_create_async_work(env, resource, resourceName, execute, complete, (void *)context.get(), &context->work));
    napi_status queueWorkStatus = napi_queue_async_work(env, context->work);
    if (queueWorkStatus == napi_ok) {
        context.release();
        DebugLog("NapiUtil HandleAsyncWork napi_queue_async_work ok");
    } else {
        std::string errorCode = std::to_string(queueWorkStatus);
        std::string errorMessage = "error at napi_queue_async_work";
        NAPI_CALL(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
    }
    DebugLog("NfcUtil HandleAsyncWork end");
    return result;
}

void Handle1ValueCallback(napi_env env, BaseContext *baseContext, napi_value callbackValue)
{
    DebugLog("Handle1ValueCallback start");
    if (baseContext == nullptr) {
        std::string errorCode = std::to_string(napi_invalid_arg);
        std::string errorMessage = "error at baseContext is nullptr";
        NAPI_CALL_RETURN_VOID(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
    }
    if (baseContext->callbackRef != nullptr) {
        DebugLog("Handle1ValueCallback start normal callback");
        napi_value recv = CreateUndefined(env);
        napi_value callbackFunc = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, baseContext->callbackRef, &callbackFunc));
        napi_value callbackValues[] = {callbackValue};
        napi_value result = nullptr;
        NAPI_CALL_RETURN_VOID(
            env, napi_call_function(env, recv, callbackFunc, std::size(callbackValues), callbackValues, &result));
        NAPI_CALL_RETURN_VOID(env, napi_delete_reference(env, baseContext->callbackRef));
    } else if (baseContext->deferred != nullptr) {
        DebugLog("Handle1ValueCallback start promise callback");
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

void Handle2ValueCallback(napi_env env, BaseContext *baseContext, napi_value callbackValue)
{
    DebugLog("Handle2ValueCallback start");
    if (baseContext == nullptr) {
        DebugLog("Handle2ValueCallback serious error baseContext nullptr");
        std::string errorCode = std::to_string(napi_invalid_arg);
        std::string errorMessage = "error at baseContext is nullptr";
        NAPI_CALL_RETURN_VOID(env, napi_throw_error(env, errorCode.c_str(), errorMessage.c_str()));
        return;
    }
    if (baseContext->callbackRef != nullptr) {
        DebugLog("Handle2ValueCallback start normal callback");
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

void DefineEnumClassByName(
    napi_env env, napi_value exports, std::string_view enumName, size_t arrSize, const napi_property_descriptor *desc)
{
    auto construct = [](napi_env env, napi_callback_info info) -> napi_value { return nullptr; };
    napi_value result = nullptr;
    napi_status status =
        napi_define_class(env, enumName.data(), NAPI_AUTO_LENGTH, construct, nullptr, arrSize, desc, &result);
    if (status != napi_ok) {
        ErrorLog("DefineEnumClassByName napi_define_class failed ret = %{public}d", status);
    }
    status = napi_set_named_property(env, exports, enumName.data(), result);
    if (status != napi_ok) {
        ErrorLog("DefineEnumClassByName napi_set_named_property failed ret = %{public}d", status);
    }
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
