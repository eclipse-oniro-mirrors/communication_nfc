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
// the command id defined for IPC, from kits to system ability.
constexpr int COMMAND_ID = 100;
constexpr int COMMAND_GET_STATE = COMMAND_ID + 1;
constexpr int COMMAND_TURN_ON = COMMAND_ID + 2;
constexpr int COMMAND_TURN_OFF = COMMAND_ID + 3;
constexpr int COMMAND_ON_NOTIFY = COMMAND_ID + 4;
constexpr int COMMAND_REGISTER_CALLBACK = COMMAND_ID + 5;
constexpr int COMMAND_UNREGISTER_CALLBACK = COMMAND_ID + 6;
constexpr int COMMAND_NFC_ENABLE = COMMAND_ID + 7;

constexpr int TAG_SESSION_START_ID = 200;
constexpr int COMMAND_CONNECT = TAG_SESSION_START_ID + 1;
constexpr int COMMAND_RECONNECT = TAG_SESSION_START_ID + 2;
constexpr int COMMAND_DISCONNECT = TAG_SESSION_START_ID + 3;
constexpr int COMMAND_GET_TECHLIST = TAG_SESSION_START_ID + 4;
constexpr int COMMAND_IS_PRESENT = TAG_SESSION_START_ID + 5;
constexpr int COMMAND_IS_NDEF = TAG_SESSION_START_ID + 6;
constexpr int COMMAND_SEND_RAW_FRAME = TAG_SESSION_START_ID + 7;
constexpr int COMMAND_NDEF_READ = TAG_SESSION_START_ID + 8;
constexpr int COMMAND_NDEF_WRITE = TAG_SESSION_START_ID + 9;
constexpr int COMMAND_NDEF_MAKE_READ_ONLY = TAG_SESSION_START_ID + 10;
constexpr int COMMAND_FORMAT_NDEF = TAG_SESSION_START_ID + 11;
constexpr int COMMAND_CAN_MAKE_READ_ONLY = TAG_SESSION_START_ID + 12;
constexpr int COMMAND_GET_MAX_TRANSCEIVE_LENGTH = TAG_SESSION_START_ID + 13;
constexpr int COMMAND_IS_SUPPORTED_APDUS_EXTENDED = TAG_SESSION_START_ID + 14;

enum NfcErrorCode : const int {
    NFC_SUCCESS = 0,
    NFC_FAILED,

    // SDK ERROR CODE
    NFC_SDK_ERROR_NOT_INITIALIZED = 0x00000100,
    NFC_SDK_ERROR_INVALID_PARAM,
    NFC_SDK_ERROR_TAG_NOT_CONNECT,
    NFC_SDK_ERROR_TAG_INVALID,
    NFC_SDK_ERROR_NOT_NDEF_TAG,
    NFC_SDK_ERROR_NFC_STATE_INVALID,
    NFC_SDK_ERROR_DEVICE_NOT_SUPPORT_NFC,
    NFC_SDK_ERROR_DISABLE_MAKE_READONLY,
    NFC_SDK_ERROR_UNKOWN,
    NFC_SDK_ERROR_PERMISSION,
    NFC_SDK_ERROR_CARDEMULATION_CONTROLLER_NOT_INITIALIZED,
    NFC_SDK_ERROR_CARDEMULATION_HANDLER_IS_NULL,

    // SERVICE ERROR CODE
    NFC_SER_ERROR_NOT_INITIALIZED = 0x00000200,
    NFC_SER_ERROR_DISCONNECT,
    NFC_SER_ERROR_IO,
    NFC_SER_ERROR_INVALID_PARAM
};

enum NfcState { STATE_OFF = 1, STATE_TURNING_ON = 2, STATE_ON = 3, STATE_TURNING_OFF = 4 };

enum NfcTask { TASK_TURN_ON, TASK_TURN_OFF, TASK_INITIALIZE };

enum class TagTechnology {
    NFC_INVALID_TECH = 0,
    NFC_A_TECH = 1,
    NFC_B_TECH = 2,
    NFC_ISODEP_TECH = 3,
    NFC_F_TECH = 4, // Felica
    NFC_V_TECH = 5, // ISO15693
    NFC_NDEF_TECH = 6,
    NFC_MIFARE_CLASSIC_TECH = 8,
    NFC_MIFARE_ULTRALIGHT_TECH = 9,
    NFC_NDEF_FORMATABLE_TECH = 10
};

class NfcSdkCommon final {
public:
    static const int SHIFT_SIZE = 8;
    static const int SHIFT_TIME = 4;

public:
    static bool IsLittleEndian();
    static std::string UnsignedCharArrayToString(const unsigned char* charArray, int length);
    static void StringToUnsignedCharArray(std::string &src, std::vector<unsigned char> &dst);
    static std::string IntToString(uint32_t num, bool bLittleEndian = true);
    static uint32_t StringToInt(std::string src, bool bLittleEndian = true);
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_SDK_COMMON_H
