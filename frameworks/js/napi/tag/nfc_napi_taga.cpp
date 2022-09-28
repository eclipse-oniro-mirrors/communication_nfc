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

#include "nfc_napi_taga.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
napi_value NapiNfcATag::GetSak(napi_env env, napi_callback_info info)
{
    DebugLog("GetNfcATag GetSak called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcATag *objectInfo = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    if (objectInfo == nullptr) {
        ErrorLog("GetSak objectInfo nullptr!");
        napi_create_int32(env, 0, &result);
        return result;
    }
    if (objectInfo->tagSession == nullptr) {
        ErrorLog("GetSak tagSession nullptr!");
        napi_create_int32(env, 0, &result);
        return result;
    }

    // transfer
    NfcATag *nfcTagPtr = static_cast<NfcATag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcTagPtr == nullptr) {
        ErrorLog("GetSak find objectInfo failed!");
        napi_create_int32(env, 0, &result);
    } else {
        int sak = nfcTagPtr->GetSak();
        napi_create_int32(env, sak, &result);
    }
    return result;
}

napi_value NapiNfcATag::GetAtqa(napi_env env, napi_callback_info info)
{
    DebugLog("GetNfcATag GetAtqa called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcATag *objectInfo = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    if (objectInfo == nullptr) {
        ErrorLog("GetAtqa objectInfo nullptr!");
        ConvertStringToNumberArray(env, result, "");
        return result;
    }
    if (objectInfo->tagSession == nullptr) {
        ErrorLog("GetAtqa tagSession nullptr!");
        ConvertStringToNumberArray(env, result, "");
        return result;
    }

    NfcATag *nfcTagPtr = static_cast<NfcATag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcTagPtr == nullptr) {
        ErrorLog("GetAtqa, nfcTagPtr is nullptr");
        ConvertStringToNumberArray(env, result, "");
    } else {
        std::string atqa = nfcTagPtr->GetAtqa();
        DebugLog("atqa %{public}s", atqa.c_str());
        ConvertStringToNumberArray(env, result, atqa);
    }
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS