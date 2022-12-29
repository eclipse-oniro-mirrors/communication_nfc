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

#include "nfc_napi_tagb.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
napi_value NapiNfcBTag::GetRespAppData(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcBTag *objectInfo = nullptr;
    napi_value result = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("GetRespAppData, napi_unwrap failed, object is null.");
        ConvertStringToNumberArray(env, result, "");
        return result;
    }

    std::string appData = "";
    NfcBTag *nfcBTagPtr = static_cast<NfcBTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcBTagPtr == nullptr) {
        ErrorLog("GetRespAppData, find objectInfo failed!");
    } else {
        appData = nfcBTagPtr->GetAppData();
    }
    ConvertStringToNumberArray(env, result, appData);
    return result;
}

napi_value NapiNfcBTag::GetRespProtocol(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcBTag *objectInfo = nullptr;
    napi_value result = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("GetRespProtocol, napi_unwrap failed, object is null.");
        ConvertStringToNumberArray(env, result, "");
        return result;
    }

    std::string protocol = "";
    NfcBTag *nfcBTagPtr = static_cast<NfcBTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcBTagPtr == nullptr) {
        ErrorLog("GetRespProtocol find objectInfo failed!");
    } else {
        protocol = nfcBTagPtr->GetProtocolInfo();
    }
    ConvertStringToNumberArray(env, result, protocol);
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
