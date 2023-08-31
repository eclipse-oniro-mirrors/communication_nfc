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
#ifndef NFC_SDK_COMMON_H
#define NFC_SDK_COMMON_H

#include <string>
#include <vector>

namespace OHOS {
namespace NFC {
namespace KITS {
const static uint32_t HEX_BYTE_LEN = 2;
const static uint32_t HEX_VALUE = 16;
const static uint32_t HALF_BYTE_BITS = 4;
static const uint32_t NFC_MANAGER_SYS_ABILITY_ID = 1140;

enum ErrorCode : const int {
    ERR_NONE = 0,

    ERR_NO_PERMISSION = 201,

    // error for nfc state operations
    ERR_NFC_BASE = 3100100,
    ERR_NFC_PARAMETERS,
    ERR_NFC_STATE_UNBIND,

    // error for tag I/O operations
    ERR_TAG_BASE = 3100200,
    ERR_TAG_PARAMETERS,
    ERR_TAG_STATE_NFC_CLOSED,
    ERR_TAG_STATE_LOST,
    ERR_TAG_STATE_DISCONNECTED,
    ERR_TAG_STATE_IO_FAILED,
    ERR_TAG_STATE_UNBIND,

    // error for card emulation operations
    ERR_CE_BASE = 3100300,
};

enum NfcState { STATE_OFF = 1, STATE_TURNING_ON = 2, STATE_ON = 3, STATE_TURNING_OFF = 4 };

enum NfcTask { TASK_TURN_ON = 101, TASK_TURN_OFF, TASK_INITIALIZE };

enum FeatureType { HCE = 0, UICC = 1, ESE = 2 };

/** Payment type of card emulation */
static const std::string TYPE_PAYMENT = "payment";

/** Other type of card emulation */
static const std::string TYPE_OHTER = "other";

/** Action for tag application declared */
const std::string ACTION_TAG_FOUND = "ohos.nfc.tag.action.TAG_FOUND";

/** Action for HCE application declared */
const std::string ACTION_HOST_APDU_SERVICE = "ohos.nfc.cardemulation.action.HOST_APDU_SERVICE";

enum class TagTechnology {
    NFC_INVALID_TECH = 0,
    NFC_A_TECH = 1,
    NFC_B_TECH = 2,
    NFC_ISODEP_TECH = 3,
    NFC_F_TECH = 4, // Felica
    NFC_V_TECH = 5, // ISO15693
    NFC_NDEF_TECH = 6,
    NFC_NDEF_FORMATABLE_TECH = 7,
    NFC_MIFARE_CLASSIC_TECH = 8,
    NFC_MIFARE_ULTRALIGHT_TECH = 9
};

class NfcSdkCommon final {
public:
    static const int SHIFT_SIZE = 8;
    static const int SHIFT_TIME = 4;

public:
    static bool IsLittleEndian();
    static std::string BytesVecToHexString(const unsigned char* charArray, uint32_t length);
    static std::string UnsignedCharToHexString(const unsigned char charArray);
    static void HexStringToBytes(std::string &src, std::vector<unsigned char> &bytes);
    static unsigned char GetByteFromHexStr(const std::string src, uint32_t index);
    static uint32_t GetHexStrBytesLen(const std::string src);
    static uint32_t StringToInt(std::string src, bool bLittleEndian = true);
    static std::string IntToHexString(uint32_t num);
    static void StringToAsciiBytes(const std::string &src, std::vector<unsigned char> &bytes);
    static std::string StringToHexString(const std::string &src);
    static uint64_t GetCurrentTime();
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_SDK_COMMON_H
