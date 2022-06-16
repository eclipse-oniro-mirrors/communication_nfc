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
    InfoLog("GetNfcATag GetSak called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcATag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, (void **)&objectInfo);
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    InfoLog("getSak objInfo %{public}p", objectInfo);
    // transfer
    NfcATag *nfcTagPtr = (NfcATag *)(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcTagPtr == nullptr) {
        ErrorLog("GetSak find objectInfo failed!");
        return nullptr;
    } else {
        int sak = nfcTagPtr->GetSak();
        InfoLog("sak %{public}d", sak);
        napi_value result = nullptr;
        napi_create_int32(env, sak, &result);
        return result;
    }
}

napi_value NapiNfcATag::GetAtqa(napi_env env, napi_callback_info info)
{
    InfoLog("GetNfcATag GetAtqa called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcATag *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, (void **)&objectInfo);
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    InfoLog("getAtqa %{public}p", objectInfo);
    NfcATag *nfcTagPtr = (NfcATag *)(static_cast<void *>(objectInfo->tagSession.get()));
    if (nfcTagPtr == nullptr) {
        ErrorLog("GetAtqa find objectInfo failed!");
        return nullptr;
    } else {
        napi_value ret = nullptr;
        std::string atqa = nfcTagPtr->GetAtqa();
        InfoLog("atqa %{public}s", atqa.c_str());
        napi_create_string_utf8(env, atqa.c_str(), NAPI_AUTO_LENGTH, &ret);
        return ret;
    }
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
