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
#include <algorithm>
#include <sstream>
#include <securec.h>
#include "loghelper.h"

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
    for (uint32_t i = 0; i < length; i++) {
        result.push_back(hexKeys[(src[i] & 0xF0) >> HALF_BYTE_BITS]);
        result.push_back(hexKeys[src[i] & 0x0F]);
    }
    return result;
}

std::string NfcSdkCommon::UnsignedCharToHexString(const unsigned char src)
{
    std::string result = "";
    const std::string hexKeys = "0123456789ABCDEF";
    result.push_back(hexKeys[(src & 0xF0) >> HALF_BYTE_BITS]);
    result.push_back(hexKeys[src & 0x0F]);
    return result;
}

void NfcSdkCommon::HexStringToBytes(std::string &src, std::vector<unsigned char> &bytes)
{
    if (src.empty()) {
        return;
    }

    uint32_t bytesLen = src.length() / HEX_BYTE_LEN;
    std::string strByte;
    unsigned int srcIntValue;
    for (uint32_t i = 0; i < bytesLen; i++) {
        strByte = src.substr(i * HEX_BYTE_LEN, HEX_BYTE_LEN);
        if (sscanf_s(strByte.c_str(), "%x", &srcIntValue) <= 0) {
            ErrorLog("HexStringToBytes, sscanf_s failed.");
            bytes.clear();
            return;
        }
        bytes.push_back(static_cast<unsigned char>(srcIntValue & 0xFF));
    }
}

uint32_t NfcSdkCommon::GetHexStrBytesLen(const std::string src)
{
    // 2 charactors consist of one byte.
    if (src.empty()) {
        return 0;
    }
    uint32_t length = src.length();
    if (length % HEX_BYTE_LEN == 0) {
        return (length / HEX_BYTE_LEN);
    } else {
        return ((length / HEX_BYTE_LEN) + 1);
    }
}

unsigned char NfcSdkCommon::GetByteFromHexStr(const std::string src, uint32_t index)
{
    // 2 charactors consist of one byte.
    if (src.empty() || index >= (src.length() - 1)) {
        return 0;
    }
    std::string strByte = src.substr(index * HEX_BYTE_LEN, HEX_BYTE_LEN);
    unsigned int srcIntValue;
    if (sscanf_s(strByte.c_str(), "%x", &srcIntValue) <= 0) {
        ErrorLog("GetByteFromHexStr, sscanf_s failed.");
        return 0;
    }
    return static_cast<unsigned char>(srcIntValue & 0xFF);
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

std::string NfcSdkCommon::IntToHexString(uint32_t num)
{
    std::stringstream ss;
    ss << std::hex << num;
    std::string result = ss.str();
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    if (result.length() % HEX_BYTE_LEN > 0) { // expend "0" if string length is odd
        result = "0" + result;
    }
    return result;
}

void NfcSdkCommon::StringToAsciiBytes(const std::string &src, std::vector<unsigned char> &bytes)
{
    if (src.empty()) {
        return;
    }
    uint32_t bytesLen = src.length();
    for (uint32_t i = 0; i < bytesLen; i++) {
        unsigned int srcAsciiIntVal = static_cast<unsigned int>(src[i]);
        bytes.push_back(static_cast<unsigned char>(srcAsciiIntVal & 0xFF));
    }
}

std::string NfcSdkCommon::StringToHexString(const std::string &src)
{
    std::vector<unsigned char> bytes;
    StringToAsciiBytes(src, bytes);
    uint32_t len = src.length();
    std::string result = BytesVecToHexString(&bytes[0], len);
    return result;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
