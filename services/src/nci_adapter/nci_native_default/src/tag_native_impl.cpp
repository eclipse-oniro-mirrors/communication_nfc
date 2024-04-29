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
#include "tag_native_impl.h"
#include "nfa_api.h"
#include "nfc_config.h"
#include "nfc_sdk_common.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace NCI {
/* The maximum length of a default IsoDep consists of:
 * CLA, INS, P1, P2, LC, LE + 255 payload bytes = 261 bytes
 */
constexpr uint32_t ISO_DEP_FRAME_MAX_LEN = 261;

constexpr uint32_t ISO_DEP_MAX_TRANSEIVE_LENGTH = 0xFEFF;

const static uint32_t MAX_NUM_TECH_LIST = 12;

TagNativeImpl& TagNativeImpl::GetInstance()
{
    static TagNativeImpl tagNativeImpl;
    return tagNativeImpl;
}

/**
 * @brief Set tag listener to receive tag status.
 * @param listener The listener to receive tag status.
 */
void TagNativeImpl::SetTagListener(std::weak_ptr<INciTagInterface::ITagListener> listener)
{
    tagListener_ = listener;
}

/**
 * @brief Get the TagHost instance by the tag discovered id.
 * @param tagDiscId The tag discovered id given from nci stack.
 */
std::weak_ptr<TagHost> TagNativeImpl::GetTag(uint32_t tagDiscId)
{
    std::map<uint32_t, std::shared_ptr<TagHost>>::iterator iter = tagHostMap_.find(tagDiscId);
    if (iter == tagHostMap_.end()) {
        return std::shared_ptr<TagHost>();
    }
    return iter->second;
}

/**
 * @brief Tag discovered, need to callback to nfc service.
 * @param tagDiscId The tag discovered id given from nci stack.
 * @param tagHost The TagHost instance created in TagNciAdapter when tag discorvered.
 */
void TagNativeImpl::OnTagDiscovered(uint32_t tagDiscId, std::shared_ptr<TagHost> tagHost)
{
    if (tagListener_.expired()) {
        return;
    }
    tagHostMap_.insert(make_pair(tagDiscId, tagHost));
    tagListener_.lock()->OnTagDiscovered(tagDiscId);
}

/**
 * @brief Tag lost, need to callback to nfc service.
 * @param tagDiscId The tag discovered id given from nci stack.
 */
void TagNativeImpl::OnTagLost(uint32_t tagDiscId)
{
    if (tagListener_.expired()) {
        return;
    }
    tagHostMap_.erase(tagDiscId);
    tagListener_.lock()->OnTagLost(tagDiscId);
}

/**
 * @brief Check can make the ndef to be read only.
 * @param ndefType The ndef type to check.
 * @return True if can make read only, otherwise false.
 */
bool TagNativeImpl::CanMakeReadOnly(uint32_t ndefType)
{
    return ndefType == KITS::EmNfcForumType::NFC_FORUM_TYPE_1 ||
        ndefType == KITS::EmNfcForumType::NFC_FORUM_TYPE_2;
}

/**
 * @brief Build the tech mask by all given technologies.
 * @param discTech The given technology list.
 * @return The technology mask.
 */
uint16_t TagNativeImpl::GetTechMaskFromTechList(const std::vector<uint32_t> &discTech)
{
    uint16_t techMask = 0;
    size_t discTechLen = discTech.size();
    if (discTechLen > MAX_NUM_TECH_LIST) {
        ErrorLog("GetTechMaskFromTechList: invalid discTech length");
        return techMask;
    }
    for (uint16_t i = 0; i < discTechLen ; i++) {
        switch (discTech[i]) {
            case static_cast<int32_t>(KITS::TagTechnology::NFC_A_TECH):
                techMask |= NFA_TECHNOLOGY_MASK_A;
                break;
            case static_cast<int32_t>(KITS::TagTechnology::NFC_B_TECH):
                techMask |= NFA_TECHNOLOGY_MASK_B;
                break;
            case static_cast<int32_t>(KITS::TagTechnology::NFC_F_TECH):
                techMask |= NFA_TECHNOLOGY_MASK_F;
                break;
            case static_cast<int32_t>(KITS::TagTechnology::NFC_V_TECH):
                techMask |= NFA_TECHNOLOGY_MASK_V;
                break;
            default:
                break;
        }
    }
    return techMask;
}

/**
 * @brief Get the max transceive length of ISO-DEP technology.
 * @return The max transceive length of ISO-DEP technology.
 */
uint32_t TagNativeImpl::GetIsoDepMaxTransceiveLength()
{
    if (NfcConfig::hasKey(NAME_ISO_DEP_MAX_TRANSCEIVE)) {
        return NfcConfig::getUnsigned(NAME_ISO_DEP_MAX_TRANSCEIVE);
    } else {
        return ISO_DEP_MAX_TRANSEIVE_LENGTH;
    }
}

/**
 * @brief Check if the nfc controller support extended APDU or not.
 * @param length The max isodep length to check.
 * @return True if the nfc controller support extended APDU, otherwise false.
 */
bool TagNativeImpl::IsExtendedLengthApduSupported(uint32_t length)
{
    return length > ISO_DEP_FRAME_MAX_LEN;
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
