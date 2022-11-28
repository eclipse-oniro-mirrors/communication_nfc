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

#include "mifareultralighttag_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "mifare_ultralight_tag.h"
#include "nfca_tag.h"
#include "nfc_sdk_common.h"
#include "taginfo.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;
    using namespace OHOS::NFC::TAG;

    constexpr const auto FUZZER_THRESHOLD = 4;
    constexpr const auto TEST_UID = "0102";
    constexpr const auto TEST_DISC_ID = 1;
    constexpr const auto TEST_SAK = 0x28;
    constexpr const auto TEST_ATQA = "0400";

    std::shared_ptr<TagInfo> FuzzGetTagInfo()
    {
        std::vector<int> tagTechList;
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH));
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_A_TECH));

        std::vector<AppExecFwk::PacMap> tagTechExtras;
        AppExecFwk::PacMap mifareUltralightExtrasData;
        AppExecFwk::PacMap nfcAExtrasData;
        nfcAExtrasData.PutIntValue(TagInfo::SAK, TEST_SAK);
        nfcAExtrasData.PutStringValue(TagInfo::ATQA, TEST_ATQA);
        tagTechExtras.push_back(mifareUltralightExtrasData);
        tagTechExtras.push_back(nfcAExtrasData);

        std::string tagUid = TEST_UID;
        int tagRfDiscId = TEST_DISC_ID;
        std::shared_ptr<TagInfo> tagInfo = std::make_shared<TagInfo>(tagTechList,
                                                                     tagTechExtras,
                                                                     tagUid,
                                                                     tagRfDiscId,
                                                                     nullptr);
        return tagInfo;
    }
    
    uint32_t ConvertToUint32(const uint8_t* ptr)
    {
        if (ptr == nullptr) {
            return 0;
        }

        // Shift the 0th number to the left by 24 bits, shift the 1st number to the left by 16 bits,
        // shift the 2nd number to the left by 8 bits, and not shift the 3rd number to the left
        return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
    }

    void FuzzGetTag(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagInfo> tagInfo = FuzzGetTagInfo();
        if (tagInfo == nullptr) {
            std::cout << "tagInfo is nullptr." << std::endl;
            return;
        }
        MifareUltralightTag::GetTag(tagInfo);
    }

    void FuzzReadMultiplePages(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagInfo> tagInfo = FuzzGetTagInfo();
        if (tagInfo == nullptr) {
            std::cout << "tagInfo is nullptr." << std::endl;
            return;
        }
        std::shared_ptr<MifareUltralightTag> mifareUltralightTag = MifareUltralightTag::GetTag(tagInfo);
        if (mifareUltralightTag == nullptr) {
            std::cout << "mifareUltralightTag is nullptr." << std::endl;
            return;
        }
        uint32_t pageIndex = ConvertToUint32(data);
        std::string hexRespData = NfcSdkCommon::BytesVecToHexString(data, size);
        mifareUltralightTag->ReadMultiplePages(pageIndex, hexRespData);
    }
    void FuzzWriteSinglePage(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagInfo> tagInfo = FuzzGetTagInfo();
        if (tagInfo == nullptr) {
            std::cout << "tagInfo is nullptr." << std::endl;
            return;
        }
        std::shared_ptr<MifareUltralightTag> mifareUltralightTag = MifareUltralightTag::GetTag(tagInfo);
        if (mifareUltralightTag == nullptr) {
            std::cout << "mifareUltralightTag is nullptr." << std::endl;
            return;
        }
        uint32_t pageIndex = ConvertToUint32(data);
        std::string hexRespData = NfcSdkCommon::BytesVecToHexString(data, size);
        mifareUltralightTag->WriteSinglePage(pageIndex, hexRespData);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }
    OHOS::FuzzGetTag(data, size);
    OHOS::FuzzReadMultiplePages(data, size);
    OHOS::FuzzWriteSinglePage(data, size);
    return 0;
}

