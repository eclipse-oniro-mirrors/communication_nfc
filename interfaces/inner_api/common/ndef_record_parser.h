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
#ifndef NDEF_RECORD_PARSER_H
#define NDEF_RECORD_PARSER_H

#include <string>
#include <vector>
#include "ndef_message.h"

namespace OHOS {
namespace NFC {
namespace KITS {

const int MIME_MAX_LENGTH = 128;

const std::string TEXT_PLAIN = "text/plain";
const std::string OTHER_PLATFORM_APP_RECORD_TYPE = "616E64726F69642E636F6D3A706B67";

class NdefRecordParser final {
public:
    static std::vector<std::string> ExtractHarPackages(const std::vector<std::shared_ptr<NdefRecord>> &records);
    static std::string GetNdefRecordMimeType(const std::shared_ptr<NdefRecord> &record);
    static std::string GetUriPayload(const std::shared_ptr<NdefRecord> &record);
    
private:
    static std::string CheckForHar(const std::shared_ptr<NdefRecord> &record);
    static bool IsOtherPlatformAppType(const std::string &appType);
    static std::string GetUriPayload(const std::shared_ptr<NdefRecord> &record, bool isSmartPoster);
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS

#endif  // NDEF_RECORD_PARSER_H