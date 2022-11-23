/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ndeftag_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "ndef_message.h"
#include "ndef_tag.h"
#include "nfca_tag.h"
#include "nfc_sdk_common.h"
#include "taginfo.h"
#include "tag_session_proxy.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;
    using namespace OHOS::NFC::TAG;

    constexpr const auto FUZZER_THRESHOLD = 4;
    constexpr const auto TEST_UID = "0102";
    constexpr const auto TEST_DISC_ID = 1;
    constexpr const auto TEST_NDEF_FORUM_TYPE = NdefTag::EmNfcForumType::NFC_FORUM_TYPE_1;
    constexpr const auto TEST_NDEF_TAG_MODE = NdefTag::EmNdefTagMode::MODE_READ_ONLY;
    constexpr const auto TEST_NDEF_MSG = "ndef";
    constexpr const auto TEST_NDEF_TAG_LENGTH = 2;

    uint32_t ConvertToUint32(const uint8_t* ptr)
    {
        if (ptr == nullptr) {
            return 0;
        }

        // Shift the 0th number to the left by 24 bits, shift the 1st number to the left by 16 bits,
        // shift the 2nd number to the left by 8 bits, and not shift the 3rd number to the left
        return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
    }

    std::shared_ptr<TagInfo> FuzzGetTagInfo()
    {
        std::vector<int> tagTechList;

        // NFC_NDEF_TECH must put at index 0, because defined TEST_NDEF_INDEX = 0;
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_NDEF_TECH));

        std::vector<AppExecFwk::PacMap> tagTechExtras;
        AppExecFwk::PacMap ndefExtrasData;
        ndefExtrasData.PutIntValue(TagInfo::NDEF_FORUM_TYPE, TEST_NDEF_FORUM_TYPE);
        ndefExtrasData.PutIntValue(TagInfo::NDEF_TAG_MODE, TEST_NDEF_TAG_MODE);
        ndefExtrasData.PutIntValue(TagInfo::NDEF_TAG_LENGTH, TEST_NDEF_TAG_LENGTH);
        ndefExtrasData.PutStringValue(TagInfo::NDEF_MSG, TEST_NDEF_MSG);
        tagTechExtras.push_back(ndefExtrasData);

        std::string tagUid = TEST_UID;
        int tagRfDiscId = TEST_DISC_ID;
        std::shared_ptr<TagInfo> tagInfo = std::make_shared<TagInfo>(tagTechList,
                                                                     tagTechExtras,
                                                                     tagUid,
                                                                     tagRfDiscId,
                                                                     nullptr);
        return tagInfo;
    }

    void FuzzGetTag(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagInfo> tagInfo = FuzzGetTagInfo();
        if (tagInfo == nullptr) {
            std::cout << "tagInfo is nullptr." << std::endl;
            return;
        }
        NdefTag::GetTag(tagInfo);
    }

    void FuzzIsEnableReadOnly(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagInfo> tagInfo = FuzzGetTagInfo();
        if (tagInfo == nullptr) {
            std::cout << "tagInfo is nullptr." << std::endl;
            return;
        }
        std::shared_ptr<NdefTag> ndefTag = NdefTag::GetTag(tagInfo);
        bool canSetReadOnly = (static_cast<int>(data[0]) % 2) == 1;
        ndefTag->IsEnableReadOnly(canSetReadOnly);
    }
    
    void FuzzGetNdefTagTypeString(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagInfo> tagInfo = FuzzGetTagInfo();
        if (tagInfo == nullptr) {
            std::cout << "tagInfo is nullptr." << std::endl;
            return;
        }
        std::shared_ptr<NdefTag> ndefTag = NdefTag::GetTag(tagInfo);
        NdefTag::EmNfcForumType emNfcForumType = static_cast<NdefTag::EmNfcForumType>(ConvertToUint32(data));
        ndefTag->GetNdefTagTypeString(emNfcForumType);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }
    /* Run your code on data */
    OHOS::FuzzGetTag(data, size);
    OHOS::FuzzIsEnableReadOnly(data, size);
    OHOS::FuzzGetNdefTagTypeString(data, size);
    return 0;
}

