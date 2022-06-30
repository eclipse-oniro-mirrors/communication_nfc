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

#include "nfc_napi_ndef_message.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
napi_value NapiNdefMessage::GetNdefRecords(napi_env env, napi_callback_info info)
{
    InfoLog("NdefMessage GetNdefRecords called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
    napi_value argv[ARGV_NUM_1] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, (void **)&objectInfo);
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    InfoLog("getNdefRecords objInfo %{public}p", objectInfo);
    // transfer
    NdefMessage *ndefMessagePtr = (NdefMessage *)(static_cast<void *>(objectInfo));
    if (ndefMessagePtr == nullptr) {
        ErrorLog("GetNdefRecords find objectInfo failed!");
        return nullptr;
    } else {
        std::vector<std::shared_ptr<NdefRecord>> ndefRecords = ndefMessagePtr->GetNdefRecords();
        napi_value result = nullptr;
        ConvertNdefRecordVectorToJS(env, result, ndefRecords);
        return result;
    }
}

napi_value NapiNdefMessage::MakeUriRecord(napi_env env, napi_callback_info info)
{
    InfoLog("NdefMessage MakeUriRecord called");
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;

    std::string uri = GetStringFromValue(env, argv[ARGV_INDEX_0]);
    InfoLog("MakeUriRecord uri = %{public}s", uri.c_str());

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, (void **)&objectInfo);
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    InfoLog("MakeUriRecord objInfo %{public}p", objectInfo);
    // transfer
    NdefMessage *ndefMessagePtr = (NdefMessage *)(static_cast<void *>(objectInfo));
    if (ndefMessagePtr == nullptr) {
        ErrorLog("MakeUriRecord find objectInfo failed!");
        return nullptr;
    } else {
        std::shared_ptr<NdefRecord> ndefRecord = ndefMessagePtr->MakeUriRecord(uri);
        napi_value result = nullptr;
        ConvertNdefRecordToJS(env, result, ndefRecord);
        return result;
    }
}

napi_value NapiNdefMessage::MakeTextRecord(napi_env env, napi_callback_info info)
{
    InfoLog("MakeTextRecord called");
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_2;
    napi_value argv[ARGV_NUM_2] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;

    std::string text = GetStringFromValue(env, argv[ARGV_INDEX_0]);
    std::string locale = GetStringFromValue(env, argv[ARGV_INDEX_1]);
    InfoLog("MakeTextRecord text = %{public}s, locale = = %{public}s", text.c_str(), locale.c_str());

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, (void **)&objectInfo);
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    InfoLog("MakeTextRecord objInfo %{public}p", objectInfo);
    // transfer
    NdefMessage *ndefMessagePtr = (NdefMessage *)(static_cast<void *>(objectInfo));
    if (ndefMessagePtr == nullptr) {
        ErrorLog("MakeTextRecord find objectInfo failed!");
        return nullptr;
    } else {
        std::shared_ptr<NdefRecord> ndefRecord = ndefMessagePtr->MakeTextRecord(text, locale);
        napi_value result = nullptr;
        ConvertNdefRecordToJS(env, result, ndefRecord);
        return result;
    }
}

napi_value NapiNdefMessage::MakeMimeRecord(napi_env env, napi_callback_info info)
{
    InfoLog("MakeMimeRecord MakeUriRecord called");
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_2;
    napi_value argv[ARGV_NUM_2] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;

    std::string mimeType = GetStringFromValue(env, argv[ARGV_INDEX_0]);
    std::string mimeData = GetStringFromValue(env, argv[ARGV_INDEX_1]);
    InfoLog("MakeMimeRecord mimeType = %{public}s, mimeData = = %{public}s", mimeType.c_str(), mimeData.c_str());

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, (void **)&objectInfo);
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    InfoLog("MakeMimeRecord objInfo %{public}p", objectInfo);
    // transfer
    NdefMessage *ndefMessagePtr = (NdefMessage *)(static_cast<void *>(objectInfo));
    if (ndefMessagePtr == nullptr) {
        ErrorLog("MakeMimeRecord find objectInfo failed!");
        return nullptr;
    } else {
        std::shared_ptr<NdefRecord> ndefRecord = ndefMessagePtr->MakeMimeRecord(mimeType, mimeData);
        napi_value result = nullptr;
        ConvertNdefRecordToJS(env, result, ndefRecord);
        return result;
    }
}

napi_value NapiNdefMessage::MakeExternalRecord(napi_env env, napi_callback_info info)
{
    InfoLog("MakeExternalRecord MakeUriRecord called");
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_3;
    napi_value argv[ARGV_NUM_3] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;

    std::string domainName = GetStringFromValue(env, argv[ARGV_INDEX_0]);
    std::string serviceName = GetStringFromValue(env, argv[ARGV_INDEX_1]);
    std::string externalData = GetStringFromValue(env, argv[ARGV_INDEX_2]);
    InfoLog("MakeExternalRecord domainName = %{public}s, serviceName  = %{public}s, externalData  = %{public}s",
            domainName.c_str(), serviceName.c_str(), externalData.c_str());

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, (void **)&objectInfo);
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    InfoLog("MakeExternalRecord objInfo %{public}p", objectInfo);
    // transfer
    NdefMessage *ndefMessagePtr = (NdefMessage *)(static_cast<void *>(objectInfo));
    if (ndefMessagePtr == nullptr) {
        ErrorLog("MakeExternalRecord find objectInfo failed!");
        return nullptr;
    } else {
        std::shared_ptr<NdefRecord> ndefRecord =
            ndefMessagePtr->MakeExternalRecord(domainName, serviceName, externalData);
        napi_value result = nullptr;
        ConvertNdefRecordToJS(env, result, ndefRecord);
        return result;
    }
}

napi_value NapiNdefMessage::MessageToString(napi_env env, napi_callback_info info)
{
    InfoLog("MessageToString MakeUriRecord called");
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;
    std::shared_ptr<NdefMessage> ndefMessage = nullptr;

    napi_status status1 = napi_unwrap(env, argv[ARGV_INDEX_0], reinterpret_cast<void **>(&ndefMessage));
    NAPI_ASSERT(env, status1 == napi_ok, "failed to get ndefMessage");

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, (void **)&objectInfo);
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    InfoLog("MessageToString objInfo %{public}p", objectInfo);
    // transfer
    NdefMessage *ndefMessagePtr = (NdefMessage *)(static_cast<void *>(objectInfo));
    if (ndefMessagePtr == nullptr) {
        ErrorLog("MessageToString find objectInfo failed!");
        return nullptr;
    NAPI_ASSERT(env, status == napi_ok, "failed to get ndefMessage");
    } else {
        std::string buffer = ndefMessagePtr->MessageToString(ndefMessage);
        napi_value result = nullptr;
        InfoLog("buffer %{public}s", buffer.c_str());
        napi_create_string_utf8(env, buffer.c_str(), NAPI_AUTO_LENGTH, &result);
        return result;
    }
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
