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

#include "nfc_napi_tagv.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
napi_value NapiNfcVTag::GetResponseFlags(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcVTag *objectInfo = nullptr;
    napi_value result = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("GetResponseFlags, napi_unwrap failed, object is null.");
        napi_create_int32(env, 0, &result);
        return result;
    }

    int respFlags = 0;
    Iso15693Tag *nfcVTagPtr = static_cast<Iso15693Tag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcVTagPtr == nullptr) {
        ErrorLog("GetResponseFlags, find objectInfo failed!");
    } else {
        respFlags = static_cast<int>(nfcVTagPtr->GetRespFlags());
    }
    napi_create_int32(env, respFlags, &result);
    return result;
}

napi_value NapiNfcVTag::GetDsfId(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcVTag *objectInfo = nullptr;
    napi_value result = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
        if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("GetDsfId, napi_unwrap failed, object is null.");
        napi_create_int32(env, 0, &result);
        return result;
    }

    int dsfId = 0;
    Iso15693Tag *nfcVTagPtr = static_cast<Iso15693Tag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcVTagPtr == nullptr) {
        ErrorLog("GetDsfId, find objectInfo failed!");
    } else {
        dsfId = static_cast<int>(nfcVTagPtr->GetDsfId());
    }
    napi_create_int32(env, dsfId, &result);
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
