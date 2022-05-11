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

int BasicTagSession::Connect()
{
    if (tagInfo_.expired()) {
        return NfcErrorCode::NFC_SDK_ERROR_TAG_INVALID;
    }
    OHOS::sptr<TAG::ITagSession> tagSession = tagInfo_.lock()->GetRemoteTagSession();
    if (!tagSession) {
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    int tagRfDiscId = GetTagRfDiscId();
    int res = tagSession->Connect(tagRfDiscId, static_cast<int>(tagTechnology_));
    DebugLog("connect tag.%d tech.%d res.%d. ", tagRfDiscId, tagTechnology_, res);
    if (res == NfcErrorCode::NFC_SUCCESS) {
        isConnected_ = true;
        SetConnectedTagTech(tagTechnology_);
    }
    return res;
}

bool BasicTagSession::IsConnected() const
{
    KITS::TagTechnology connectedTagTech = GetConnectedTagTech();
    if ((connectedTagTech != tagTechnology_) || (connectedTagTech == KITS::TagTechnology::NFC_INVALID_TECH)) {
        return false;
    }
    return true;
}

int BasicTagSession::Close()
{
    /* Note that we don't want to physically disconnect the tag,
     * but just reconnect to it to reset its state
     */
    OHOS::sptr<TAG::ITagSession> tagSession = GetRemoteTagSession();
    if (tagInfo_.expired() || !tagSession) {
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }

    if (tagSession->Reconnect(GetTagRfDiscId()) != NfcErrorCode::NFC_SUCCESS) {
        DebugLog("[BasicTagSession::Close] Reconnect fail!");
    }
    isConnected_ = false;
    tagInfo_.lock()->SetConnectedTagTech(KITS::TagTechnology::NFC_INVALID_TECH);
    return NfcErrorCode::NFC_SUCCESS;
}

std::string BasicTagSession::GetTagUid()
{
    if (tagInfo_.expired()) {
        return "";
    }
    return tagInfo_.lock()->GetTagUid();
}

std::string BasicTagSession::SendCommand(std::string& data, bool raw, int& response)
{
    DebugLog("BasicTagSession::SendCommand in");
    std::string result = "";
    OHOS::sptr<TAG::ITagSession> tagSession = GetRemoteTagSession();
    if (!tagSession) {
        DebugLog("BasicTagSession::SendCommand tagSession invalid");
        return result;
    }

    std::unique_ptr<TAG::ResResult> res = tagSession->SendRawFrame(GetTagRfDiscId(), data, raw);
    if (res) {
        response = res->GetResult();
        if (res->GetResult() == TAG::ResResult::ResponseResult::RESULT_SUCCESS) {
            result = res->GetResData();
        }
        DebugLog("[BasicTagSession::SendCommand] result.%d", response);
    }
    return result;
}

int BasicTagSession::GetMaxSendCommandLength() const
{
    if (tagInfo_.expired() || (tagTechnology_ == KITS::TagTechnology::NFC_INVALID_TECH)) {
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }
    OHOS::sptr<TAG::ITagSession> tagSession = GetRemoteTagSession();
    if (!tagSession) {
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    return tagSession->GetMaxTransceiveLength(static_cast<int>(tagTechnology_));
}

OHOS::sptr<TAG::ITagSession> BasicTagSession::GetRemoteTagSession() const
{
    DebugLog("BasicTagSession::GetRemoteTagSession in.");
    if (!IsConnected() || tagInfo_.expired()) {
        DebugLog("[BasicTagSession::GetRemoteTagSession] tag is null.");
        return OHOS::sptr<TAG::ITagSession>();
    }

    return tagInfo_.lock()->GetRemoteTagSession();
}

int BasicTagSession::GetTagRfDiscId() const
{
    if (tagInfo_.expired()) {
        DebugLog("[BasicTagSession::GetTagRfDiscId] tag is null.");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_INVALID;
    }
    return tagInfo_.lock()->GetTagRfDiscId();
}

void BasicTagSession::SetConnectedTagTech(KITS::TagTechnology tech) const
{
    if (tagInfo_.expired()) {
        DebugLog("[BasicTagSession::SetConnectedTagTech] tag is null.");
        return;
    }
    tagInfo_.lock()->SetConnectedTagTech(tech);
}

KITS::TagTechnology BasicTagSession::GetConnectedTagTech() const
{
    if (tagInfo_.expired()) {
        DebugLog("[BasicTagSession::GetConnectedTagTech] tag is null.");
        return KITS::TagTechnology::NFC_INVALID_TECH;
    }

    return tagInfo_.lock()->GetConnectedTagTech();
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
