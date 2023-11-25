/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#include "ndef_bt_oob_data_parser.h"

#include "loghelper.h"
#include "ndef_message.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
#define RTD_TYPE_BT_OOB         "application/vnd.bluetooth.ep.oob"
#define RTD_TYPE_BLE_OOB        "application/vnd.bluetooth.le.oob"

#define UNSIGNED_BYTE_TO_INT_MASK   0xFF

#define CARRIER_PWR_STA_INACTIVE    0
#define CARRIER_PWR_STA_ACTIVE      1
#define CARRIER_PWR_STA_ACTIVATING  2
#define CARRIER_PWR_STA_UNKNOWN     3

#define BT_OOB_TYPE_MAC                         0x1B
#define BT_OOB_TYPE_LE_ROLE                     0x1C
#define BT_OOB_TYPE_LONG_LOCAL_NAME             0x09
#define BT_OOB_TYPE_SHORT_LOCAL_NAME            0x08
#define BT_OOB_TYPE_16_BIT_UUIDS_PARTIAL        0x02
#define BT_OOB_TYPE_16_BIT_UUIDS_COMPLETE       0x03
#define BT_OOB_TYPE_32_BIT_UUIDS_PARTIAL        0x04
#define BT_OOB_TYPE_32_BIT_UUIDS_COMPLETE       0x05
#define BT_OOB_TYPE_128_BIT_UUIDS_PARTIAL       0x06
#define BT_OOB_TYPE_128_BIT_UUIDS_COMPLETE      0x07
#define BT_OOB_TYPE_CLASS_OF_DEVICE             0x0D
#define BT_OOB_TYPE_SEC_MGR_TK                  0x10
#define BT_OOB_TYPE_APPEARANCE                  0x19
#define BT_OOB_TYPE_LE_SC_CONFIRMATION          0x22
#define BT_OOB_TYPE_LE_SC_RANDOM                0x23
#define BT_OOB_TYPE_VENDOR                      0xFF

#define BT_OOB_LE_ROLE_CENTRAL_ONLY             0x01

#define SEC_MGR_TK_SIZE         16
#define SEC_MGR_LE_SC_C_SIZE    16
#define SEC_MGR_LE_SC_R_SIZE    16
#define CLASS_OF_DEVICE_SIZE    3
#define VENDOR_SERIAL_NUM_SIZE  2

#define UUID_BYTES_16_BIT_LEN    2
#define UUID_BYTES_32_BIT_LEN    4
#define UUID_BYTES_128_BIT_LEN   16

using namespace OHOS::NFC::KITS;

NdefBtOobDataParser::NdefBtOobDataParser()
{
}

std::string NdefBtOobDataParser::GetBtMacFromPayload(const std::string& payload, uint32_t& offset)
{
    uint32_t macLen = 6;
    if (macLen * HEX_BYTE_LEN > payload.length() - (offset * HEX_BYTE_LEN)) {
        ErrorLog("NdefBtOobDataParser::GetBtMacFromPayload, data error, "
            "payload len %{public}lu offset.%{public}d", payload.length(), offset);
        return "";
    }
    std::string mac = payload.substr(offset * HEX_BYTE_LEN, macLen * HEX_BYTE_LEN);
    offset += macLen;
    return mac;
}

std::string NdefBtOobDataParser::GetDataFromPayload(const std::string& payload, uint32_t& offset, uint32_t datalen)
{
    if (datalen * HEX_BYTE_LEN > (payload.length() - (offset * HEX_BYTE_LEN))) {
        return "";
    }
    std::string data = payload.substr(offset * HEX_BYTE_LEN, datalen * HEX_BYTE_LEN);
    offset += datalen;
    return data;
}

std::string NdefBtOobDataParser::GetUuidFromPayload(const std::string& payload, uint32_t& offset,
                                                    uint32_t type, uint32_t len)
{
    // uuids can have several groups, uuidsSize is the size of each group
    uint32_t uuidSize;
    switch (type) {
        case BT_OOB_TYPE_16_BIT_UUIDS_PARTIAL:
        case BT_OOB_TYPE_16_BIT_UUIDS_COMPLETE:
            uuidSize = UUID_BYTES_16_BIT_LEN;
            break;
        case BT_OOB_TYPE_32_BIT_UUIDS_PARTIAL:
        case BT_OOB_TYPE_32_BIT_UUIDS_COMPLETE:
            uuidSize = UUID_BYTES_32_BIT_LEN;
            break;
        case BT_OOB_TYPE_128_BIT_UUIDS_PARTIAL:
        case BT_OOB_TYPE_128_BIT_UUIDS_COMPLETE:
            uuidSize = UUID_BYTES_128_BIT_LEN;
            break;
        default:
            ErrorLog("NdefBtOobDataParser::GetUuidFromPayload, unknown type of UUID");
            return "";
    }
    if (len == 0 || (len % uuidSize != 0) || len * HEX_BYTE_LEN > (payload.length() - (offset * HEX_BYTE_LEN))) {
        return "";
    }
    std::string uuid = payload.substr(offset * HEX_BYTE_LEN, len * HEX_BYTE_LEN);
    offset += len;
    return uuid;
}

/*
 * BT OOB RECORD STRUCTURE
 * BtOobLen(2 BYTEs) | MacAddr(6 BYTES, reverted) | LTV data
 * LTV data:
 * LEN(1 BYTE) |TYPE(1 BYTE) |VALUE(LEN -1 BYTES)
 */
std::shared_ptr<BtOobData> NdefBtOobDataParser::ParseBtOobRecord(const std::string& payload)
{
    std::shared_ptr<BtOobData> data = std::make_shared<BtOobData>();
    data->isValid_ = false;
    uint32_t offset = 0; // offset is for byte parse position, payload is hex string
                         // to compare need to * HEX_BYTE_LEN

    uint32_t btOobLen = 2;
    offset += btOobLen;
    std::string macAddress = GetBtMacFromPayload(payload, offset);
    if (macAddress.empty()) {
        ErrorLog("NdefBtOobDataParser::ParseBtOobRecord, macAddress error, "
            "payload .len %{public}lu offset.%{public}d", payload.length(), offset);
        return data;
    }
    data->macAddress_ = macAddress;
    data->isValid_ = true;

    while ((offset * HEX_BYTE_LEN) < payload.length()) {
        bool isValid = false;
        std::string name;
        uint32_t len = NfcSdkCommon::GetByteFromHexStr(payload, offset++) & UNSIGNED_BYTE_TO_INT_MASK;
        uint32_t type = NfcSdkCommon::GetByteFromHexStr(payload, offset++) & UNSIGNED_BYTE_TO_INT_MASK;
        switch (type) {
            case BT_OOB_TYPE_SHORT_LOCAL_NAME: {
                if (len < 1) {
                    ErrorLog("NdefBtOobDataParser::ParseBtOobRecord, invalid  local name len. ");
                    data->isValid_ = false;
                    return data;
                }
                name = GetDataFromPayload(payload, offset, len - 1);
                if (name.empty()) {
                    ErrorLog("NdefBtOobDataParser::ParseBtOobRecord, name error, "
                        "payload len.%{public}lu offset.%{public}d type.%{public}d", payload.length(), offset, type);
                    break;
                }
                data->name_ = name;
                isValid = true;
                break;
            }
            case BT_OOB_TYPE_LONG_LOCAL_NAME: {
                if (!data->name_.empty()) {
                    offset += (len - 1);
                    break; // already contains short name
                }
                if (len < 1) {
                    ErrorLog("NdefBtOobDataParser::ParseBtOobRecord, invalid  long local name len. ");
                    data->isValid_ = false;
                    return data;
                }
                name = GetDataFromPayload(payload, offset, len - 1);
                if (name.empty()) {
                    ErrorLog("NdefBtOobDataParser::ParseBtOobRecord, name error, "
                        "payload len.%{public}lu offset.%{public}d type.%{public}d", payload.length(), offset, type);
                    break;
                }
                data->name_ = name;
                isValid = true;
                break;
            }
            case BT_OOB_TYPE_16_BIT_UUIDS_PARTIAL:
            case BT_OOB_TYPE_16_BIT_UUIDS_COMPLETE:
            case BT_OOB_TYPE_32_BIT_UUIDS_PARTIAL:
            case BT_OOB_TYPE_32_BIT_UUIDS_COMPLETE:
            case BT_OOB_TYPE_128_BIT_UUIDS_PARTIAL:
            case BT_OOB_TYPE_128_BIT_UUIDS_COMPLETE: {
                data->uuids_ = GetUuidFromPayload(payload, offset, type, len - 1);
                if (!data->uuids_.empty()) {
                    isValid = true;
                }
                break;
            }
            case BT_OOB_TYPE_CLASS_OF_DEVICE: {
                if (len - 1 != CLASS_OF_DEVICE_SIZE) {
                    ErrorLog("NdefBtOobDataParser::ParseBtOobRecord, invalid  class of Device len");
                    break;
                }
                offset += CLASS_OF_DEVICE_SIZE;
                isValid = true;
                break;
            }
            case BT_OOB_TYPE_VENDOR: {
                std::string vendorPayload = GetDataFromPayload(payload, offset, len - 1);
                if (vendorPayload.empty()) {
                    ErrorLog("NdefBtOobDataParser::ParseBtOobRecord, vendor error, "
                        "payload len.%{public}lu offset.%{public}d type.%{public}d", payload.length(), offset, type);
                    break;
                }
                data->vendorPayload_ = vendorPayload;
                isValid = true;
                break;
            }
            default: {
                offset += (len - 1);
                ErrorLog("NdefBtOobDataParser::ParseBtOobRecord, unknown type = %{public}d", type);
                break;
            }
        }
        if (!isValid) {
            ErrorLog("NdefBtOobDataParser::ParseBtOobRecord, vendor error, "
                "payload len.%{public}lu offset.%{public}d type.%{public}d", payload.length(), offset, type);
            data->isValid_ = false;
            return data;
        }
    }
    return data;
}

std::shared_ptr<BtOobData> NdefBtOobDataParser::ParseBleOobRecord(const std::string& payload)
{
    std::shared_ptr<BtOobData> data = std::make_shared<BtOobData>();
    data->isValid_ = false;
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
            case BT_OOB_TYPE_MAC: {
                uint32_t bdaddrLen = 7; // 6 bytes for mac, 1 for address type
                bdaddr = GetDataFromPayload(payload, offset, bdaddrLen);
                if (bdaddr.empty()) {
                    ErrorLog("NdefBtOobDataParser::ParseBleOobRecord, bdaddr error, "
                        "payload len.%{public}lu offset.%{public}d type.%{public}d", payload.length(), offset, type);
                    break;
                }
                macAddress = GetBtMacFromPayload(payload, offset);
                if (macAddress.empty()) {
                    ErrorLog("NdefBtOobDataParser::ParseBleOobRecord, macAddress error, "
                        "payload len.%{public}lu offset.%{public}d type.%{public}d", payload.length(), offset, type);
                    break;
                }
                offset++; // advance over random byte
                data->isValid_ = true;
                break;
            }
            case BT_OOB_TYPE_LE_ROLE: {
                role = NfcSdkCommon::GetByteFromHexStr(payload, offset++) & UNSIGNED_BYTE_TO_INT_MASK;
                if (role == BT_OOB_LE_ROLE_CENTRAL_ONLY) {
                    data->isValid_ = false;
                    return data;
                }
                break;
            }
            case BT_OOB_TYPE_LONG_LOCAL_NAME: {
                name = GetDataFromPayload(payload, offset, len - 1);
                if (name.empty()) {
                    ErrorLog("NdefBtOobDataParser::ParseBleOobRecord, name error, "
                        "payload len.%{public}lu offset.%{public}d type.%{public}d", payload.length(), offset, type);
                    break;
                }
                data->name_ = name;
                break;
            }
            case BT_OOB_TYPE_SEC_MGR_TK: {
                if (len - 1 != SEC_MGR_TK_SIZE) {
                    ErrorLog("NdefBtOobDataParser::ParseBleOobRecord, SM TK len error, should be %{public}d",
                        SEC_MGR_TK_SIZE);
                    break;
                }
                secMgrTK = GetDataFromPayload(payload, offset, len);
                if (leScC.empty()) {
                    ErrorLog("NdefBtOobDataParser::ParseBleOobRecord, secMgrTK error, "
                        "payload len.%{public}lu offset.%{public}d type.%{public}d", payload.length(), offset, type);
                    break;
                }
                break;
            }
            case BT_OOB_TYPE_LE_SC_CONFIRMATION: {
                if (len - 1 != SEC_MGR_LE_SC_C_SIZE) {
                    ErrorLog("NdefBtOobDataParser::ParseBleOobRecord, LE SC Confirmation len error, "
                        "should be %{public}d", SEC_MGR_LE_SC_C_SIZE);
                    break;
                }
                leScC = GetDataFromPayload(payload, offset, len - 1);
                if (leScC.empty()) {
                    ErrorLog("NdefBtOobDataParser::ParseBleOobRecord, leScC Confirmation error, "
                        "payload len.%{public}lu offset.%{public}d type.%{public}d", payload.length(), offset, type);
                    break;
                }
                break;
            }
            case BT_OOB_TYPE_LE_SC_RANDOM: {
                if (len - 1 != SEC_MGR_LE_SC_R_SIZE) {
                    ErrorLog("NdefBtOobDataParser::ParseBleOobRecord, LE SC Random len error, should be %{public}d",
                        SEC_MGR_LE_SC_R_SIZE);
                    break;
                }
                leScR = GetDataFromPayload(payload, offset, len - 1);
                if (leScR.empty()) {
                    ErrorLog("NdefBtOobDataParser::ParseBleOobRecord, leScC Random error, "
                        "payload len.%{public}lu offset.%{public}d type.%{public}d", payload.length(), offset, type);
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

std::shared_ptr<BtOobData> NdefBtOobDataParser::CheckBtRecord(const std::string& msg)
{
    if (msg.empty()) {
        ErrorLog("NdefBtOobDataParser::CheckBtRecord: msg is empty");
        return std::make_shared<BtOobData>();
    }
    std::shared_ptr<NdefMessage> ndef = NdefMessage::GetNdefMessage(msg);
    if (ndef == nullptr || (ndef->GetNdefRecords().size() == 0)) {
        ErrorLog("NdefBtOobDataParser::CheckBtRecord: ndef is null");
        return std::make_shared<BtOobData>();
    }
    std::shared_ptr<NdefRecord> record = ndef->GetNdefRecords()[0];
    if (record == nullptr) {
        ErrorLog("NdefBtOobDataParser::CheckBtRecord: record is null");
        return std::make_shared<BtOobData>();
    }

    // Check BT OOB
    if (record->tnf_ == NdefMessage::TNF_MIME_MEDIA &&
        (record->tagRtdType_.compare(NfcSdkCommon::StringToHexString(RTD_TYPE_BT_OOB)) == 0)) {
        InfoLog("NdefBtOobDataParser::CheckBtRecord: is bt oob");
        return ParseBtOobRecord(record->payload_);
    }

    // Check BLE OOB
    if (record->tnf_ == NdefMessage::TNF_MIME_MEDIA &&
        (record->tagRtdType_.compare(NfcSdkCommon::StringToHexString(RTD_TYPE_BLE_OOB)) == 0)) {
        InfoLog("NdefBtOobDataParser::CheckBtRecord: is ble oob, currently not supported");
        return std::make_shared<BtOobData>();
    }

    // Check Handover Select, followed by a BT OOB record
    if (record->tnf_ == NdefMessage::TNF_WELL_KNOWN &&
        (record->tagRtdType_.compare(NdefMessage::GetTagRtdType(NdefMessage::RTD_HANDOVER_SELECT)) == 0)) {
        InfoLog("NdefBtOobDataParser::CheckBtRecord: is handover select, currently not supported");
        return std::make_shared<BtOobData>();
    }
    return std::make_shared<BtOobData>();
}
} // namespace NFC
} // namespace OHOS