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
#ifndef TAG_NATIVE_IMPL_H
#define TAG_NATIVE_IMPL_H
#include "tag_host.h"
#include "inci_tag_interface.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class TagNativeImpl final {
public:
    static TagNativeImpl& GetInstance();

    /**
     * @brief Set tag listener to receive tag status.
     * @param listener The listener to receive tag status.
     */
    void SetTagListener(std::weak_ptr<INciTagInterface::ITagListener> listener);

    /**
     * @brief Tag discovered, need to callback to nfc service.
     * @param tagDiscId The tag discovered id given from nci stack.
     * @param tagHost The TagHost instance created in TagNciAdapter when tag discorvered.
     */
    void OnTagDiscovered(uint32_t tagDiscId, std::shared_ptr<TagHost> tagHost);

    /**
     * @brief Tag lost, need to callback to nfc service.
     * @param tagDiscId The tag discovered id given from nci stack.
     */
    void OnTagLost(uint32_t tagDiscId);

    /**
     * @brief Get the TagHost instance by the tag discovered id.
     * @param tagDiscId The tag discovered id given from nci stack.
     */
    std::weak_ptr<TagHost> GetTag(uint32_t tagDiscId);

    /**
     * @brief Check can make the ndef to be read only.
     * @param ndefType The ndef type to check.
     * @return True if can make read only, otherwise false.
     */
    bool CanMakeReadOnly(uint32_t ndefType);

    /**
     * @brief Build the tech mask by all given technologies.
     * @param discTech The given technology list.
     * @return The technology mask.
     */
    uint16_t GetTechMaskFromTechList(const std::vector<uint32_t> &discTech);

    /**
     * @brief Get the max transceive length of ISO-DEP technology.
     * @return The max transceive length of ISO-DEP technology.
     */
    uint32_t GetIsoDepMaxTransceiveLength();

    /**
     * @brief Check if the nfc controller support extended APDU or not.
     * @param length The max isodep length to check.
     * @return True if the nfc controller support extended APDU, otherwise false.
     */
    bool IsExtendedLengthApduSupported(uint32_t length);
private:
    std::weak_ptr<INciTagInterface::ITagListener> tagListener_ {};
    std::map<uint32_t, std::shared_ptr<TagHost>> tagHostMap_ {};
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_NATIVE_IMPL_H
