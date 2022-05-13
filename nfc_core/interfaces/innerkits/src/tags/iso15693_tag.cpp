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
    if (tag.expired()) {
        DebugLog("Iso15693Tag::Iso15693Tag tag invalid ");
        return;
    }
    AppExecFwk::PacMap extraData = tag.lock()->GetTechExtrasData(KITS::TagTechnology::NFC_V_TECH);
    if (!extraData.IsEmpty()) {
        DebugLog("Iso15693Tag::Iso15693Tag extra data invalid");
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
        return std::shared_ptr<Iso15693Tag>();
    }

    return std::make_shared<Iso15693Tag>(tag);
}

std::string Iso15693Tag::ReadSingleBlock(int flag, int blockIndex)
{
    InfoLog("Iso15693Tag::ReadSingleBlock in flag= %d blockIndex= %d", flag, blockIndex);
    if ((flag < 0 || flag >= ISO15693_MAX_FLAG_COUNT) || (blockIndex < 0 || blockIndex >= ISO15693_MAX_BLOCK_INDEX) ||
        !IsConnected()) {
        DebugLog("[Iso15693Tag::ReadSingleBlock] flag= %d blockIndex= %d err", flag, blockIndex);
        return "";
    }

    std::string tagUid = GetTagUid();
    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {char(flag & 0xFF), CMD_READ_SINGLE_BLOCK};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand = sendCommand + tagUid + char(blockIndex & 0xFF);

    int response = TAG::ResResult::ResponseResult::RESULT_FAILURE;
    return SendCommand(sendCommand, false, response);
}

int Iso15693Tag::WriteSingleBlock(int flag, int blockIndex, const std::string& data)
{
    InfoLog("Iso15693Tag::WriteSingleBlock in");
    if (!IsConnected()) {
        DebugLog("[Iso15693Tag::WriteSingleBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if ((flag < 0 || flag >= ISO15693_MAX_FLAG_COUNT) || (blockIndex < 0 || blockIndex >= ISO15693_MAX_BLOCK_INDEX)) {
        DebugLog("[Iso15693Tag::WriteSingleBlock] flag= %d blockIndex= %d err", flag, blockIndex);
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }

    std::string tagUid = GetTagUid();
    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {char(flag & 0xFF), CMD_WRITE_SINGLE_BLOCK};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand = sendCommand + tagUid + char(blockIndex & 0xFF) + data;

    int response = TAG::ResResult::ResponseResult::RESULT_FAILURE;
    std::string res = SendCommand(sendCommand, false, response);
    return response;
}

int Iso15693Tag::LockSingleBlock(int flag, int blockIndex)
{
    InfoLog("Iso15693Tag::LockSingleBlock in");
    if (!IsConnected()) {
        DebugLog("[Iso15693Tag::LockSingleBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if ((flag < 0 || flag >= ISO15693_MAX_FLAG_COUNT) || (blockIndex < 0 || blockIndex >= ISO15693_MAX_BLOCK_INDEX)) {
        DebugLog("[Iso15693Tag::LockSingleBlock] flag= %d blockIndex= %d err", flag, blockIndex);
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }

    std::string tagUid = GetTagUid();
    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {char(flag & 0xFF), CMD_LOCK_SINGLE_BLOCK};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand = sendCommand + tagUid + char(blockIndex & 0xFF);

    int response = TAG::ResResult::ResponseResult::RESULT_FAILURE;
    std::string res = SendCommand(sendCommand, false, response);
    return response;
}

std::string Iso15693Tag::ReadMultipleBlock(int flag, int blockIndex, int blockNum)
{
    InfoLog("Iso15693Tag::ReadMultipleBlock in flag= %d blockIndex= %d blockNum=%d", flag, blockIndex, blockNum);
    if ((flag < 0 || flag >= ISO15693_MAX_FLAG_COUNT) || (blockIndex < 0 || blockIndex >= ISO15693_MAX_BLOCK_INDEX) ||
        (blockNum < 0 || blockNum >= ISO15693_MAX_BLOCK_INDEX) || !IsConnected()) {
        DebugLog(
            "[Iso15693Tag::ReadMultipleBlock] flag= %d blockIndex= %d "
            "blockNum=%d err",
            flag,
            blockIndex,
            blockNum);
        return "";
    }

    std::string tagUid = GetTagUid();
    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {char(flag & 0xFF), CMD_READ_MULTIPLE_BLOCK};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand = sendCommand + tagUid + char(blockIndex & 0xFF) + char(blockNum & 0xFF);

    int response = TAG::ResResult::ResponseResult::RESULT_FAILURE;
    return SendCommand(sendCommand, false, response);
}

int Iso15693Tag::WriteMultipleBlock(int flag, int blockIndex, int blockNum, const std::string& data)
{
    InfoLog("Iso15693Tag::WriteMultipleBlock in");
    if (!IsConnected()) {
        DebugLog("[Iso15693Tag::WriteMultipleBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if ((flag < 0 || flag >= ISO15693_MAX_FLAG_COUNT) || (blockIndex < 0 || blockIndex >= ISO15693_MAX_BLOCK_INDEX) ||
        (blockNum <= 0 || blockNum > ISO15693_MAX_BLOCK_INDEX)) {
        DebugLog("[Iso15693Tag::WriteMultipleBlock] flag=%d blockIndex= %d err", flag, blockIndex);
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }

    std::string tagUid = GetTagUid();
    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {char(flag & 0xFF), CMD_WRITE_MULTIPLE_BLOCK};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand = sendCommand + tagUid + char(blockIndex & 0xFF) + char(blockNum & 0xFF) + data;

    int response = TAG::ResResult::ResponseResult::RESULT_FAILURE;
    std::string res = SendCommand(sendCommand, false, response);
    return response;
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