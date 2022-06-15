/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "nfc_napi_tag_sesstion.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
napi_value NapiNfcTagSession::ConnectTag(napi_env env, napi_callback_info info)
{
    InfoLog("GetTagSession ConnectTag called");
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    napi_value thisVar = nullptr;
    bool isConnected = false;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcTagSession *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, (void **)&objectInfo);
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    BasicTagSession *nfcTagPtr = objectInfo->tagSession.get();
    if (nfcTagPtr == nullptr) {
        ErrorLog("ConnectTag find objectInfo failed!");
        return nullptr;
    } else {
        napi_value ret = nullptr;
        isConnected = nfcTagPtr->Connect();
        napi_get_boolean(env, isConnected, &result);
        return ret;
    }
}

napi_value NapiNfcTagSession::Reset(napi_env env, napi_callback_info info)
{
    InfoLog("TagSession Reset called");
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcTagSession *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, (void **)&objectInfo);
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    BasicTagSession *nfcTagPtr = objectInfo->tagSession.get();
    if (nfcTagPtr == nullptr) {
        ErrorLog("Reset find objectInfo failed!");
        return nullptr;
    } else {
        int err = nfcTagPtr->Close();
        if (err != NfcErrorCode::NFC_SUCCESS) {
            InfoLog("Reset failed!");
        } else {
            InfoLog("Reset finished.");
        }
        return result;
    }
}

napi_value NapiNfcTagSession::IsTagConnected(napi_env env, napi_callback_info info)
{
    InfoLog("GetTagSession IsTagConnected called");
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    napi_value thisVar = nullptr;
    bool connectTag = false;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcTagSession *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, (void **)&objectInfo);
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    BasicTagSession *nfcTagPtr = objectInfo->tagSession.get();
    if (nfcTagPtr == nullptr) {
        ErrorLog("IsTagConnected find objectInfo failed!");
        return nullptr;
    } else {
        connectTag = nfcTagPtr->IsConnected();
        napi_get_boolean(env, connectTag, &result);
        return result;
    }
}

napi_value NapiNfcTagSession::GetMaxSendLength(napi_env env, napi_callback_info info)
{
    InfoLog("TagSession GetMaxSendLength called");
    std::size_t argc = 0;
    napi_value argv[] = {nullptr};
    napi_value result = nullptr;
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNfcTagSession *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, (void **)&objectInfo);
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    BasicTagSession *nfcTagPtr = objectInfo->tagSession.get();
    if (nfcTagPtr == nullptr) {
        ErrorLog("GetMaxSendLength find objectInfo failed!");
        return nullptr;
    } else {
        int maxsendlen = nfcTagPtr->GetMaxSendCommandLength();
        napi_create_int32(env, maxsendlen, &result);
        return result;
    }
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS