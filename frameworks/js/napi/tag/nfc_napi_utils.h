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

#ifndef NFC_NAPI_UTILS_H_
#define NFC_NAPI_UTILS_H_

#include <chrono>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "ndef_message.h"

namespace OHOS {
namespace NFC {
namespace KITS {
enum NapiError : int32_t {
    ERROR_NONE = 0,
    ERROR_DEFAULT = -1,
    ERROR_SERVICE_UNAVAILABLE = -2,
    ERROR_PARAMETER_VALUE_INVALID = -3,
    ERROR_PARAMETER_COUNTS_INVALID = -4,
    ERROR_PARAMETER_TYPE_INVALID = -5,
    ERROR_NATIVE_API_EXECUTE_FAIL = -6,
};

enum JS_CALLBACK_ARGV : size_t {
    CALLBACK_ARGV_INDEX_0 = 0,
    CALLBACK_ARGV_INDEX_1,
    CALLBACK_ARGV_CNT,
};

enum JS_ARGV_NUM : size_t {
    ARGV_NUM_0 = 0,
    ARGV_NUM_1 = 1,
    ARGV_NUM_2 = 2,
    ARGV_NUM_3 = 3,
    ARGV_NUM_4 = 4,
    ARGV_NUM_5 = 5,
};

enum JS_ARGV_INDEX : size_t {
    ARGV_INDEX_0 = 0,
    ARGV_INDEX_1,
    ARGV_INDEX_2,
    ARGV_INDEX_3,
    ARGV_INDEX_4,
};

struct NfcAsyncContext {
    napi_async_work work = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callbackRef = nullptr;
    int32_t result;
    int32_t uid = 0;
    bool flag = false;
};

struct BaseContext {
    napi_async_work work = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callbackRef = nullptr;
    bool resolved = false;
    int32_t errorCode = ERROR_DEFAULT;
};

class AsyncContext {
public:
    napi_env env;
    napi_async_work work;
    napi_deferred deferred;
    napi_ref callback[2] = {0};
    std::function<void(void *)> executeFunc;
    std::function<void(void *)> completeFunc;
    napi_value resourceName;
    napi_value result;
    int errorCode;

    explicit AsyncContext(napi_env e, napi_async_work w = nullptr, napi_deferred d = nullptr)
    {
        env = e;
        work = w;
        deferred = d;
        executeFunc = nullptr;
        completeFunc = nullptr;
        result = nullptr;
        errorCode = ERROR_NONE;
    }

    AsyncContext() = delete;

    virtual ~AsyncContext() {}
};

template<typename T, typename D>
struct CallBackContext : BaseContext {
    T value;
    D *objectInfo;
};
template<typename T, std::enable_if_t<std::is_same_v<T, bool>, int32_t> = 0>
napi_value GetNapiValue(napi_env env, T val)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_boolean(env, val, &result));
    return result;
}

template<typename T, std::enable_if_t<std::is_same_v<T, int32_t>, int32_t> = 0>
napi_value GetNapiValue(napi_env env, T val)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, val, &result));
    return result;
}

template<typename T, std::enable_if_t<std::is_same_v<T, int64_t>, int64_t> = 0>
napi_value GetNapiValue(napi_env env, T val)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int64(env, val, &result));
    return result;
}

template<typename T, std::enable_if_t<std::is_same_v<T, std::string>, int32_t> = 0>
napi_value GetNapiValue(napi_env env, const T &val)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, val.c_str(), val.length(), &result));
    return result;
}

template<typename T, std::enable_if_t<std::is_same_v<T, char>, int32_t> = 0>
napi_value GetNapiValue(napi_env env, const T *val)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_string_utf8(env, val, NAPI_AUTO_LENGTH, &result));
    return result;
}

template<typename T, std::enable_if_t<std::is_same_v<T, napi_value>, int32_t> = 0>
napi_value GetNapiValue(napi_env env, T val)
{
    return val;
}

bool ParseString(napi_env env, std::string &param, napi_value args);
bool ParseInt32(napi_env env, int32_t &param, napi_value args);
bool ParseBool(napi_env env, bool &param, napi_value args);
bool ParseArrayBuffer(napi_env env, uint8_t **data, size_t &size, napi_value args);
std::vector<std::string> ConvertStringVector(napi_env env, napi_value jsValue);
napi_value CreateErrorMessage(napi_env env, std::string message, int32_t errorCode = ERROR_DEFAULT);
napi_value CreateUndefined(napi_env env);
std::string GetNapiStringValue(
    napi_env env, napi_value napiValue, const std::string &name, const std::string &defValue = "");
std::string GetStringFromValue(napi_env env, napi_value value);
napi_value GetNamedProperty(napi_env env, napi_value object, const std::string &propertyName);
int32_t GetNapiInt32Value(napi_env env, napi_value napiValue, const std::string &name, const int32_t &defValue = 0);
std::string UnwrapStringFromJS(napi_env env, napi_value param);
void ConvertStringVectorToJS(napi_env env, napi_value &result, std::vector<std::string> &stringVector);
void ConvertIntVectorToJS(napi_env env, napi_value &result, std::vector<int> &intVector);
void ConvertStringToNumberArray(napi_env env, napi_value &result, std::string srcValue);
void ConvertNdefRecordVectorToJS(napi_env env, napi_value &result,
                                 std::vector<std::shared_ptr<NdefRecord>> &ndefRecords);
void ConvertNdefRecordToJS(napi_env env, napi_value &result, std::shared_ptr<NdefRecord> &ndefRecord);
bool MatchParameters(napi_env env, const napi_value parameters[], std::initializer_list<napi_valuetype> valueTypes);
napi_value HandleAsyncWork(napi_env env, BaseContext *context, const std::string &workName,
    napi_async_execute_callback execute, napi_async_complete_callback complete);
void Handle1ValueCallback(napi_env env, BaseContext *context, napi_value callbackValue);
void Handle2ValueCallback(napi_env env, BaseContext *context, napi_value callbackValue);
void DefineEnumClassByName(napi_env env, napi_value exports, std::string_view enumName, size_t arrSize,
    const napi_property_descriptor *desc);
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif