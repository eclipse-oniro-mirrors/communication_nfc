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
#include "nci_tag_impl_vendor.h"
#include "nci_native_adapter.h"

namespace OHOS {
namespace NFC {
namespace NCI {
NciTagImplVendor::NciTagImplVendor()
{
    vendorTagImpl_ = NciNativeAdapter::GetInstance().GetNciTagInterface();
}

NciTagImplVendor::~NciTagImplVendor()
{
}

/**
 * @brief Set tag listener to receive tag status.
 * @param listener The listener to receive tag status.
 */
void NciTagImplVendor::SetTagListener(std::weak_ptr<ITagListener> listener)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->SetTagListener(listener);
    }
}

/**
 * @brief Get the discovered technologies found.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The technologies list.
 */
std::vector<int> NciTagImplVendor::GetTechList(uint32_t tagDiscId)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->GetTechList(tagDiscId);
    }
    return std::vector<int>();
}

/**
 * @brief Get the connected technology, the technology specific when call Connect(uint32_t technology).
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The connected technology.
 */
uint32_t NciTagImplVendor::GetConnectedTech(uint32_t tagDiscId)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->GetConnectedTech(tagDiscId);
    }
    return 0;
}

/**
 * @brief Get the extra data of all discovered technologies, Key and value.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The extra data of all discovered technologies.
 */
std::vector<AppExecFwk::PacMap> NciTagImplVendor::GetTechExtrasData(uint32_t tagDiscId)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->GetTechExtrasData(tagDiscId);
    }
    return std::vector<AppExecFwk::PacMap>();
}

/**
 * @brief Get the uid of discovered tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The uid of discovered tag, such as DD236DEB.
 */
std::string NciTagImplVendor::GetTagUid(uint32_t tagDiscId)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->GetTagUid(tagDiscId);
    }
    return std::string{};
}

/**
 * @brief Connect the tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param technology The technology of the tag using to connect.
 * @return True if success, otherwise false.
 */
bool NciTagImplVendor::Connect(uint32_t tagDiscId, uint32_t technology)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->Connect(tagDiscId, technology);
    }
    return false;
}

/**
 * @brief Disconnect the tag
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return True if success, otherwise false.
 */
bool NciTagImplVendor::Disconnect(uint32_t tagDiscId)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->Disconnect(tagDiscId);
    }
    return false;
}

/**
 * @brief Reconnect the tag
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return True if success, otherwise false.
 */
bool NciTagImplVendor::Reconnect(uint32_t tagDiscId)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->Reconnect(tagDiscId);
    }
    return false;
}

/**
 * @brief Send command to tag and receive response.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param command The command to send.
 * @param response The response from the tag.
 * @return The status code to transceive the command.
 */
int NciTagImplVendor::Transceive(uint32_t tagDiscId, const std::string& command, std::string& response)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->Transceive(tagDiscId, command, response);
    }
    return 0;
}

/**
 * @brief Read the NDEF tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The data read from NDEF tag.
 */
std::string NciTagImplVendor::ReadNdef(uint32_t tagDiscId)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->ReadNdef(tagDiscId);
    }
    return std::string{};
}

/**
 * @brief Find the NDEF tag technology from the NDEF tag data.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The data read from NDEF tag.
 */
std::string NciTagImplVendor::FindNdefTech(uint32_t tagDiscId)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->FindNdefTech(tagDiscId);
    }
    return std::string{};
}

/**
 * @brief Write command to NDEF tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param command The command to write to NDEF tag.
 * @return True if success, otherwise false.
 */
bool NciTagImplVendor::WriteNdef(uint32_t tagDiscId, std::string& command)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->WriteNdef(tagDiscId, command);
    }
    return false;
}

/**
 * @brief Format NDEF tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param key The key used to format the NDEF.
 * @return True if success, otherwise false.
 */
bool NciTagImplVendor::FormatNdef(uint32_t tagDiscId, const std::string& key)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->FormatNdef(tagDiscId, key);
    }
    return false;
}

/**
 * @brief Format NDEF tag.
 * @param key The key used to format the NDEF.
 * @return True if success, otherwise false.
 */
bool NciTagImplVendor::CanMakeReadOnly(uint32_t ndefType)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->CanMakeReadOnly(ndefType);
    }
    return false;
}

/**
 * @brief Set the NDEF to be read only.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return True if success, otherwise false.
 */
bool NciTagImplVendor::SetNdefReadOnly(uint32_t tagDiscId)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->SetNdefReadOnly(tagDiscId);
    }
    return false;
}

/**
 * @brief Detect the NDEF info, includes the max size and the mode.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param ndefInfo The output to save the detected result.
 * @return True if success, otherwise false.
 */
bool NciTagImplVendor::DetectNdefInfo(uint32_t tagDiscId, std::vector<int>& ndefInfo)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->DetectNdefInfo(tagDiscId, ndefInfo);
    }
    return false;
}

/**
 * @brief Check current tag is field on or not.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return True if current tag is field on, otherwise false.
 */
bool NciTagImplVendor::IsTagFieldOn(uint32_t tagDiscId)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->IsTagFieldOn(tagDiscId);
    }
    return false;
}

/**
 * @brief Start filed on checking for tag. If tag lost, callback to notify.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param delayedMs The delayed time to check the tag lost.
 */
void NciTagImplVendor::StartFieldOnChecking(uint32_t tagDiscId, uint32_t delayedMs)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->StartFieldOnChecking(tagDiscId, delayedMs);
    }
}

/**
 * @brief Config the timeout value to nfc controller when read or write tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param timeout The timeout value to config.
 * @param technology The technology to config.
 */
void NciTagImplVendor::SetTimeout(uint32_t tagDiscId, uint32_t timeout, uint32_t technology)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->SetTimeout(tagDiscId, timeout, technology);
    }
}

/**
 * @brief Get the max transceive length of ISO-DEP technology.
 * @return The max transceive length of ISO-DEP technology.
 */
uint32_t NciTagImplVendor::GetIsoDepMaxTransceiveLength()
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->GetIsoDepMaxTransceiveLength();
    }
    return 0;
}

/**
 * @brief Check if the nfc controller support extended APDU or not.
 * @return True if the nfc controller support extended APDU, otherwise false.
 */
bool NciTagImplVendor::IsExtendedLengthApduSupported()
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->IsExtendedLengthApduSupported();
    }
    return false;
}

/**
 * @brief Build the tech mask by all given technologies.
 * @param discTech The given technology list.
 * @return The technology mask.
 */
uint16_t NciTagImplVendor::GetTechMaskFromTechList(const std::vector<uint32_t> &discTech)
{
    if (vendorTagImpl_) {
        return vendorTagImpl_->GetTechMaskFromTechList(discTech);
    }
    return 0;
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
