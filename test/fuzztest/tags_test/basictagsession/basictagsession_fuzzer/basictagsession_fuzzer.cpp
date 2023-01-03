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

#include "basictagsession_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "basic_tag_session.h"
#include "nfc_sdk_common.h"
#include "taginfo.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;
    using namespace OHOS::NFC::TAG;

    constexpr const auto FUZZER_THRESHOLD = 4;
    constexpr const auto TEST_UID = "0102";
    constexpr const auto TEST_DISC_ID = 1;
    constexpr const auto TAG_TECHNOLOGY_MAX_LEN = 10;

    std::shared_ptr<TagInfo> GetTagInfo()
    {
        std::vector<int> tagTechList;
        std::vector<AppExecFwk::PacMap> tagTechExtras;
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

    void FuzzSetTimeout(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagInfo> tagInfo = GetTagInfo();
        TagTechnology tagTechnology = static_cast<TagTechnology>(size % TAG_TECHNOLOGY_MAX_LEN);
        BasicTagSession basicTagSession{tagInfo, tagTechnology};
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        basicTagSession.SetTimeout(timeOutArray[0]);
    }

    void FuzzSendCommand(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagInfo> tagInfo = GetTagInfo();
        TagTechnology tagTechnology = static_cast<TagTechnology>(size % TAG_TECHNOLOGY_MAX_LEN);
        BasicTagSession basicTagSession{tagInfo, tagTechnology};
        std::string hexCmdData = NfcSdkCommon::UnsignedCharToHexString(data[0]);

        // Calculate the remainder of 2 and convert the int type parameter to boolean type randomly
        bool raw = (data[1]) % 2 == 1;

        // 2 is the subscript. Convert this parameter to string type
        std::string hexRespData = NfcSdkCommon::UnsignedCharToHexString(data[2]);
        basicTagSession.SendCommand(hexCmdData, raw, hexRespData);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzSetTimeout(data, size);
    OHOS::FuzzSendCommand(data, size);
    return 0;
}

