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
#include "mifare_classic_tag.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
const char MifareClassicTag::MC_KEY_DEFAULT[MC_KEY_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
const char MifareClassicTag::MC_KEY_MIFARE_APPLICATION_DIRECTORY[MC_KEY_LEN] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5};
const char MifareClassicTag::MC_KEY_NFC_FORUM[MC_KEY_LEN] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7};

MifareClassicTag::MifareClassicTag(std::weak_ptr<TagInfo> tag)
    : BasicTagSession(tag, KITS::TagTechnology::NFC_MIFARE_CLASSIC_TECH)
{
    DebugLog("MifareClassicTag::MifareClassicTag in");
    if (tag.expired()) {
        DebugLog("MifareClassicTag::MifareClassicTag tag invalid");
        return;
    }
    AppExecFwk::PacMap extraData = tag.lock()->GetTechExtrasData(KITS::TagTechnology::NFC_MIFARE_CLASSIC_TECH);
    if (!extraData.IsEmpty()) {
        DebugLog("MifareClassicTag::MifareClassicTag extra data invalid");
        return;
    }
    int sak = tag.lock()->GetIntExtrasData(extraData, TagInfo::SAK);
    std::string atqa = tag.lock()->GetStringExtrasData(extraData, TagInfo::ATQA);

    DebugLog("MifareClassicTag::MifareClassicTag sak.%d atqa.(%d)%s", sak, atqa.size(), atqa.c_str());
    for (size_t i = 0; i < atqa.size(); i++) {
        printf("%02x ", atqa.at(i));
    }
    printf("\n");

    isEmulated_ = false;
    mifareTagType_ = EmMifareTagType::TYPE_CLASSIC;

    SetSakSize(sak);
}

void MifareClassicTag::SetSakSize(int sak)
{
    switch (sak) {
        case SAK01:
        case SAK08:
        case SAK88:
            size_ = MC_SIZE_1K;
            break;
        case SAK09:
            size_ = MC_SIZE_MINI;
            break;
        case SAK10:
            size_ = MC_SIZE_2K;
            mifareTagType_ = EmMifareTagType::TYPE_PLUS;
            break;
        case SAK11:
            size_ = MC_SIZE_4K;
            mifareTagType_ = EmMifareTagType::TYPE_PLUS;
            break;
        case SAK18:
            size_ = MC_SIZE_4K;
            break;
        case SAK28:
            size_ = MC_SIZE_1K;
            isEmulated_ = true;
            break;
        case SAK38:
            size_ = MC_SIZE_4K;
            isEmulated_ = true;
            break;
        case SAK98:
        case SAKB8:
            size_ = MC_SIZE_4K;
            mifareTagType_ = EmMifareTagType::TYPE_PRO;
            break;
        default:
            break;
    }
}

std::shared_ptr<MifareClassicTag> MifareClassicTag::GetTag(std::weak_ptr<TagInfo> tag)
{
    if (tag.expired() || !tag.lock()->IsTechSupported(KITS::TagTechnology::NFC_MIFARE_CLASSIC_TECH)) {
        return std::shared_ptr<MifareClassicTag>();
    }

    return std::make_shared<MifareClassicTag>(tag);
}

bool MifareClassicTag::AuthenticateSector(int sectorIndex, const std::string& key, bool bIsKeyA)
{
    if ((sectorIndex < 0 || sectorIndex >= MC_MAX_SECTOR_COUNT) || !IsConnected() || key.empty()) {
        ErrorLog(
            "[MifareClassicTag::AuthenticateSector] param err! sectorIndex.%d "
            "keyLen.%d",
            sectorIndex,
            key.size());
        return false;
    }

    char command[TagInfo::SEND_COMMAND_MAX_LEN];
    int commandLen = 0;
    if (bIsKeyA) {
        command[commandLen++] = AUTHENTICATION_WITH_KEY_A;
    } else {
        command[commandLen++] = AUTHENTICATION_WITH_KEY_B;
    }

    command[commandLen++] = char(GetBlockIndexFromSector(sectorIndex));
    std::string sendCommand(command, commandLen);
    std::string tagUid = GetTagUid();
    static const int tagSubLen = 4;
    // Take the first 4 bytes of the tag as part of command
    sendCommand += tagUid.substr(0, tagSubLen) + key;

    int response = TAG::ResResult::ResponseResult::RESULT_FAILURE;
    SendCommand(sendCommand, false, response);
    return (response == TAG::ResResult::ResponseResult::RESULT_SUCCESS);
}

std::string MifareClassicTag::ReadSingleBlock(int blockIndex)
{
    InfoLog("MifareClassicTag::ReadSingleBlock in");
    if ((blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) || !IsConnected()) {
        DebugLog("[MifareClassicTag::ReadSingleBlock] blockIndex= %d err", blockIndex);
        return "";
    }

    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_READ, char(blockIndex & 0xFF)};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);

    int response = TAG::ResResult::ResponseResult::RESULT_FAILURE;
    return SendCommand(sendCommand, false, response);
}

