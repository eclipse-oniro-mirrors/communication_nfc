/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ndefmessage_fuzzer.h"

#include <iostream>
#include <cstddef>
#include <cstdint>

#include "ndef_message.h"
#include "nfc_sdk_common.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;
    using namespace std;

    constexpr const auto FUZZER_THRESHOLD_4 = 4;
    constexpr const auto FUZZER_THRESHOLD_2 = 2;
    constexpr const auto FUZZER_THRESHOLD_3 = 3;
    constexpr const auto FUZZER_THRESHOLD_7 = 7;
    constexpr const uint8_t MAX_ENUM_EMRTDTYPE_NUMS = 9;
    constexpr const uint8_t MAX_TNF_NUMS = 7;
    constexpr const uint8_t GET_BOOL_FACTOR = 2;

    bool CheckTnf(short tnf, const std::string& tagRtdType, const std::string& id, const std::string& payload)
    {
        switch (tnf) {
            case NdefMessage::TNF_EMPTY:
                if (!tagRtdType.empty() || !id.empty() || !payload.empty()) {
                    return false;
                }
                break;
            case NdefMessage::TNF_WELL_KNOWN: // fall-through
            case NdefMessage::TNF_MIME_MEDIA: // fall-through
            case NdefMessage::TNF_ABSOLUTE_URI: // fall-through
            case NdefMessage::TNF_EXTERNAL_TYPE: // fall-through
                return true;
            case NdefMessage::TNF_UNKNOWN: // fall-through
            case NdefMessage::TNF_RESERVED:
                if (tagRtdType.empty()) {
                    return false;
                }
                return true;
            case NdefMessage::TNF_UNCHANGED:
                return false;
            default:
                break;
        }
        return false;
    }

    std::shared_ptr<NdefRecord> CreateNdefRecord(short tnf,
                                                 const std::string& id,
                                                 const std::string& payload,
                                                 const std::string& tagRtdType)
    {
        bool isValidTnf = OHOS::CheckTnf(tnf, tagRtdType, id, payload);
        if (!isValidTnf) {
            return std::shared_ptr<NdefRecord>();
        }
        std::shared_ptr<NdefRecord> ndefRecord = std::make_shared<NdefRecord>();
        ndefRecord->tnf_ = tnf;
        ndefRecord->id_ = id;
        ndefRecord->payload_ = payload;
        ndefRecord->tagRtdType_ = tagRtdType;
        return ndefRecord;
    }

    void FuzzGetNdefMessageByNdefRecord(const uint8_t* data, size_t size)
    {
        if (size < FUZZER_THRESHOLD_4) {
            return;
        }
        short tnf = static_cast<short>(data[0] % OHOS::MAX_TNF_NUMS);
        std::string id = NfcSdkCommon::UnsignedCharToHexString(data[1]);

        // 2 is an array subscript, which requires 3 strings to form ndefrecord
        std::string payload = NfcSdkCommon::UnsignedCharToHexString(data[2]);

        // 3 is an array subscript, which requires 3 strings to form ndefrecord
        std::string tagRtdType = NfcSdkCommon::UnsignedCharToHexString(data[3]);
        std::vector<std::shared_ptr<NdefRecord>> ndefRecords;
        std::shared_ptr<NdefRecord> ndefRecord = CreateNdefRecord(tnf, id, payload, tagRtdType);
        ndefRecords.push_back(ndefRecord);
        NdefMessage::GetNdefMessage(ndefRecords);
    }

    void FuzzGetTagRtdType(const uint8_t* data, size_t size)
    {
        // Need enum type input parameter, use % 9 Randomly convert data to EmRtdType
        NdefMessage::EmRtdType rtdtype = static_cast<NdefMessage::EmRtdType>(data[0] % OHOS::MAX_ENUM_EMRTDTYPE_NUMS);
        NdefMessage::GetTagRtdType(rtdtype);
    }

    void FuzzMakeUriRecord(const uint8_t* data, size_t size)
    {
        std::string uriString = NfcSdkCommon::BytesVecToHexString(data, size);
        NdefMessage::MakeUriRecord(uriString);
    }

    void FuzzMakeTextRecord(const uint8_t* data, size_t size)
    {
        if (size < FUZZER_THRESHOLD_2) {
            return;
        }
        std::string text = NfcSdkCommon::UnsignedCharToHexString(data[0]);
        std::string locale = NfcSdkCommon::UnsignedCharToHexString(data[1]);
        NdefMessage::MakeTextRecord(text, locale);
    }

    void FuzzMakeMimeRecord(const uint8_t* data, size_t size)
    {
        if (size < FUZZER_THRESHOLD_2) {
            return;
        }
        std::string mimeType = NfcSdkCommon::UnsignedCharToHexString(data[0]);
        std::string mimeData = NfcSdkCommon::UnsignedCharToHexString(data[1]);
        NdefMessage::MakeMimeRecord(mimeType, mimeData);
    }

    void FuzzMakeExternalRecord(const uint8_t* data, size_t size)
    {
        if (size < FUZZER_THRESHOLD_3) {
            return;
        }
        std::string domainName = NfcSdkCommon::UnsignedCharToHexString(data[0]);
        std::string serviceName = NfcSdkCommon::UnsignedCharToHexString(data[1]);

        // 2 is an array subscript, which requires 3 strings to form ndefrecord
        std::string externalData = NfcSdkCommon::UnsignedCharToHexString(data[2]);
        NdefMessage::MakeExternalRecord(domainName, serviceName, externalData);
    }

    void FuzzMessageToString(const uint8_t* data, size_t size)
    {
        if (size < FUZZER_THRESHOLD_4) {
            return;
        }
        short tnf = static_cast<short>(data[0] % OHOS::MAX_TNF_NUMS);
        std::string id = NfcSdkCommon::UnsignedCharToHexString(data[1]);

        // 2 is an array subscript, which requires 3 strings to form ndefrecord
        std::string payload = NfcSdkCommon::UnsignedCharToHexString(data[2]);

        // 3 is an array subscript, which requires 3 strings to form ndefrecord
        std::string tagRtdType = NfcSdkCommon::UnsignedCharToHexString(data[3]);
        std::vector<std::shared_ptr<NdefRecord>> ndefRecords;
        std::shared_ptr<NdefRecord> ndefRecord = CreateNdefRecord(tnf, id, payload, tagRtdType);
        ndefRecords.push_back(ndefRecord);
        std::shared_ptr<NdefMessage> ndefMessage = NdefMessage::GetNdefMessage(ndefRecords);
        NdefMessage::MessageToString(ndefMessage);
    }

    void FuzzNdefRecordToString(const uint8_t* data, size_t size)
    {
        if (size < FUZZER_THRESHOLD_7) {
            return;
        }
        short tnf = static_cast<short>(data[0] % OHOS::MAX_TNF_NUMS);
        std::string id = NfcSdkCommon::UnsignedCharToHexString(data[1]);

        // 2 is an array subscript, which requires 3 strings to form ndefrecord
        std::string payload = NfcSdkCommon::UnsignedCharToHexString(data[2]);

        // 3 is an array subscript, which requires 3 strings to form ndefrecord
        std::string tagRtdType = NfcSdkCommon::UnsignedCharToHexString(data[3]);
        std::shared_ptr<NdefRecord> ndefRecord = CreateNdefRecord(tnf, id, payload, tagRtdType);
        std::string buffer = NfcSdkCommon::UnsignedCharToHexString(data[4]); // Get input parameters from data array 4
        bool bIsMB = static_cast<bool>(data[5] % OHOS::GET_BOOL_FACTOR); // Get input parameters from data array 5
        bool bIsME = static_cast<bool>(data[6] % OHOS::GET_BOOL_FACTOR); // Get input parameters from data array 6
        NdefMessage::NdefRecordToString(ndefRecord, buffer, bIsMB, bIsME);
    }

    void FuzzGetNdefRecords(const uint8_t* data, size_t size)
    {
        if (size < FUZZER_THRESHOLD_4) {
            return;
        }
        short tnf = static_cast<short>(data[0] % OHOS::MAX_TNF_NUMS);
        std::string id = NfcSdkCommon::UnsignedCharToHexString(data[1]);

        // 2 is an array subscript, which requires 3 strings to form ndefrecord
        std::string payload = NfcSdkCommon::UnsignedCharToHexString(data[2]);

        // 3 is an array subscript, which requires 3 strings to form ndefrecord
        std::string tagRtdType = NfcSdkCommon::UnsignedCharToHexString(data[3]);
        std::vector<std::shared_ptr<NdefRecord>> ndefRecords;
        std::shared_ptr<NdefRecord> ndefRecord = CreateNdefRecord(tnf, id, payload, tagRtdType);
        ndefRecords.push_back(ndefRecord);
        std::shared_ptr<NdefMessage> ndefMessage = NdefMessage::GetNdefMessage(ndefRecords);
        ndefMessage->GetNdefRecords();
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::FuzzGetNdefMessageByNdefRecord(data, size);
    OHOS::FuzzGetTagRtdType(data, size);
    OHOS::FuzzMakeUriRecord(data, size);
    OHOS::FuzzMakeTextRecord(data, size);
    OHOS::FuzzMakeMimeRecord(data, size);
    OHOS::FuzzMakeExternalRecord(data, size);
    OHOS::FuzzMessageToString(data, size);
    OHOS::FuzzNdefRecordToString(data, size);
    OHOS::FuzzGetNdefRecords(data, size);
    return 0;
}

