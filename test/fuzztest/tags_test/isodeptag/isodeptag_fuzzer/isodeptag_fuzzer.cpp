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

#include "isodeptag_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "isodep_tag.h"
#include "nfca_tag.h"
#include "nfcb_tag.h"
#include "nfc_sdk_common.h"
#include "taginfo.h"
#include <securec.h>

namespace OHOS {
    using namespace OHOS::NFC::KITS;

    constexpr const auto TEST_UID = "0102";
    constexpr const auto TEST_DISC_ID = 1;
    constexpr const auto TEST_HISTORICAL_BYTES = "1015";
    constexpr const auto TEST_HILAYER_RESPONSE = "0106";

    const uint8_t *g_baseFuzzData = nullptr;
    size_t g_baseFuzzSize = 0;
    size_t g_baseFuzzPos = 0;

    std::shared_ptr<TagInfo> FuzzGetTagInfo()
    {
        std::vector<int> tagTechList;
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_A_TECH));
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_B_TECH));
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_ISODEP_TECH));

        std::vector<AppExecFwk::PacMap> tagTechExtras;
        AppExecFwk::PacMap nfcAExtrasData;
        AppExecFwk::PacMap nfcBExtrasData;
        AppExecFwk::PacMap isodepExtrasData;
        isodepExtrasData.PutStringValue(TagInfo::HISTORICAL_BYTES, TEST_HISTORICAL_BYTES);
        isodepExtrasData.PutStringValue(TagInfo::HILAYER_RESPONSE, TEST_HILAYER_RESPONSE);
        tagTechExtras.push_back(nfcAExtrasData);
        tagTechExtras.push_back(nfcBExtrasData);
        tagTechExtras.push_back(isodepExtrasData);

        std::string tagUid = TEST_UID;
        int tagRfDiscId = TEST_DISC_ID;
        std::shared_ptr<TagInfo> tagInfo = std::make_shared<TagInfo>(tagTechList,
                                                                     tagTechExtras,
                                                                     tagUid,
                                                                     tagRfDiscId,
                                                                     nullptr);
        return tagInfo;
    }

    template <class T> T GetData()
    {
        T object{};
        size_t objectSize = sizeof(object);
        if (g_baseFuzzData == nullptr || objectSize > g_baseFuzzSize - g_baseFuzzPos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_baseFuzzData + g_baseFuzzPos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_baseFuzzPos += objectSize;
        return object;
    }

    void FuzzGetTag(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::vector<int> tagTechList;
        std::vector<AppExecFwk::PacMap> tagTechExtras;
        std::string tagUid = std::string(reinterpret_cast<const char*>(data), size);
        int tagRfDiscId = GetData<int>();
        std::shared_ptr<TagInfo> tagInfo =
            std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
        if (tagInfo == nullptr) {
            std::cout << "tagInfo is nullptr." << std::endl;
            return;
        }
        IsoDepTag::GetTag(tagInfo);
    }

    void FuzzIsExtendedApduSupported(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::shared_ptr<TagInfo> tagInfo = FuzzGetTagInfo();
        if (tagInfo == nullptr) {
            std::cout << "tagInfo is nullptr." << std::endl;
            return;
        }
        std::shared_ptr<IsoDepTag> isoDepTag = IsoDepTag::GetTag(tagInfo);
        bool isSupported = (GetData<int>() % 2) == 1;
        isoDepTag->IsExtendedApduSupported(isSupported);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::FuzzGetTag(data, size);
    OHOS::FuzzIsExtendedApduSupported(data, size);
    return 0;
}

