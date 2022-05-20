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

namespace OHOS {
namespace NFC {
namespace KITS {
MifareUltralightTag::MifareUltralightTag(std::weak_ptr<TagInfo> tag)
    : BasicTagSession(tag, KITS::TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH)
{
    InfoLog("MifareUltralightTag::MifareUltralightTag in");
    if (tag.expired()) {
        InfoLog("MifareUltralightTag::MifareUltralightTag tag invalid ");
        return;
    }
    AppExecFwk::PacMap extraData = tag.lock()->GetTechExtrasData(KITS::TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH);
    if (!extraData.IsEmpty()) {
        DebugLog("MifareUltralightTag::MifareUltralightTag extra data invalid");
        return;
    }
    InfoLog("MifareUltralightTag::MifareUltralightTag sak.%d tagid.%d",
            tag.lock()->GetIntExtrasData(extraData, TagInfo::SAK),
            tag.lock()->GetTagUid().at(0));
    if ((tag.lock()->GetIntExtrasData(extraData, TagInfo::SAK) == 0x00) &&
        tag.lock()->GetTagUid().at(0) == NXP_MANUFACTURER_ID) {
        InfoLog("MifareUltralightTag::MifareUltralightTag Ctype.%d",
                tag.lock()->GetIntExtrasData(extraData, TagInfo::MIFARE_ULTRALIGHT_C_TYPE));
        if (tag.lock()->GetIntExtrasData(extraData, TagInfo::MIFARE_ULTRALIGHT_C_TYPE)) {
            type_ = EmMifareUltralightType::TYPE_ULTRALIGHT_C;
        } else {
            type_ = EmMifareUltralightType::TYPE_ULTRALIGHT;
        }
    }
}

MifareUltralightTag::~MifareUltralightTag() {}

std::shared_ptr<MifareUltralightTag> MifareUltralightTag::GetTag(std::weak_ptr<TagInfo> tag)
{
    InfoLog("MifareUltralightTag::GetTag in tech len.%d ", tag.lock()->GetTagTechList().size());
    if (tag.expired() || !tag.lock()->IsTechSupported(KITS::TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH)) {
        InfoLog("MifareUltralightTag::GetTag tag invalid");
        return nullptr;
    }

    return std::make_shared<MifareUltralightTag>(tag);
}

std::string MifareUltralightTag::ReadMultiplePages(int pageIndex)
{
    InfoLog("MifareUltralightTag::ReadMultiplePages in.");
    if ((pageIndex > 0 && pageIndex < MU_MAX_PAGE_COUNT) && IsConnected()) {
        char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_ULTRALIGHT_READ, char(pageIndex & 0xFF)};
        std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
        DebugLog("%02X  %02X   ", command[0], command[1]);

        int response = TAG::ResResult::ResponseResult::RESULT_FAILURE;
        return SendCommand(sendCommand, false, response);
    } else {
        DebugLog("[MifareUltralightTag::ReadMultiplePages] pageindex.%d err!", pageIndex);
    }
    return "";
}

int MifareUltralightTag::WriteSinglePages(int pageIndex, const std::string& data)
{
    InfoLog("MifareUltralightTag::WriteSinglePages in.");
    if (!IsConnected()) {
        DebugLog("[MifareUltralightTag::WriteSinglePages] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if ((pageIndex > 0 && pageIndex < MU_MAX_PAGE_COUNT) && (data.size() == MU_PAGE_SIZE)) {
        char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_ULTRALIGHT_WRITE, char(pageIndex & 0xFF)};
        std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
        sendCommand += data;

        int response = TAG::ResResult::ResponseResult::RESULT_FAILURE;
        SendCommand(sendCommand, false, response);
        return response;
    }

    InfoLog("MifareUltralightTag::WriteSinglePages param error!");
    return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
}

MifareUltralightTag::EmMifareUltralightType MifareUltralightTag::GetType() const
{
    return type_;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
