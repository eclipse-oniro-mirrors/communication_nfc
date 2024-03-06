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
#ifndef NCI_CE_PROXY_H
#define NCI_CE_PROXY_H
#include "inci_ce_interface.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class NciCeProxy final : public INciCeInterface {
public:
    NciCeProxy();

    /**
     * @brief Set the listener to receive the card emulation notifications.
     * @param listener The listener to receive the card emulation notifications.
     */
    void SetCeHostListener(std::weak_ptr<ICeHostListener> listener) override;

    /**
     * @brief compute the routing parameters based on the default payment app
     * and all installed app.
     * @return True if success, otherwise false.
     */
    bool ComputeRoutingParams(int defaultPaymentType) override;

    /**
     * @brief Commit the routing parameters to nfc controller.
     * @return True if success, otherwise false.
     */
    bool CommitRouting() override;

    /**
     * @brief  send raw frame data
     * @param  hexCmdData the data to send
     * @return True if success, otherwise false.
     */
    bool SendRawFrame(std::string &hexCmdData) override;
    /**
     * @brief  add aid routing
     * @param  aidStr: aid
     * @param  route: route dest
     * @param  aidInfo: prefix subset etc
     * @param  power: power state
     * @return True if success, otherwise false.
     */
    bool AddAidRouting(const std::string &aidStr, int route, int aidInfo,
                       int power) override;
    /**
     * @brief  clear aid table
     * @return True if success, otherwise false.
     */
    bool ClearAidTable() override;

    /**
     * @brief get sim bundle name of the vendor
     * @return sim bundle name of the vendor
     */
    std::string GetSimVendorBundleName() override;

private:
    std::shared_ptr<INciCeInterface> nciCeInterface_;
};
} // namespace NCI
} // namespace NFC
} // namespace OHOS
#endif /* NCI_CE_PROXY_H */