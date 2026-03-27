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

#include "nfc_napi_ctrl_utils.h"
#include <cstring>
#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "securec.h"
#include <map>

namespace OHOS {
namespace NFC {
namespace KITS {
constexpr const char* KEY_CODE = "code";
constexpr const char* KEY_DATA = "data";

static const std::map<int, std::string> ERR_MSG_MAP = {
    { ErrorCode::ERR_NFC_BASE,
        "NFC service base error." },
    { ErrorCode::ERR_NFC_PARAMETERS,
        "Invalid parameter. The inner parameter is invalid or null." },
    { ErrorCode::ERR_NFC_STATE_UNBIND,
        "Service not available. The NFC service is not running or has been disconnected." },
    { ErrorCode::ERR_NFC_STATE_INVALID,
        "Service error. The NFC service state is abnormal." },
    { ErrorCode::ERR_NFC_EDM_DISALLOWED,
        "Operation not allowed. NFC is disabled by the enterprise device management policy." },
};

napi_value CreateUndefined(napi_env env)
{
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &result));
    return result;
}

static std::string GetErrorMsg(int errCode)
{
    auto it = ERR_MSG_MAP.find(errCode);
    if (it != ERR_MSG_MAP.end()) {
        return it->second;
    }
    return "Unknown error message";
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
    }
    return GetErrorMsg(errCode);
}

napi_value GenerateBusinessError(const napi_env &env, int errCode, const std::string &errMessage)
{
    napi_value code = nullptr;
    napi_create_uint32(env, errCode, &code);
    napi_value message = nullptr;
    napi_create_string_utf8(env, errMessage.c_str(), NAPI_AUTO_LENGTH, &message);
    napi_value businessError = nullptr;
    napi_create_error(env, nullptr, message, &businessError);
    napi_set_named_property(env, businessError, KEY_CODE, code);
    napi_set_named_property(env, businessError, KEY_DATA, message);
    return businessError;
}

bool CheckNfcStatusCodeAndThrow(const napi_env &env, int statusCode, const std::string funcName)
{
    if (statusCode == BUSI_ERR_PERM) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PERM,
            BuildErrorMessage(BUSI_ERR_PERM, funcName, NFC_PERM_DESC, "", "")));
        return false;
    } else if (statusCode >= ErrorCode::ERR_NFC_BASE && statusCode < ErrorCode::ERR_TAG_BASE) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_NFC_STATE_INVALID,
            BuildErrorMessage(statusCode, "", "", "", "")));
        return false;
    }
    return true;
}

void ThrowCapabilityError(const napi_env &env)
{
    napi_throw(env, GenerateBusinessError(env, BUSI_ERR_CAPABILITY, "Capability not supported"));
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
