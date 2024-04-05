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
#ifndef NDEF_BT_DATA_PARSER_H
#define NDEF_BT_DATA_PARSER_H

#include <string>
#include "bluetooth_def.h"
#include "bluetooth_device_class.h"
#include "uuid.h"

namespace OHOS {
namespace NFC {
namespace TAG {
class BtData {
public:
    bool isValid_ = false;
    std::string name_ = "";
    std::string oobData_ = "";
    bool carrierActivating_ = false;
    uint8_t transport_ = Bluetooth::GATT_TRANSPORT_TYPE_AUTO;
    std::vector<Bluetooth::UUID> uuids_ {};
    Bluetooth::BluetoothDeviceClass btClass_ {};

    std::string macAddress_ = "";
    std::string vendorSerialNum_ = "";
    std::string vendorPayload_ = "";
};

class NdefBtDataParser {
public:
    NdefBtDataParser();
    ~NdefBtDataParser() {}
    static std::shared_ptr<BtData> CheckBtRecord(const std::string& msg);

private:
    static std::shared_ptr<BtData> ParseBtRecord(const std::string& payload);
    static std::shared_ptr<BtData> ParseBleRecord(const std::string& payload);
    static std::vector<Bluetooth::UUID> GetUuidFromPayload(const std::string& payload, uint32_t& offset,
                                                           uint32_t type, uint32_t len);
    static std::string GetDataFromPayload(const std::string& payload, uint32_t& offset, uint32_t datalen);
    static std::string GetBtMacFromPayload(const std::string& payload, uint32_t& offset);
    static bool GetBtDevClass(const std::string& payload, uint32_t& offset,
                              Bluetooth::BluetoothDeviceClass& btClass);
    static std::string RevertUuidStr(const std::string& uuid);
    static Bluetooth::UUID FormatUuidTo128Bit(const std::string& uuid);
};
} // namespace TAG
} // namespace NFC
} // namespace OHOS
#endif // NDEF_BT_DATA_PARSER_H