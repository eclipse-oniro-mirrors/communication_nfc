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
#include "ndef_tag.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
NdefTag::NdefTag(std::weak_ptr<TagInfo> tag) : BasicTagSession(tag, KITS::TagTechnology::NFC_NDEF_TECH)
{
    DebugLog("NdefTag::NdefTag in");
    if (tag.expired()) {
        DebugLog("NdefTag::NdefTag tag invalid");
        return;
    }
    AppExecFwk::PacMap extraData = tag.lock()->GetTechExtrasData(KITS::TagTechnology::NFC_NDEF_TECH);
    if (!extraData.IsEmpty()) {
        DebugLog("NdefTag::NdefTag extra data invalid");
        return;
    }

    nfcForumType_ = (EmNfcForumType)tag.lock()->GetIntExtrasData(extraData, TagInfo::NDEF_FORUM_TYPE);
    ndefTagMode_ = (EmNdefTagMode)tag.lock()->GetIntExtrasData(extraData, TagInfo::NDEF_TAG_MODE);
    ndefMsg_ = tag.lock()->GetStringExtrasData(extraData, TagInfo::NDEF_MSG);

    DebugLog("NdefTag::NdefTag nfcForumType_(%d) ndefTagMode_(%d) ndefMsg_(%s)",
             nfcForumType_, ndefTagMode_, ndefMsg_.c_str());
}

std::shared_ptr<NdefTag> NdefTag::GetTag(std::weak_ptr<TagInfo> tag)
{
    if (tag.expired() || !tag.lock()->IsTechSupported(KITS::TagTechnology::NFC_NDEF_TECH)) {
        DebugLog("NdefTag::GetTag error");
        return nullptr;
    }

    return std::make_shared<NdefTag>(tag);
}

NdefTag::EmNfcForumType NdefTag::GetNdefTagType() const
{
    return nfcForumType_;
}

NdefTag::EmNdefTagMode NdefTag::GetNdefTagMode() const
{
    return ndefTagMode_;
}

std::shared_ptr<NdefMessage> NdefTag::GetCachedNdefMsg() const
{
    return NdefMessage::GetNdefMessage(ndefMsg_);
}

std::string NdefTag::GetNdefTagTypeString(EmNfcForumType emNfcForumType)
{
    InfoLog("NdefTag::GetNdefTagTypeString in");
    std::string typeString;
    switch (emNfcForumType) {
        case NFC_FORUM_TYPE_1:
            typeString = STRING_NFC_FORUM_TYPE_1;
            break;
        case NFC_FORUM_TYPE_2:
            typeString = STRING_NFC_FORUM_TYPE_2;
            break;
        case NFC_FORUM_TYPE_3:
            typeString = STRING_NFC_FORUM_TYPE_3;
            break;
        case NFC_FORUM_TYPE_4:
            typeString = STRING_NFC_FORUM_TYPE_4;
            break;
        case MIFARE_CLASSIC:
            typeString = STRING_MIFARE_CLASSIC;
            break;
        case ICODE_SLI:
            typeString = STRING_ICODE_SLI;
            break;
        default:
            break;
    }
    DebugLog("[NdefTag::GetNdefTagTypeString] typeString=%d.", emNfcForumType);
    return typeString;
}

bool NdefTag::IsNdefWritable() const
{
    return (ndefTagMode_ == EmNdefTagMode::MODE_READ_WRITE);
}

std::shared_ptr<NdefMessage> NdefTag::ReadNdef()
{
    DebugLog("NdefTag::ReadNdef in.");
    OHOS::sptr<TAG::ITagSession> tagSession = GetRemoteTagSession();
    if (!tagSession) {
        DebugLog("[NdefTag::ReadNdef] tagSession is null.");
        return std::shared_ptr<NdefMessage>();
    }

    if (tagSession->IsNdef(GetTagRfDiscId())) {
        std::string MessageData = tagSession->NdefRead(GetTagRfDiscId());
        if (MessageData.empty() && !tagSession->IsTagFieldOn(GetTagRfDiscId())) {
            DebugLog("[NdefTag::ReadNdef] read ndef message is null and tag is not field on");
            return std::shared_ptr<NdefMessage>();
        }

        return NdefMessage::GetNdefMessage(MessageData);
    } else {
        if (!tagSession->IsTagFieldOn(GetTagRfDiscId())) {
            DebugLog("[NdefTag::ReadNdef] tag is not field on.");
            return std::shared_ptr<NdefMessage>();
        }
    }
    return std::shared_ptr<NdefMessage>();
}

int NdefTag::WriteNdef(std::shared_ptr<NdefMessage> msg)
{
    InfoLog("NdefTag::WriteNdef in.");
    if (!IsConnected()) {
        DebugLog("[NdefTag::WriteNdef] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }

    OHOS::sptr<TAG::ITagSession> tagSession = GetRemoteTagSession();
    if (!tagSession) {
        DebugLog("[NdefTag::WriteNdef] tagSession is null.");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }

    if (tagSession->IsNdef(GetTagRfDiscId())) {
        std::string ndefMessage = NdefMessage::MessageToString(msg);
        for (size_t i = 0; i < ndefMessage.size(); i++) {
            printf("%02x ", ndefMessage.at(i));
        }
        printf("\n");
        return tagSession->NdefWrite(GetTagRfDiscId(), ndefMessage);
    } else {
        DebugLog("[NdefTag::WriteNdef] is not ndef tag!");
        return NfcErrorCode::NFC_SDK_ERROR_NOT_NDEF_TAG;
    }
}

bool NdefTag::IsEnableReadOnly()
{
    OHOS::sptr<TAG::ITagSession> tagSession = GetRemoteTagSession();
    if (!tagSession) {
        DebugLog("[NdefTag::IsEnableReadOnly] tagSession is null.");
        return 0;
    }
    return tagSession->CanMakeReadOnly(nfcForumType_);
}

int NdefTag::EnableReadOnly()
{
    if (!IsConnected()) {
        DebugLog("[NdefTag::EnableReadOnly] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    OHOS::sptr<TAG::ITagSession> tagSession = GetRemoteTagSession();
    if (!tagSession) {
        DebugLog("[NdefTag::EnableReadOnly] tagSession is null.");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    return tagSession->NdefMakeReadOnly(GetTagRfDiscId());
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS