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
static const std::string NFC_MANAGER_SYS_ABILITY_NAME = "nfc_service";

enum ErrorCode : const int {
    ERR_NONE = 0,

    ERR_NO_PERMISSION = 201,
    ERR_NOT_SYSTEM_APP = 202,

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
    ERR_TAG_APP_NOT_FOREGROUND,
    ERR_TAG_APP_NOT_REGISTERED,

    // error for card emulation operations
    ERR_CE_BASE = 3100300,
    ERR_HCE_PARAMETERS,
    ERR_HCE_STATE_NFC_CLOSED,
    ERR_HCE_STATE_LOST,
    ERR_HCE_STATE_DISCONNECTED,
    ERR_HCE_STATE_IO_FAILED,
    ERR_HCE_STATE_UNBIND,
    ERR_HCE_NOT_GET_PAYMENT_SERVICES,

    // error for nfc database operations
    ERR_NFC_DATABASE_RW = 3100400,
};

enum NfcState { STATE_OFF = 1, STATE_TURNING_ON = 2, STATE_ON = 3, STATE_TURNING_OFF = 4 };

enum NfcTask { TASK_TURN_ON = 101, TASK_TURN_OFF, TASK_INITIALIZE };

enum FeatureType { HCE = 0, UICC = 1, ESE = 2 };

/** NFC state changed for common event notification */
const std::string COMMON_EVENT_NFC_ACTION_STATE_CHANGED = "usual.event.nfc.action.ADAPTER_STATE_CHANGED";
const std::string NFC_EXTRA_STATE = "ohos.nfc.extra.ADAPTER_STATE";

/** Payment type of card emulation */
static const std::string TYPE_PAYMENT = "payment";

/** Other type of card emulation */
static const std::string TYPE_OHTER = "other";

/** Payment type of card emulation metadata name */
const std::string KEY_PAYMENT_AID = "payment-aid";

/** Other type of card emulation metadata name */
const std::string KEY_OHTER_AID = "other-aid";

/** Action for tag application declared */
const std::string ACTION_TAG_FOUND = "ohos.nfc.tag.action.TAG_FOUND";

/** Action for HCE application declared */
const std::string ACTION_HOST_APDU_SERVICE = "ohos.nfc.cardemulation.action.HOST_APDU_SERVICE";

/** Action for off host*/
const std::string ACTION_OFF_HOST_APDU_SERVICE = "ohos.nfc.cardemulation.action.OFF_HOST_APDU_SERVICE";

/** Database key for nfc state. */
const std::string NFC_DATA_URI_ID =
    "/com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=data_key_nfc_state";
const std::string NFC_DATA_ABILITY_PREFIX = "datashare://";
const std::string NFC_DATA_URI = NFC_DATA_ABILITY_PREFIX + NFC_DATA_URI_ID;
const std::string DATA_SHARE_KEY_STATE = "data_key_nfc_state";
const std::string NFC_DATA_COLUMN_KEYWORD = "KEYWORD";
const std::string NFC_DATA_COLUMN_VALUE = "VALUE";

/** Database key for payment default app. */
const std::string NFC_PAYMENT_DEFAULT_APP =
    "/com.ohos.settingsdata/entry/settingsdata/SETTINGSDATA?Proxy=true&key=nfc_payment_default_app";
const std::string NFC_DATA_URI_PAYMENT_DEFAULT_APP = NFC_DATA_ABILITY_PREFIX + NFC_PAYMENT_DEFAULT_APP;
const std::string DATA_SHARE_KEY_NFC_PAYMENT_DEFAULT_APP = "nfc_payment_default_app";

/** NFC field on/off common event */
const std::string COMMON_EVENT_NFC_ACTION_RF_FIELD_ON_DETECTED = "usual.event.nfc.action.RF_FIELD_ON_DETECTED";
const std::string COMMON_EVENT_NFC_ACTION_RF_FIELD_OFF_DETECTED = "usual.event.nfc.action.RF_FIELD_OFF_DETECTED";

const int DATA_SHARE_INVALID_VALUE = -1;

/** type const of hce napi on */
const std::string EVENT_HCE_CMD = "hceCmd";

/** type const of max apdu length */
const uint32_t MAX_APDU_DATA_BYTE = 1024;
const uint32_t MAX_APDU_DATA_HEX_STR = MAX_APDU_DATA_BYTE * 2;
const uint32_t MAX_AID_LIST_NUM_PER_APP = 100;

#ifdef VENDOR_APPLICATIONS_ENABLED
const int VENDOR_APP_INIT_DONE = 1;
const int VENDOR_APP_CHANGE = 2;
#endif

enum class DefaultPaymentType {
    TYPE_HCE = 0,
    TYPE_UICC = 1,
    TYPE_ESE = 2,
    TYPE_EMPTY = 3,
    TYPE_UNINSTALLED = 4,
};

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

enum EmNfcForumType {
    NFC_FORUM_TYPE_UNKNOWN = 0,
    NFC_FORUM_TYPE_1 = 1,
    NFC_FORUM_TYPE_2 = 2,
    NFC_FORUM_TYPE_3 = 3,
    NFC_FORUM_TYPE_4 = 4,
    MIFARE_CLASSIC = 101,
    ICODE_SLI = 102
};

class NfcSdkCommon final {
public:
    static const int SHIFT_SIZE = 8;
    static const int SHIFT_TIME = 4;

public:
    static bool IsLittleEndian();
    static std::string BytesVecToHexString(const unsigned char* src, uint32_t length);
    static std::string UnsignedCharToHexString(const unsigned char src);
    static void HexStringToBytes(const std::string &src, std::vector<unsigned char> &bytes);
    static unsigned char GetByteFromHexStr(const std::string src, uint32_t index);
    static uint32_t GetHexStrBytesLen(const std::string src);
    static uint32_t StringToInt(std::string src, bool bLittleEndian = true);
    static std::string IntToHexString(uint32_t num);
    static void StringToAsciiBytes(const std::string &src, std::vector<unsigned char> &bytes);
    static std::string StringToHexString(const std::string &src);
    static std::string HexStringToAsciiString(const std::string &src);
    static uint64_t GetCurrentTime();
    static uint64_t GetRelativeTime();
    static std::string CodeMiddlePart(const std::string &src);
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_SDK_COMMON_H
