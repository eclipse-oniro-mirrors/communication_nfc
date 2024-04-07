/*
 * Copyright (C) 2023 - 2024 Huawei Device Co., Ltd.
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
#include "ndef_bt_data_parser.h"

#include <string>
#include "loghelper.h"
#include "ndef_message.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace TAG {
#define RTD_TYPE_BT                    "application/vnd.bluetooth.ep.oob"
#define RTD_TYPE_BLE                   "application/vnd.bluetooth.le.oob"

#define UNSIGNED_BYTE_TO_INT_MASK       0xFF

#define CARRIER_PWR_STA_INACTIVE        0
#define CARRIER_PWR_STA_ACTIVE          1
#define CARRIER_PWR_STA_ACTIVATING      2
#define CARRIER_PWR_STA_UNKNOWN         3

#define TYPE_MAC                         0x1B
#define TYPE_LE_ROLE                     0x1C
#define TYPE_LONG_LOCAL_NAME             0x09
#define TYPE_SHORT_LOCAL_NAME            0x08
#define TYPE_16_BIT_UUIDS_PARTIAL        0x02
#define TYPE_16_BIT_UUIDS_COMPLETE       0x03
#define TYPE_32_BIT_UUIDS_PARTIAL        0x04
#define TYPE_32_BIT_UUIDS_COMPLETE       0x05
#define TYPE_128_BIT_UUIDS_PARTIAL       0x06
#define TYPE_128_BIT_UUIDS_COMPLETE      0x07
#define TYPE_CLASS_OF_DEVICE             0x0D
#define TYPE_SEC_MGR_TK                  0x10
#define TYPE_APPEARANCE                  0x19
#define TYPE_LE_SC_CONFIRMATION          0x22
#define TYPE_LE_SC_RANDOM                0x23
#define TYPE_VENDOR                      0xFF

#define BLE_ROLE_CENTRAL_ONLY            0x01

#define SEC_MGR_TK_SIZE                  16
#define SEC_MGR_LE_SC_C_SIZE             16
#define SEC_MGR_LE_SC_R_SIZE             16
#define CLASS_OF_DEVICE_SIZE             3
#define VENDOR_SERIAL_NUM_SIZE           2

#define UUID_SEPARATOR                   "-"
#define MAC_SEPARATOR                    ":"
#define SHIFT_ONE_BYTE                   8

using namespace OHOS::NFC::KITS;

NdefBtDataParser::NdefBtDataParser()
{
}

std::string NdefBtDataParser::GetBtMacFromPayload(const std::string& payload, uint32_t& offset)
{
    uint32_t macLen = 6;
    if (macLen * HEX_BYTE_LEN > payload.length() - (offset * HEX_BYTE_LEN)) {
        ErrorLog("NdefBtDataParser::GetBtMacFromPayload, data error, "
            "payload len %{public}lu offset.%{public}d", payload.length(), offset);
        return "";
    }
    std::string mac = payload.substr(offset * HEX_BYTE_LEN, macLen * HEX_BYTE_LEN);
    offset += macLen;

    std::string result = "";
    for (uint32_t i = macLen - 1; i > 0; i--) {
        result += mac.substr(i * HEX_BYTE_LEN, HEX_BYTE_LEN);
        result += MAC_SEPARATOR;
    }
    result += mac.substr(0, HEX_BYTE_LEN);
    return result;
}

bool NdefBtDataParser::GetBtDevClass(const std::string& payload, uint32_t& offset,
                                     Bluetooth::BluetoothDeviceClass& btClass)
{
    if (payload.length() == 0 || (payload.length() < (offset + CLASS_OF_DEVICE_SIZE) * HEX_BYTE_LEN)) {
        return false;
    }
    unsigned char firstByte = KITS::NfcSdkCommon::GetByteFromHexStr(payload, offset++);
    unsigned char secondByte = KITS::NfcSdkCommon::GetByteFromHexStr(payload, offset++);
    unsigned char thirdByte = KITS::NfcSdkCommon::GetByteFromHexStr(payload, offset++);
    int devClass = (firstByte << (SHIFT_ONE_BYTE * 2)) + (secondByte << SHIFT_ONE_BYTE) + thirdByte;
    btClass = Bluetooth::BluetoothDeviceClass(devClass);
    return true;
}

std::string NdefBtDataParser::RevertUuidStr(const std::string& uuid)
{
    std::string res = "";
    uint32_t len = uuid.length();
    if (len % HEX_BYTE_LEN != 0) {
        ErrorLog("uuid len not even");
        return res;
    }
    for (uint32_t i = len; i >= HEX_BYTE_LEN; i -= HEX_BYTE_LEN) {
        res += uuid.substr(i - HEX_BYTE_LEN, HEX_BYTE_LEN);
    }
    return res;
}

Bluetooth::UUID  NdefBtDataParser::FormatUuidTo128Bit(const std::string& uuid)
{
    const uint32_t uuidPrefixLen = 8;
    const uint32_t separatorPoz1 = 8;
    const uint32_t separatorPoz2 = 12;
    const uint32_t separatorPoz3 = 16;
    const uint32_t separatorPoz4 = 20;
    std::string uuidSubfix = Bluetooth::BLUETOOTH_UUID_BASE_UUID.substr(uuidPrefixLen,
        Bluetooth::BLUETOOTH_UUID_BASE_UUID.length() - uuidPrefixLen);
    std::string prefix16Bit = "0000";
    std::string res = "";

    if ((uuid.length() / HEX_BYTE_LEN) == Bluetooth::BLE_UUID_LEN_16) {
        res = prefix16Bit + RevertUuidStr(uuid) + uuidSubfix;
    } else if (uuid.length() == Bluetooth::BLE_UUID_LEN_32) {
        res = RevertUuidStr(uuid) + uuidSubfix;
    } else if (uuid.length() == Bluetooth::BLE_UUID_LEN_128) {
        res = RevertUuidStr(uuid);
        res.insert(separatorPoz4, UUID_SEPARATOR);
        res.insert(separatorPoz3, UUID_SEPARATOR);
        res.insert(separatorPoz2, UUID_SEPARATOR);
        res.insert(separatorPoz1, UUID_SEPARATOR);
    }
    return Bluetooth::UUID::FromString(res);
}

std::string NdefBtDataParser::GetDataFromPayload(const std::string& payload, uint32_t& offset, uint32_t dataLen)
{
    if (dataLen * HEX_BYTE_LEN > (payload.length() - (offset * HEX_BYTE_LEN))) {
        return "";
    }
    std::string data = payload.substr(offset * HEX_BYTE_LEN, dataLen * HEX_BYTE_LEN);
    offset += dataLen;
    return data;
}

std::vector<Bluetooth::UUID> NdefBtDataParser::GetUuidFromPayload(const std::string& payload, uint32_t& offset,
                                                                  uint32_t type, uint32_t len)
{
    // uuids can have several groups, uuidsSize is the size of each group
    uint32_t uuidSize;
    std::vector<Bluetooth::UUID> uuids;
    switch (type) {
        case TYPE_16_BIT_UUIDS_PARTIAL:
        case TYPE_16_BIT_UUIDS_COMPLETE:
            uuidSize = Bluetooth::BLE_UUID_LEN_16;
            break;
        case TYPE_32_BIT_UUIDS_PARTIAL:
        case TYPE_32_BIT_UUIDS_COMPLETE:
            uuidSize = Bluetooth::BLE_UUID_LEN_32;
            break;
        case TYPE_128_BIT_UUIDS_PARTIAL:
        case TYPE_128_BIT_UUIDS_COMPLETE:
            uuidSize = Bluetooth::BLE_UUID_LEN_128;
            break;
        default:
            ErrorLog("NdefBtDataParser::GetUuidFromPayload, unknown type of UUID");
            return uuids;
    }
    if (len == 0 || (len % uuidSize != 0) || len * HEX_BYTE_LEN > (payload.length() - (offset * HEX_BYTE_LEN))) {
        return uuids;
    }
    uint32_t uuidNum = len / uuidSize;
    for (uint32_t i = 0; i < uuidNum; i++) {
        std::string uuid = payload.substr(offset * HEX_BYTE_LEN, uuidSize * HEX_BYTE_LEN);
        offset += uuidSize;
        uuids.push_back(FormatUuidTo128Bit(uuid));
    }
    return uuids;
}

/*
 * BT RECORD STRUCTURE
 * Len(2 BYTEs) | MacAddr(6 BYTES, reverted) | LTV data
 * LTV data:
 * LEN(1 BYTE) |TYPE(1 BYTE) |VALUE(LEN -1 BYTES)
 */
std::shared_ptr<BtData> NdefBtDataParser::ParseBtRecord(const std::string& payload)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = false;
    uint32_t offset = 0; // offset is for byte parse position, payload is hex string
                         // to compare need to * HEX_BYTE_LEN

    uint32_t len = 2;
    offset += len;
    std::string macAddress = GetBtMacFromPayload(payload, offset);
    if (macAddress.empty()) {
        ErrorLog("NdefBtDataParser::ParseBtRecord, macAddress error, "
            "payload .len %{public}lu offset.%{public}d", payload.length(), offset);
        return data;
    }
    data->macAddress_ = macAddress;
    data->isValid_ = true;

    while ((offset * HEX_BYTE_LEN) < payload.length()) {
        bool isValid = false;
        std::string name;
        uint32_t tvLen = NfcSdkCommon::GetByteFromHexStr(payload, offset++) & UNSIGNED_BYTE_TO_INT_MASK;
        uint32_t type = NfcSdkCommon::GetByteFromHexStr(payload, offset++) & UNSIGNED_BYTE_TO_INT_MASK;
        InfoLog("NdefBtDataParser::ParseBtRecord, len:%{public}d type:0x%{public}X", tvLen, type);
        switch (type) {
            case TYPE_SHORT_LOCAL_NAME: {
                if (tvLen < 1) {
                    ErrorLog("NdefBtDataParser::ParseBtRecord, invalid  local name len. ");
                    data->isValid_ = false;
                    return data;
                }
                name = GetDataFromPayload(payload, offset, tvLen - 1);
                if (name.empty()) {
                    ErrorLog("NdefBtDataParser::ParseBtRecord, name error, "
                        "payload len.%{public}lu offset.%{public}d type.0x%{public}X", payload.length(), offset, type);
                    break;
                }
                data->name_ = KITS::NfcSdkCommon::HexStringToAsciiString(name);
                isValid = true;
                break;
            }
            case TYPE_LONG_LOCAL_NAME: {
                if (!data->name_.empty()) {
                    offset += (tvLen - 1);
                    break; // already contains short name
                }
                if (tvLen < 1) {
                    ErrorLog("NdefBtDataParser::ParseBtRecord, invalid  long local name len. ");
                    data->isValid_ = false;
                    return data;
                }
                name = GetDataFromPayload(payload, offset, tvLen - 1);
                if (name.empty()) {
                    ErrorLog("NdefBtDataParser::ParseBtRecord, name error, "
                        "payload len.%{public}lu offset.%{public}d type.0x%{public}X", payload.length(), offset, type);
                    break;
                }
                data->name_ = KITS::NfcSdkCommon::HexStringToAsciiString(name);
                isValid = true;
                break;
            }
            case TYPE_16_BIT_UUIDS_PARTIAL:
            case TYPE_16_BIT_UUIDS_COMPLETE:
            case TYPE_32_BIT_UUIDS_PARTIAL:
            case TYPE_32_BIT_UUIDS_COMPLETE:
            case TYPE_128_BIT_UUIDS_PARTIAL:
            case TYPE_128_BIT_UUIDS_COMPLETE: {
                data->uuids_.clear();
                data->uuids_ = GetUuidFromPayload(payload, offset, type, tvLen - 1);
                if (!data->uuids_.empty()) {
                    isValid = true;
                }
                break;
            }
            case TYPE_CLASS_OF_DEVICE: {
                if (tvLen - 1 != CLASS_OF_DEVICE_SIZE) {
                    ErrorLog("NdefBtDataParser::ParseBtRecord, invalid  class of Device len");
                    break;
                }
                isValid = GetBtDevClass(payload, offset, data->btClass_);
                break;
            }
            case TYPE_VENDOR: {
                std::string vendorPayload = GetDataFromPayload(payload, offset, tvLen - 1);
                if (vendorPayload.empty()) {
                    ErrorLog("NdefBtDataParser::ParseBtRecord, vendor error, "
                        "payload len.%{public}lu offset.%{public}d type.0x%{public}X", payload.length(), offset, type);
                    break;
                }
                data->vendorPayload_ = vendorPayload;
                isValid = true;
                break;
            }
            default: {
                offset += (tvLen - 1);
                ErrorLog("NdefBtDataParser::ParseBtRecord, unknown type = %{public}d", type);
                isValid = true;
                break;
            }
        }
        if (!isValid) {
            ErrorLog("NdefBtDataParser::ParseBtRecord, vendor error, "
                "payload len.%{public}lu offset.%{public}d type.0x%{public}X", payload.length(), offset, type);
            data->isValid_ = false;
            return data;
        }
    }
    return data;
}

std::shared_ptr<BtData> NdefBtDataParser::ParseBleRecord(const std::string& payload)
{
    std::shared_ptr<BtData> data = std::make_shared<BtData>();
    data->isValid_ = false;
    data->transport_ = Bluetooth::GATT_TRANSPORT_TYPE_LE;
    uint32_t offset = 0; // offset is for byte parse position, payload is hex string
                         // to compare need to * HEX_BYTE_LEN

    std::string bdaddr = "";
    unsigned char role = 0xF; // invalid default
    std::string leScC = "";
    std::string leScR = "";
    std::string name = "";
    std::string secMgrTK = "";
    std::string macAddress = "";
    while ((offset * HEX_BYTE_LEN) < payload.length()) {
        uint32_t len = NfcSdkCommon::GetByteFromHexStr(payload, offset++) & UNSIGNED_BYTE_TO_INT_MASK;
        uint32_t type = NfcSdkCommon::GetByteFromHexStr(payload, offset++) & UNSIGNED_BYTE_TO_INT_MASK;
        switch (type) {
            case TYPE_MAC: {
                uint32_t bdaddrLen = 7; // 6 bytes for mac, 1 for address type
                bdaddr = GetDataFromPayload(payload, offset, bdaddrLen);
                if (bdaddr.empty()) {
                    ErrorLog("NdefBtDataParser::ParseBleRecord, bdaddr error, "
                        "payload len.%{public}lu offset.%{public}d type.0x%{public}X", payload.length(), offset, type);
                    break;
                }
                macAddress = GetBtMacFromPayload(payload, offset);
                if (macAddress.empty()) {
                    ErrorLog("NdefBtDataParser::ParseBleRecord, macAddress error, "
                        "payload len.%{public}lu offset.%{public}d type.0x%{public}X", payload.length(), offset, type);
                    break;
                }
                offset++; // advance over random byte
                data->isValid_ = true;
                break;
            }
            case TYPE_LE_ROLE: {
                role = NfcSdkCommon::GetByteFromHexStr(payload, offset++) & UNSIGNED_BYTE_TO_INT_MASK;
                if (role == BLE_ROLE_CENTRAL_ONLY) {
                    data->isValid_ = false;
                    return data;
                }
                break;
            }
            case TYPE_LONG_LOCAL_NAME: {
                name = GetDataFromPayload(payload, offset, len - 1);
                if (name.empty()) {
                    ErrorLog("NdefBtDataParser::ParseBleRecord, name error, "
                        "payload len.%{public}lu offset.%{public}d type.0x%{public}X", payload.length(), offset, type);
                    break;
                }
                data->name_ = KITS::NfcSdkCommon::HexStringToAsciiString(name);
                break;
            }
            case TYPE_SEC_MGR_TK: {
                if (len - 1 != SEC_MGR_TK_SIZE) {
                    ErrorLog("NdefBtDataParser::ParseBleRecord, SM TK len error, should be %{public}d",
                        SEC_MGR_TK_SIZE);
                    break;
                }
                secMgrTK = GetDataFromPayload(payload, offset, len);
                if (leScC.empty()) {
                    ErrorLog("NdefBtDataParser::ParseBleRecord, secMgrTK error, "
                        "payload len.%{public}lu offset.%{public}d type.0x%{public}X", payload.length(), offset, type);
                    break;
                }
                break;
            }
            case TYPE_LE_SC_CONFIRMATION: {
                if (len - 1 != SEC_MGR_LE_SC_C_SIZE) {
                    ErrorLog("NdefBtDataParser::ParseBleRecord, LE SC Confirmation len error, "
                        "should be %{public}d", SEC_MGR_LE_SC_C_SIZE);
                    break;
                }
                leScC = GetDataFromPayload(payload, offset, len - 1);
                if (leScC.empty()) {
                    ErrorLog("NdefBtDataParser::ParseBleRecord, leScC Confirmation error, "
                        "payload len.%{public}lu offset.%{public}d type.0x%{public}X", payload.length(), offset, type);
                    break;
                }
                break;
            }
            case TYPE_LE_SC_RANDOM: {
                if (len - 1 != SEC_MGR_LE_SC_R_SIZE) {
                    ErrorLog("NdefBtDataParser::ParseBleRecord, LE SC Random len error, should be %{public}d",
                        SEC_MGR_LE_SC_R_SIZE);
                    break;
                }
                leScR = GetDataFromPayload(payload, offset, len - 1);
                if (leScR.empty()) {
                    ErrorLog("NdefBtDataParser::ParseBleRecord, leScC Random error, "
                        "payload len.%{public}lu offset.%{public}d type.0x%{public}X", payload.length(), offset, type);
                    break;
                }
                break;
            }
            default: {
                offset += (len - 1);
                break;
            }
        }
    }
    return data;
}

