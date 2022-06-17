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

std::string NfcSdkCommon::UnsignedCharArrayToString(const unsigned char* charArray, int length)
{
    std::string result = "";
    for (int i = 0; i < length; i++) {
        result += charArray[i];
    }
    return result;
}

void NfcSdkCommon::StringToUnsignedCharArray(std::string &src, std::vector<unsigned char> &dst)
{
    if (src.empty()) {
        return;
    }
    int len = src.length();
    for (int i = 0; i < len; i++) {
        dst.push_back((unsigned char) src[i]);
    }
}

std::string NfcSdkCommon::IntToString(int num, bool bLittleEndian)
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

int NfcSdkCommon::StringToInt(std::string src, bool bLittleEndian)
{
    int value = 0;
    if (bLittleEndian) {
        for (size_t i = SHIFT_TIME; i > 0; i--) {
            value += (int)(src.at(SHIFT_TIME - i)) << (i * SHIFT_SIZE - SHIFT_SIZE);
        }
    } else {
        for (size_t i = 0; i < SHIFT_TIME; i++) {
            value += (int)(src.at(i)) << (i * SHIFT_SIZE);
        }
    }
    return value;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
