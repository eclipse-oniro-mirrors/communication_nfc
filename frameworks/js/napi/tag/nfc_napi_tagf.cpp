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

#include "nfc_napi_tagf.h"

#include "loghelper.h"
#include "nfc_napi_utils.h"

namespace OHOS {
namespace NFC {
namespace KITS {
napi_value NapiNfcFTag::GetSystemCode(napi_env env, napi_callback_info info)
{
    DebugLog("GetNfcFTag GetSystemCode called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcFTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    DebugLog("getSystemCode objInfo %{public}p", objectInfo);
    // transfer
    NfcFTag *nfcFTagPtr = static_cast<NfcFTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcFTagPtr == nullptr) {
        ErrorLog("GetSystemCode find objectInfo failed!");
        return nullptr;
    } else {
        std::vector<unsigned char> sysCode = nfcFTagPtr->getSystemCode();     
        for(size_t i=0; i<sysCode.size();i++){
        DebugLog("NapiNfcFTag::systemCode  %{public}zu is %{public}hhu", i, sysCode[i]);
        }
        napi_value result = nullptr;
        napi_create_array_with_length(env, sysCode.size(), &result);
        ConvertUsignedCharVectorToJS(env, result, sysCode);
        return result;
    }
}

napi_value NapiNfcFTag::GetPmm(napi_env env, napi_callback_info info)
{
    DebugLog("GetNfcFTag GetPmm called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcFTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    DebugLog("getGetPmm objInfo %{public}p", objectInfo);
    // transfer
    NfcFTag *nfcFTagPtr = static_cast<NfcFTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcFTagPtr == nullptr) {
        ErrorLog("GetPmm find objectInfo failed!");
        return nullptr;
    } else {
        std::vector<unsigned char> pmm = nfcFTagPtr->getPmm();  
        for(size_t i=0; i<pmm.size();i++){
        DebugLog("NapiNfcFTag::pmm  %{public}zu is %{public}hhu", i, pmm[i]);
        }
        napi_value result = nullptr;
        napi_create_array_with_length(env, pmm.size(), &result);
        ConvertUsignedCharVectorToJS(env, result, pmm);
        return result;
    }
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
