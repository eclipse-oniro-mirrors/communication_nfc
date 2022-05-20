/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "ndef_formatable_tag.h"

#include "loghelper.h"
#include "mifare_classic_tag.h"

namespace OHOS {
namespace NFC {
namespace KITS {
NdefFormatableTag::NdefFormatableTag(std::weak_ptr<TagInfo> tag)
    : BasicTagSession(tag, KITS::TagTechnology::NFC_NDEF_FORMATABLE_TECH) {}

std::shared_ptr<NdefFormatableTag> NdefFormatableTag::GetTag(std::weak_ptr<TagInfo> tag)
{
    if (tag.expired() || !tag.lock()->IsTechSupported(KITS::TagTechnology::NFC_NDEF_FORMATABLE_TECH)) {
        return nullptr;
    }

    return std::make_shared<NdefFormatableTag>(tag);
}

int NFC::KITS::NdefFormatableTag::Format(std::weak_ptr<NdefMessage> firstMessage)
{
    return Format(firstMessage, false);
}

int NFC::KITS::NdefFormatableTag::FormatReadOnly(std::weak_ptr<NdefMessage> firstMessage)
{
    return Format(firstMessage, true);
}

int NFC::KITS::NdefFormatableTag::Format(std::weak_ptr<NdefMessage> firstMessage, bool bMakeReadOnly)
{
    OHOS::sptr<TAG::ITagSession> tagSession = GetRemoteTagSession();
    if (!tagSession) {
        ErrorLog("[NdefTag::ReadNdef] tagSession is null.");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_INVALID;
    }

    int tagRfDiscId = GetTagRfDiscId();
    std::string keyDefault(MifareClassicTag::MC_KEY_DEFAULT, MifareClassicTag::MC_KEY_LEN);
    int res = tagSession->FormatNdef(tagRfDiscId, keyDefault);
    if (res != NfcErrorCode::NFC_SUCCESS) {
        return res;
    }

    if (!tagSession->IsNdef(tagRfDiscId)) {
        return NfcErrorCode::NFC_SDK_ERROR_TAG_INVALID;
    }

    if (!firstMessage.expired()) {
        std::string ndefMessage = NdefMessage::MessageToString(firstMessage);
        if (ndefMessage.empty()) {
            return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
        }
        res = tagSession->NdefWrite(tagRfDiscId, ndefMessage);
        if (res != NfcErrorCode::NFC_SUCCESS) {
            return res;
        }
    }

    if (bMakeReadOnly) {
        if (!tagSession->CanMakeReadOnly(tagRfDiscId)) {
            return NfcErrorCode::NFC_SDK_ERROR_DISABLE_MAKE_READONLY;
        }
        res = tagSession->NdefMakeReadOnly(tagRfDiscId);
        if (res != NfcErrorCode::NFC_SUCCESS) {
            return res;
        }
    }
    return NfcErrorCode::NFC_SUCCESS;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
