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
#ifndef NCI_NFCC_PROXY_H
#define NCI_NFCC_PROXY_H
#include "inci_nfcc_interface.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class NciNfccProxy final : public INciNfccInterface {
public:
    NciNfccProxy();

    /**
     * @brief Initialize when turn on NFC
     * @return True if success, otherwise false.
     */
    bool Initialize() override;

    /**
     * @brief Deinitialize when turn off NFC
     * @return True if success, otherwise false.
     */
    bool Deinitialize() override;

    /**
     * @brief Start polling and listening
     * @param techMask bitmask of the technologies
     * @param enableReaderMode if enable tag polling
     * @param enableHostRouting if enable host routing
     * @param restart true if need restart, otherwise false.
     */
    void EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart) override;

    /**
     * @brief Stop polling and listening
     */
    void DisableDiscovery() override;

    /**
     * @brief Set the screen statue to nfc controller.
     * @param screenStateMask the bitmask of the screen state
     * @return True if success, otherwise false.
     */
    bool SetScreenStatus(uint8_t screenStateMask) override;

    /**
     * @brief Get Nci version supprted by nfc controller.
     * @return 0x20 if it's NCI2.0, otherwise 0x10 if it's NCI1.0.
     */
    int GetNciVersion() override;

    /**
     * @brief Abort the nfc controller if NCI timeout.
     */
    void Abort() override;

    /**
     * @brief Do factory reset for nfc controller.
     */
    void FactoryReset() override;

    /**
     * @brief Shutdown the device. Enable the nfc functionality if support power off case.
     */
    void Shutdown() override;

private:
    std::shared_ptr<INciNfccInterface> nfccInterface_;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif /* NCI_NFCC_PROXY_H */