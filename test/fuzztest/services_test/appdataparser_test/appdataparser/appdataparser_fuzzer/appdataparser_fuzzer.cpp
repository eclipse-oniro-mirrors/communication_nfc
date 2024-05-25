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
#include "appdataparser_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "app_data_parser.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
    using namespace OHOS::NFC;
    using namespace OHOS::NFC::KITS;

    constexpr const auto FUZZER_THRESHOLD = 4;

class IQueryAppInfoCallbackImpl : public IQueryAppInfoCallback {
public:
    IQueryAppInfoCallbackImpl() {}

    virtual ~IQueryAppInfoCallbackImpl() {}

public:
    bool OnQueryAppInfo(std::string type, std::vector<int> techList, std::vector<AAFwk::Want> &hceAppList,
        std::vector<AppExecFwk::ElementName> &elementNameList) override
    {
        return false;
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

class IOnCardEmulationNotifyCbImpl : public IOnCardEmulationNotifyCb {
public:
    IOnCardEmulationNotifyCbImpl() {}

    virtual ~IOnCardEmulationNotifyCbImpl() {}

public:
    bool OnCardEmulationNotify(uint32_t eventType, std::string apduData) override
    {
        return false;
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    void FuzzGetDispatchTagAppsByTech(const uint8_t* data, size_t size)
    {
        AppDataParser& appDataParser = AppDataParser::GetInstance();
        std::vector<int> discTechList;
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        discTechList.push_back(timeOutArray[0]);
        appDataParser.GetDispatchTagAppsByTech(discTechList);
    }

    void FuzzGetVendorDispatchTagAppsByTech1(const uint8_t* data, size_t size)
    {
        AppDataParser& appDataParser = AppDataParser::GetInstance();
        std::vector<int> discTechList;
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        discTechList.push_back(timeOutArray[0]);
        appDataParser.GetVendorDispatchTagAppsByTech(discTechList);
    }

    void FuzzGetVendorDispatchTagAppsByTech2(const uint8_t* data, size_t size)
    {
        AppDataParser& appDataParser = AppDataParser::GetInstance();
        sptr<IQueryAppInfoCallbackImpl> iQueryAppInfoCallbackImpl =
            sptr<IQueryAppInfoCallbackImpl>(new (std::nothrow) IQueryAppInfoCallbackImpl());
        std::vector<int> discTechList;
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        discTechList.push_back(timeOutArray[0]);
        appDataParser.RegQueryApplicationCb(iQueryAppInfoCallbackImpl);
        appDataParser.GetVendorDispatchTagAppsByTech(discTechList);
    }

    void FuzzGetNotifyCardEmulationCallback(const uint8_t* data, size_t size)
    {
        AppDataParser& appDataParser = AppDataParser::GetInstance();
        sptr<IOnCardEmulationNotifyCbImpl> iOnCardEmulationNotifyCbImpl =
            sptr<IOnCardEmulationNotifyCbImpl>(new (std::nothrow) IOnCardEmulationNotifyCbImpl());
        appDataParser.RegCardEmulationNotifyCb(iOnCardEmulationNotifyCbImpl);
        appDataParser.GetNotifyCardEmulationCallback();
    }

    void FuzzIsBundleInstalled(const uint8_t* data, size_t size)
    {
        AppDataParser& appDataParser = AppDataParser::GetInstance();
        std::string bundleName = NfcSdkCommon::BytesVecToHexString(data, size);
        appDataParser.IsBundleInstalled(bundleName);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzGetDispatchTagAppsByTech(data, size);
    OHOS::FuzzGetVendorDispatchTagAppsByTech1(data, size);
    OHOS::FuzzGetVendorDispatchTagAppsByTech2(data, size);
    OHOS::FuzzGetNotifyCardEmulationCallback(data, size);
    OHOS::FuzzIsBundleInstalled(data, size);
    return 0;
}

