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
#include "mifare_ultralight_tag.h"
#include "loghelper.h"
#include "nfca_tag.h"

namespace OHOS {
namespace NFC {
namespace KITS {
MifareUltralightTag::MifareUltralightTag(std::weak_ptr<TagInfo> tag)
    : BasicTagSession(tag, KITS::TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH)
{
    InfoLog("MifareUltralightTag::MifareUltralightTag in");
    if (tag.expired()) {
        ErrorLog("MifareUltralightTag, tag invalid");
        return;
    }
    type_ = EmType::TYPE_UNKNOWN;
    std::shared_ptr<NfcATag> nfcA = NfcATag::GetTag(tag);
    if (nfcA == nullptr) {
        ErrorLog("MifareUltralightTag, not support NfcA.");
        return;
    }
    if (tag.lock()->GetTagUid().empty()) {
        ErrorLog("MifareUltralightTag, tag uid is empty.");
        return;
    }

    if (nfcA->GetSak() == 0x00 &&
        KITS::NfcSdkCommon::GetByteFromHexStr(tag.lock()->GetTagUid(), 0) == NXP_MANUFACTURER_ID) {
        AppExecFwk::PacMap extraData = tag.lock()->GetTechExtrasByTech(KITS::TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH);
        if (tag.lock()->GetBoolExtrasData(extraData, TagInfo::MIFARE_ULTRALIGHT_C_TYPE)) {
            type_ = EmType::TYPE_ULTRALIGHT_C;
        } else {
            type_ = EmType::TYPE_ULTRALIGHT;
        }
    }
}

MifareUltralightTag::~MifareUltralightTag()
{
}

std::shared_ptr<MifareUltralightTag> MifareUltralightTag::GetTag(std::weak_ptr<TagInfo> tag)
{
    InfoLog("MifareUltralightTag::GetTag in tech len.%{public}d ", (int)tag.lock()->GetTagTechList().size());
    if (tag.expired() || !tag.lock()->IsTechSupported(KITS::TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH)) {
        ErrorLog("MifareUltralightTag::GetTag tag invalid");
        return nullptr;
    }

    return std::make_shared<MifareUltralightTag>(tag);
}

std::string MifareUltralightTag::ReadMultiplePages(uint32_t pageIndex)
{
    InfoLog("MifareUltralightTag::ReadMultiplePages in.");
    if (!IsConnected()) {
        DebugLog("[MifareUltralightTag::ReadMultiplePages] connect tag first!");
        return "";
    }
    if ((pageIndex > 0 && pageIndex < MU_MAX_PAGE_COUNT)) {
        char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_ULTRALIGHT_READ, char(pageIndex & 0xFF)};
        std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
        DebugLog("%02X  %02X   ", command[0], command[1]);

        int response = TAG::TagRwResponse::Status::STATUS_FAILURE;
        return SendCommand(sendCommand, false, response);
    } else {
        ErrorLog("[MifareUltralightTag::ReadMultiplePages] pageindex.%{public}d err!", pageIndex);
    }
    return "";
}

int MifareUltralightTag::WriteSinglePages(uint32_t pageIndex, const std::string& data)
{
    InfoLog("MifareUltralightTag::WriteSinglePages in.");
    if (!IsConnected()) {
        DebugLog("[MifareUltralightTag::WriteSinglePages] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if ((pageIndex > 0 && pageIndex < MU_MAX_PAGE_COUNT) && KITS::NfcSdkCommon::GetHexStrBytesLen(data) == MU_PAGE_SIZE) {
        char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_ULTRALIGHT_WRITE, char(pageIndex & 0xFF)};
        std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
        sendCommand += data;

        int response = TAG::TagRwResponse::Status::STATUS_FAILURE;
        SendCommand(sendCommand, false, response);
        return response;
    }

    ErrorLog("MifareUltralightTag::WriteSinglePages param error!");
    return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
}

MifareUltralightTag::EmType MifareUltralightTag::GetType() const
{
    return type_;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
