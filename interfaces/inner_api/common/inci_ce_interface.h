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
#ifndef I_NCI_CE_INTERFACE_H
#define I_NCI_CE_INTERFACE_H
#include <string>

namespace OHOS {
namespace NFC {
namespace NCI {
class INciCeInterface {
public:
    class ICeHostListener {
    public:
        virtual ~ICeHostListener() {}

        /**
         * @brief The notification for field on.
         */
        virtual void FieldActivated() = 0;

        /**
         * @brief The notification for field off.
         */
        virtual void FieldDeactivated() = 0;
        /**
         * @brief deal with card emulation data
         * @note
         * @param  data: card emulation data
         */
        virtual void OnCardEmulationData(const std::vector<uint8_t> &data) = 0;
        /**
         * @brief  card emulation activate
         * @note
         */
        virtual void OnCardEmulationActivated() = 0;
        /**
         * @brief  card emulation deactivate
         * @note
         */
        virtual void OnCardEmulationDeactivated() = 0;
    };

    virtual ~INciCeInterface() = default;

    /**
     * @brief Set the listener to receive the card emulation notifications.
     * @param listener The listener to receive the card emulation notifications.
     */
    virtual void SetCeHostListener(std::weak_ptr<ICeHostListener> listener) = 0;

    /**
     * @brief compute the routing parameters based on the default payment app
     * and all installed app.
     * @param  defaultPaymentType: default payment type
     * @return True if success, otherwise false.
     */
    virtual bool ComputeRoutingParams(int defaultPaymentType) = 0;

    /**
     * @brief Commit the routing parameters to nfc controller.
     * @return True if success, otherwise false.
     */
    virtual bool CommitRouting() = 0;

    /**
     * @brief  send raw frame data
     * @param  hexCmdData the data to send
     * @return True if success, otherwise false.
     */
    virtual bool SendRawFrame(std::string &hexCmdData) = 0;

    /**
     * @brief  add aid routing
     * @param  aidStr: aid
     * @param  route: route dest
     * @param  aidInfo: prefix subset etc
     * @param  power: power state
     * @return True if success, otherwise false.
     */
    virtual bool AddAidRouting(const std::string &aidStr, int route, int aidInfo, int power) = 0;
    /**
     * @brief  clear aid table
     * @return True if success, otherwise false.
     */
    virtual bool ClearAidTable() = 0;

    /**
     * @brief get sim bundle name of the vendor
     * @return sim bundle name of the vendor
     */
    virtual std::string GetSimVendorBundleName() = 0;
};
} // namespace NCI
} // namespace NFC
} // namespace OHOS
#endif // I_NCI_CE_INTERFACE_H
