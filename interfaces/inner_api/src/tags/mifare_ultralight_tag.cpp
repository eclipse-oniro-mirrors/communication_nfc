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
    if (tag.expired() || !tag.lock()->IsTechSupported(KITS::TagTechnology::NFC_A_TECH) ||
        !tag.lock()->IsTechSupported(KITS::TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH)) {
        ErrorLog("MifareUltralightTag::GetTag error, no mathced technology.");
        return nullptr;
    }

    return std::make_shared<MifareUltralightTag>(tag);
}

int MifareUltralightTag::ReadMultiplePages(uint32_t pageIndex, std::string &hexRespData)
{
    if (!IsConnected()) {
        DebugLog("[MifareUltralightTag::ReadMultiplePages] connect tag first!");
        return ErrorCode::ERR_TAG_STATE_DISCONNECTED;
    }
    if ((pageIndex > 0 && pageIndex < MU_MAX_PAGE_COUNT)) {
        std::string sendCommand = "";
        sendCommand += NfcSdkCommon::UnsignedCharToHexString(MIFARE_ULTRALIGHT_READ);
        sendCommand += NfcSdkCommon::UnsignedCharToHexString(pageIndex & 0xFF);
        return SendCommand(sendCommand, false, hexRespData);
    }
    return ErrorCode::ERR_TAG_PARAMETERS;
}

int MifareUltralightTag::WriteSinglePage(uint32_t pageIndex, const std::string& data)
{
    if (!IsConnected()) {
        DebugLog("[MifareUltralightTag::WriteSinglePage] connect tag first!");
        return ErrorCode::ERR_TAG_STATE_DISCONNECTED;
    }
    if ((pageIndex > 0 && pageIndex < MU_MAX_PAGE_COUNT) &&
        KITS::NfcSdkCommon::GetHexStrBytesLen(data) == MU_PAGE_SIZE) {
        std::string sendCommand = "";
        sendCommand += NfcSdkCommon::UnsignedCharToHexString(MIFARE_ULTRALIGHT_WRITE);
        sendCommand += NfcSdkCommon::UnsignedCharToHexString(pageIndex & 0xFF);
        sendCommand += data;
        std::string hexRespData;
        return SendCommand(sendCommand, false, hexRespData);
    }
    return ErrorCode::ERR_TAG_PARAMETERS;
}

MifareUltralightTag::EmType MifareUltralightTag::GetType() const
{
    return type_;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