int MifareClassicTag::WriteSingleBlock(int blockIndex, const std::string& data)
{
    InfoLog("MifareClassicTag::WriteSingleBlock in");
    if (!IsConnected()) {
        DebugLog("[MifareClassicTag::WriteSingleBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if ((blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) || (data.size() != MC_BLOCK_SIZE)) {
        DebugLog("[MifareClassicTag::WriteSingleBlock] blockIndex= %d dataLen= %d err", blockIndex, data.size());
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }

    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_WRITE, char(blockIndex & 0xFF)};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand += data;

    int response = TAG::ResResult::ResponseResult::RESULT_FAILURE;
    SendCommand(sendCommand, false, response);
    return response;
}

int MifareClassicTag::IncrementBlock(int blockIndex, int value)
{
    InfoLog("MifareClassicTag::IncrementBlock in");
    if (!IsConnected()) {
        DebugLog("[MifareClassicTag::IncrementBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if ((blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) || value < 0) {
        DebugLog("[MifareClassicTag::IncrementBlock] blockIndex= %d value=%d err", blockIndex, value);
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }

    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_INCREMENT, char(blockIndex & 0xFF)};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand += NfcSdkCommon::IntToString(value, NfcSdkCommon::IsLittleEndian());

    int response = TAG::ResResult::ResponseResult::RESULT_FAILURE;
    SendCommand(sendCommand, false, response);
    return response;
}

int MifareClassicTag::DecrementBlock(int blockIndex, int value)
{
    InfoLog("MifareClassicTag::DecrementBlock in");
    if (!IsConnected()) {
        DebugLog("[MifareClassicTag::DecrementBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if (blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX || value < 0) {
        DebugLog("[MifareClassicTag::DecrementBlock] blockIndex= %d value=%d err", blockIndex, value);
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }

    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_DECREMENT, char(blockIndex & 0xFF)};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand += NfcSdkCommon::IntToString(value, NfcSdkCommon::IsLittleEndian());

    int response = TAG::ResResult::ResponseResult::RESULT_FAILURE;
    SendCommand(sendCommand, false, response);
    return response;
}

int MifareClassicTag::TransferToBlock(int blockIndex)
{
    InfoLog("MifareClassicTag::TransferToBlock in");
    if (!IsConnected()) {
        DebugLog("[MifareClassicTag::TransferToBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if (blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) {
        DebugLog("[MifareClassicTag::TransferToBlock] blockIndex= %d err", blockIndex);
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }
    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_TRANSFER, char(blockIndex & 0xFF)};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);

    int response = TAG::ResResult::ResponseResult::RESULT_FAILURE;
    SendCommand(sendCommand, false, response);
    return response;
}

int MifareClassicTag::RestoreFromBlock(int blockIndex)
{
    InfoLog("MifareClassicTag::RestoreFromBlock in");
    if (!IsConnected()) {
        DebugLog("[MifareClassicTag::TransferToBlock] connect tag first!");
        return NfcErrorCode::NFC_SDK_ERROR_TAG_NOT_CONNECT;
    }
    if (blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) {
        DebugLog("[MifareClassicTag::RestoreFromBlock] blockIndex= %d err", blockIndex);
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }
    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_RESTORE, char(blockIndex & 0xFF)};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);

    int response = TAG::ResResult::ResponseResult::RESULT_FAILURE;
    SendCommand(sendCommand, false, response);
    return response;
}

int MifareClassicTag::GetSectorCount() const
{
    size_t count = 0;
    switch (size_) {
        case MC_SIZE_1K:
            count = MC_SECTOR_COUNT_OF_SIZE_1K;
            break;
        case MC_SIZE_2K:
            count = MC_SECTOR_COUNT_OF_SIZE_2K;
            break;
        case MC_SIZE_4K:
            count = MC_MAX_SECTOR_COUNT;
            break;
        case MC_SIZE_MINI:
            count = MC_SECTOR_COUNT_OF_SIZE_MINI;
            break;
        default:
            break;
    }
    return count;
}

int MifareClassicTag::GetBlockCountInSector(int sectorIndex) const
{
    if (sectorIndex >= 0 && sectorIndex < MC_SECTOR_COUNT_OF_SIZE_2K) {
        return MC_BLOCK_COUNT;
    } else if (sectorIndex >= MC_SECTOR_COUNT_OF_SIZE_2K && sectorIndex < MC_MAX_SECTOR_COUNT) {
        return MC_BLOCK_COUNT_OF_4K;
    }
    return NfcErrorCode::NFC_SDK_ERROR_UNKOWN;
}

size_t MifareClassicTag::GetMifareTagType() const
{
    return mifareTagType_;
}

int MifareClassicTag::GetSize() const
{
    return size_;
}

bool MifareClassicTag::IsEmulated() const
{
    return isEmulated_;
}

int MifareClassicTag::GetBlockIndexFromSector(int sectorIndex) const
{
    if (sectorIndex >= 0 && sectorIndex < MC_SECTOR_COUNT_OF_SIZE_2K) {
        return sectorIndex * MC_BLOCK_COUNT;
    }
    if (sectorIndex >= MC_SECTOR_COUNT_OF_SIZE_2K && sectorIndex < MC_MAX_SECTOR_COUNT) {
        return MC_SECTOR_COUNT_OF_SIZE_2K * MC_BLOCK_COUNT +
               (sectorIndex - MC_SECTOR_COUNT_OF_SIZE_2K) * MC_BLOCK_COUNT_OF_4K;
    }
    return NfcErrorCode::NFC_SDK_ERROR_UNKOWN;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
