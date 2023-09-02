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
#ifndef VENDOR_EXT_SERVICE_H
#define VENDOR_EXT_SERVICE_H
#include <string>

namespace OHOS {
namespace NFC {
namespace NCI {
class VendorExtService {
private:

public:
    VendorExtService();
    ~VendorExtService();
    static std::string chipType;
    static bool OnStartExtService(void);
    static std::string GetNfcChipType(void);
    static void VendorEventCallback(uint8_t dmEvent, uint16_t dataLen, const char* eventData);
    static void OnStopExtService(void);
    typedef const char* (*GET_CHIP_TYPE)();
    typedef void (*VENDOR_NFC_EVENT_CALLBACK)(uint8_t dmEvent, uint16_t dataLen, const char* eventData);
};

} // namespace NCI
} // namespace NFC
} // namespace OHOS

#endif