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

#include "getboolextrasdata_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "taginfo.h"
#include "tag_session_proxy.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;
    using namespace OHOS::NFC::TAG;

    constexpr const auto FUZZER_TEST_UID = "0102";
    constexpr const auto FUZZER_TEST_DISC_ID = 1;

    std::shared_ptr<TagInfo> FuzzGetTagInfo()
    {
        std::vector<int> tagTechList;
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_A_TECH));
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_ISODEP_TECH));

        std::vector<AppExecFwk::PacMap> tagTechExtras;
        AppExecFwk::PacMap tagTechExtrasData;
        std::string tagUid = FUZZER_TEST_UID;
        int tagRfDiscId = FUZZER_TEST_DISC_ID;
        tagTechExtras.push_back(tagTechExtrasData);
        return std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
    }

    void FuzzGetBoolExtrasData(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagInfo> tagInfo = FuzzGetTagInfo();
        if (tagInfo == nullptr) {
            std::cout << "tagInfo is nullptr." << std::endl;
            return;
        }
        AppExecFwk::PacMap extrasData = tagInfo->GetTechExtrasByTech(TagTechnology::NFC_A_TECH);
        std::string extrasName = NfcSdkCommon::BytesVecToHexString(data, size);

        tagInfo->GetBoolExtrasData(extrasData, extrasName);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::FuzzGetBoolExtrasData(data, size);
    return 0;
}

