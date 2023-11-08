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
#include "nci_tag_impl_default.h"
#include "native_impl_default/tag_native_impl.h"

namespace OHOS {
namespace NFC {
namespace NCI {
NciTagImplDefault::NciTagImplDefault()
{
}

NciTagImplDefault::~NciTagImplDefault()
{
}

/**
 * @brief Set tag listener to receive tag status.
 * @param listener The listener to receive tag status.
 */
void NciTagImplDefault::SetTagListener(std::weak_ptr<ITagListener> listener)
{
    TagNativeImpl::GetInstance().SetTagListener(listener);
}

/**
 * @brief Get the discovered technologies found.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The technologies list.
 */
std::vector<int> NciTagImplDefault::GetTechList(uint32_t tagDiscId)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return std::vector<int>();
    }
    return tag.lock()->GetTechList();
}

/**
 * @brief Get the connected technology, the technology specific when call Connect(uint32_t technology).
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The connected technology.
 */
uint32_t NciTagImplDefault::GetConnectedTech(uint32_t tagDiscId)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return 0;
    }
    return tag.lock()->GetConnectedTech();
}

/**
 * @brief Get the extra data of all discovered technologies, Key and value.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The extra data of all discovered technologies.
 */
std::vector<AppExecFwk::PacMap> NciTagImplDefault::GetTechExtrasData(uint32_t tagDiscId)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return std::vector<AppExecFwk::PacMap>();
    }
    return tag.lock()->GetTechExtrasData();
}

/**
 * @brief Get the uid of discovered tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The uid of discovered tag, such as DD236DEB.
 */
std::string NciTagImplDefault::GetTagUid(uint32_t tagDiscId)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return std::string{};
    }
    return tag.lock()->GetTagUid();
}

/**
 * @brief Connect the tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param technology The technology of the tag using to connect.
 * @return True if success, otherwise false.
 */
bool NciTagImplDefault::Connect(uint32_t tagDiscId, uint32_t technology)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return false;
    }
    return tag.lock()->Connect(technology);
}

/**
 * @brief Disconnect the tag
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return True if success, otherwise false.
 */
bool NciTagImplDefault::Disconnect(uint32_t tagDiscId)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return false;
    }
    return tag.lock()->Disconnect();
}

/**
 * @brief Reconnect the tag
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return True if success, otherwise false.
 */
bool NciTagImplDefault::Reconnect(uint32_t tagDiscId)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return false;
    }
    return tag.lock()->Reconnect();
}

/**
 * @brief Send command to tag and receive response.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param command The command to send.
 * @param response The response from the tag.
 * @return The status code to transceive the command.
 */
int NciTagImplDefault::Transceive(uint32_t tagDiscId, const std::string& command, std::string& response)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return 0;
    }
    return tag.lock()->Transceive(command, response);
}

/**
 * @brief Read the NDEF tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The data read from NDEF tag.
 */
std::string NciTagImplDefault::ReadNdef(uint32_t tagDiscId)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return std::string{};
    }
    return tag.lock()->ReadNdef();
}

/**
 * @brief Find the NDEF tag technology from the NDEF tag data.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return The data read from NDEF tag.
 */
std::string NciTagImplDefault::FindNdefTech(uint32_t tagDiscId)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return std::string{};
    }
    return tag.lock()->FindNdefTech();
}

/**
 * @brief Write command to NDEF tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param command The command to write to NDEF tag.
 * @return True if success, otherwise false.
 */
bool NciTagImplDefault::WriteNdef(uint32_t tagDiscId, std::string& command)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return false;
    }
    return tag.lock()->WriteNdef(command);
}

/**
 * @brief Format NDEF tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param key The key used to format the NDEF.
 * @return True if success, otherwise false.
 */
bool NciTagImplDefault::FormatNdef(uint32_t tagDiscId, const std::string& key)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return false;
    }
    return tag.lock()->FormatNdef(key);
}

/**
 * @brief Check can make the ndef to be read only.
 * @param ndefType The ndef type to check.
 * @return True if can make read only, otherwise false.
 */
bool NciTagImplDefault::CanMakeReadOnly(uint32_t ndefType)
{
    return TagNativeImpl::GetInstance().CanMakeReadOnly(ndefType);
}

/**
 * @brief Set the NDEF to be read only.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return True if success, otherwise false.
 */
bool NciTagImplDefault::SetNdefReadOnly(uint32_t tagDiscId)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return false;
    }
    return tag.lock()->SetNdefReadOnly();
}

/**
 * @brief Detect the NDEF info, includes the max size and the mode.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param ndefInfo The output to save the detected result.
 * @return True if success, otherwise false.
 */
bool NciTagImplDefault::DetectNdefInfo(uint32_t tagDiscId, std::vector<int>& ndefInfo)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return false;
    }
    return tag.lock()->DetectNdefInfo(ndefInfo);
}

/**
 * @brief Check current tag is field on or not.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @return True if current tag is field on, otherwise false.
 */
bool NciTagImplDefault::IsTagFieldOn(uint32_t tagDiscId)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return false;
    }
    return tag.lock()->IsTagFieldOn();
}

/**
 * @brief Start filed on checking for tag. If tag lost, callback to notify.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param delayedMs The delayed time to check the tag lost.
 */
void NciTagImplDefault::StartFieldOnChecking(uint32_t tagDiscId, uint32_t delayedMs)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return;
    }
    return tag.lock()->StartFieldOnChecking(delayedMs);
}

/**
 * @brief Config the timeout value to nfc controller when read or write tag.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param timeout The timeout value to config.
 * @param technology The technology to config.
 */
void NciTagImplDefault::SetTimeout(uint32_t tagDiscId, uint32_t timeout, uint32_t technology)
{
    std::weak_ptr<TagHost> tag = TagNativeImpl::GetInstance().GetTag(tagDiscId);
    if (tag.expired()) {
        return;
    }
    return tag.lock()->SetTimeout(timeout, technology);
}

/**
 * @brief Get the max transceive length of ISO-DEP technology.
 * @return The max transceive length of ISO-DEP technology.
 */
uint32_t NciTagImplDefault::GetIsoDepMaxTransceiveLength()
{
    return TagNativeImpl::GetInstance().GetIsoDepMaxTransceiveLength();
}

/**
 * @brief Check if the nfc controller support extended APDU or not.
 * @return True if the nfc controller support extended APDU, otherwise false.
 */
bool NciTagImplDefault::IsExtendedLengthApduSupported()
{
    uint32_t length = TagNativeImpl::GetInstance().GetIsoDepMaxTransceiveLength();
    return TagNativeImpl::GetInstance().IsExtendedLengthApduSupported(length);
}

/**
 * @brief Build the tech mask by all given technologies.
 * @param discTech The given technology list.
 * @return The technology mask.
 */
uint16_t NciTagImplDefault::GetTechMaskFromTechList(const std::vector<uint32_t> &discTech)
{
    return TagNativeImpl::GetInstance().GetTechMaskFromTechList(discTech);
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
