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
#ifndef NCI_TAG_PROXY_H
#define NCI_TAG_PROXY_H
#include "inci_tag_interface.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class NciTagProxy final : public INciTagInterface {
public:
    NciTagProxy();

    /**
     * @brief Set tag listener to receive tag status.
     * @param listener The listener to receive tag status.
     */
    void SetTagListener(std::weak_ptr<ITagListener> listener) override;

    /**
     * @brief Get the discovered technologies found.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return The technologies list.
     */
    std::vector<int> GetTechList(uint32_t tagDiscId) override;

    /**
     * @brief Get the connected technology, the technology specific when call Connect(uint32_t technology).
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return The connected technology.
     */
    uint32_t GetConnectedTech(uint32_t tagDiscId) override;

    /**
     * @brief Get the extra data of all discovered technologies, Key and value.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return The extra data of all discovered technologies.
     */
    std::vector<AppExecFwk::PacMap> GetTechExtrasData(uint32_t tagDiscId) override;

    /**
     * @brief Get the uid of discovered tag.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return The uid of discovered tag, such as DD236DEB.
     */
    std::string GetTagUid(uint32_t tagDiscId) override;

    /**
     * @brief Connect the tag.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param technology The technology of the tag using to connect.
     * @return True if success, otherwise false.
     */
    bool Connect(uint32_t tagDiscId, uint32_t technology) override;

    /**
     * @brief Disconnect the tag
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return True if success, otherwise false.
     */
    bool Disconnect(uint32_t tagDiscId) override;

    /**
     * @brief Reconnect the tag
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return True if success, otherwise false.
     */
    bool Reconnect(uint32_t tagDiscId) override;

    /**
     * @brief Send command to tag and receive response.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param command The command to send.
     * @param response The response from the tag.
     * @return The status code to transceive the command.
     */
    int Transceive(uint32_t tagDiscId, const std::string& command, std::string& response) override;

    /**
     * @brief Read the NDEF tag.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return The data read from NDEF tag.
     */
    std::string ReadNdef(uint32_t tagDiscId) override;

    /**
     * @brief Find the NDEF tag technology from the NDEF tag data.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return The data read from NDEF tag.
     */
    std::string FindNdefTech(uint32_t tagDiscId) override;

    /**
     * @brief Write command to NDEF tag.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param command The command to write to NDEF tag.
     * @return True if success, otherwise false.
     */
    bool WriteNdef(uint32_t tagDiscId, std::string& command) override;

    /**
     * @brief Format NDEF tag.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param key The key used to format the NDEF.
     * @return True if success, otherwise false.
     */
    bool FormatNdef(uint32_t tagDiscId, const std::string& key) override;

    /**
     * @brief Format NDEF tag.
     * @param key The key used to format the NDEF.
     * @return True if success, otherwise false.
     */
    bool CanMakeReadOnly(uint32_t ndefType) override;

    /**
     * @brief Set the NDEF to be read only.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return True if success, otherwise false.
     */
    bool SetNdefReadOnly(uint32_t tagDiscId) override;

    /**
     * @brief Detect the NDEF info, includes the max size and the mode.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param ndefInfo The output to save the detected result.
     * @return True if success, otherwise false.
     */
    bool DetectNdefInfo(uint32_t tagDiscId, std::vector<int>& ndefInfo) override;

    /**
     * @brief Check current tag is field on or not.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return True if current tag is field on, otherwise false.
     */
    bool IsTagFieldOn(uint32_t tagDiscId) override;

    /**
     * @brief Start field on checking for tag. If tag lost, callback to notify.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param delayedMs The delayed time to check the tag lost.
     */
    void StartFieldOnChecking(uint32_t tagDiscId, uint32_t delayedMs) override;

    /**
     * @brief Stop field on checking for tag if tag is connected.
     */
    void StopFieldChecking() override;

    /**
     * @brief Config the timeout value to nfc controller when read or write tag.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param timeout The timeout value to config.
     * @param technology The technology to config.
     */
    void SetTimeout(uint32_t tagDiscId, uint32_t timeout, uint32_t technology) override;

    /**
     * @brief Get the timeout value to nfc controller when read or write tag.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param timeout The timeout value to config.
     * @param technology The technology to config.
     */
    void GetTimeout(uint32_t tagDiscId, uint32_t &timeout, uint32_t technology) override;

    /**
     * @brief Reset the timeout value to nfc controller when read or write tag.
     * @param tagDiscId The tag discovered id given from nci stack.
     */
    void ResetTimeout(uint32_t tagDiscId) override;

    /**
     * @brief Get the max transceive length of ISO-DEP technology.
     * @return The max transceive length of ISO-DEP technology.
     */
    uint32_t GetIsoDepMaxTransceiveLength() override;

    /**
     * @brief Check if the nfc controller support extended APDU or not.
     * @return True if the nfc controller support extended APDU, otherwise false.
     */
    bool IsExtendedLengthApduSupported() override;

    /**
     * @brief Build the tech mask by all given technologies.
     * @param discTech The given technology list.
     * @return The technology mask.
     */
    uint16_t GetTechMaskFromTechList(const std::vector<uint32_t> &discTech) override;

    /**
     * @brief Get browser bundle name of the vendor.
     * @return Browser bundle name of the vendor.
     */
    std::string GetVendorBrowserBundleName() override;
private:
    std::shared_ptr<INciTagInterface> nciTagInterface_;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // NCI_TAG_PROXY_H