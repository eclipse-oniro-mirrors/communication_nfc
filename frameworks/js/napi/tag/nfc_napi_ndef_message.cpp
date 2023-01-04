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

    // unwrap from thisVar to retrieve the native instance
    NapiNdefMessage *objectInfo = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->ndefMessage == nullptr) {
        ErrorLog("GetNdefRecords, napi_unwrap failed, object is null.");
        return CreateUndefined(env);
    }

    std::shared_ptr<NdefMessage> ndefMessagePtr = objectInfo->ndefMessage;
    std::vector<std::shared_ptr<NdefRecord>> ndefRecords = ndefMessagePtr->GetNdefRecords();
    napi_value result = nullptr;
    ConvertNdefRecordVectorToJS(env, result, ndefRecords);
    return result;
}

static void CheckNdefRecordAndThrow(const napi_env &env, std::shared_ptr<NdefRecord> ndefRecord)
{
    if (ndefRecord == nullptr) {
        // ndefRecord is null, means that the input arguments can't parsed as NdefRecord
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
            BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
    }
}

napi_value NapiNdefMessage::MakeUriRecord(napi_env env, napi_callback_info info)
{
    std::size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    // check parameter number
    CheckArgCountAndThrow(env, argc, ARGV_NUM_1);
    CheckStringAndThrow(env, argv[ARGV_INDEX_0], "uri", "string");

    std::string uri = GetStringFromValue(env, argv[ARGV_INDEX_0]);
    std::shared_ptr<NdefRecord> ndefRecord = NdefMessage::MakeUriRecord(uri);
    CheckNdefRecordAndThrow(env, ndefRecord);
    napi_value result = nullptr;
    ConvertNdefRecordToJS(env, result, ndefRecord);
    return result;
}

napi_value NapiNdefMessage::MakeTextRecord(napi_env env, napi_callback_info info)
{
    std::size_t argc = ARGV_NUM_2;
    napi_value argv[ARGV_NUM_2] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    // check parameter number
    CheckArgCountAndThrow(env, argc, ARGV_NUM_2);
    CheckStringAndThrow(env, argv[ARGV_INDEX_0], "text", "string");
    CheckStringAndThrow(env, argv[ARGV_INDEX_1], "locale", "string");

    std::string text = GetStringFromValue(env, argv[ARGV_INDEX_0]);
    std::string locale = GetStringFromValue(env, argv[ARGV_INDEX_1]);
    std::shared_ptr<NdefRecord> ndefRecord = NdefMessage::MakeTextRecord(text, locale);
    CheckNdefRecordAndThrow(env, ndefRecord);
    napi_value result = nullptr;
    ConvertNdefRecordToJS(env, result, ndefRecord);
    return result;
}

napi_value NapiNdefMessage::MakeMimeRecord(napi_env env, napi_callback_info info)
{
    std::size_t argc = ARGV_NUM_2;
    napi_value argv[ARGV_NUM_2] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    // check parameter number
    CheckArgCountAndThrow(env, argc, ARGV_NUM_2);
    CheckStringAndThrow(env, argv[ARGV_INDEX_0], "mimeType", "string");
    CheckArrayNumberAndThrow(env, argv[ARGV_INDEX_1], "mimeData", "number[]");

    std::string mimeType = GetStringFromValue(env, argv[ARGV_INDEX_0]);
    std::vector<unsigned char> dataVec;
    ParseBytesVector(env, dataVec, argv[ARGV_INDEX_1]);
    std::string mimeData = NfcSdkCommon::BytesVecToHexString(static_cast<unsigned char *>(dataVec.data()),
                                                             dataVec.size());

    std::shared_ptr<NdefRecord> ndefRecord = NdefMessage::MakeMimeRecord(mimeType, mimeData);
    CheckNdefRecordAndThrow(env, ndefRecord);
    napi_value result = nullptr;
    ConvertNdefRecordToJS(env, result, ndefRecord);
    return result;
}

napi_value NapiNdefMessage::MakeExternalRecord(napi_env env, napi_callback_info info)
{
    std::size_t argc = ARGV_NUM_3;
    napi_value argv[ARGV_NUM_3] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    
    // check parameter number
    CheckArgCountAndThrow(env, argc, ARGV_NUM_3);
    CheckStringAndThrow(env, argv[ARGV_INDEX_0], "domainName", "string");
    CheckStringAndThrow(env, argv[ARGV_INDEX_1], "type", "string");
    CheckArrayNumberAndThrow(env, argv[ARGV_INDEX_2], "externalData", "number[]");

    std::string domainName = GetStringFromValue(env, argv[ARGV_INDEX_0]);
    std::string type = GetStringFromValue(env, argv[ARGV_INDEX_1]);
    std::vector<unsigned char> dataVec;
    ParseBytesVector(env, dataVec, argv[ARGV_INDEX_2]);
    std::string externalData = NfcSdkCommon::BytesVecToHexString(static_cast<unsigned char *>(dataVec.data()),
                                                                 dataVec.size());

    std::shared_ptr<NdefRecord> ndefRecord = NdefMessage::MakeExternalRecord(domainName, type, externalData);
    CheckNdefRecordAndThrow(env, ndefRecord);
    napi_value result = nullptr;
    ConvertNdefRecordToJS(env, result, ndefRecord);
    return result;
}

napi_value NapiNdefMessage::MessageToBytes(napi_env env, napi_callback_info info)
{
    std::size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    // check parameter number
    CheckArgCountAndThrow(env, argc, ARGV_NUM_1);
    CheckObjectAndThrow(env, argv[ARGV_INDEX_0], "ndefMessage", "NdefMessage");

    // unwrap for argument of NdefMessage.
    NapiNdefMessage *argNdefMsg = nullptr;
    napi_status status = napi_unwrap(env, argv[ARGV_INDEX_0], reinterpret_cast<void **>(&argNdefMsg));
    CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID);

    // parse to get the raw bytes.
    napi_value result = nullptr;
    std::shared_ptr<NdefMessage> ndefMsg = argNdefMsg->ndefMessage;
    std::string buffer = NdefMessage::MessageToString(ndefMsg);
    ConvertStringToNumberArray(env, result, buffer.c_str());
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
