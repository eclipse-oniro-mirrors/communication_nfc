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

#include "nfc_cardEmulation_ffi.h"

#include <cstring>

#include "ability_info.h"
#include "cardEmulation.h"
#include "element_name.h"
#include "hce_service.h"
#include "loghelper.h"
#include "nfc_cardEmulation_controller.h"

namespace OHOS {
namespace NFC {
namespace KITS {
const int8_t HCE_CMD = 0;

class CjHceCmdListenerEvent : public IHceCmdCallback {
public:
    CjHceCmdListenerEvent() {}

    virtual ~CjHceCmdListenerEvent() {}

public:
    void OnCeApduData(const std::vector<uint8_t>& data) override
    {
        CjNfcCardEmulationController::GetInstance()->HceCmd(data);
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

sptr<CjHceCmdListenerEvent> cjHceCmdListenerEvent =
    sptr<CjHceCmdListenerEvent>(new (std::nothrow) CjHceCmdListenerEvent());
static bool isEventRegistered = false;

std::vector<std::string> CharPtrToVector(char** charPtr, int32_t size)
{
    std::vector<std::string> result;
    for (int32_t i = 0; i < size; i++) {
        if (charPtr) {
            result.push_back(std::string(charPtr[i]));
        }
    }
    return result;
}

extern "C" {
int32_t FfiNfcCardEmulationisDefaultService(
    char* cBundleName, char* cAbilityName, char* cModuleName, char* cardTypeName, bool* ret)
{
    bool isDefaultService = false;
    std::string type(cardTypeName);
    std::string bundleName(cBundleName);
    std::string moduleName(cModuleName);
    std::string abilityName(cAbilityName);
    ElementName element;
    element.SetBundleName(bundleName);
    element.SetModuleName(moduleName);
    element.SetAbilityName(abilityName);
    HceService hceService = HceService::GetInstance();
    int32_t errorCode = hceService.IsDefaultService(element, type, isDefaultService);
    *ret = isDefaultService;
    return errorCode;
}

int32_t FfiNfcCardEmulationstart(char* cBundleName, char* cAbilityName, char* cModuleName, CArrString cAidList)
{
    std::string bundleName(cBundleName);
    std::string moduleName(cModuleName);
    std::string abilityName(cAbilityName);
    ElementName element;
    element.SetBundleName(bundleName);
    element.SetModuleName(moduleName);
    element.SetAbilityName(abilityName);
    std::vector<std::string> aidVec;
    aidVec = CharPtrToVector(cAidList.head, cAidList.size);
    HceService hceService = HceService::GetInstance();
    int32_t errorCode = hceService.StartHce(element, aidVec);
    return errorCode;
}

int32_t FfiNfcCardEmulationOn(int8_t eventType, int64_t id)
{
    if (!isEventRegistered) {
        HceService hceService = HceService::GetInstance();
        ErrorCode ret = hceService.RegHceCmdCallback(cjHceCmdListenerEvent, KITS::EVENT_HCE_CMD);
        if (ret != KITS::ERR_NONE) {
            return ret;
        }
        isEventRegistered = true;
    }
    auto controller = CjNfcCardEmulationController::GetInstance();
    if (controller == nullptr) {
        return ERR_NO_MEMORY;
    }
    return controller->Subscribe(eventType, id);
}

int32_t FfiNfcCardEmulationstop(char* cBundleName, char* cAbilityName, char* cModuleName)
{
    std::string bundleName(cBundleName);
    std::string moduleName(cModuleName);
    std::string abilityName(cAbilityName);
    ElementName element;
    element.SetBundleName(bundleName);
    element.SetModuleName(moduleName);
    element.SetAbilityName(abilityName);
    HceService hceService = HceService::GetInstance();
    ErrorCode ret = hceService.StopHce(element);
    if (ret != KITS::ERR_NONE) {
        return ret;
    }
    isEventRegistered = false;
    auto controller = CjNfcCardEmulationController::GetInstance();
    if (controller == nullptr) {
        return ERR_NO_MEMORY;
    }
    return controller->UnSubscribe(HCE_CMD);
}

int32_t FfiNfcCardEmulationTransmit(CArrUI8 cResponseApdu)
{
    std::string dataBytes = NfcSdkCommon::BytesVecToHexString(cResponseApdu.head, cResponseApdu.size);
    std::string hexRespData;
    HceService hceService = HceService::GetInstance();
    int32_t errorCode = hceService.SendRawFrame(dataBytes, true, hexRespData);
    return errorCode;
}
}

} // namespace KITS
} // namespace NFC
} // namespace OHOS
