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
#ifndef I_NCI_NFCC_INTERFACE_H
#define I_NCI_NFCC_INTERFACE_H
#include <string>

namespace OHOS {
namespace NFC {
namespace NCI {
class INciNfccInterface {
public:
    virtual ~INciNfccInterface() = default;

    /**
     * @brief Initialize when turn on NFC
     * @return True if success, otherwise false.
     */
    virtual bool Initialize() = 0;

    /**
     * @brief Deinitialize when turn off NFC
     * @return True if success, otherwise false.
     */
    virtual bool Deinitialize() = 0;

    /**
     * @brief Start polling and listening
     * @param techMask bitmask of the technologies
     * @param enableReaderMode if enable tag polling
     * @param enableHostRouting if enable host routing
     * @param restart true if need restart, otherwise false.
     */
    virtual void EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart) = 0;

    /**
     * @brief Stop polling and listening
     */
    virtual void DisableDiscovery() = 0;

    /**
     * @brief Set the screen statue to nfc controller.
     * @param screenStateMask the bitmask of the screen state
     * @return True if success, otherwise false.
     */
    virtual bool SetScreenStatus(uint8_t screenStateMask) = 0;

    /**
     * @brief Get Nci version supprted by nfc controller.
     * @return 0x20 if it's NCI2.0, otherwise 0x10 if it's NCI1.0.
     */
    virtual int GetNciVersion() = 0;

    /**
     * @brief Abort the nfc controller if NCI timeout.
     */
    virtual void Abort() = 0;

    /**
     * @brief Do factory reset for nfc controller.
     */
    virtual void FactoryReset() = 0;

    /**
     * @brief Shutdown the device. Enable the nfc functionality if support power off case.
     */
    virtual void Shutdown() = 0;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // I_NCI_NFCC_INTERFACE_H
