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

class NdefHarDataParser {
public:
    NdefHarDataParser(std::weak_ptr<NCI::INciTagInterface> nciTagProxy);
    ~NdefHarDataParser() {}
    bool TryNdef(const std::string& msg, std::shared_ptr<KITS::TagInfo> tagInfo);

private:
    std::string IsWebUri(std::shared_ptr<NdefRecord> record);
    std::string GetUriPayload(std::shared_ptr<NdefRecord> record);
    std::string GetUriPayload(std::shared_ptr<NdefRecord> record, bool isSmartPoster);
    bool ParseWebLink(std::vector<std::shared_ptr<NdefRecord>> records);
    bool ParseHarPackage(
        std::vector<std::string> harPackages, std::shared_ptr<KITS::TagInfo> tagInfo, const std::string &mimeType);
    bool ParseUriLink(std::vector<std::shared_ptr<NdefRecord>> records);
    bool ParseOtherType(std::vector<std::shared_ptr<NdefRecord>> records, std::shared_ptr<KITS::TagInfo> tagInfo);
    std::string ToMimeType(std::shared_ptr<NdefRecord> record);
    std::vector<std::string> ExtractHarPackages(std::vector<std::shared_ptr<NdefRecord>> records);
    std::string CheckForHar(std::shared_ptr<NdefRecord> record);
    bool IsOtherPlatformAppType(const std::string &appType);

    std::shared_ptr<NdefHarDispatch> ndefHarDispatch_ {nullptr};
    std::weak_ptr<NCI::INciTagInterface> nciTagProxy_ {};
};
} // namespace TAG
} // namespace NFC
} // namespace OHOS
#endif // NDEF_HAR_DATA_PARSER_H