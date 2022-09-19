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
        ErrorLog("Connect, NFC_SDK_ERROR_NOT_INITIALIZED");
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    int tagRfDiscId = GetTagRfDiscId();
    int ret = tagSession->Connect(tagRfDiscId, static_cast<int>(tagTechnology_));
    DebugLog("Connect, id = %{public}d, tech = %{public}d, ret = %{public}d", tagRfDiscId, tagTechnology_, ret);
    if (ret == NfcErrorCode::NFC_SUCCESS) {
        isConnected_ = true;
        SetConnectedTagTech(tagTechnology_);
    }
    return ret;
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
    // do reconnect to reset the tag's state.
    OHOS::sptr<TAG::ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr) {
        ErrorLog("Close, NFC_SDK_ERROR_NOT_INITIALIZED");
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }

    if (tagSession->Reconnect(GetTagRfDiscId()) != NfcErrorCode::NFC_SUCCESS) {
        ErrorLog("[BasicTagSession::Close] Reconnect fail!");
    }
    isConnected_ = false;
    tagInfo_.lock()->SetConnectedTagTech(KITS::TagTechnology::NFC_INVALID_TECH);
    return NfcErrorCode::NFC_SUCCESS;
}

bool BasicTagSession::SetTimeout(uint32_t timeout)
{
    OHOS::sptr<TAG::ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr) {
        ErrorLog("SetTimeout, NFC_SDK_ERROR_NOT_INITIALIZED");
        return false;
    }
    bool ret = tagSession->SetTimeout(timeout, static_cast<int>(tagTechnology_));
    DebugLog("SetTimeout, timeout = %{public}d, ret = %{public}d", timeout, ret);
    return ret;
}

uint32_t BasicTagSession::GetTimeout()
{
    OHOS::sptr<TAG::ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr) {
        ErrorLog("GetTimeout, NFC_SDK_ERROR_NOT_INITIALIZED");
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    uint32_t ret = tagSession->GetTimeout(static_cast<int>(tagTechnology_));
    DebugLog("GetTimeout, ret = %{public}d", ret);
    return ret;
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
    OHOS::sptr<TAG::ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr) {
        ErrorLog("BasicTagSession::SendCommand tagSession invalid");
        return result;
    }

    std::unique_ptr<TAG::TagRwResponse> res = tagSession->SendRawFrame(GetTagRfDiscId(), data, raw);
    if (res) {
        response = res->GetResult();
        if (res->GetResult() == TAG::TagRwResponse::Status::STATUS_SUCCESS) {
            result = res->GetResData();
        }
        DebugLog("[BasicTagSession::SendCommand] result.%{public}d", response);
    }
    return result;
}

int BasicTagSession::GetMaxSendCommandLength() const
{
    if (tagInfo_.expired() || (tagTechnology_ == KITS::TagTechnology::NFC_INVALID_TECH)) {
        ErrorLog("GetMaxSendCommandLength NFC_SDK_ERROR_INVALID_PARAM");
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }
    OHOS::sptr<TAG::ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr) {
        ErrorLog("GetMaxSendCommandLength NFC_SDK_ERROR_NOT_INITIALIZED");
        return NfcErrorCode::NFC_SDK_ERROR_NOT_INITIALIZED;
    }
    return tagSession->GetMaxTransceiveLength(static_cast<int>(tagTechnology_));
}

int BasicTagSession::GetTagRfDiscId() const
{
    if (tagInfo_.expired()) {
        ErrorLog("[BasicTagSession::GetTagRfDiscId] tag is null.");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_INVALID;
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
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
