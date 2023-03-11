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
#include "basic_tag_session.h"

#include "loghelper.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace KITS {
BasicTagSession::BasicTagSession(std::weak_ptr<TagInfo> tagInfo, KITS::TagTechnology technology)
    : tagInfo_(tagInfo), tagTechnology_(technology), isConnected_(false)
{
}

OHOS::sptr<TAG::ITagSession> BasicTagSession::GetTagSessionProxy() const
{
    if (tagInfo_.expired()) {
        ErrorLog("[BasicTagSession::GetTagSessionProxy] tag is null.");
        return nullptr;
    }
    return tagInfo_.lock()->GetTagSessionProxy();
}

int BasicTagSession::Connect()
{
    OHOS::sptr<TAG::ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr) {
        ErrorLog("Connect, ERR_TAG_STATE_UNBIND");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }
    int tagRfDiscId = GetTagRfDiscId();
    int ret = tagSession->Connect(tagRfDiscId, static_cast<int>(tagTechnology_));
    if (ret == ErrorCode::ERR_NONE) {
        isConnected_ = true;
        SetConnectedTagTech(tagTechnology_);
    }
    return ret;
}

bool BasicTagSession::IsConnected() const
{
    if (!isConnected_) {
        return false;
    }
    KITS::TagTechnology connectedTagTech = GetConnectedTagTech();
    if ((connectedTagTech != tagTechnology_) || (connectedTagTech == KITS::TagTechnology::NFC_INVALID_TECH)) {
        return false;
    }
    return true;
}

int BasicTagSession::Close()
{
    OHOS::sptr<TAG::ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr) {
        ErrorLog("Close, ERR_TAG_STATE_UNBIND");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }

    // do reconnect to reset the tag's state.
    isConnected_ = false;
    tagInfo_.lock()->SetConnectedTagTech(KITS::TagTechnology::NFC_INVALID_TECH);

    int statusCode = tagSession->Reconnect(GetTagRfDiscId());
    if (statusCode == ErrorCode::ERR_NONE) {
        isConnected_ = true;
        SetConnectedTagTech(tagTechnology_);
    }
    return statusCode;
}

int BasicTagSession::SetTimeout(int timeout)
{
    OHOS::sptr<TAG::ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr) {
        ErrorLog("SetTimeout, ERR_TAG_STATE_UNBIND");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }
    return tagSession->SetTimeout(timeout, static_cast<int>(tagTechnology_));
}

int BasicTagSession::GetTimeout(int &timeout)
{
    OHOS::sptr<TAG::ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr) {
        ErrorLog("GetTimeout, ERR_TAG_STATE_UNBIND");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }
    return tagSession->GetTimeout(static_cast<int>(tagTechnology_), timeout);
}

std::string BasicTagSession::GetTagUid()
{
    if (tagInfo_.expired()) {
        return "";
    }
    return tagInfo_.lock()->GetTagUid();
}

int BasicTagSession::SendCommand(std::string& hexCmdData, bool raw, std::string &hexRespData)
{
    OHOS::sptr<TAG::ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr) {
        ErrorLog("BasicTagSession::SendCommand tagSession invalid");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }
    return tagSession->SendRawFrame(GetTagRfDiscId(), hexCmdData, raw, hexRespData);
}

int BasicTagSession::GetMaxSendCommandLength(int &maxSize) const
{
    if (tagInfo_.expired() || (tagTechnology_ == KITS::TagTechnology::NFC_INVALID_TECH)) {
        ErrorLog("GetMaxSendCommandLength ERR_TAG_PARAMETERS");
        return ErrorCode::ERR_TAG_PARAMETERS;
    }
    OHOS::sptr<TAG::ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr) {
        ErrorLog("GetMaxSendCommandLength ERR_TAG_STATE_UNBIND");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }
    return tagSession->GetMaxTransceiveLength(static_cast<int>(tagTechnology_), maxSize);
}

int BasicTagSession::GetTagRfDiscId() const
{
    if (tagInfo_.expired()) {
        ErrorLog("[BasicTagSession::GetTagRfDiscId] tag is null.");
        return ErrorCode::ERR_TAG_PARAMETERS;
    }
    return tagInfo_.lock()->GetTagRfDiscId();
}

void BasicTagSession::SetConnectedTagTech(KITS::TagTechnology tech) const
{
    if (tagInfo_.expired()) {
        ErrorLog("[BasicTagSession::SetConnectedTagTech] tag is null.");
        return;
    }
    tagInfo_.lock()->SetConnectedTagTech(tech);
}

KITS::TagTechnology BasicTagSession::GetConnectedTagTech() const
{
    if (tagInfo_.expired()) {
        ErrorLog("[BasicTagSession::GetConnectedTagTech] tag is null.");
        return KITS::TagTechnology::NFC_INVALID_TECH;
    }

    return tagInfo_.lock()->GetConnectedTagTech();
}

std::weak_ptr<TagInfo> BasicTagSession::GetTagInfo() const
{
    return tagInfo_;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
