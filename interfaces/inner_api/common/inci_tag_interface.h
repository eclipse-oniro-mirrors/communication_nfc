/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#ifndef I_NCI_TAG_INTERFACE_H
#define I_NCI_TAG_INTERFACE_H
#include <string>
#include <vector>
#include "pac_map.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class INciTagInterface {
public:
    class ITagListener {
    public:
        virtual void OnTagDiscovered(uint32_t tagDiscId) = 0;
        virtual void OnTagLost(uint32_t tagDiscId) = 0;
    };

    virtual ~INciTagInterface() = default;

    /**
     * @brief Set tag listener to receive tag status.
     * @param listener The listener to receive tag status.
     */
    virtual void SetTagListener(std::weak_ptr<ITagListener> listener) = 0;

    /**
     * @brief Get the discovered technologies found.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return The technologies list.
     */
    virtual std::vector<int> GetTechList(uint32_t tagDiscId) = 0;

    /**
     * @brief Get the connected technology, the technology specific when call Connect(uint32_t technology).
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return The connected technology.
     */
    virtual uint32_t GetConnectedTech(uint32_t tagDiscId) = 0;

    /**
     * @brief Get the extra data of all discovered technologies, Key and value.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return The extra data of all discovered technologies.
     */
    virtual std::vector<AppExecFwk::PacMap> GetTechExtrasData(uint32_t tagDiscId) = 0;

    /**
     * @brief Get the uid of discovered tag.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return The uid of discovered tag, such as DD236DEB.
     */
    virtual std::string GetTagUid(uint32_t tagDiscId) = 0;

    /**
     * @brief Connect the tag.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param technology The technology of the tag using to connect.
     * @return True if success, otherwise false.
     */
    virtual bool Connect(uint32_t tagDiscId, uint32_t technology) = 0;

    /**
     * @brief Disconnect the tag
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return True if success, otherwise false.
     */
    virtual bool Disconnect(uint32_t tagDiscId) = 0;

    /**
     * @brief Reconnect the tag
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return True if success, otherwise false.
     */
    virtual bool Reconnect(uint32_t tagDiscId) = 0;

    /**
     * @brief Send command to tag and receive response.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param command The command to send.
     * @param response The response from the tag.
     * @return The status code to transceive the command.
     */
    virtual int Transceive(uint32_t tagDiscId, const std::string& command, std::string& response) = 0;

    /**
     * @brief Read the NDEF tag.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return The data read from NDEF tag.
     */
    virtual std::string ReadNdef(uint32_t tagDiscId) = 0;

    /**
     * @brief Find the NDEF tag technology from the NDEF tag data.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return The data read from NDEF tag.
     */
    virtual std::string FindNdefTech(uint32_t tagDiscId) = 0;

    /**
     * @brief Write command to NDEF tag.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param command The command to write to NDEF tag.
     * @return True if success, otherwise false.
     */
    virtual bool WriteNdef(uint32_t tagDiscId, std::string& command) = 0;

    /**
     * @brief Format NDEF tag.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param key The key used to format the NDEF.
     * @return True if success, otherwise false.
     */
    virtual bool FormatNdef(uint32_t tagDiscId, const std::string& key) = 0;

    /**
     * @brief Format NDEF tag.
     * @param key The key used to format the NDEF.
     * @return True if success, otherwise false.
     */
    virtual bool CanMakeReadOnly(uint32_t ndefType) = 0;

    /**
     * @brief Set the NDEF to be read only.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return True if success, otherwise false.
     */
    virtual bool SetNdefReadOnly(uint32_t tagDiscId) = 0;

    /**
     * @brief Detect the NDEF info, includes the max size and the mode.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param ndefInfo The output to save the detected result.
     * @return True if success, otherwise false.
     */
    virtual bool DetectNdefInfo(uint32_t tagDiscId, std::vector<int>& ndefInfo) = 0;

    /**
     * @brief Check current tag is field on or not.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @return True if current tag is field on, otherwise false.
     */
    virtual bool IsTagFieldOn(uint32_t tagDiscId) = 0;

    /**
     * @brief Start field on checking for tag. If tag lost, callback to notify.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param delayedMs The delayed time to check the tag lost.
     */
    virtual void StartFieldOnChecking(uint32_t tagDiscId, uint32_t delayedMs) = 0;

    /**
     * @brief Stop field on checking for tag if tag is connected.
     */
    virtual void StopFieldChecking() = 0;

    /**
     * @brief Config the timeout value to nfc controller when read or write tag.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param timeout The timeout value to config.
     * @param technology The technology to config.
     */
    virtual void SetTimeout(uint32_t tagDiscId, uint32_t timeout, uint32_t technology) = 0;

    /**
     * @brief Get the Timeout value of tag operations
     * @param tagDiscId the rf disc id of tag
     * @param technology the tag technology
     * @param timeout the output argument to read the timeout.
     */
    virtual void GetTimeout(uint32_t tagDiscId, uint32_t &timeout, uint32_t technology) = 0;

    /**
     * @brief Reset the Timeout value of tag operations
     *
     * @param tagDiscId the rf disc id of tag
     */
    virtual void ResetTimeout(uint32_t tagDiscId) = 0;

    /**
     * @brief Get the max transceive length of ISO-DEP technology.
     * @return The max transceive length of ISO-DEP technology.
     */
    virtual uint32_t GetIsoDepMaxTransceiveLength() = 0;

    /**
     * @brief Check if the nfc controller support extended APDU or not.
     * @return True if the nfc controller support extended APDU, otherwise false.
     */
    virtual bool IsExtendedLengthApduSupported() = 0;

    /**
     * @brief Build the tech mask by all given technologies.
     * @param discTech The given technology list.
     * @return The technology mask.
     */
    virtual uint16_t GetTechMaskFromTechList(const std::vector<uint32_t> &discTech) = 0;

    /**
     * @brief Get browser bundle name of the vendor.
     * @return Browser bundle name of the vendor.
     */
    virtual std::string GetVendorBrowserBundleName() = 0;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif // I_NCI_TAG_INTERFACE_H
