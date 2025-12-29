/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "tagabilitydispatcher_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "access_token.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include "tag_ability_dispatcher.h"
#include <securec.h>

namespace OHOS {
namespace NFC {
    using namespace OHOS::NFC::TAG;
    using namespace OHOS::NFC::KITS;
    constexpr const auto FUZZER_THRESHOLD = 4;

    const uint8_t *g_baseFuzzData = nullptr;
    size_t g_baseFuzzSize = 0;
    size_t g_baseFuzzPos = 0;

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
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

    void FuzzSetWantExtraParam(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::vector<int> tagTechList;
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_A_TECH));
        std::vector<AppExecFwk::PacMap> tagTechExtras;
        AppExecFwk::PacMap tagTechExtrasData;
        tagTechExtras.push_back(tagTechExtrasData);
        int tagRfDiscId = GetData<int>();
        std::string tagUid = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<KITS::TagInfo> tagInfo =
            std::make_shared<KITS::TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
        AAFwk::Want want;
        std::shared_ptr<TagAbilityDispatcher> tagAbilityDispatcher = std::make_shared<TagAbilityDispatcher>();
        tagAbilityDispatcher->SetWantExtraParam(tagInfo, want);
    }

    void FuzzSetWantExtraParam1(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::vector<int> tagTechList;
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_B_TECH));
        std::vector<AppExecFwk::PacMap> tagTechExtras;
        AppExecFwk::PacMap tagTechExtrasData;
        tagTechExtras.push_back(tagTechExtrasData);
        int tagRfDiscId = GetData<int>();
        std::string tagUid = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<KITS::TagInfo> tagInfo =
            std::make_shared<KITS::TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
        AAFwk::Want want;
        std::shared_ptr<TagAbilityDispatcher> tagAbilityDispatcher = std::make_shared<TagAbilityDispatcher>();
        tagAbilityDispatcher->SetWantExtraParam(tagInfo, want);
    }

    void FuzzSetWantExtraParam2(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::vector<int> tagTechList;
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_F_TECH));
        std::vector<AppExecFwk::PacMap> tagTechExtras;
        AppExecFwk::PacMap tagTechExtrasData;
        tagTechExtras.push_back(tagTechExtrasData);
        int tagRfDiscId = GetData<int>();
        std::string tagUid = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<KITS::TagInfo> tagInfo =
            std::make_shared<KITS::TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
        AAFwk::Want want;
        std::shared_ptr<TagAbilityDispatcher> tagAbilityDispatcher = std::make_shared<TagAbilityDispatcher>();
        tagAbilityDispatcher->SetWantExtraParam(tagInfo, want);
    }

    void FuzzSetWantExtraParam3(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::vector<int> tagTechList;
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_V_TECH));
        std::vector<AppExecFwk::PacMap> tagTechExtras;
        AppExecFwk::PacMap tagTechExtrasData;
        tagTechExtras.push_back(tagTechExtrasData);
        int tagRfDiscId = GetData<int>();
        std::string tagUid = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<KITS::TagInfo> tagInfo =
            std::make_shared<KITS::TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
        AAFwk::Want want;
        std::shared_ptr<TagAbilityDispatcher> tagAbilityDispatcher = std::make_shared<TagAbilityDispatcher>();
        tagAbilityDispatcher->SetWantExtraParam(tagInfo, want);
    }

    void FuzzSetWantExtraParam4(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::vector<int> tagTechList;
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_ISODEP_TECH));
        std::vector<AppExecFwk::PacMap> tagTechExtras;
        AppExecFwk::PacMap tagTechExtrasData;
        tagTechExtras.push_back(tagTechExtrasData);
        int tagRfDiscId = GetData<int>();
        std::string tagUid = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<KITS::TagInfo> tagInfo =
            std::make_shared<KITS::TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
        AAFwk::Want want;
        std::shared_ptr<TagAbilityDispatcher> tagAbilityDispatcher = std::make_shared<TagAbilityDispatcher>();
        tagAbilityDispatcher->SetWantExtraParam(tagInfo, want);
    }

    void FuzzSetWantExtraParam5(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::vector<int> tagTechList;
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH));
        std::vector<AppExecFwk::PacMap> tagTechExtras;
        AppExecFwk::PacMap tagTechExtrasData;
        tagTechExtras.push_back(tagTechExtrasData);
        int tagRfDiscId = GetData<int>();
        std::string tagUid = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<KITS::TagInfo> tagInfo =
            std::make_shared<KITS::TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
        AAFwk::Want want;
        std::shared_ptr<TagAbilityDispatcher> tagAbilityDispatcher = std::make_shared<TagAbilityDispatcher>();
        tagAbilityDispatcher->SetWantExtraParam(tagInfo, want);
    }

    void FuzzSetWantExtraParam6(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::vector<int> tagTechList;
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_NDEF_TECH));
        std::vector<AppExecFwk::PacMap> tagTechExtras;
        AppExecFwk::PacMap tagTechExtrasData;
        tagTechExtras.push_back(tagTechExtrasData);
        int tagRfDiscId = GetData<int>();
        std::string tagUid = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<KITS::TagInfo> tagInfo =
            std::make_shared<KITS::TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
        AAFwk::Want want;
        std::shared_ptr<TagAbilityDispatcher> tagAbilityDispatcher = std::make_shared<TagAbilityDispatcher>();
        tagAbilityDispatcher->SetWantExtraParam(tagInfo, want);
    }

    void FuzzStartVibratorOnce(const uint8_t* data, size_t size)
    {
        std::shared_ptr<TagAbilityDispatcher> tagAbilityDispatcher = std::make_shared<TagAbilityDispatcher>();
        tagAbilityDispatcher->StartVibratorOnce();
    }

    void FuzzDispatchTagAbility(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;
        
        std::vector<int> tagTechList;
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_A_TECH));
        std::vector<AppExecFwk::PacMap> tagTechExtras;
        AppExecFwk::PacMap tagTechExtrasData;
        tagTechExtras.push_back(tagTechExtrasData);
        int tagRfDiscId = GetData<int>();
        std::string tagUid = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<KITS::TagInfo> tagInfo =
            std::make_shared<KITS::TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
        OHOS::sptr<IRemoteObject> tagServiceIface = nullptr;
        std::shared_ptr<TagAbilityDispatcher> tagAbilityDispatcher = std::make_shared<TagAbilityDispatcher>();
        tagAbilityDispatcher->DispatchTagAbility(tagInfo, tagServiceIface);
    }

    void FuzzDispatchTagAbility1(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::vector<int> tagTechList;
        std::vector<AppExecFwk::PacMap> tagTechExtras;
        std::string tagUid = std::string(reinterpret_cast<const char*>(data), size);
        int tagRfDiscId = GetData<int>();
        std::shared_ptr<KITS::TagInfo> tagInfo =
            std::make_shared<KITS::TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
        OHOS::sptr<IRemoteObject> tagServiceIface = nullptr;
        std::shared_ptr<TagAbilityDispatcher> tagAbilityDispatcher = std::make_shared<TagAbilityDispatcher>();
        tagAbilityDispatcher->DispatchTagAbility(tagInfo, tagServiceIface);
    }

    void FuzzDispatchAbilityMultiApp(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::vector<int> tagTechList;
        std::vector<AppExecFwk::PacMap> tagTechExtras;
        std::string tagUid = std::string(reinterpret_cast<const char*>(data), size);
        int tagRfDiscId = GetData<int>();
        std::shared_ptr<KITS::TagInfo> tagInfo =
            std::make_shared<KITS::TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
        AAFwk::Want want;
        std::shared_ptr<TagAbilityDispatcher> tagAbilityDispatcher = std::make_shared<TagAbilityDispatcher>();
        tagAbilityDispatcher->DispatchAbilityMultiApp(tagInfo, want);
    }

    void FuzzDispatchAbilityMultiApp1(const uint8_t* data, size_t size)
    {
        g_baseFuzzData = data;
        g_baseFuzzSize = size;
        g_baseFuzzPos = 0;

        std::vector<int> tagTechList;
        tagTechList.push_back(static_cast<int>(TagTechnology::NFC_A_TECH));
        std::vector<AppExecFwk::PacMap> tagTechExtras;
        AppExecFwk::PacMap tagTechExtrasData;
        tagTechExtras.push_back(tagTechExtrasData);
        int tagRfDiscId = GetData<int>();
        std::string tagUid = NfcSdkCommon::BytesVecToHexString(data, size);
        std::shared_ptr<KITS::TagInfo> tagInfo =
            std::make_shared<KITS::TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, nullptr);
        AAFwk::Want want;
        std::shared_ptr<TagAbilityDispatcher> tagAbilityDispatcher = std::make_shared<TagAbilityDispatcher>();
        tagAbilityDispatcher->DispatchAbilityMultiApp(tagInfo, want);
    }
} // namespace NFC
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::NFC::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::NFC::FuzzSetWantExtraParam(data, size);
    OHOS::NFC::FuzzSetWantExtraParam1(data, size);
    OHOS::NFC::FuzzSetWantExtraParam2(data, size);
    OHOS::NFC::FuzzSetWantExtraParam3(data, size);
    OHOS::NFC::FuzzSetWantExtraParam4(data, size);
    OHOS::NFC::FuzzSetWantExtraParam5(data, size);
    OHOS::NFC::FuzzSetWantExtraParam6(data, size);
    OHOS::NFC::FuzzStartVibratorOnce(data, size);
    OHOS::NFC::FuzzDispatchTagAbility(data, size);
    OHOS::NFC::FuzzDispatchTagAbility1(data, size);
    OHOS::NFC::FuzzDispatchAbilityMultiApp(data, size);
    OHOS::NFC::FuzzDispatchAbilityMultiApp1(data, size);
    return 0;
}

