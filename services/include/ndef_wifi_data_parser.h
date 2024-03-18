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
#ifndef NDEF_WIFI_DATA_PARSER_H
#define NDEF_WIFI_DATA_PARSER_H

#include <string>
#include "wifi_msg.h"

namespace OHOS {
namespace NFC {
namespace TAG {
class WifiData {
public:
    bool isValid_ = false;
    // config will be used in wifi_connection_manager.cpp
    // this pointer should be deleted when WifiData is invalid or wifi connection is finished
    Wifi::WifiDeviceConfig* config_ = nullptr;
    std::string vendorPayload_ = "";
};
class NdefWifiDataParser {
public:
    NdefWifiDataParser();
    ~NdefWifiDataParser() {}
    static std::shared_ptr<WifiData> CheckWifiRecord(const std::string& msg);

private:
    static uint16_t GetTypeFromPayload(const std::string& src, uint32_t& offset);
    static std::string GetValueFromPayload(const std::string& payload, uint32_t& offset, uint16_t dataLen);
    static void SetKeyMgmt(std::string& keyMgmt, uint16_t authType);
    static std::shared_ptr<WifiData> ParseWiFiPayload(const std::string& payload);
};
} // namespace TAG
} // namespace NFC
} // namespace OHOS
#endif // NDEF_WIFI_DATA_PARSER_H