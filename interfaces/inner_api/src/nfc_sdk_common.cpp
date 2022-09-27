/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "nfc_sdk_common.h"

#include <sstream>

namespace OHOS {
namespace NFC {
namespace KITS {
bool NfcSdkCommon::IsLittleEndian()
{
    const char LAST_DATA_BYTE = 0x78;
    union CheckData {
        int x;
        char y;
    };

    union CheckData data;
    data.x = 0x12345678;
    if (data.y == LAST_DATA_BYTE) {
        return true;
    }
    return false;
}

std::string NfcSdkCommon::BytesVecToHexString(const unsigned char* src, uint32_t length)
{
    std::string result = "";
    if (length <= 0) {
        return result;
    }
    const std::string hexKeys = "0123456789ABCDEF";
    for(uint32_t i = 0; i < length; i++) {
       result.push_back(hexKeys[(src[i] & 0xF0) >> 4]);
       result.push_back(hexKeys[src[i] & 0x0F]);
    }
    return result;
}

std::string NfcSdkCommon::UnsignedCharToHexString(const unsigned char src)
{
    std::string result = "";
    const std::string hexKeys = "0123456789ABCDEF";
    result.push_back(hexKeys[(src & 0xF0) >> 4]);
    result.push_back(hexKeys[src & 0x0F]);
    return result;
}

void NfcSdkCommon::HexStringToBytes(std::string &src, std::vector<unsigned char> &bytes)
{
    if (src.empty()) {
        return;
    }

    // two charactors consist of one hex byte.
    for (uint32_t i = 0; i < (src.size() - 1); i += 2) {
        std::string byte = src.substr(i, 2);
        unsigned char value = static_cast<unsigned char>(std::stoi(byte, 0, 16));
        bytes.push_back(value);
    }
}

uint32_t NfcSdkCommon::GetHexStrBytesLen(const std::string src)
{
    // 2 charactors consist of one byte.
    if (src.empty()) {
        return 0;
    }
    uint32_t length = src.length(); 
    if (length % 2 == 0) {
        return (length / 2);
    } else {
        return ((length / 2) + 1);
    }
}

unsigned char NfcSdkCommon::GetByteFromHexStr(const std::string src, uint32_t index)
{
    // 2 charactors consist of one byte.
    if (src.empty() || index >= (src.length() - 1)) {
        return 0;
    }
    std::string byte = src.substr(index, 2);
    unsigned char value = static_cast<unsigned char>(std::stoi(byte, 0, 16));
    return value;
}

std::string NfcSdkCommon::IntToString(uint32_t num, bool bLittleEndian)
{
    std::stringstream ss;
    if (bLittleEndian) {
        for (size_t i = 0; i < SHIFT_TIME; i++) {
            ss << char((num >> (i * SHIFT_SIZE)) & 0xFF);
        }
    } else {
        for (size_t i = SHIFT_TIME; i > 0; i--) {
            ss << char((num >> (i * SHIFT_SIZE - SHIFT_SIZE)) & 0xFF);
        }
    }

    return ss.str();
}

uint32_t NfcSdkCommon::StringToInt(std::string src, bool bLittleEndian)
{
    uint32_t value = 0;
    if (bLittleEndian) {
        for (size_t i = SHIFT_TIME; i > 0; i--) {
            value += (uint32_t)(src.at(SHIFT_TIME - i)) << (i * SHIFT_SIZE - SHIFT_SIZE);
        }
    } else {
        for (size_t i = 0; i < SHIFT_TIME; i++) {
            value += (uint32_t)(src.at(i)) << (i * SHIFT_SIZE);
        }
    }
    return value;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
