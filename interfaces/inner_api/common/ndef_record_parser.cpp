/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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
#include "ndef_record_parser.h"
#include "ndef_message.h"
#include "nfc_sdk_common.h"
#include "uri.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {

using namespace OHOS::NFC::KITS;

/* Package name resolution */
std::vector<std::string> NdefRecordParser::ExtractHarPackages(const std::vector<std::shared_ptr<NdefRecord>> &records)
{
    InfoLog("enter");
    std::vector<std::string> harPackages;
    if (records.size() == 0) {
        ErrorLog("records is empty");
        return harPackages;
    }
    for (std::shared_ptr<NdefRecord> record : records) {
        std::string bundle = CheckForHar(record);
        if (!bundle.empty()) {
            harPackages.push_back(bundle);
        }
    }
    return harPackages;
}

std::string NdefRecordParser::CheckForHar(const std::shared_ptr<NdefRecord> &record)
{
    InfoLog("enter");
    if (record == nullptr) {
        ErrorLog("record is nullptr");
        return "";
    }
    std::string bundle = "";
    /* app type judgment */
    if (record->tnf_ == NdefMessage::TNF_EXTERNAL_TYPE &&
        (IsOtherPlatformAppType(record->tagRtdType_) ||
         record->tagRtdType_.compare(
             NfcSdkCommon::StringToHexString(NdefMessage::GetTagRtdType(NdefMessage::RTD_OHOS_APP))) == 0)) {
        return record->payload_;
    }
    return bundle;
}

/* support parse and launch for other platform app type */
bool NdefRecordParser::IsOtherPlatformAppType(const std::string &appType)
{
    if (appType.compare(OTHER_PLATFORM_APP_RECORD_TYPE) == 0) {
        return true;
    }
    InfoLog("exit");
    return false;
}

std::string NdefRecordParser::GetNdefRecordMimeType(const std::shared_ptr<NdefRecord> &record)
{
    if (record == nullptr) {
        ErrorLog("record is nullptr");
        return "";
    }
    std::string mimeTypeStr = "";
    InfoLog("record.tnf_: %{public}d", record->tnf_);
    switch (record->tnf_) {
        case NdefMessage::TNF_WELL_KNOWN:
            if (record->tagRtdType_.compare(
                NfcSdkCommon::StringToHexString(NdefMessage::GetTagRtdType(NdefMessage::RTD_TEXT))) == 0) {
                mimeTypeStr = TEXT_PLAIN;
            }
            break;
        case NdefMessage::TNF_MIME_MEDIA:
            mimeTypeStr = NfcSdkCommon::HexStringToAsciiString(record->tagRtdType_);
            break;
        default:
            mimeTypeStr = "";
            break;
    }
    if (mimeTypeStr.size() > MIME_MAX_LENGTH) {
        ErrorLog("mimeType too long");
        return "";
    }
    InfoLog("mimeTypeStr=%{public}s", mimeTypeStr.c_str());
    return mimeTypeStr;
}

std::string NdefRecordParser::GetUriPayload(const std::shared_ptr<NdefRecord> &record)
{
    if (record == nullptr) {
        ErrorLog("record is nullptr");
        return "";
    }
    return GetUriPayload(record, false);
}

/* get uri data */
std::string NdefRecordParser::GetUriPayload(const std::shared_ptr<NdefRecord> &record, bool isSmartPoster)
{
    InfoLog("enter");
    if (record == nullptr) {
        ErrorLog("record is nullptr");
        return "";
    }
    std::string uri = "";
    InfoLog("record.tnf_: %{public}d", record->tnf_);
    switch (record->tnf_) {
        case NdefMessage::TNF_WELL_KNOWN:
            InfoLog("tagRtdType: %{public}s", NfcSdkCommon::CodeMiddlePart(record->tagRtdType_).c_str());
            if ((record->tagRtdType_.compare(NfcSdkCommon::StringToHexString(
                NdefMessage::GetTagRtdType(NdefMessage::RTD_SMART_POSTER))) == 0) && !isSmartPoster) {
                std::shared_ptr<NdefMessage> nestMessage = NdefMessage::GetNdefMessage(record->payload_);
                InfoLog("payload: %{public}s", NfcSdkCommon::CodeMiddlePart(record->payload_).c_str());
                if (nestMessage == nullptr) {
                    ErrorLog("nestMessage is nullptr");
                    return "";
                }
                std::vector<std::shared_ptr<NdefRecord>> nestRecords = nestMessage->GetNdefRecords();
                for (std::shared_ptr<NdefRecord> nestRecord : nestRecords) {
                    uri = GetUriPayload(nestRecord, true);
                    return uri;
                }
            } else if ((record->tagRtdType_.compare(
                NfcSdkCommon::StringToHexString(NdefMessage::GetTagRtdType(NdefMessage::RTD_URI))) == 0)) {
                uri = record->payload_;
                InfoLog("uri: %{public}s", NfcSdkCommon::CodeMiddlePart(uri).c_str());
                if (uri.size() <= 2) {  // 2 is uri identifier length
                    return NfcSdkCommon::HexArrayToStringWithoutChecking(uri);
                }
                int32_t num = 0;
                // 2 is uri identifier length
                if (!NfcSdkCommon::SecureStringToInt(uri.substr(0, 2), num, DECIMAL_NOTATION)) {
                    ErrorLog("SecureStringToInt error");
                    return "";
                }
                std::array<std::string, MAX_URI_CODE_NUM> uriPrefixes = NdefMessage::GetUriPrefixes();
                if (num < 0 || num >= static_cast<int>(uriPrefixes.size())) {
                    return "";
                }
                std::string uriPrefix = uriPrefixes[num];
                InfoLog("uriPrefix = %{public}s", uriPrefix.c_str());
                // 2 is uri identifier length
                return uriPrefix + NfcSdkCommon::HexArrayToStringWithoutChecking(uri.substr(2));
            }
            break;
        default:
            break;
    }
    return uri;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS