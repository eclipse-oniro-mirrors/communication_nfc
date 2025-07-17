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
#define private public
#define protected public
#include "settingdatashareimpl_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "setting_data_share_impl.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
    using namespace OHOS::NFC;
    using namespace OHOS::NFC::KITS;

    constexpr const auto FUZZER_THRESHOLD = 4;

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    void FuzzRegisterDataObserver(const uint8_t* data, size_t size)
    {
        std::shared_ptr<SettingDataShareImpl> settingDataShareImpl = std::make_shared<SettingDataShareImpl>();
        Uri uri(NfcSdkCommon::BytesVecToHexString(data, size));
        settingDataShareImpl->RegisterDataObserver(uri, nullptr);
    }

    void FuzzReleaseDataObserver(const uint8_t* data, size_t size)
    {
        std::shared_ptr<SettingDataShareImpl> settingDataShareImpl = std::make_shared<SettingDataShareImpl>();
        Uri uri(NfcSdkCommon::BytesVecToHexString(data, size));
        settingDataShareImpl->ReleaseDataObserver(uri, nullptr);
    }

    void FuzzRegisterDataObserverWithNull(const uint8_t* data, size_t size)
    {
        std::shared_ptr<SettingDataShareImpl> settingDataShareImpl = std::make_shared<SettingDataShareImpl>();
        sptr<AAFwk::IDataAbilityObserver> dataObserver;
        settingDataShareImpl->dataShareHelper_ = nullptr;
        Uri uri(NfcSdkCommon::BytesVecToHexString(data, size));
        settingDataShareImpl->RegisterDataObserver(uri, dataObserver);
    }

    void FuzzReleaseDataObserverWithNull(const uint8_t* data, size_t size)
    {
        std::shared_ptr<SettingDataShareImpl> settingDataShareImpl = std::make_shared<SettingDataShareImpl>();
        settingDataShareImpl->dataShareHelper_ = nullptr;
        Uri uri(NfcSdkCommon::BytesVecToHexString(data, size));
        settingDataShareImpl->ReleaseDataObserver(uri, nullptr);
    }

    void FuzzGetElementName(const uint8_t* data, size_t size)
    {
        std::shared_ptr<SettingDataShareImpl> settingDataShareImpl = std::make_shared<SettingDataShareImpl>();
        Uri uri(NfcSdkCommon::BytesVecToHexString(data, size));
        std::string column = NfcSdkCommon::BytesVecToHexString(data, size);
        ElementName value;
        value.GetURI() = NfcSdkCommon::BytesVecToHexString(data, size);
        settingDataShareImpl->GetElementName(uri, column, value);
    }

    void FuzzSetElementName(const uint8_t* data, size_t size)
    {
        std::shared_ptr<SettingDataShareImpl> settingDataShareImpl = std::make_shared<SettingDataShareImpl>();
        Uri uri(NfcSdkCommon::BytesVecToHexString(data, size));
        std::string column = NfcSdkCommon::BytesVecToHexString(data, size);
        ElementName value;
        value.GetURI() = NfcSdkCommon::BytesVecToHexString(data, size);
        settingDataShareImpl->SetElementName(uri, column, value);
    }

    void FuzzParseElementURI(const uint8_t* data, size_t size)
    {
        Uri uri(NfcSdkCommon::BytesVecToHexString(data, size));
        std::shared_ptr<SettingDataShareImpl> settingDataShareImpl = std::make_shared<SettingDataShareImpl>();
        std::string urlStr = "abcd/1/2";
        ElementName value;
        settingDataShareImpl->ParseElementURI(urlStr, value);
    }

}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzRegisterDataObserver(data, size);
    OHOS::FuzzRegisterDataObserverWithNull(data, size);
    OHOS::FuzzReleaseDataObserver(data, size);
    OHOS::FuzzReleaseDataObserverWithNull(data, size);
    OHOS::FuzzGetElementName(data, size);
    OHOS::FuzzSetElementName(data, size);
    OHOS::FuzzParseElementURI(data, size);
    return 0;
}

