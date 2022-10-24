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
#include "nfca_tag.h"

namespace OHOS {
namespace NFC {
namespace KITS {
const char MifareClassicTag::MC_KEY_DEFAULT[MC_KEY_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
// MIFARE Application Directory (MAD)
const char MifareClassicTag::MC_KEY_MAD[MC_KEY_LEN] = {0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5};
const char MifareClassicTag::MC_KEY_NFC_FORUM[MC_KEY_LEN] = {0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7};

MifareClassicTag::MifareClassicTag(std::weak_ptr<TagInfo> tag)
    : BasicTagSession(tag, KITS::TagTechnology::NFC_MIFARE_CLASSIC_TECH)
{
    isEmulated_ = false;
    mifareTagType_ = EmType::TYPE_UNKNOWN;
    std::shared_ptr<NfcATag> nfcA = NfcATag::GetTag(tag);
    if (nfcA == nullptr) {
        ErrorLog("MifareClassicTag, not support NfcA.");
        return;
    }

    DebugLog("MifareClassicTag, sak %{public}x", nfcA->GetSak());
    mifareTagType_ = EmType::TYPE_CLASSIC;
    SetSizeBySak(nfcA->GetSak());
}

void MifareClassicTag::SetSizeBySak(int sak)
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
            mifareTagType_ = EmType::TYPE_PLUS;
            break;
        case SAK11:
            size_ = MC_SIZE_4K;
            mifareTagType_ = EmType::TYPE_PLUS;
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
            mifareTagType_ = EmType::TYPE_PRO;
            break;
        default:
            ErrorLog("[MifareClassicTag::SetSizeBySak] err! sak %{public}x", sak);
            break;
    }
}

std::shared_ptr<MifareClassicTag> MifareClassicTag::GetTag(std::weak_ptr<TagInfo> tag)
{
    if (tag.expired() || !tag.lock()->IsTechSupported(KITS::TagTechnology::NFC_A_TECH) ||
        !tag.lock()->IsTechSupported(KITS::TagTechnology::NFC_MIFARE_CLASSIC_TECH)) {
        ErrorLog("MifareClassicTag::GetTag error, no mathced technology.");
        return nullptr;
    }

    return std::make_shared<MifareClassicTag>(tag);
}

int MifareClassicTag::AuthenticateSector(int sectorIndex, const std::string& key, bool bIsKeyA)
{
    if ((sectorIndex < 0 || sectorIndex >= MC_MAX_SECTOR_COUNT)) {
        ErrorLog("AuthenticateSector err! sectorIndex %{public}d", sectorIndex);
        return ErrorCode::ERR_TAG_PARAMETERS;
    }
    if (key.empty()) {
        ErrorLog("AuthenticateSector err! key empty");
        return ErrorCode::ERR_TAG_PARAMETERS;
    }
    if (!IsConnected()) {
        ErrorLog("AuthenticateSector err! tag is not connected");
        return ErrorCode::ERR_TAG_STATE_DISCONNECTED;
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
    std::string hexRespData;
    return SendCommand(sendCommand, false, hexRespData);
}

int MifareClassicTag::ReadSingleBlock(uint32_t blockIndex, std::string &hexRespData)
{
    if ((blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) || !IsConnected()) {
        ErrorLog("[MifareClassicTag::ReadSingleBlock] blockIndex= %{public}d err", blockIndex);
        return ErrorCode::ERR_TAG_PARAMETERS;
    }

    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_READ, char(blockIndex & 0xFF)};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    return SendCommand(sendCommand, false, hexRespData);
}

int MifareClassicTag::WriteSingleBlock(uint32_t blockIndex, const std::string& data)
{
    if (!IsConnected()) {
        ErrorLog("[MifareClassicTag::WriteSingleBlock] connect tag first!");
        return ErrorCode::ERR_TAG_STATE_DISCONNECTED;
    }
    if ((blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) ||
        KITS::NfcSdkCommon::GetHexStrBytesLen(data) != MC_BLOCK_SIZE) {
        ErrorLog("[MifareClassicTag::WriteSingleBlock] blockIndex= %{public}d dataLen= %{public}d err",
            blockIndex, KITS::NfcSdkCommon::GetHexStrBytesLen(data));
        return ErrorCode::ERR_TAG_PARAMETERS;
    }

    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_WRITE, char(blockIndex & 0xFF)};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand += data;
    std::string hexRespData;
    return SendCommand(sendCommand, false, hexRespData);
}

int MifareClassicTag::IncrementBlock(uint32_t blockIndex, int value)
{
    if (!IsConnected()) {
        ErrorLog("[MifareClassicTag::IncrementBlock] connect tag first!");
        return ErrorCode::ERR_TAG_STATE_DISCONNECTED;
    }
    if ((blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) || value < 0) {
        ErrorLog("[MifareClassicTag::IncrementBlock] blockIndex= %{public}d value=%{public}d err", blockIndex, value);
        return ErrorCode::ERR_TAG_PARAMETERS;
    }

    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_INCREMENT, char(blockIndex & 0xFF)};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand += NfcSdkCommon::IntToString(value, NfcSdkCommon::IsLittleEndian());
    std::string hexRespData;
    return SendCommand(sendCommand, false, hexRespData);
}

int MifareClassicTag::DecrementBlock(uint32_t blockIndex, int value)
{
    InfoLog("MifareClassicTag::DecrementBlock in");
    if (!IsConnected()) {
        ErrorLog("[MifareClassicTag::DecrementBlock] connect tag first!");
        return ErrorCode::ERR_TAG_STATE_DISCONNECTED;
    }
    if (blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX || value < 0) {
        ErrorLog("[MifareClassicTag::DecrementBlock] blockIndex= %{public}d value=%{public}d err", blockIndex, value);
        return ErrorCode::ERR_TAG_PARAMETERS;
    }

    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_DECREMENT, char(blockIndex & 0xFF)};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    sendCommand += NfcSdkCommon::IntToString(value, NfcSdkCommon::IsLittleEndian());
    std::string hexRespData;
    return SendCommand(sendCommand, false, hexRespData);
}

int MifareClassicTag::TransferToBlock(uint32_t blockIndex)
{
    if (!IsConnected()) {
        ErrorLog("[MifareClassicTag::TransferToBlock] connect tag first!");
        return ErrorCode::ERR_TAG_STATE_DISCONNECTED;
    }
    if (blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) {
        ErrorLog("[MifareClassicTag::TransferToBlock] blockIndex= %{public}d err", blockIndex);
        return ErrorCode::ERR_TAG_PARAMETERS;
    }
    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_TRANSFER, char(blockIndex & 0xFF)};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    std::string hexRespData;
    return SendCommand(sendCommand, false, hexRespData);
}

int MifareClassicTag::RestoreFromBlock(uint32_t blockIndex)
{
    if (!IsConnected()) {
        ErrorLog("[MifareClassicTag::TransferToBlock] connect tag first!");
        return ErrorCode::ERR_TAG_STATE_DISCONNECTED;
    }
    if (blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) {
        ErrorLog("[MifareClassicTag::RestoreFromBlock] blockIndex= %{public}d err", blockIndex);
        return ErrorCode::ERR_TAG_PARAMETERS;
    }
    char command[TagInfo::SEND_COMMAND_HEAD_LEN_2] = {MIFARE_RESTORE, char(blockIndex & 0xFF)};
    std::string sendCommand(command, TagInfo::SEND_COMMAND_HEAD_LEN_2);
    std::string hexRespData;
    return SendCommand(sendCommand, false, hexRespData);
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
    ErrorLog("GetBlockCountInSector, error sectorIndex %{public}d", sectorIndex);
    return MC_ERROR_VALUE;
}

MifareClassicTag::EmType MifareClassicTag::GetMifareTagType() const
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
    ErrorLog("GetBlockIndexFromSector, error sectorIndex %{public}d", sectorIndex);
    return MC_ERROR_VALUE;
}

int MifareClassicTag::GetSectorIndexFromBlock(int blockIndex) const
{
    if (blockIndex < 0 || blockIndex >= MC_MAX_BLOCK_INDEX) {
        ErrorLog("GetSectorIndexFromBlock, error blockIndex %{public}d", blockIndex);
        return MC_ERROR_VALUE;
    }
    if (blockIndex < MC_SECTOR_COUNT_OF_SIZE_2K * MC_BLOCK_COUNT) {
        return blockIndex / MC_BLOCK_COUNT;
    } else {
        return MC_SECTOR_COUNT_OF_SIZE_2K + (blockIndex - MC_SECTOR_COUNT_OF_SIZE_2K * MC_BLOCK_COUNT) / MC_BLOCK_SIZE;
    }
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
