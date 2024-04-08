/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "ndef_har_data_parser.h"

#include "ndef_har_dispatch.h"
#include "nfc_sdk_common.h"
#include "uri.h"
#include "loghelper.h"
#include "nfc_hisysevent.h"
#include "external_deps_proxy.h"

namespace OHOS {
namespace NFC {
namespace TAG {
const std::string HTTP_PREFIX = "http";
const std::string TEL_PREFIX = "tel";
const std::string SMS_PREFIX = "sms";
const std::string MAIL_PREFIX = "mailto";
const std::string TEXT_PLAIN = "text/plain";
const std::string TEXT_VCARD = "text/vcard";

using namespace OHOS::NFC::KITS;

NdefHarDataParser::NdefHarDataParser(std::weak_ptr<NCI::INciTagInterface> nciTagProxy)
    : nciTagProxy_(nciTagProxy)
{
    ndefHarDispatch_ = std::make_shared<NdefHarDispatch>();
}

/* Ndef process function provided to HandleNdefDispatch */
bool NdefHarDataParser::TryNdef(const std::string& msg, std::shared_ptr<KITS::TagInfo> tagInfo)
{
    InfoLog("NdefHarDataParser::TryNdef enter");
    if (msg.empty()) {
        ErrorLog("NdefHarDataParser::TryNdef msg is empty");
        return false;
    }
    std::shared_ptr<NdefMessage> ndef = NdefMessage::GetNdefMessage(msg);
    if (ndef == nullptr) {
        ErrorLog("NdefHarDataParser::TryNdef ndef is nullptr");
        return false;
    }
    std::vector<std::shared_ptr<NdefRecord>> records = ndef->GetNdefRecords();
    /* pull up app */
    std::vector<std::string> harPackages = ExtractHarPackages(records);
    if (harPackages.size() > 0) {
        std::string mimeType = ToMimeType(records[0]);
        if (ParseHarPackage(harPackages, tagInfo, mimeType)) {
            InfoLog("NdefHarDataParser::TryNdef matched HAR to NDEF");
            return true;
        }
        /* Handle uninstalled applications */
        NfcFailedParams err;
        ExternalDepsProxy::GetInstance().BuildFailedParams(
            err, MainErrorCode::NDEF_APP_NOT_INSTALL, SubErrorCode::DEFAULT_ERR_DEF);
        ExternalDepsProxy::GetInstance().WriteNfcFailedHiSysEvent(&err);
        harPackages.clear();
    }
    /* Pull up browser */
    if (ParseWebLink(records)) {
        InfoLog("NdefHarDataParser::matched web link");
        return true;
    }
    /* URI parsing of phone, SMS, email, pull URI type app */
    if (ParseUriLink(records)) {
        return true;
    }
    /* Handle notepads, contacts, and mimetype app */
    if (ParseOtherType(records, tagInfo)) {
        return true;
    }
    InfoLog("NdefHarDataParser::TryNdef exit");
    return false;
}

bool NdefHarDataParser::ParseHarPackage(
    std::vector<std::string> harPackages, std::shared_ptr<KITS::TagInfo> tagInfo, const std::string &mimeType)
{
    InfoLog("NdefHarDataParser::ParseHarPackage enter");
    if (harPackages.size() <= 0) {
        ErrorLog("NdefHarDataParser::ParseHarPackage harPackages is empty");
        return false;
    }
    for (std::string harPackage : harPackages) {
        if (ndefHarDispatch_ != nullptr && ndefHarDispatch_->DispatchBundleAbility(harPackage, tagInfo, mimeType)) {
            return true;
        }
    }
    ErrorLog("NdefHarDataParser::ParseHarPackage package not exist");
    return false;
}

/* Handle notepads, contacts, and mimetype app */
bool NdefHarDataParser::ParseOtherType(
    std::vector<std::shared_ptr<NdefRecord>> records, std::shared_ptr<KITS::TagInfo> tagInfo)
{
    InfoLog("NdefHarDataParser::ParseOtherType enter");
    if (records.size() <= 0 || records[0] == nullptr) {
        ErrorLog("NdefHarDataParser::ParseOtherType records is empty");
        return false;
    }
    NfcFailedParams err;
    std::string type = ToMimeType(records[0]);
    if (!type.empty()) {
        if (type == TEXT_PLAIN) {
            ErrorLog("NdefHarDataParser::ParseOtherType -> TEXT");
            ExternalDepsProxy::GetInstance().BuildFailedParams(
                err, MainErrorCode::NDEF_TEXT_EVENT, SubErrorCode::DEFAULT_ERR_DEF);
            ExternalDepsProxy::GetInstance().WriteNfcFailedHiSysEvent(&err);
            return true;
        } else if (type == TEXT_VCARD) {
            ErrorLog("NdefHarDataParser::ParseOtherType -> VCARD");
            ExternalDepsProxy::GetInstance().BuildFailedParams(
                err, MainErrorCode::NDEF_VCARD_EVENT, SubErrorCode::DEFAULT_ERR_DEF);
            ExternalDepsProxy::GetInstance().WriteNfcFailedHiSysEvent(&err);
            return true;
        } else {
            if (ndefHarDispatch_ != nullptr && ndefHarDispatch_->DispatchMimeType(type, tagInfo)) {
                return true;
            }
        }
    }
    InfoLog("NdefHarDataParser::ParseOtherType exit");
    return false;
}

/* URI parsing of phone, SMS, email, pull URI type app */
bool NdefHarDataParser::ParseUriLink(std::vector<std::shared_ptr<NdefRecord>> records)
{
    InfoLog("NdefHarDataParser::ParseUriLink enter");
    if (records.size() <= 0 || records[0] == nullptr) {
        ErrorLog("NdefHarDataParser::ParseUriLink records is empty");
        return false;
    }
    if (records[0]->tnf_ == static_cast<short>(NdefMessage::TNF_WELL_KNOWN)) {
        std::string uri = GetUriPayload(records[0]);
        InfoLog("NdefHarDataParser::ParseUriLink uri: %{public}s", NfcSdkCommon::CodeMiddlePart(uri).c_str());
        Uri ndefUri(uri);
        std::string scheme = ndefUri.GetScheme();
        if (!scheme.empty()) {
            NfcFailedParams err;
            if ((scheme.size() >= 3) && (scheme.substr(0, 3) == TEL_PREFIX)) {  // 3 is tel length
                ErrorLog("NdefHarDataParser::ParseUriLink -> TEL");
                ExternalDepsProxy::GetInstance().BuildFailedParams(
                    err, MainErrorCode::NDEF_TEL_EVENT, SubErrorCode::DEFAULT_ERR_DEF);
                ExternalDepsProxy::GetInstance().WriteNfcFailedHiSysEvent(&err);
                return true;
            } else if ((scheme.size() >= 3) && (scheme.substr(0, 3) == SMS_PREFIX)) {   // 3 is sms length
                ErrorLog("NdefHarDataParser::ParseUriLink -> SMS");
                ExternalDepsProxy::GetInstance().BuildFailedParams(
                    err, MainErrorCode::NDEF_SMS_EVENT, SubErrorCode::DEFAULT_ERR_DEF);
                ExternalDepsProxy::GetInstance().WriteNfcFailedHiSysEvent(&err);
                return true;
            } else if ((scheme.size() >= 6) && (scheme.substr(0, 6) == MAIL_PREFIX)) {  // 6 is mailto length
                ErrorLog("NdefHarDataParser::ParseUriLink -> MAIL");
                ExternalDepsProxy::GetInstance().BuildFailedParams(
                    err, MainErrorCode::NDEF_MAIL_EVENT, SubErrorCode::DEFAULT_ERR_DEF);
                ExternalDepsProxy::GetInstance().WriteNfcFailedHiSysEvent(&err);
                return true;
            }
        }
        /* uri to package */
        if (ndefHarDispatch_ != nullptr && ndefHarDispatch_->DispatchUriToBundleAbility(uri)) {
            return true;
        }
    }
    InfoLog("NdefHarDataParser::ParseUriLink exit");
    return false;
}

/* handle uri types */
bool NdefHarDataParser::ParseWebLink(std::vector<std::shared_ptr<NdefRecord>> records)
{
    InfoLog("NdefHarDataParser::ParseWebLink enter");
    if (records.size() <= 0) {
        ErrorLog("NdefHarDataParser::ParseWebLink records is empty");
        return false;
    }
    std::string uri = IsWebUri(records[0]);
    if (!uri.empty()) {
        if (nciTagProxy_.expired()) {
            ErrorLog("NdefHarDataParser::ParseWebLink nciTagProxy_ is nullptr");
            return false;
        }
        std::string browserBundleName = nciTagProxy_.lock()->GetVendorBrowserBundleName();
        if (ndefHarDispatch_ == nullptr) {
            ErrorLog("NdefHarDataParser::ParseWebLink ndefHarDispatch_ is nullptr");
            return false;
        }
        if (ndefHarDispatch_->DispatchWebLink(uri, browserBundleName)) {
            return true;
        }
    }
    ErrorLog("NdefHarDataParser::ParseWebLink fail");
    return false;
}

/* Is it HTTP */
std::string NdefHarDataParser::IsWebUri(std::shared_ptr<NdefRecord> record)
{
    InfoLog("NdefHarDataParser::IsWebUri enter");
    if (record == nullptr) {
        ErrorLog("NdefHarDataParser::IsWebUri record is nullptr");
        return "";
    }
    std::string uri = GetUriPayload(record);
    InfoLog("NdefHarDataParser::IsWebUri is uri size: %{public}s", NfcSdkCommon::CodeMiddlePart(uri).c_str());
    Uri ndefUri(uri);
    std::string scheme = ndefUri.GetScheme();
    if (!scheme.empty()) {
        if ((scheme.size() >= 4) && (scheme.substr(0, 4) == HTTP_PREFIX)) { // 4 is http length
            return uri;
        }
    }
    ErrorLog("NdefHarDataParser::IsWebUri exit");
    return "";
}

/* get mimetype data */
std::string NdefHarDataParser::ToMimeType(std::shared_ptr<NdefRecord> record)
{
    InfoLog("NdefHarDataParser::ToMimeType enter");
    if (record == nullptr) {
        ErrorLog("NdefHarDataParser::ToMimeType record is nullptr");
        return "";
    }
    std::string type = "";
    InfoLog("NdefHarDataParser::ToMimeType record.tnf_: %{public}d", record->tnf_);
    switch (record->tnf_) {
        case NdefMessage::TNF_WELL_KNOWN:
            if (record->tagRtdType_.compare(
                NfcSdkCommon::StringToHexString(NdefMessage::GetTagRtdType(NdefMessage::RTD_TEXT))) == 0) {
                return TEXT_PLAIN;
            }
            break;
        case NdefMessage::TNF_MIME_MEDIA:
            type = NfcSdkCommon::HexStringToAsciiString(record->tagRtdType_);
            return type;
    }
    return type;
}

std::string NdefHarDataParser::GetUriPayload(std::shared_ptr<NdefRecord> record)
{
    if (record == nullptr) {
        ErrorLog("NdefHarDataParser::GetUriPayload record is nullptr");
        return "";
    }
    return GetUriPayload(record, false);
}

/* get uri data */
std::string NdefHarDataParser::GetUriPayload(std::shared_ptr<NdefRecord> record, bool isSmartPoster)
{
    InfoLog("NdefHarDataParser::GetUriPayload enter");
    if (record == nullptr) {
        ErrorLog("NdefHarDataParser::GetUriPayload record is nullptr");
        return "";
    }
    std::string uri = "";
    InfoLog("NdefHarDataParser::GetUriPayload record.tnf_: %{public}d", record->tnf_);
    switch (record->tnf_) {
        case NdefMessage::TNF_WELL_KNOWN:
            InfoLog("GetUriPayload tagRtdType: %{public}s", NfcSdkCommon::CodeMiddlePart(record->tagRtdType_).c_str());
            if ((record->tagRtdType_.compare(NfcSdkCommon::StringToHexString(
                NdefMessage::GetTagRtdType(NdefMessage::RTD_SMART_POSTER))) == 0) && !isSmartPoster) {
                std::shared_ptr<NdefMessage> nestMessage = NdefMessage::GetNdefMessage(record->payload_);
                InfoLog("GetUriPayload payload: %{public}s", NfcSdkCommon::CodeMiddlePart(record->payload_).c_str());
                if (nestMessage == nullptr) {
                    ErrorLog("NdefHarDataParser::GetUriPayload nestMessage is nullptr");
                    return "";
                }
                std::vector<std::shared_ptr<NdefRecord>> nestRecords = nestMessage->GetNdefRecords();
                for (std::shared_ptr<NdefRecord> nestRecord : nestRecords) {
                    uri = GetUriPayload(nestRecord, true);
                    return uri;
                }
            } else if ((record->tagRtdType_.compare(NfcSdkCommon::StringToHexString(
                NdefMessage::GetTagRtdType(NdefMessage::RTD_URI))) == 0)) {
                uri = record->payload_;
                InfoLog("NdefHarDataParser::GetUriPayload uri: %{public}s", NfcSdkCommon::CodeMiddlePart(uri).c_str());
                if (uri.size() <= 2) {  // 2 is uri identifier length
                    return NfcSdkCommon::HexStringToAsciiString(uri);
                }
                if (std::stoi(uri.substr(0, 2)) < 0 ||  // 2 is uri identifier length
                    std::stoi(uri.substr(0, 2)) >= static_cast<int>(g_uriPrefix.size())) {
                    return "";
                }
                std::string uriPrefix = g_uriPrefix[std::stoi(uri.substr(0, 2))];   // 2 is uri identifier length
                InfoLog("NdefHarDataParser::GetUriPayload uriPrefix = %{public}s", uriPrefix.c_str());
                return uriPrefix + NfcSdkCommon::HexStringToAsciiString(uri.substr(2));  // 2 is uri identifier length
            }
            break;
        default:
            break;
    }
    return uri;
}

/* Package name resolution */
std::vector<std::string> NdefHarDataParser::ExtractHarPackages(std::vector<std::shared_ptr<NdefRecord>> records)
{
    InfoLog("NdefHarDataParser::ExtractHarPackages enter");
    std::vector<std::string> harPackages;
    if (records.size() <= 0) {
        ErrorLog("NdefHarDataParser::ExtractHarPackages records is empty");
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

std::string NdefHarDataParser::CheckForHar(std::shared_ptr<NdefRecord> record)
{
    InfoLog("NdefHarDataParser:: CheckForHar enter");
    if (record == nullptr) {
        ErrorLog("NdefHarDataParser::CheckForHar record is nullptr");
        return "";
    }
    std::string bundle = "";
    /* app type judgment */
    if (record->tnf_ == NdefMessage::TNF_EXTERNAL_TYPE &&
        (IsOtherPlatformAppType(record->tagRtdType_) || record->tagRtdType_.compare(
            NfcSdkCommon::StringToHexString(NdefMessage::GetTagRtdType(NdefMessage::RTD_OHOS_APP))))) {
        return record->payload_;
    }
    return bundle;
}

/* support parse and launch for other platform app type */
bool NdefHarDataParser::IsOtherPlatformAppType(const std::string &appType)
{
    const std::string OTHER_PLATFORM_APP_RECORD_TYPE = "android.com:pkg";
    if (appType.compare(NfcSdkCommon::StringToHexString(OTHER_PLATFORM_APP_RECORD_TYPE)) == 0) {
        return true;
    }
    InfoLog("NdefHarDataParser::IsOtherPlatformAppType exit");
    return false;
}
} // namespace TAG
} // namespace NFC
} // namespace OHOS