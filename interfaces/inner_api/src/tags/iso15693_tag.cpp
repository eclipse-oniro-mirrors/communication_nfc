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
#include "iso15693_tag.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
Iso15693Tag::Iso15693Tag(std::weak_ptr<TagInfo> tag) : BasicTagSession(tag, KITS::TagTechnology::NFC_V_TECH)
{
    AppExecFwk::PacMap extraData = tag.lock()->GetTechExtrasByTech(KITS::TagTechnology::NFC_V_TECH);
    if (extraData.IsEmpty()) {
        ErrorLog("Iso15693Tag::Iso15693Tag extra data invalid");
        return;
    }
    dsfId_ = char(tag.lock()->GetIntExtrasData(extraData, TagInfo::DSF_ID));
    respFlags_ = char(tag.lock()->GetIntExtrasData(extraData, TagInfo::RESPONSE_FLAGS));
}

Iso15693Tag::~Iso15693Tag()
{
    dsfId_ = 0;
    respFlags_ = 0;
}

std::shared_ptr<Iso15693Tag> Iso15693Tag::GetTag(std::weak_ptr<TagInfo> tag)
{
    if (tag.expired() || !tag.lock()->IsTechSupported(KITS::TagTechnology::NFC_V_TECH)) {
        ErrorLog("Iso15693Tag::GetTag error, no mathced technology.");
        return nullptr;
    }
    return std::make_shared<Iso15693Tag>(tag);
}

int Iso15693Tag::ReadSingleBlock(uint32_t flag, uint32_t blockIndex, std::string &hexRespData)
{
    if (!IsConnected()) {
        ErrorLog("[Iso15693Tag::ReadSingleBlock] connect tag first!");
        return ErrorCode::ERR_TAG_STATE_DISCONNECTED;
    }
    if ((flag < 0 || flag >= ISO15693_MAX_FLAG_COUNT) || (blockIndex < 0 || blockIndex >= ISO15693_MAX_BLOCK_INDEX)) {
        ErrorLog("[Iso15693Tag::ReadSingleBlock] flag= %{public}d blockIndex= %{public}d err", flag, blockIndex);
        return ErrorCode::ERR_TAG_PARAMETERS;
    }

    std::string tagUid = GetTagUid();
    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {char(flag & 0xFF), CMD_READ_SINGLE_BLOCK};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand = sendCommand + tagUid + char(blockIndex & 0xFF);
    return SendCommand(sendCommand, false, hexRespData);
}

int Iso15693Tag::WriteSingleBlock(uint32_t flag, uint32_t blockIndex, const std::string& hexCmdData)
{
    if (!IsConnected()) {
        ErrorLog("[Iso15693Tag::WriteSingleBlock] connect tag first!");
        return ErrorCode::ERR_TAG_STATE_DISCONNECTED;
    }
    if ((flag < 0 || flag >= ISO15693_MAX_FLAG_COUNT) || (blockIndex < 0 || blockIndex >= ISO15693_MAX_BLOCK_INDEX)) {
        ErrorLog("[Iso15693Tag::WriteSingleBlock] flag= %{public}d blockIndex= %{public}d err", flag, blockIndex);
        return ErrorCode::ERR_TAG_PARAMETERS;
    }

    std::string tagUid = GetTagUid();
    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {char(flag & 0xFF), CMD_WRITE_SINGLE_BLOCK};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand = sendCommand + tagUid + char(blockIndex & 0xFF) + hexCmdData;

    std::string hexRespData;
    return SendCommand(sendCommand, false, hexRespData);
}

int Iso15693Tag::LockSingleBlock(uint32_t flag, uint32_t blockIndex)
{
    if (!IsConnected()) {
        ErrorLog("[Iso15693Tag::LockSingleBlock] connect tag first!");
        return ErrorCode::ERR_TAG_STATE_DISCONNECTED;
    }
    if ((flag < 0 || flag >= ISO15693_MAX_FLAG_COUNT) || (blockIndex < 0 || blockIndex >= ISO15693_MAX_BLOCK_INDEX)) {
        ErrorLog("[Iso15693Tag::LockSingleBlock] flag= %{public}d blockIndex= %{public}d err", flag, blockIndex);
        return ErrorCode::ERR_TAG_PARAMETERS;
    }

    std::string tagUid = GetTagUid();
    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {char(flag & 0xFF), CMD_LOCK_SINGLE_BLOCK};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand = sendCommand + tagUid + char(blockIndex & 0xFF);

    std::string hexRespData;
    return SendCommand(sendCommand, false, hexRespData);
}

int Iso15693Tag::ReadMultipleBlock(uint32_t flag, uint32_t blockIndex, uint32_t blockNum, std::string &hexRespData)
{
    if ((flag < 0 || flag >= ISO15693_MAX_FLAG_COUNT) || (blockIndex < 0 || blockIndex >= ISO15693_MAX_BLOCK_INDEX) ||
        (blockNum < 0 || blockNum >= ISO15693_MAX_BLOCK_INDEX) || !IsConnected()) {
        ErrorLog(
            "[Iso15693Tag::ReadMultipleBlock] flag= %{public}d blockIndex= %{public}d "
            "blockNum=%{public}d err", flag, blockIndex, blockNum);
        return ErrorCode::ERR_TAG_PARAMETERS;
    }

    std::string tagUid = GetTagUid();
    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {char(flag & 0xFF), CMD_READ_MULTIPLE_BLOCK};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand = sendCommand + tagUid + char(blockIndex & 0xFF) + char(blockNum & 0xFF);
    return SendCommand(sendCommand, false, hexRespData);
}

int Iso15693Tag::WriteMultipleBlock(uint32_t flag, uint32_t blockIndex, uint32_t blockNum,
    const std::string& hexCmdData)
{
    if (!IsConnected()) {
        ErrorLog("[Iso15693Tag::WriteMultipleBlock] connect tag first!");
        return ErrorCode::ERR_TAG_STATE_DISCONNECTED;
    }
    if ((flag < 0 || flag >= ISO15693_MAX_FLAG_COUNT) || (blockIndex < 0 || blockIndex >= ISO15693_MAX_BLOCK_INDEX) ||
        (blockNum <= 0 || blockNum > ISO15693_MAX_BLOCK_INDEX)) {
        ErrorLog("[Iso15693Tag::WriteMultipleBlock] flag=%{public}d blockIndex= %{public}d err", flag, blockIndex);
        return ErrorCode::ERR_TAG_PARAMETERS;
    }

    std::string tagUid = GetTagUid();
    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {char(flag & 0xFF), CMD_WRITE_MULTIPLE_BLOCK};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand = sendCommand + tagUid + char(blockIndex & 0xFF) + char(blockNum & 0xFF) + hexCmdData;
    std::string hexRespData;
    return SendCommand(sendCommand, false, hexRespData);
}

char Iso15693Tag::GetDsfId() const
{
    return dsfId_;
}

char Iso15693Tag::GetRespFlags() const
{
    return respFlags_;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS