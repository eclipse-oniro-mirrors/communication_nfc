/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "ndef_wifi_data_parser.h"

#include "loghelper.h"
#include "ndef_message.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace TAG {
#define RTD_TYPE_WIFI               "application/vnd.wfa.wsc"

#define UNSIGNED_BYTE_TO_INT_MASK   0xFF
#define WIFI_TYPE_LEN               2
#define WIFI_TYPE_TO_INT_MASK       0xFFFF

#define CREDENTIAL_FIELD_TYPE       0x100E
#define WIFI_SSID_TYPE              0x1045
#define WIFI_NETWORK_KEY_TYPE       0x1027
#define WIFI_AUTH_TYPE_TYPE         0x1003
#define WIFI_VENDOR_EXT_TYPE        0x1049

#define AUTH_TYPE_OPEN              0x0001
#define AUTH_TYPE_WPA_PSK           0x0002
#define AUTH_TYPE_WPA_EAP           0x0008
#define AUTH_TYPE_WPA2_EAP          0x0010
#define AUTH_TYPE_WPA2_PSK          0x0020
#define AUTH_TYPE_WPA_AND_WPA2_PSK  0x0022

#define RECORDS_MAX_SIZE            2000
#define NETWORK_KEY_MAX_SIZE        64
#define AUTH_TYPE_SIZE              2
#define MAX_VALUE_LENGTH            2000

using namespace OHOS::NFC::KITS;

NdefWifiDataParser::NdefWifiDataParser()
{
}

uint16_t NdefWifiDataParser::GetTypeFromPayload(const std::string& src, uint32_t &offset)
{
    if (src.length() == 0 || (src.length() < (offset + WIFI_TYPE_LEN) * HEX_BYTE_LEN)) {
        return 0;
    }
    unsigned char firstByte = KITS::NfcSdkCommon::GetByteFromHexStr(src, offset);
    offset++;
    unsigned char secondByte = KITS::NfcSdkCommon::GetByteFromHexStr(src, offset);
    offset++;
    uint8_t shift = 8; // 8 bits for one byte
    return ((firstByte << shift) +  secondByte) & WIFI_TYPE_TO_INT_MASK;
}

std::string NdefWifiDataParser::GetValueFromPayload(const std::string& payload, uint32_t& offset, uint16_t dataLen)
{
    if (dataLen > MAX_VALUE_LENGTH) {
        return "";
    }
    if (dataLen * HEX_BYTE_LEN > (payload.length() - (offset * HEX_BYTE_LEN))) {
        return "";
    }
    std::string data = payload.substr(offset * HEX_BYTE_LEN, dataLen * HEX_BYTE_LEN);
    offset += dataLen;
    return data;
}

void NdefWifiDataParser::SetKeyMgmt(std::string& keyMgmt, uint16_t authType)
{
    switch (authType) {
        case AUTH_TYPE_OPEN:
            keyMgmt = Wifi::KEY_MGMT_NONE;
            break;
        case AUTH_TYPE_WPA_PSK:
            // fall-through
        case AUTH_TYPE_WPA2_PSK:
            // fall-through
        case AUTH_TYPE_WPA_AND_WPA2_PSK:
            keyMgmt = Wifi::KEY_MGMT_WPA_PSK;
            break;
        case AUTH_TYPE_WPA_EAP:
            // fall-through
        case AUTH_TYPE_WPA2_EAP:
            keyMgmt = Wifi::KEY_MGMT_EAP;
            break;
        default:
            break;
    }
}

/*
 * WIFI RECORD STRUCTURE
 * Credential Type(2 BYTES) | Length(2 BYTES) | TLV data
 * LTV data:
 * TYPE(2 BYTES) | LEN(2 BYTES) | VALUE(LEN BYTES)
 */
std::shared_ptr<WifiData> NdefWifiDataParser::ParseWiFiPayload(const std::string& payload)
{
    DebugLog("ParseWiFiPayload");
    std::shared_ptr<WifiData> data = std::make_shared<WifiData>();
    uint32_t offset = 0;
    uint16_t fieldId = GetTypeFromPayload(payload, offset);
    if (fieldId != CREDENTIAL_FIELD_TYPE) {
        return data;
    }
    uint32_t fieldLen = GetTypeFromPayload(payload, offset);
    if (fieldLen == 0) {
        return data;
    }
    data->isValid_ = true;
    if (!data->config_) {
        data->config_ = new Wifi::WifiDeviceConfig();
    }
    while ((offset * HEX_BYTE_LEN) < payload.length()) {
        uint16_t type = GetTypeFromPayload(payload, offset);
        uint16_t len = GetTypeFromPayload(payload, offset);
        InfoLog("NdefWifiDataParser::ParseWiFiPayload, type: 0x%{public}X, len: %{public}d", type, len);
        switch (type) {
            case WIFI_SSID_TYPE: {
                std::string ssid = GetValueFromPayload(payload, offset, len);
                if (ssid.empty()) {
                    ErrorLog("NdefWifiDataParser::ParseWiFiPayload, SSID error, "
                        "payload len.%{public}lu offset.%{public}d type.0x%{public}X", payload.length(), offset, type);
                    data->isValid_ = false;
                    return data;
                }
                data->config_->ssid = KITS::NfcSdkCommon::HexStringToAsciiString(ssid);
                InfoLog("NdefWifiDataParser::ParseWiFiPayload, SSID: %{private}s", data->config_->ssid.c_str());
                break;
            }
            case WIFI_NETWORK_KEY_TYPE: {
                if (len > NETWORK_KEY_MAX_SIZE) {
                    ErrorLog("NdefWifiDataParser::ParseWiFiPayload, invalid network key length: %{public}d", len);
                    data->isValid_ = false;
                    return data;
                }
                std::string key = GetValueFromPayload(payload, offset, len);
                if (key.empty()) {
                    ErrorLog("NdefWifiDataParser::ParseWiFiPayload, name error, "
                        "payload len.%{public}lu offset.%{public}d type.0x%{public}X", payload.length(), offset, type);
                    data->isValid_ = false;
                    return data;
                }
                data->config_->preSharedKey = KITS::NfcSdkCommon::HexStringToAsciiString(key);
                break;
            }
            case WIFI_AUTH_TYPE_TYPE: {
                if (len != AUTH_TYPE_SIZE) {
                    ErrorLog("NdefWifiDataParser::ParseWiFiPayload, invalid auth type len");
                    data->isValid_ = false;
                    return data;
                }
                uint16_t authType = GetTypeFromPayload(payload, offset);
                if (authType == 0) {
                    ErrorLog("NdefWifiDataParser::ParseWiFiPayload, invalid auth type value");
                    data->isValid_ = false;
                    return data;
                }
                SetKeyMgmt(data->config_->keyMgmt, authType);
                InfoLog("NdefWifiDataParser::ParseWiFiPayload, keyMgmt: %{public}s, authType: %{public}d",
                    data->config_->keyMgmt.c_str(), authType);
                break;
            }
            case WIFI_VENDOR_EXT_TYPE: {
                std::string vendorPayload = GetValueFromPayload(payload, offset, len);
                if (vendorPayload.empty()) {
                    ErrorLog("NdefWifiDataParser::ParseWiFiPayload, vendor error, "
                        "payload len.%{public}lu offset.%{public}d type.0x%{public}X", payload.length(), offset, type);
                    data->isValid_ = false;
                    return data;
                }
                data->vendorPayload_ = vendorPayload;
                break;
            }
            default: {
                offset += len;
                InfoLog("NdefWifiDataParser::ParseWiFiPayload, unknown type = 0x%{public}X", type);
                break;
            }
        }
    }
    if (!data->config_->ssid.empty()) {
        if (data->config_->keyMgmt == Wifi::KEY_MGMT_NONE) {
            WarnLog("key should be null when keyMgmt is NONE");
            data->config_->preSharedKey = "";
        } else if (data->config_->preSharedKey.empty()) {
            ErrorLog("key should not be null when keyMgmt is not NONE");
            data->isValid_ = false;
        }
    }
    if (!data->isValid_ && data->config_) {
        delete data->config_;
        data->config_ = nullptr;
    }
    InfoLog("parse end, valid: %{public}d", data->isValid_);
    return data;
}

std::shared_ptr<WifiData> NdefWifiDataParser::CheckWifiRecord(const std::string& msg)
{
    if (msg.empty()) {
        ErrorLog("NdefWifiDataParser::CheckWifiRecord: msg is empty");
        return std::make_shared<WifiData>();
    }
    std::shared_ptr<NdefMessage> ndef = NdefMessage::GetNdefMessage(msg);
    if (ndef == nullptr || (ndef->GetNdefRecords().size() == 0)) {
        ErrorLog("NdefWifiDataParser::CheckWifiRecord: ndef is null");
        return std::make_shared<WifiData>();
    }
    std::vector<std::shared_ptr<NdefRecord>> records = ndef->GetNdefRecords();
    if (records.size() > RECORDS_MAX_SIZE) {
        ErrorLog("NdefWifiDataParser::CheckWifiRecord: invalid records size");
        return std::make_shared<WifiData>();
    }
    for (std::shared_ptr<NdefRecord> record : records) {
        if (!record) {
            ErrorLog("NdefWifiDataParser::CheckWifiRecord: record is null");
            return std::make_shared<WifiData>();
        }
        if (record->tagRtdType_.compare(KITS::NfcSdkCommon::StringToHexString(RTD_TYPE_WIFI)) == 0) {
            return ParseWiFiPayload(record->payload_);
        }
    }
    return std::make_shared<WifiData>();
}
} // namespace TAG
} // namespace NFC
} // namespace OHOS