std::shared_ptr<BtData> NdefBtDataParser::CheckBtRecord(const std::string& msg)
{
    if (msg.empty()) {
        ErrorLog("NdefBtDataParser::CheckBtRecord: msg is empty");
        return std::make_shared<BtData>();
    }
    std::shared_ptr<NdefMessage> ndef = NdefMessage::GetNdefMessage(msg);
    if (ndef == nullptr || (ndef->GetNdefRecords().size() == 0)) {
        ErrorLog("NdefBtDataParser::CheckBtRecord: ndef is null");
        return std::make_shared<BtData>();
    }
    std::shared_ptr<NdefRecord> record = ndef->GetNdefRecords()[0];
    if (record == nullptr) {
        ErrorLog("NdefBtDataParser::CheckBtRecord: record is null");
        return std::make_shared<BtData>();
    }

    // Check BT
    if (record->tnf_ == NdefMessage::TNF_MIME_MEDIA &&
        (record->tagRtdType_.compare(NfcSdkCommon::StringToHexString(RTD_TYPE_BT)) == 0)) {
        InfoLog("NdefBtDataParser::CheckBtRecord: is bt");
        return ParseBtRecord(record->payload_);
    }

    // Check BLE
    if (record->tnf_ == NdefMessage::TNF_MIME_MEDIA &&
        (record->tagRtdType_.compare(NfcSdkCommon::StringToHexString(RTD_TYPE_BLE)) == 0)) {
        InfoLog("NdefBtDataParser::CheckBtRecord: is ble, currently not supported");
        return std::make_shared<BtData>();
    }

    // Check Handover Select, followed by a BT record
    if (record->tnf_ == NdefMessage::TNF_WELL_KNOWN &&
        (record->tagRtdType_.compare(NdefMessage::GetTagRtdType(NdefMessage::RTD_HANDOVER_SELECT)) == 0)) {
        InfoLog("NdefBtDataParser::CheckBtRecord: is handover select, currently not supported");
        return std::make_shared<BtData>();
    }
    return std::make_shared<BtData>();
}
} // namespace TAG
} // namespace NFC
} // namespace OHOS