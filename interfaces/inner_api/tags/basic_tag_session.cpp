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
#include "nfc_controller.h"
#include "nfc_sdk_common.h"
#include "tag_session_proxy.h"

namespace OHOS {
namespace NFC {
namespace KITS {
BasicTagSession::BasicTagSession(std::weak_ptr<TagInfo> tagInfo, KITS::TagTechnology technology)
    : tagInfo_(tagInfo), tagTechnology_(technology), isConnected_(false)
{
}

OHOS::sptr<ITagSession> BasicTagSession::GetTagSessionProxy()
{
    bool isNfcOpen = false;
    NfcController::GetInstance().IsNfcOpen(isNfcOpen);
    if (!isNfcOpen) {
        ErrorLog("GetTagSessionProxy: nfc is not open");
        return nullptr;
    }
    return iface_cast<ITagSession>(NfcController::GetInstance().GetTagServiceIface());
}

int BasicTagSession::Connect()
{
    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr || tagSession->AsObject() == nullptr) {
        ErrorLog("Connect, ERR_TAG_STATE_UNBIND");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }
    int tagRfDiscId = GetTagRfDiscId();
    ErrCode ret = tagSession->Connect(tagRfDiscId, static_cast<int>(tagTechnology_));
    if (ret == ERR_NONE) {
        SetConnectedTagTech(tagTechnology_);
        isConnected_ = true;
    }
    return static_cast<int>(ret);
}

bool BasicTagSession::IsConnected()
{
    if (!isConnected_) {
        return false;
    }
    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr || tagSession->AsObject() == nullptr) {
        ErrorLog("IsConnected, ERR_TAG_STATE_UNBIND");
        return false;
    }
    tagSession->IsTagFieldOn(GetTagRfDiscId(), isConnected_);
    return isConnected_;
}

int BasicTagSession::Close()
{
    isConnected_ = false;
    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr || tagSession->AsObject() == nullptr || tagInfo_.expired()) {
        ErrorLog("Close, ERR_TAG_STATE_UNBIND");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }

    // do reconnect to reset the tag's state.
    tagInfo_.lock()->SetConnectedTagTech(KITS::TagTechnology::NFC_INVALID_TECH);

    ErrCode statusCode = tagSession->Reconnect(GetTagRfDiscId());
    if (statusCode == ERR_NONE) {
        SetConnectedTagTech(tagTechnology_);
    }
    return static_cast<int>(statusCode);
}

int BasicTagSession::SetTimeout(int timeout)
{
    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr || tagSession->AsObject() == nullptr) {
        ErrorLog("SetTimeout, ERR_TAG_STATE_UNBIND");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }
    return static_cast<int>(tagSession->SetTimeout(GetTagRfDiscId(), timeout, static_cast<int>(tagTechnology_)));
}

int BasicTagSession::GetTimeout(int &timeout)
{
    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr || tagSession->AsObject() == nullptr) {
        ErrorLog("GetTimeout, ERR_TAG_STATE_UNBIND");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }
    return static_cast<int>(tagSession->GetTimeout(GetTagRfDiscId(), static_cast<int>(tagTechnology_), timeout));
}

void BasicTagSession::ResetTimeout()
{
    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr || tagSession->AsObject() == nullptr) {
        ErrorLog("ResetTimeout, ERR_TAG_STATE_UNBIND");
        return;
    }
    tagSession->ResetTimeout(GetTagRfDiscId());
}

std::string BasicTagSession::GetTagUid()
{
    if (tagInfo_.expired()) {
        ErrorLog("taginfo expired.");
        return "";
    }
    return tagInfo_.lock()->GetTagUid();
}

int BasicTagSession::SendCommand(const std::string& hexCmdData, bool raw, std::string &hexRespData)
{
    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr || tagSession->AsObject() == nullptr) {
        ErrorLog("BasicTagSession::SendCommand tagSession invalid");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }
    return static_cast<int>(tagSession->SendRawFrame(GetTagRfDiscId(), hexCmdData, raw, hexRespData));
}

int BasicTagSession::GetMaxSendCommandLength(int &maxSize)
{
    if (tagInfo_.expired() || (tagTechnology_ == KITS::TagTechnology::NFC_INVALID_TECH)) {
        ErrorLog("GetMaxSendCommandLength ERR_TAG_PARAMETERS");
        return ErrorCode::ERR_TAG_PARAMETERS;
    }
    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr || tagSession->AsObject() == nullptr) {
        ErrorLog("GetMaxSendCommandLength ERR_TAG_STATE_UNBIND");
        return ErrorCode::ERR_TAG_STATE_UNBIND;
    }
    return static_cast<int>(tagSession->GetMaxTransceiveLength(static_cast<int>(tagTechnology_), maxSize));
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
