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
    DebugLog("NdefMessage GetNdefRecords called");
    napi_value thisVar = nullptr;
    std::size_t argc = 0;
    napi_value argv[ARGV_NUM_1] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;
    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    // transfer
    std::shared_ptr<NdefMessage> ndefMessagePtr = objectInfo->ndefMessage;
    if (ndefMessagePtr == nullptr) {
        ErrorLog("GetNdefRecords find objectInfo failed!");
        return CreateUndefined(env);
    } else {
        std::vector<std::shared_ptr<NdefRecord>> ndefRecords = ndefMessagePtr->GetNdefRecords();
        napi_value result = nullptr;
        ConvertNdefRecordVectorToJS(env, result, ndefRecords);
        return result;
    }
}

napi_value NapiNdefMessage::MakeUriRecord(napi_env env, napi_callback_info info)
{
    DebugLog("NdefMessage MakeUriRecord called");
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;

    // check parameter number
    if (argc != ARGV_NUM_1) {
        ErrorLog("NapiNdefMessage::MakeUriRecord, Invalid number of arguments!");
        return CreateUndefined(env);
    }
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[ARGV_INDEX_0], &valueType));
    // check parameter data type
    if (valueType != napi_string) {
        ErrorLog("NapiNdefMessage::MakeUriRecord, Invalid data type!");
        return CreateUndefined(env);
    }

    std::string uri = GetStringFromValue(env, argv[ARGV_INDEX_0]);
    DebugLog("MakeUriRecord uri = %{public}s", uri.c_str());

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    // transfer
    std::shared_ptr<NdefMessage> ndefMessagePtr = objectInfo->ndefMessage;
    if (ndefMessagePtr == nullptr) {
        ErrorLog("MakeUriRecord find objectInfo failed!");
        return CreateUndefined(env);
    } else {
        std::shared_ptr<NdefRecord> ndefRecord = ndefMessagePtr->MakeUriRecord(uri);
        napi_value result = nullptr;
        ConvertNdefRecordToJS(env, result, ndefRecord);
        return result;
    }
}

napi_value NapiNdefMessage::MakeTextRecord(napi_env env, napi_callback_info info)
{
    DebugLog("MakeTextRecord called");
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_2;
    napi_value argv[ARGV_NUM_2] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;

    // check parameter number
    if (argc != ARGV_NUM_2) {
        ErrorLog("NapiNdefMessage::MakeTextRecord, Invalid number of arguments!");
        return CreateUndefined(env);
    }
    napi_valuetype valueType1 = napi_undefined;
    napi_valuetype valueType2 = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[ARGV_INDEX_0], &valueType1));
    NAPI_CALL(env, napi_typeof(env, argv[ARGV_INDEX_1], &valueType2));
    // check parameter data type
    if ((valueType1 != napi_string) || (valueType2 != napi_string)) {
        ErrorLog("NapiNdefMessage::MakeTextRecord, Invalid data type!");
        return CreateUndefined(env);
    }

    std::string text = GetStringFromValue(env, argv[ARGV_INDEX_0]);
    std::string locale = GetStringFromValue(env, argv[ARGV_INDEX_1]);
    DebugLog("MakeTextRecord text = %{public}s, locale = = %{public}s", text.c_str(), locale.c_str());

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    // transfer
    std::shared_ptr<NdefMessage> ndefMessagePtr = objectInfo->ndefMessage;
    if (ndefMessagePtr == nullptr) {
        ErrorLog("MakeTextRecord find objectInfo failed!");
        return CreateUndefined(env);
    } else {
        std::shared_ptr<NdefRecord> ndefRecord = ndefMessagePtr->MakeTextRecord(text, locale);
        napi_value result = nullptr;
        ConvertNdefRecordToJS(env, result, ndefRecord);
        return result;
    }
}

napi_value NapiNdefMessage::MakeMimeRecord(napi_env env, napi_callback_info info)
{
    DebugLog("NapiNdefMessage MakeMimeRecord called");
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_2;
    napi_value argv[ARGV_NUM_2] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;

    // check parameter number
    if (argc != ARGV_NUM_2) {
        ErrorLog("NapiNdefMessage::MakeMimeRecord, Invalid number of arguments!");
        return CreateUndefined(env);
    }
    napi_valuetype valueType1 = napi_undefined;
    napi_valuetype valueType2 = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[ARGV_INDEX_0], &valueType1));
    NAPI_CALL(env, napi_typeof(env, argv[ARGV_INDEX_1], &valueType2));
    // check parameter data type
    if ((valueType1 != napi_string) || (valueType2 != napi_object)) {
        ErrorLog("NapiNdefMessage::MakeMimeRecord, Invalid data type!");
        return CreateUndefined(env);
    }
    std::string mimeType = GetStringFromValue(env, argv[ARGV_INDEX_0]);
    std::vector<unsigned char> dataVec;
    ParseBytesVector(env, dataVec, argv[ARGV_INDEX_1]);
    std::string mimeData = NfcSdkCommon::BytesVecToHexString(static_cast<unsigned char *>(dataVec.data()),
                                                             dataVec.size());
    DebugLog("MakeMimeRecord mimeType = %{public}s, mimeData = = %{public}s", mimeType.c_str(), mimeData.c_str());

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    // transfer
    std::shared_ptr<NdefMessage> ndefMessagePtr = objectInfo->ndefMessage;
    if (ndefMessagePtr == nullptr) {
        ErrorLog("MakeMimeRecord find objectInfo failed!");
        return CreateUndefined(env);
    } else {
        std::shared_ptr<NdefRecord> ndefRecord = ndefMessagePtr->MakeMimeRecord(mimeType, mimeData);
        napi_value result = nullptr;
        ConvertNdefRecordToJS(env, result, ndefRecord);
        return result;
    }
}

napi_value NapiNdefMessage::MakeExternalRecord(napi_env env, napi_callback_info info)
{
    DebugLog("NapiNdefMessage MakeUriRecord called");
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_3;
    napi_value argv[ARGV_NUM_3] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;
    
    // check parameter number
    if (argc != ARGV_NUM_3) {
        ErrorLog("NapiNdefMessage::MakeExternalRecord, Invalid number of arguments!");
        return CreateUndefined(env);
    }
    napi_valuetype valueType1 = napi_undefined;
    napi_valuetype valueType2 = napi_undefined;
    napi_valuetype valueType3 = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[ARGV_INDEX_0], &valueType1));
    NAPI_CALL(env, napi_typeof(env, argv[ARGV_INDEX_1], &valueType2));
    NAPI_CALL(env, napi_typeof(env, argv[ARGV_INDEX_2], &valueType3));
    // check parameter data type
    if ((valueType1 != napi_string) || (valueType2 != napi_string) || (valueType3 != napi_string)) {
        ErrorLog("NapiNdefMessage::MakeExternalRecord, Invalid data type!");
        return CreateUndefined(env);
    }

    std::string domainName = GetStringFromValue(env, argv[ARGV_INDEX_0]);
    std::string serviceName = GetStringFromValue(env, argv[ARGV_INDEX_1]);
    std::vector<unsigned char> dataVec;
    ParseBytesVector(env, dataVec, argv[ARGV_INDEX_2]);
    std::string externalData = NfcSdkCommon::BytesVecToHexString(static_cast<unsigned char *>(dataVec.data()),
                                                                 dataVec.size());
    DebugLog("MakeExternalRecord domainName = %{public}s, serviceName  = %{public}s, externalData  = %{public}s",
             domainName.c_str(), serviceName.c_str(), externalData.c_str());

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    // transfer
    std::shared_ptr<NdefMessage> ndefMessagePtr = objectInfo->ndefMessage;
    if (ndefMessagePtr == nullptr) {
        ErrorLog("MakeExternalRecord find objectInfo failed!");
        return CreateUndefined(env);
    } else {
        std::shared_ptr<NdefRecord> ndefRecord =
            ndefMessagePtr->MakeExternalRecord(domainName, serviceName, externalData);
        napi_value result = nullptr;
        ConvertNdefRecordToJS(env, result, ndefRecord);
        return result;
    }
}

napi_value NapiNdefMessage::MessageToBytes(napi_env env, napi_callback_info info)
{
    DebugLog("NapiNdefMessage  MessageToBytes called");
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
    napi_value result = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;
    std::shared_ptr<NdefMessage> ndefMessage = nullptr;
    
    // check parameter number
    if (argc != ARGV_NUM_1) {
        ErrorLog("NapiNdefMessage::MessageToBytes, Invalid number of arguments!");
        return CreateUndefined(env);
    }
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[ARGV_INDEX_0], &valueType));
    // check parameter data type
    if (valueType != napi_object) {
        ErrorLog("NapiNdefMessage::MessageToBytes, Invalid data type!");
        return CreateUndefined(env);
    }

    napi_status status1 = napi_unwrap(env, argv[ARGV_INDEX_0], reinterpret_cast<void **>(&ndefMessage));
    NAPI_ASSERT(env, status1 == napi_ok, "failed to get ndefMessage");

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");

    // transfer
    std::shared_ptr<NdefMessage> ndefMessagePtr = objectInfo->ndefMessage;
    if (ndefMessagePtr == nullptr) {
        ErrorLog("MessageToString find objectInfo failed!");
        ConvertStringToNumberArray(env, result, "");
    } else {
        std::string buffer = ndefMessagePtr->MessageToString(ndefMessage);
        ConvertStringToNumberArray(env, result, buffer.c_str());
    }
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
