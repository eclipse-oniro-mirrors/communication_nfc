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

#ifndef NFC_NAPI_CTRL_UTILS_H_
#define NFC_NAPI_CTRL_UTILS_H_
#include <chrono>
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace NFC {
namespace KITS {
// business error code, throw these errors to applcation.
const static int BUSI_ERR_PERM = 201; // Permission denied.
const static int BUSI_ERR_PARAM = 401; // The parameter check failed.
const static int BUSI_ERR_CAPABILITY = 801; // Capability not supported.
const static int BUSI_ERR_NFC_STATE_INVALID = 3100101; // nfc state is abnormal.

const std::string KEY_CODE = "code";
const std::string NFC_PERM_DESC = "ohos.permission.MANAGE_SECURE_SETTINGS";

napi_value CreateUndefined(napi_env env);

std::string BuildErrorMessage(int errCode, std::string funcName, std::string forbiddenPerm,
    std::string paramName, std::string expertedType);
napi_value GenerateBusinessError(const napi_env &env, int errCode, const std::string &errMessage);

bool CheckNfcStatusCodeAndThrow(const napi_env &env, int statusCode, const std::string funcName);
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif