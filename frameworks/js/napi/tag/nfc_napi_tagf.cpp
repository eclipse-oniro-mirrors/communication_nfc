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
#include "nfc_napi_tag_utils.h"

namespace OHOS {
namespace NFC {
namespace KITS {
napi_value NapiNfcFTag::GetSystemCode(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcFTag *objectInfo = nullptr;
    napi_value result = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("GetSystemCode, napi_unwrap failed, object is null.");
        ConvertStringToNumberArray(env, result, "");
        return result;
    }

    std::string sysCode = "";
    NfcFTag *nfcFTagPtr = static_cast<NfcFTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcFTagPtr == nullptr) {
        ErrorLog("GetSystemCode find objectInfo failed!");
    } else {
        sysCode = nfcFTagPtr->getSystemCode();
    }
    ConvertStringToNumberArray(env, result, sysCode);
    return result;
}

napi_value NapiNfcFTag::GetPmm(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcFTag *objectInfo = nullptr;
    napi_value result = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("GetPmm, napi_unwrap failed, object is null.");
        ConvertStringToNumberArray(env, result, "");
        return result;
    }

    std::string pmm = "";
    NfcFTag *nfcFTagPtr = static_cast<NfcFTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcFTagPtr == nullptr) {
        ErrorLog("GetPmm, find objectInfo failed!");
    } else {
        pmm = nfcFTagPtr->getPmm();
    }
    ConvertStringToNumberArray(env, result, pmm);
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
