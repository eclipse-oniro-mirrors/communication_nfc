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
    DebugLog("GetNfcBTag GetRespAppData called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcBTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    DebugLog("getRespAppData objInfo %{public}p", objectInfo);
    // transfer
    NfcBTag *nfcBTagPtr = static_cast<NfcBTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcBTagPtr == nullptr) {
        ErrorLog("GetRespAppData find objectInfo failed!");
        return nullptr;
    } else {
        napi_value ret = nullptr;
        std::string data = nfcBTagPtr->GetAppData();
        DebugLog("app data %{public}s", data.c_str());
        napi_create_string_utf8(env, data.c_str(), NAPI_AUTO_LENGTH, &ret);
        return ret;
    }
}

napi_value NapiNfcBTag::GetRespProtocol(napi_env env, napi_callback_info info)
{
    DebugLog("GetNfcBTag GetRespProtocol called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcBTag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    DebugLog("getRespAppData objInfo %{public}p", objectInfo);
    // transfer
    NfcBTag *nfcBTagPtr = static_cast<NfcBTag *>(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcBTagPtr == nullptr) {
        ErrorLog("GetRespProtocol find objectInfo failed!");
        return nullptr;
    } else {
        napi_value ret = nullptr;
        std::string protocol = nfcBTagPtr->GetProtocolInfo();
        DebugLog("protocol info %{public}s", protocol.c_str());
        napi_create_string_utf8(env, protocol.c_str(), NAPI_AUTO_LENGTH, &ret);
        return ret;
    }
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
