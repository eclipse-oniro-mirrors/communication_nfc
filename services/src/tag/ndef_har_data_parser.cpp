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

using namespace OHOS::NFC::KITS;

std::map<RecordsType, MainErrorCode> g_unsupportTypeAndSysEvent = {
    {TYPE_RTP_SCHEME_TEL, MainErrorCode::NDEF_TEL_EVENT},
    {TYPE_RTP_SCHEME_SMS, MainErrorCode::NDEF_SMS_EVENT},
    {TYPE_RTP_SCHEME_MAIL, MainErrorCode::NDEF_MAIL_EVENT},
    {TYPE_RTP_MIME_TEXT_PLAIN, MainErrorCode::NDEF_TEXT_EVENT},
    {TYPE_RTP_MIME_TEXT_VCARD, MainErrorCode::NDEF_VCARD_EVENT},
};

static void WriteNfcFailedHiSysEvent(MainErrorCode mainErrorCode)
{
    ErrorLog("mainErrorCode %{public}d", static_cast<short>(mainErrorCode));
    NfcFailedParams err;
    ExternalDepsProxy::GetInstance().BuildFailedParams(
        err, mainErrorCode, SubErrorCode::DEFAULT_ERR_DEF);
    ExternalDepsProxy::GetInstance().WriteNfcFailedHiSysEvent(&err);
}

NdefHarDataParser::NdefHarDataParser(std::weak_ptr<NCI::INciTagInterface> nciTagProxy)
    : nciTagProxy_(nciTagProxy)
{
    ndefHarDispatch_ = std::make_shared<NdefHarDispatch>();
}

/* Ndef process function provided to HandleNdefDispatch */
bool NdefHarDataParser::TryNdef(const std::string& msg, const std::shared_ptr<KITS::TagInfo> &tagInfo)
{
    if (msg.empty()) {
        ErrorLog("msg is empty");
        return false;
    }
    std::shared_ptr<NdefMessage> ndef = NdefMessage::GetNdefMessage(msg);
    if (ndef == nullptr) {
        ErrorLog("ndef is nullptr");
        return false;
    }
    std::vector<std::shared_ptr<NdefRecord>> records = ndef->GetNdefRecords();
    ParseMimeTypeAndStr(records);
    if (DispatchByHarBundleName(records, tagInfo)) {
        InfoLog("DispatchByHarBundleName succ");
        return true;
    }

    ParseRecordsProperty(records);
    // handle uri start with HTTP
    if (DispatchByHttpWebLink()) {
        InfoLog("DispatchByHttpWebLink succ");
        return true;
    }
    // handle uri not start with HTTP
    if (DispatchUriToBundleAbility(records)) {
        InfoLog("DispatchUriToBundleAbility succ");
        return true;
    }
    // dispatch Mime type
    if (DispatchMimeToBundleAbility(tagInfo)) {
        InfoLog("DispatchMimeToBundleAbility succ");
        return true;
    }
    WarnLog("TryNdef no handle");
    return false;
}

bool NdefHarDataParser::DispatchByHarBundleName(
    const std::vector<std::shared_ptr<NdefRecord>> &records, const std::shared_ptr<KITS::TagInfo> &tagInfo)
{
    InfoLog("enter");
    std::vector<std::string> harPackages = ExtractHarPackages(records);
    if (harPackages.size() > 0) {
        InfoLog("mimeTypeStr_:%{public}s", mimeTypeStr_.c_str());
        std::string mimeTypeStr = (mimeTypeStr_.size() > MIME_MAX_LENGTH) ? "" : mimeTypeStr_;
        std::string uri = GetUriPayload(records[0]);
        if (uri.size() > URI_MAX_LENGTH) {
            ErrorLog("uri too long");
            uri = "";
        }
        if (ParseHarPackage(harPackages, tagInfo, mimeTypeStr, uri)) {
            InfoLog("matched HAR to NDEF");
            return true;
        }
        /* Handle uninstalled applications */
        WriteNfcFailedHiSysEvent(MainErrorCode::NDEF_APP_NOT_INSTALL);
        harPackages.clear();
    }
    return false;
}

bool NdefHarDataParser::ParseHarPackage(std::vector<std::string> harPackages,
    const std::shared_ptr<KITS::TagInfo> &tagInfo, const std::string &mimeType, const std::string &uri)
{
    if (ParseHarPackageInner(harPackages, tagInfo, mimeType, uri)) {
        return true;
    }
    /* Try vendor parse harPackage */
    if (nciTagProxy_.expired()) {
        ErrorLog("nciTagProxy_ is nullptr");
    } else if (!nciTagProxy_.lock()->VendorParseHarPackage(harPackages)) {
        return false;
    }
    /* Pull up vendor parsed harPackage */
    if (ParseHarPackageInner(harPackages, tagInfo, mimeType, uri)) {
        return true;
    }
    return false;
}

bool NdefHarDataParser::ParseHarPackageInner(const std::vector<std::string> &harPackages,
    const std::shared_ptr<KITS::TagInfo> &tagInfo, const std::string &mimeType, const std::string &uri)
{
    InfoLog("enter");
    if (harPackages.size() == 0) {
        ErrorLog("harPackages is empty");
        return false;
    }
    for (std::string harPackage : harPackages) {
        if (ndefHarDispatch_ != nullptr
            && ndefHarDispatch_->DispatchBundleAbility(harPackage, tagInfo, mimeType, uri)) {
            return true;
        }
    }
    ErrorLog("package not exist");
    return false;
}

bool NdefHarDataParser::StartsWith(const std::string &str, const std::string &prefix)
{
    return str.compare(0, prefix.size(), prefix) == 0;
}

void NdefHarDataParser::ParseRecordsProperty(const std::vector<std::shared_ptr<NdefRecord>> &records)
{
    if (records.size() == 0 || records[0] == nullptr) {
        ErrorLog("records is empty");
        schemeType_ = TYPE_RTP_UNKNOWN;
        return;
    }
    uriAddress_ = GetUriPayload(records[0]);
    InfoLog("uri %{public}s", NfcSdkCommon::CodeMiddlePart(uriAddress_).c_str());
    Uri ndefUri(uriAddress_);
    std::string scheme = ndefUri.GetScheme();
    if (scheme.empty()) {
        schemeType_ = TYPE_RTP_UNKNOWN;
    } else if (scheme == TEL_PREFIX) {
        schemeType_ = TYPE_RTP_SCHEME_TEL;
    } else if (scheme == SMS_PREFIX || scheme == SMSTO_PREFIX) {
        schemeType_ = TYPE_RTP_SCHEME_SMS;
    } else if (StartsWith(scheme, HTTP_PREFIX)) {
        schemeType_ = TYPE_RTP_SCHEME_HTTP_WEB_URL;
        webLink_ = uriAddress_;
    } else if (StartsWith(scheme, MAIL_PREFIX)) {
        schemeType_ = TYPE_RTP_SCHEME_MAIL;
    } else {
        schemeType_ = TYPE_RTP_SCHEME_OTHER;
    }
    InfoLog("schemeType_[%{public}d] scheme[%{public}s]", static_cast<short>(schemeType_), scheme.c_str());
}

bool NdefHarDataParser::DispatchByHttpWebLink()
{
    InfoLog("enter");
    if (schemeType_ == TYPE_RTP_SCHEME_HTTP_WEB_URL) {
        if (nciTagProxy_.expired()) {
            ErrorLog("nciTagProxy_ is nullptr");
            return false;
        }
        if (ndefHarDispatch_ != nullptr && ndefHarDispatch_->DispatchHttpWebLink(webLink_)) {
            return true;
        }
    }
    return false;
}

bool NdefHarDataParser::DispatchUriToBundleAbility(const std::vector<std::shared_ptr<NdefRecord>> &records)
{
    InfoLog("enter");
    if (records.size() == 0 || records[0] == nullptr) {
        ErrorLog("records is empty");
        return false;
    }
    if (records[0]->tnf_ != static_cast<short>(NdefMessage::TNF_WELL_KNOWN)) {
        ErrorLog("tnf_ %{public}d", records[0]->tnf_);
        return false;
    }
    if (schemeType_ == TYPE_RTP_SCHEME_OTHER) {
        if (ndefHarDispatch_ != nullptr && ndefHarDispatch_->DispatchUriToBundleAbility(uriAddress_)) {
            return true;
        }
    } else {
        if (schemeType_ != TYPE_RTP_UNKNOWN) {
            if (g_unsupportTypeAndSysEvent.find(schemeType_) != g_unsupportTypeAndSysEvent.end()) {
                WriteNfcFailedHiSysEvent(g_unsupportTypeAndSysEvent[schemeType_]);
                return true;
            }
        }
    }
    return false;
}

bool NdefHarDataParser::DispatchMimeToBundleAbility(const std::shared_ptr<KITS::TagInfo> &tagInfo)
{
    InfoLog("enter");
    if (mimeType_ == TYPE_RTP_MIME_OTHER) {
        if (ndefHarDispatch_ != nullptr && ndefHarDispatch_->DispatchMimeType(mimeTypeStr_, tagInfo)) {
            return true;
        }
    } else {
        if (mimeType_ != TYPE_RTP_UNKNOWN) {
            if (g_unsupportTypeAndSysEvent.find(mimeType_) != g_unsupportTypeAndSysEvent.end()) {
                WriteNfcFailedHiSysEvent(g_unsupportTypeAndSysEvent[mimeType_]);
                return true;
            }
        }
    }
    return false;
}

/* get mimetype and mime string */
void NdefHarDataParser::ParseMimeTypeAndStr(const std::vector<std::shared_ptr<NdefRecord>> &records)
{
    InfoLog("enter");
    if (records.size() == 0 || records[0] == nullptr) {
        ErrorLog("records is empty");
        mimeType_ = TYPE_RTP_UNKNOWN;
        return;
    }
    InfoLog("record.tnf_: %{public}d", records[0]->tnf_);
    switch (records[0]->tnf_) {
        case NdefMessage::TNF_WELL_KNOWN:
            if (records[0]->tagRtdType_.compare(
                NfcSdkCommon::StringToHexString(NdefMessage::GetTagRtdType(NdefMessage::RTD_TEXT))) == 0) {
                mimeTypeStr_ = TEXT_PLAIN;
            }
            break;
        case NdefMessage::TNF_MIME_MEDIA:
            mimeTypeStr_ = NfcSdkCommon::HexStringToAsciiString(records[0]->tagRtdType_);
            break;
        default:
            mimeTypeStr_ = "";
            break;
    }
    if (mimeTypeStr_ == TEXT_PLAIN) {
        mimeType_ = TYPE_RTP_MIME_TEXT_PLAIN;
    } else if (mimeTypeStr_ == TEXT_VCARD) {
        mimeType_ = TYPE_RTP_MIME_TEXT_VCARD;
    } else if (!mimeTypeStr_.empty()) {
        mimeType_ = TYPE_RTP_MIME_OTHER;
    } else {
        mimeType_ = TYPE_RTP_UNKNOWN;
    }
    InfoLog("mimeType_[%{public}d] mimeTypeStr_[%{public}s]", static_cast<short>(mimeType_), mimeTypeStr_.c_str());
}

std::string NdefHarDataParser::GetUriPayload(const std::shared_ptr<NdefRecord> &record)
{
    if (record == nullptr) {
        ErrorLog("record is nullptr");
        return "";
    }
    return GetUriPayload(record, false);
}

/* get uri data */
std::string NdefHarDataParser::GetUriPayload(const std::shared_ptr<NdefRecord> &record, bool isSmartPoster)
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
            } else if ((record->tagRtdType_.compare(NfcSdkCommon::StringToHexString(
                NdefMessage::GetTagRtdType(NdefMessage::RTD_URI))) == 0)) {
                uri = record->payload_;
                InfoLog("uri: %{public}s", NfcSdkCommon::CodeMiddlePart(uri).c_str());
                if (uri.size() <= 2) {  // 2 is uri identifier length
                    return NfcSdkCommon::HexStringToAsciiString(uri);
                }
                int32_t num = 0;
                if (!KITS::NfcSdkCommon::SecureStringToInt(uri.substr(0, 2), num, KITS::DECIMAL_NOTATION)) { // 2 is uri identifier length
                    ErrorLog("SecureStringToInt error");
                    return 0; // return invalid nfc state
                }
                if (num < 0 || num >= static_cast<int>(g_uriPrefix.size())) {
                    return "";
                }
                std::string uriPrefix = g_uriPrefix[num];
                InfoLog("uriPrefix = %{public}s", uriPrefix.c_str());
                return uriPrefix + NfcSdkCommon::HexStringToAsciiString(uri.substr(2));  // 2 is uri identifier length
            }
            break;
        default:
            break;
    }
    return uri;
}

/* Package name resolution */
std::vector<std::string> NdefHarDataParser::ExtractHarPackages(const std::vector<std::shared_ptr<NdefRecord>> &records)
{
    InfoLog("enter");
    std::vector<std::string> harPackages;
    if (records.size() <= 0) {
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

std::string NdefHarDataParser::CheckForHar(const std::shared_ptr<NdefRecord> &record)
{
    InfoLog("enter");
    if (record == nullptr) {
        ErrorLog("record is nullptr");
        return "";
    }
    std::string bundle = "";
    /* app type judgment */
    if (record->tnf_ == NdefMessage::TNF_EXTERNAL_TYPE &&
        (IsOtherPlatformAppType(record->tagRtdType_) || record->tagRtdType_.compare(
            NfcSdkCommon::StringToHexString(NdefMessage::GetTagRtdType(NdefMessage::RTD_OHOS_APP))) == 0)) {
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
    InfoLog("exit");
    return false;
}
} // namespace TAG
} // namespace NFC
} // namespace OHOS