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
#include "nci_tag_proxy.h"
#include "nci_native_selector.h"

namespace OHOS {
namespace NFC {
namespace NCI {
NciTagProxy::NciTagProxy()
{
    nciTagInterface_ = NciNativeSelector::GetInstance().GetNciTagInterface();
}


/**
 * @brief Set tag listener to receive tag status.
 * @param listener The listener to receive tag status.
 */
void NciTagProxy::SetTagListener(std::weak_ptr<ITagListener> listener)
{
    if (nciTagInterface_) {
        return nciTagInterface_->SetTagListener(listener);
    }
}

/**
 * @brief Get the discovered technologies found.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The technologies list.
 */
std::vector<int> NciTagProxy::GetTechList(uint32_t tagDiscId)
{
    if (nciTagInterface_) {
        return nciTagInterface_->GetTechList(tagDiscId);
    }
    return std::vector<int>();
}

/**
 * @brief Get the connected technology, the technology specific when call Connect(uint32_t technology).
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The connected technology.
 */
uint32_t NciTagProxy::GetConnectedTech(uint32_t tagDiscId)
{
    if (nciTagInterface_) {
        return nciTagInterface_->GetConnectedTech(tagDiscId);
    }
    return 0;
}

/**
 * @brief Get the extra data of all discovered technologies, Key and value.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The extra data of all discovered technologies.
 */
std::vector<AppExecFwk::PacMap> NciTagProxy::GetTechExtrasData(uint32_t tagDiscId)
{
    if (nciTagInterface_) {
        return nciTagInterface_->GetTechExtrasData(tagDiscId);
    }
    return std::vector<AppExecFwk::PacMap>();
}

/**
 * @brief Get the uid of discovered tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The uid of discovered tag, such as DD236DEB.
 */
std::string NciTagProxy::GetTagUid(uint32_t tagDiscId)
{
    if (nciTagInterface_) {
        return nciTagInterface_->GetTagUid(tagDiscId);
    }
    return {};
}

/**
 * @brief Connect the tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param technology The technology of the tag using to connect.
 * @return True if success, otherwise false.
 */
bool NciTagProxy::Connect(uint32_t tagDiscId, uint32_t technology)
{
    if (nciTagInterface_) {
        return nciTagInterface_->Connect(tagDiscId, technology);
    }
    return false;
}

/**
 * @brief Disconnect the tag
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return True if success, otherwise false.
 */
bool NciTagProxy::Disconnect(uint32_t tagDiscId)
{
    if (nciTagInterface_) {
        return nciTagInterface_->Disconnect(tagDiscId);
    }
    return false;
}

/**
 * @brief Reconnect the tag
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return True if success, otherwise false.
 */
bool NciTagProxy::Reconnect(uint32_t tagDiscId)
{
    if (nciTagInterface_) {
        return nciTagInterface_->Reconnect(tagDiscId);
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
int NciTagProxy::Transceive(uint32_t tagDiscId, const std::string& command, std::string& response)
{
    if (nciTagInterface_) {
        return nciTagInterface_->Transceive(tagDiscId, command, response);
    }
    return 0;
}

/**
 * @brief Read the NDEF tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The data read from NDEF tag.
 */
std::string NciTagProxy::ReadNdef(uint32_t tagDiscId)
{
    if (nciTagInterface_) {
        return nciTagInterface_->ReadNdef(tagDiscId);
    }
    return {};
}

/**
 * @brief Find the NDEF tag technology from the NDEF tag data.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The data read from NDEF tag.
 */
std::string NciTagProxy::FindNdefTech(uint32_t tagDiscId)
{
    if (nciTagInterface_) {
        return nciTagInterface_->FindNdefTech(tagDiscId);
    }
    return {};
}

/**
 * @brief Write command to NDEF tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param command The command to write to NDEF tag.
 * @return True if success, otherwise false.
 */
bool NciTagProxy::WriteNdef(uint32_t tagDiscId, std::string& command)
{
    if (nciTagInterface_) {
        return nciTagInterface_->WriteNdef(tagDiscId, command);
    }
    return false;
}

/**
 * @brief Format NDEF tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param key The key used to format the NDEF.
 * @return True if success, otherwise false.
 */
bool NciTagProxy::FormatNdef(uint32_t tagDiscId, const std::string& key)
{
    if (nciTagInterface_) {
        return nciTagInterface_->FormatNdef(tagDiscId, key);
    }
    return false;
}

/**
 * @brief Format NDEF tag.
 * @param key The key used to format the NDEF.
 * @return True if success, otherwise false.
 */
bool NciTagProxy::CanMakeReadOnly(uint32_t ndefType)
{
    if (nciTagInterface_) {
        return nciTagInterface_->CanMakeReadOnly(ndefType);
    }
    return true;
}

/**
 * @brief Set the NDEF to be read only.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return True if success, otherwise false.
 */
bool NciTagProxy::SetNdefReadOnly(uint32_t tagDiscId)
{
    if (nciTagInterface_) {
        return nciTagInterface_->SetNdefReadOnly(tagDiscId);
    }
    return false;
}

/**
 * @brief Detect the NDEF info, includes the max size and the mode.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param ndefInfo The output to save the detected result.
 * @return True if success, otherwise false.
 */
bool NciTagProxy::DetectNdefInfo(uint32_t tagDiscId, std::vector<int>& ndefInfo)
{
    if (nciTagInterface_) {
        return nciTagInterface_->DetectNdefInfo(tagDiscId, ndefInfo);
    }
    return false;
}

/**
 * @brief Check current tag is field on or not.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return True if current tag is field on, otherwise false.
 */
bool NciTagProxy::IsTagFieldOn(uint32_t tagDiscId)
{
    if (nciTagInterface_) {
        return nciTagInterface_->IsTagFieldOn(tagDiscId);
    }
    return false;
}

/**
 * @brief Start field on checking for tag. If tag lost, callback to notify.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param callback The callback to notify.
 * @param delayedMs The delayed time to check the tag lost.
 */
void NciTagProxy::StartFieldOnChecking(uint32_t tagDiscId, uint32_t delayedMs)
{
    if (nciTagInterface_) {
        return nciTagInterface_->StartFieldOnChecking(tagDiscId, delayedMs);
    }
}

/**
 * @brief Stop field on checking for tag if tag is connected.
 */
void NciTagProxy::StopFieldChecking()
{
    if (nciTagInterface_) {
        return nciTagInterface_->StopFieldChecking();
    }
}

/**
 * @brief Config the timeout value to nfc controller when read or write tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param timeout The timeout value to config.
 * @param technology The technology to config.
 */
void NciTagProxy::SetTimeout(uint32_t tagDiscId, uint32_t timeout, uint32_t technology)
{
    if (nciTagInterface_) {
        return nciTagInterface_->SetTimeout(tagDiscId, timeout, technology);
    }
}

/**
 * @brief Get the timeout value to nfc controller when read or write tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param timeout The timeout value to config.
 * @param technology The technology to config.
 */
void NciTagProxy::GetTimeout(uint32_t tagDiscId, uint32_t &timeout, uint32_t technology)
{
    if (nciTagInterface_) {
        return nciTagInterface_->GetTimeout(tagDiscId, timeout, technology);
    }
}

/**
 * @brief Reset the timeout value to nfc controller when read or write tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 */
void NciTagProxy::ResetTimeout(uint32_t tagDiscId)
{
    if (nciTagInterface_) {
        return nciTagInterface_->ResetTimeout(tagDiscId);
    }
}

/**
 * @brief Get the max transceive length of ISO-DEP technology.
 * @return The max transceive length of ISO-DEP technology.
 */
uint32_t NciTagProxy::GetIsoDepMaxTransceiveLength()
{
    if (nciTagInterface_) {
        return nciTagInterface_->GetIsoDepMaxTransceiveLength();
    }
    return 0;
}

/**
 * @brief Check if the nfc controller support extended APDU or not.
 * @return True if the nfc controller support extended APDU, otherwise false.
 */
bool NciTagProxy::IsExtendedLengthApduSupported()
{
    if (nciTagInterface_) {
        return nciTagInterface_->IsExtendedLengthApduSupported();
    }
    return true;
}

/**
 * @brief Build the tech mask by all given technologies.
 * @param discTech The given technology list.
 * @return The technology mask.
 */
uint16_t NciTagProxy::GetTechMaskFromTechList(const std::vector<uint32_t> &discTech)
{
    if (nciTagInterface_) {
        return nciTagInterface_->GetTechMaskFromTechList(discTech);
    }
    return 0;
}

/**
 * @brief Get browser bundle name of the vendor.
 * @return Browser bundle name of the vendor.
 */
std::string NciTagProxy::GetVendorBrowserBundleName()
{
    if (nciTagInterface_) {
        return nciTagInterface_->GetVendorBrowserBundleName();
    }
    return "";
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS