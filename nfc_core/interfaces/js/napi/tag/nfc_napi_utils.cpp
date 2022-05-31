/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
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
std::vector<std::string> ConvertStringVector(napi_env env, napi_value jsValue)
{
    bool isTypedArray = false;
    napi_status status = napi_is_typedarray(env, jsValue, &isTypedArray);
    if (status != napi_ok || !isTypedArray) {
        DebugLog("%{public}s called, napi_is_typedarray error", __func__);
        return {};
    }

    napi_typedarray_type type;
    size_t length = 0;
    napi_value buffer = nullptr;
    size_t offset = 0;
    NAPI_CALL_BASE(env, napi_get_typedarray_info(env, jsValue, &type, &length, nullptr, &buffer, &offset), {});
    if (type != napi_uint8_array) {
        DebugLog("%{public}s called, napi_uint8_array is null", __func__);
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
} // namespace KITS
} // namespace NFC
} // namespace OHOS
