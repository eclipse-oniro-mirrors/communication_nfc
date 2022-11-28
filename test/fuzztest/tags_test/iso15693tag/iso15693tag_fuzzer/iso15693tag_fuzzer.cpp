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

#include "iso15693tag_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "iso15693_tag.h"
#include "nfca_tag.h"
#include "nfc_sdk_common.h"
#include "taginfo.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;
    using namespace OHOS::NFC::TAG;

    constexpr const auto FUZZER_THRESHOLD = 12;
    constexpr const auto TEST_UID = "0102";
    constexpr const auto TEST_DISC_ID = 1;
    constexpr const auto TEST_DSF_ID = '1';
    constexpr const auto TEST_RESPONSE_FLAGS = '0';

    std::shared_ptr<TagInfo> GetTagInfo()
    {
        std::vector<int> tagTechList;
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_V_TECH));
        std::vector<AppExecFwk::PacMap> tagTechExtras;
        AppExecFwk::PacMap iso15693ExtrasData;
        iso15693ExtrasData.PutIntValue(TagInfo::RESPONSE_FLAGS, TEST_RESPONSE_FLAGS);
        iso15693ExtrasData.PutIntValue(TagInfo::DSF_ID, TEST_DSF_ID);
        tagTechExtras.push_back(iso15693ExtrasData);

        std::string tagUid = TEST_UID;
        int tagRfDiscId = TEST_DISC_ID;
        std::shared_ptr<TagInfo> tagInfo = std::make_shared<TagInfo>(tagTechList,
                                                                     tagTechExtras,
                                                                     tagUid,
                                                                     tagRfDiscId,
                                                                     nullptr);
        return tagInfo;
    }

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    void FuzzGetTag(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagInfo> tagInfo = GetTagInfo();
        if (tagInfo == nullptr) {
            std::cout << "tagInfo is nullptr." << std::endl;
            return;
        }
        Iso15693Tag::GetTag(tagInfo);
    }

    void FuzzReadSingleBlock(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagInfo> tagInfo = GetTagInfo();
        if (tagInfo == nullptr) {
            std::cout << "tagInfo is nullptr." << std::endl;
            return;
        }
        std::shared_ptr<Iso15693Tag> iso15693Tag = Iso15693Tag::GetTag(tagInfo);

        // The interface to be tested requires 2 uint32 parameters
        uint32_t uint32Array[2];
        ConvertToUint32s(data, uint32Array, sizeof(uint32Array) / sizeof(uint32_t));
        std::string hexRespData = NfcSdkCommon::BytesVecToHexString(data, size);
        iso15693Tag->ReadSingleBlock(uint32Array[0], uint32Array[1], hexRespData);
    }
    
    void FuzzWriteSingleBlock(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagInfo> tagInfo = GetTagInfo();
        if (tagInfo == nullptr) {
            std::cout << "tagInfo is nullptr." << std::endl;
            return;
        }
        std::shared_ptr<Iso15693Tag> iso15693Tag = Iso15693Tag::GetTag(tagInfo);

        // The interface to be tested requires 2 uint32 parameters
        uint32_t uint32Array[2];
        ConvertToUint32s(data, uint32Array, sizeof(uint32Array) / sizeof(uint32_t));
        std::string hexCmdData = NfcSdkCommon::BytesVecToHexString(data, size);
        iso15693Tag->WriteSingleBlock(uint32Array[0], uint32Array[1], hexCmdData);
    }

    void FuzzLockSingleBlock(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagInfo> tagInfo = GetTagInfo();
        if (tagInfo == nullptr) {
            std::cout << "tagInfo is nullptr." << std::endl;
            return;
        }
        std::shared_ptr<Iso15693Tag> iso15693Tag = Iso15693Tag::GetTag(tagInfo);

        // The interface to be tested requires 2 uint32 parameters
        uint32_t uint32Array[2];
        ConvertToUint32s(data, uint32Array, sizeof(uint32Array) / sizeof(uint32_t));
        iso15693Tag->LockSingleBlock(uint32Array[0], uint32Array[1]);
    }

    void FuzzReadMultipleBlock(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagInfo> tagInfo = GetTagInfo();
        if (tagInfo == nullptr) {
            std::cout << "tagInfo is nullptr." << std::endl;
            return;
        }
        std::shared_ptr<Iso15693Tag> iso15693Tag = Iso15693Tag::GetTag(tagInfo);

        // The interface to be tested requires 3 uint32 parameters
        uint32_t uint32Array[3];
        ConvertToUint32s(data, uint32Array, sizeof(uint32Array) / sizeof(uint32_t));
        std::string hexRespData = NfcSdkCommon::BytesVecToHexString(data, size);
         // 2 is the subscript, representing the third element in the array as the input parameter
        iso15693Tag->ReadMultipleBlock(uint32Array[0], uint32Array[1], uint32Array[2], hexRespData);
    }

    void FuzzWriteMultipleBlock(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagInfo> tagInfo = GetTagInfo();
        if (tagInfo == nullptr) {
            std::cout << "tagInfo is nullptr." << std::endl;
            return;
        }
        std::shared_ptr<Iso15693Tag> iso15693Tag = Iso15693Tag::GetTag(tagInfo);

        // The interface to be tested requires 3 uint32 parameters
        uint32_t uint32Array[3];
        ConvertToUint32s(data, uint32Array, sizeof(uint32Array) / sizeof(uint32_t));
        std::string hexCmdData = NfcSdkCommon::BytesVecToHexString(data, size);
        // 2 is the subscript, representing the third element in the array as the input parameter
        iso15693Tag->WriteMultipleBlock(uint32Array[0], uint32Array[1], uint32Array[2], hexCmdData);
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
    OHOS::FuzzReadSingleBlock(data, size);
    OHOS::FuzzWriteSingleBlock(data, size);
    OHOS::FuzzLockSingleBlock(data, size);
    OHOS::FuzzReadMultipleBlock(data, size);
    OHOS::FuzzWriteMultipleBlock(data, size);
    return 0;
}

