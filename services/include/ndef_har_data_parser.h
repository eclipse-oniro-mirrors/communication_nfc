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
#include "ndef_message.h"
#include "ndef_har_dispatch.h"
#include "inci_tag_interface.h"
#include "taginfo.h"

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

const std::string HTTP_PREFIX = "http";
const std::string TEL_PREFIX = "tel";
const std::string SMS_PREFIX = "sms";
const std::string SMSTO_PREFIX = "smsto";
const std::string MAIL_PREFIX = "mailto";
const std::string TEXT_PLAIN = "text/plain";
const std::string TEXT_VCARD = "text/vcard";
const int MIME_MAX_LENGTH = 128;
const int URI_MAX_LENGTH = 2048;

class NdefHarDataParser {
public:
    NdefHarDataParser(
        std::weak_ptr<NCI::INciTagInterface> nciTagProxy, std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy);
    ~NdefHarDataParser() {}
    bool TryNdef(const std::string& msg, const std::shared_ptr<KITS::TagInfo> &tagInfo);

private:
    std::string GetUriPayload(const std::shared_ptr<NdefRecord> &record);
    std::string GetUriPayload(const std::shared_ptr<NdefRecord> &record, bool isSmartPoster);
    bool DispatchByHarBundleName(
        const std::vector<std::shared_ptr<NdefRecord>> &records, const std::shared_ptr<KITS::TagInfo> &tagInfo);
    bool ParseHarPackage(std::vector<std::string> harPackages, const std::shared_ptr<KITS::TagInfo> &tagInfo,
        const std::string &mimeType, const std::string &uri);
    bool ParseHarPackageInner(const std::vector<std::string> &harPackages,
        const std::shared_ptr<KITS::TagInfo> &tagInfo, const std::string &mimeType, const std::string &uri);
    void ParseMimeTypeAndStr(const std::vector<std::shared_ptr<NdefRecord>> &records);
    std::vector<std::string> ExtractHarPackages(const std::vector<std::shared_ptr<NdefRecord>> &records);
    std::string CheckForHar(const std::shared_ptr<NdefRecord> &record);
    bool IsOtherPlatformAppType(const std::string &appType);
    bool StartsWith(const std::string &str, const std::string &prefix);
    void ParseRecordsProperty(const std::vector<std::shared_ptr<NdefRecord>> &records);
    bool DispatchByAppLinkMode();
    bool HandleUnsupportSchemeType(const std::vector<std::shared_ptr<NdefRecord>> &records);
    bool DispatchMimeToBundleAbility(const std::shared_ptr<KITS::TagInfo> &tagInfo);

    std::shared_ptr<NdefHarDispatch> ndefHarDispatch_ {nullptr};
    std::weak_ptr<NCI::INciTagInterface> nciTagProxy_ {};
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy_ {};
    RecordsType schemeType_;
    RecordsType mimeType_;
    std::string mimeTypeStr_ {};
    std::string uriAddress_ {};
    std::string uriSchemeValue_ {};
};
} // namespace TAG
} // namespace NFC
} // namespace OHOS
#endif // NDEF_HAR_DATA_PARSER_H