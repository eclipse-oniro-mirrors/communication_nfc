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
#ifndef NDEF_HAR_DATA_PARSER_H
#define NDEF_HAR_DATA_PARSER_H

#include <string>
#include <vector>
#include "ndef_message.h"
#include "ndef_har_dispatch.h"
#include "inci_tag_interface.h"
#include "taginfo.h"
#include "nfc_service.h"

namespace OHOS {
namespace NFC {
namespace TAG {
using namespace OHOS::NFC::KITS;

enum RecordsType {
    TYPE_RTP_UNKNOWN = 0,
    TYPE_RTP_SCHEME_TEL,
    TYPE_RTP_SCHEME_SMS,
    TYPE_RTP_SCHEME_HTTP_WEB_URL,
    TYPE_RTP_SCHEME_MAIL,
    TYPE_RTP_SCHEME_OTHER,
    TYPE_RTP_MIME_TEXT_PLAIN,
    TYPE_RTP_MIME_TEXT_VCARD,
    TYPE_RTP_MIME_OTHER
};

enum DispatchResult : const uint16_t {
    DISPATCH_UNKNOWN     = 0,
    DISPATCH_FOREGROUND  = 1,
    DISPATCH_READERMODE  = 2,
    DISPATCH_BT          = 3,
    DISPATCH_WIFI        = 4,
    DISPATCH_CALLBACK    = 5,
    DISPATCH_TRAFFIC     = 6,
    DISPATCH_HAR         = 7,
    DISPATCH_TEXT        = 8,
    DISPATCH_UNKNOWN_TAG = 9,
    DISPATCH_BUNDLENAME  = 10,
    DISPATCH_APP_LINK    = 11,
    NDEF_TEL_EVENT       = 301, // copy g_unsupportTypeAndSysEvent MainErrorCode
    NDEF_SMS_EVENT       = 302,
    NDEF_MAIL_EVENT      = 303,
    NDEF_TEXT_EVENT      = 304,
    NDEF_VCARD_EVENT     = 305,
};

const std::string HTTP_PREFIX = "http";
const std::string TEL_PREFIX = "tel";
const std::string SMS_PREFIX = "sms";
const std::string SMSTO_PREFIX = "smsto";
const std::string MAIL_PREFIX = "mailto";
const std::string TEXT_PLAIN = "text/plain";
const std::string TEXT_VCARD = "text/vcard";
const int MIME_MAX_LENGTH = 128;
const int URI_MAX_LENGTH = 2048;
const int RECORD_LIST_MAX_SIZE = 20;

class NdefHarDataParser {
public:
    static NdefHarDataParser &GetInstance();
    void Initialize(std::weak_ptr<NfcService> nfcService, std::weak_ptr<NCI::INciTagInterface> nciTagProxy,
        std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy);
    uint16_t TryNdef(const std::string &msg, const std::shared_ptr<KITS::TagInfo> &tagInfo);
    std::string GetRecord0Uri();
    void ClearRecord0Uri();

private:
    NdefHarDataParser();
    ~NdefHarDataParser() {}
    std::string GetUriPayload(const std::shared_ptr<NdefRecord> &record);
    std::string GetUriPayload(const std::shared_ptr<NdefRecord> &record, bool isSmartPoster);
    uint16_t DispatchByHarBundleName(
        const std::vector<std::shared_ptr<NdefRecord>> &records, const std::shared_ptr<KITS::TagInfo> &tagInfo);
    bool ParseHarPackage(std::vector<std::string> harPackages, const std::shared_ptr<KITS::TagInfo> &tagInfo,
        const std::string &mimeType, const std::string &uri);
    bool DispatchAllHarPackage(const std::vector<std::string> &harPackages,
        const std::shared_ptr<KITS::TagInfo> &tagInfo, const std::string &mimeType, const std::string &uri);
    void ParseMimeTypeAndStr(const std::vector<std::shared_ptr<NdefRecord>> &records);
    std::vector<std::string> ExtractHarPackages(const std::vector<std::shared_ptr<NdefRecord>> &records);
    std::string CheckForHar(const std::shared_ptr<NdefRecord> &record);
    bool IsOtherPlatformAppType(const std::string &appType);
    bool StartsWith(const std::string &str, const std::string &prefix);
    void ParseRecordsProperty(const std::vector<std::shared_ptr<NdefRecord>> &records);
    uint16_t DispatchByAppLinkMode(const std::shared_ptr<KITS::TagInfo> &tagInfo);
    uint16_t HandleUnsupportSchemeType(const std::vector<std::shared_ptr<NdefRecord>> &records);
    uint16_t DispatchMimeToBundleAbility(const std::shared_ptr<KITS::TagInfo> &tagInfo);
    uint16_t DispatchValidNdef(
        const std::vector<std::shared_ptr<NdefRecord>> &records, const std::shared_ptr<KITS::TagInfo> &tagInfo);
    void ClearNdefDispatchParam();

    std::shared_ptr<NdefHarDispatch> ndefHarDispatch_ {nullptr};
    std::weak_ptr<NCI::INciTagInterface> nciTagProxy_ {};
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy_ {};
    RecordsType schemeType_ {RecordsType::TYPE_RTP_UNKNOWN};
    std::string uriAddress_ {};
    std::string uriSchemeValue_ {};
    std::vector<std::pair<RecordsType, std::string>> mimeTypeVec_ {};

    std::weak_ptr<NfcService> nfcService_ {};
    std::mutex mutex_ {};
    bool isInitialized_ = false;
    std::string recordUriInfo_ = "";
};
} // namespace TAG
} // namespace NFC
} // namespace OHOS
#endif // NDEF_HAR_DATA_PARSER_H