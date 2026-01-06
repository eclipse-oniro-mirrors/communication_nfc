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
    auto tagPtr = tag.lock();
    if (tagPtr == nullptr) {
        ErrorLog("tag is null.");
        return;
    }
    AppExecFwk::PacMap extraData = tagPtr->GetTechExtrasByTech(KITS::TagTechnology::NFC_NDEF_TECH);
    if (extraData.IsEmpty()) {
        ErrorLog("NdefTag::NdefTag extra data invalid");
        return;
    }

    nfcForumType_ = (EmNfcForumType)tagPtr->GetIntExtrasData(extraData, TagInfo::NDEF_FORUM_TYPE);
    ndefTagMode_ = (EmNdefTagMode)tagPtr->GetIntExtrasData(extraData, TagInfo::NDEF_TAG_MODE);
    ndefMsg_ = tagPtr->GetStringExtrasData(extraData, TagInfo::NDEF_MSG);
    maxTagSize_ = static_cast<uint32_t>(tagPtr->GetIntExtrasData(extraData, TagInfo::NDEF_TAG_LENGTH));

    InfoLog("NdefTag::NdefTag nfcForumType_(%{public}d) ndefTagMode_(%{public}d) maxTagSize_(%{public}d)",
        nfcForumType_, ndefTagMode_, maxTagSize_);
}

std::shared_ptr<NdefTag> NdefTag::GetTag(std::weak_ptr<TagInfo> tag)
{
    auto tagPtr = tag.lock();
    if (tagPtr == nullptr) {
        ErrorLog("tag is null.");
        return nullptr;
    }
    if (tag.expired() || !tagPtr->IsTechSupported(KITS::TagTechnology::NFC_NDEF_TECH)) {
        ErrorLog("NdefTag::GetTag error, no mathced technology.");
        return nullptr;
    }

    return std::make_shared<NdefTag>(tag);
}

EmNfcForumType NdefTag::GetNdefTagType() const
{
    return nfcForumType_;
}

NdefTag::EmNdefTagMode NdefTag::GetNdefTagMode() const
{
    return ndefTagMode_;
}

uint32_t NdefTag::GetMaxTagSize() const
{
    return maxTagSize_;
}

std::shared_ptr<NdefMessage> NdefTag::GetCachedNdefMsg() const
{
    return NdefMessage::GetNdefMessage(ndefMsg_);
}

std::string NdefTag::GetNdefTagTypeString(EmNfcForumType emNfcForumType) const
{
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
    return typeString;
}

bool NdefTag::IsNdefWritable() const
{
    return (ndefTagMode_ == EmNdefTagMode::MODE_READ_WRITE);
}

int NdefTag::ReadNdef(std::shared_ptr<NdefMessage> &ndefMessage)
{
    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (!tagSession || tagSession->AsObject() == nullptr) {
        ErrorLog("[NdefTag::ReadNdef] tagSession is null.");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }

    bool isNdef = false;
    bool isTagFieldOn = false;
    tagSession->IsNdef(GetTagRfDiscId(), isNdef);
    if (isNdef) {
        std::string messageData {};
        ErrCode ret = tagSession->NdefRead(GetTagRfDiscId(), messageData);
        if (ret == ERR_NONE) {
            tagSession->IsTagFieldOn(GetTagRfDiscId(), isTagFieldOn);
            if (messageData.empty() && !isTagFieldOn) {
                ErrorLog("[NdefTag::ReadNdef] read ndef message is null and tag is not field on");
                return ErrorCode::ERR_TAG_STATE_LOST;
            }
        }
        ndefMessage = NdefMessage::GetNdefMessage(messageData);
        return ret;
    } else {
        tagSession->IsTagFieldOn(GetTagRfDiscId(), isTagFieldOn);
        if (!isTagFieldOn) {
            WarnLog("[NdefTag::ReadNdef] tag is not field on.");
            return ErrorCode::ERR_TAG_STATE_LOST;
        }
    }
    return ErrorCode::ERR_TAG_PARAMETERS;
}

int NdefTag::WriteNdef(std::shared_ptr<NdefMessage> msg)
{
    if (!IsConnected()) {
        ErrorLog("[NdefTag::WriteNdef] connect tag first!");
        return ErrorCode::ERR_TAG_STATE_DISCONNECTED;
    }

    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (!tagSession || tagSession->AsObject() == nullptr) {
        ErrorLog("[NdefTag::WriteNdef] tagSession is null.");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }
    bool isNdef = false;
    tagSession->IsNdef(GetTagRfDiscId(), isNdef);
    if (!isNdef) {
        ErrorLog("[NdefTag::WriteNdef] not ndef tag.");
        return ErrorCode::ERR_TAG_PARAMETERS;
    }
    if (!IsNdefWritable()) {
        ErrorLog("[NdefTag::WriteNdef] not writable.");
        return ErrorCode::ERR_TAG_STATE_IO_FAILED;
    }
    std::string ndefMessage = NdefMessage::MessageToString(msg);
    return static_cast<int>(tagSession->NdefWrite(GetTagRfDiscId(), ndefMessage));
}

int NdefTag::IsEnableReadOnly(bool &canSetReadOnly)
{
    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (!tagSession || tagSession->AsObject() == nullptr) {
        DebugLog("[NdefTag::IsEnableReadOnly] tagSession is null.");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }
    return static_cast<int>(tagSession->CanMakeReadOnly(nfcForumType_, canSetReadOnly));
}

int NdefTag::EnableReadOnly()
{
    if (!IsConnected()) {
        ErrorLog("[NdefTag::EnableReadOnly] connect tag first!");
        return ErrorCode::ERR_TAG_STATE_DISCONNECTED;
    }
    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (!tagSession || tagSession->AsObject() == nullptr) {
        ErrorLog("[NdefTag::EnableReadOnly] tagSession is null.");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }
    return static_cast<int>(tagSession->NdefMakeReadOnly(GetTagRfDiscId()));
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS