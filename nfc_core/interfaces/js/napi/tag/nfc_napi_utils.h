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

#ifndef NFC_NAPI_UTILS_H_
#define NFC_NAPI_UTILS_H_

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace NFC {
namespace KITS {
enum class JS_CALLBACK_ARGV {
    CALLBACK_ARGV_INDEX_0 = 0,
    CALLBACK_ARGV_INDEX_1,
    CALLBACK_ARGV_CNT,
};

enum class JS_ARGV_NUM {
    ARGV_NUM_0 = 0,
    ARGV_NUM_1,
    ARGV_NUM_2,
    ARGV_NUM_3,
    ARGV_NUM_4,
    ARGV_NUM_5,
};

enum class JS_ARGV_INDEX {
    ARGV_INDEX_0 = 0,
    ARGV_INDEX_1,
    ARGV_INDEX_2,
    ARGV_INDEX_3,
    ARGV_INDEX_4,
};

std::vector<std::string> ConvertStringVector(napi_env env, napi_value jsValue);
napi_value CreateUndefined(napi_env env);
std::string GetNapiStringValue(
    napi_env env, napi_value napiValue, const std::string &name, const std::string &defValue = "");
std::string GetStringFromValue(napi_env env, napi_value value);
napi_value GetNamedProperty(napi_env env, napi_value object, const std::string &propertyName);
int32_t GetNapiInt32Value(napi_env env, napi_value napiValue, const std::string &name, const int32_t &defValue = 0);
std::string UnwrapStringFromJS(napi_env env, napi_value param);
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif