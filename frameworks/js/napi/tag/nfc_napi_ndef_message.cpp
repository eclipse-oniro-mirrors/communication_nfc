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
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
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
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;

    // check parameter number
    if (argc != ARGV_NUM_1) {
        ErrorLog("NapiNdefMessage::MakeUriRecord, Invalid number of arguments!");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return CreateUndefined(env);
    }

    // check parameter data type
    if (!IsString(env, argv[ARGV_INDEX_0])) {
        ErrorLog("NapiNdefMessage::MakeUriRecord, Invalid data type!");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "uri", "string")));
        return CreateUndefined(env);
    }
    std::string uri = GetStringFromValue(env, argv[ARGV_INDEX_0]);

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
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_2;
    napi_value argv[ARGV_NUM_2] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;

    // check parameter number
    if (argc != ARGV_NUM_2) {
        ErrorLog("NapiNdefMessage::MakeTextRecord, Invalid number of arguments!");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return CreateUndefined(env);
    }

    // check parameter data type
    if (!IsString(env, argv[ARGV_INDEX_0])) {
        ErrorLog("NapiNdefMessage::MakeTextRecord, Invalid text type!");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "text", "string")));
        return CreateUndefined(env);
    }
    if (!IsString(env, argv[ARGV_INDEX_1])) {
        ErrorLog("NapiNdefMessage::MakeTextRecord, Invalid locale type!");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "locale", "string")));
        return CreateUndefined(env);
    }
    std::string text = GetStringFromValue(env, argv[ARGV_INDEX_0]);
    std::string locale = GetStringFromValue(env, argv[ARGV_INDEX_1]);

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
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_2;
    napi_value argv[ARGV_NUM_2] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;

    // check parameter number
    if (argc != ARGV_NUM_2) {
        ErrorLog("NapiNdefMessage::MakeMimeRecord, Invalid number of arguments!");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return CreateUndefined(env);
    }

    // check parameter data type
    if (!IsString(env, argv[ARGV_INDEX_0])) {
        ErrorLog("NapiNdefMessage::MakeMimeRecord, Invalid mimeType type!");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "mimeType", "string")));
        return CreateUndefined(env);
    }
    if (!IsNumberArray(env, argv[ARGV_INDEX_1])) {
        ErrorLog("NapiNdefMessage::MakeMimeRecord, Invalid mimeData type!");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "mimeData", "number[]")));
        return CreateUndefined(env);
    }
    std::string mimeType = GetStringFromValue(env, argv[ARGV_INDEX_0]);
    std::vector<unsigned char> dataVec;
    ParseBytesVector(env, dataVec, argv[ARGV_INDEX_1]);
    std::string mimeData = NfcSdkCommon::BytesVecToHexString(static_cast<unsigned char *>(dataVec.data()),
                                                             dataVec.size());

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
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_3;
    napi_value argv[ARGV_NUM_3] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiNdefMessage *objectInfo = nullptr;
    
    // check parameter number
    if (argc != ARGV_NUM_3) {
        ErrorLog("NapiNdefMessage::MakeExternalRecord, Invalid number of arguments!");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return CreateUndefined(env);
    }

    // check parameter data type
    if (!IsString(env, argv[ARGV_INDEX_0])) {
        ErrorLog("NapiNdefMessage::MakeExternalRecord, Invalid domainName type!");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "domainName", "string")));
        return CreateUndefined(env);
    }
    if (!IsString(env, argv[ARGV_INDEX_1])) {
        ErrorLog("NapiNdefMessage::MakeExternalRecord, Invalid serviceName type!");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "serviceName", "string")));
        return CreateUndefined(env);
    }
    if (!IsNumberArray(env, argv[ARGV_INDEX_2])) {
        ErrorLog("NapiNdefMessage::MakeExternalRecord, Invalid externalData type!");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "externalData", "number[]")));
        return CreateUndefined(env);
    }
    std::string domainName = GetStringFromValue(env, argv[ARGV_INDEX_0]);
    std::string serviceName = GetStringFromValue(env, argv[ARGV_INDEX_1]);
    std::vector<unsigned char> dataVec;
    ParseBytesVector(env, dataVec, argv[ARGV_INDEX_2]);
    std::string externalData = NfcSdkCommon::BytesVecToHexString(static_cast<unsigned char *>(dataVec.data()),
                                                                 dataVec.size());

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
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return CreateUndefined(env);
    }

    // check parameter data type
    if (!IsObject(env, argv[ARGV_INDEX_0])) {
        ErrorLog("NapiNdefMessage::MessageToBytes, Invalid ndefMessage type!");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "ndefMessage", "NdefMessage")));
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
