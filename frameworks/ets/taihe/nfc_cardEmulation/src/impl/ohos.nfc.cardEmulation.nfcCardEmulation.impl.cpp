/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ohos.nfc.cardEmulation.nfcCardEmulation.proj.hpp"
#include "ohos.nfc.cardEmulation.nfcCardEmulation.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"

#include "ability_info.h"
#include "common_fun_ani.h"
#include "element_name.h"
#include "hce_service.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "nfc_taihe_card_emulation_event.h"

using namespace taihe;
using namespace ohos::nfc::cardEmulation::nfcCardEmulation;
using namespace OHOS::AppExecFwk;
using namespace OHOS::NFC;

const uint16_t MAX_ARRAY_LEN = 512;
const uint16_t MAX_AID_LIST_NUM = 100;

constexpr const char* EVENT_TYPE_HCE_CMD = "hceCmd";

namespace {
class HceServiceImpl {
    public:
    HceServiceImpl()
    {
        InfoLog("HceServiceImpl constructor.");
    }

    void onHceCmd(::taihe::callback_view<void (uintptr_t err, ::taihe::array_view<uint8_t> data)> callback)
    {
        InfoLog("onHceCmd enter");
        KITS::NfcHceEventRegister::GetInstance().Register(EVENT_TYPE_HCE_CMD, callback);
    }

    void offHceCmd(
        ::taihe::optional_view<::taihe::callback<void(uintptr_t err, ::taihe::array_view<uint8_t> data)>> callback)
    {
        InfoLog("offHceCmd enter");
        KITS::NfcHceEventRegister::GetInstance().Unregister(EVENT_TYPE_HCE_CMD);
    }

    void start(uintptr_t elementName, array_view<::taihe::string> aidList)
    {
        InfoLog("StartHce enter");
        ElementName element;
        CommonFunAni::ParseElementName(get_env(), reinterpret_cast<ani_object>(elementName), element);
        if (aidList.size() > MAX_AID_LIST_NUM) {
            ErrorLog("data size exceed.");
            return;
        }

        std::vector<std::string> aidVec;
        for (uint16_t i = 0; i < aidList.size(); i++) {
            aidVec.push_back(aidList[i].c_str());
        }

        KITS::ErrorCode ret = KITS::HceService::GetInstance().StartHce(element, aidVec);
        InfoLog("StartHce, statusCode = %{public}d", ret);
    }

    void stop(uintptr_t elementName)
    {
        InfoLog("StopHce enter");
        ElementName element;
        CommonFunAni::ParseElementName(get_env(), reinterpret_cast<ani_object>(elementName), element);

        KITS::ErrorCode ret = KITS::HceService::GetInstance().StopHce(element);
        InfoLog("StopHce, statusCode = %{public}d", ret);
    }

    void transmitImpl(array_view<uint8_t> data)
    {
        InfoLog("transmit enter");
        if (data.size() > MAX_ARRAY_LEN) {
            ErrorLog("data size exceed.");
            return;
        }
        std::vector<uint8_t> dataBytes = {};
        for (uint16_t i = 0; i < data.size(); i++)  {
            dataBytes.push_back(data[i]);
        }
        std::string hexCmdData = KITS::NfcSdkCommon::BytesVecToHexString(&dataBytes[0], dataBytes.size());

        std::string hexRspData;
        int errorCode = KITS::HceService::GetInstance().SendRawFrame(hexCmdData, true, hexRspData);
        InfoLog("transmit, errorCode = %{public}d", errorCode);
    }
};

bool hasHceCapability()
{
    InfoLog("enter.");
    return true;
}

bool isDefaultService(uintptr_t elementName, CardType type)
{
    InfoLog("isDefaultService enter.");
    ElementName element;
    CommonFunAni::ParseElementName(get_env(), reinterpret_cast<ani_object>(elementName), element);

    if (type.get_key() != CardType::key_t::PAYMENT) {
        ErrorLog("IsDefaultService: unsupported card type");
        return false;
    }

    bool isDefaultService = false;
    int statusCode = KITS::HceService::GetInstance().IsDefaultService(
        element, type.get_value(), isDefaultService);
    InfoLog("isDefaultService statusCode %{public}d, isDefaultService %{public}s.",
        statusCode, isDefaultService ? "true" : "false");
    return isDefaultService;
}

array<uintptr_t> getPaymentServices()
{
    InfoLog("GetPaymentServices enter.");
    std::vector<AbilityInfo> paymentAbilityInfos;
    int statusCode = KITS::HceService::GetInstance().GetPaymentServices(paymentAbilityInfos);
    InfoLog("GetPaymentServices statusCode %{public}d, ability size %{public}zu.",
        statusCode, paymentAbilityInfos.size());

    std::vector<uintptr_t> abilityInfoAniVec;
    for (uint16_t i = 0; i < paymentAbilityInfos.size(); i++) {
        abilityInfoAniVec.push_back(reinterpret_cast<uintptr_t>(
            CommonFunAni::ConvertAbilityInfo(get_env(), paymentAbilityInfos[i])));
    }
    return array<uintptr_t>(abilityInfoAniVec);
}

::ohos::nfc::cardEmulation::nfcCardEmulation::HceService MakeHceService()
{
    //The parameters in the make_holder function should be of the same type
    //as the parameters in the constructor of the actual implementation class.
    return taihe::make_holder<HceServiceImpl, ::ohos::nfc::cardEmulation::nfcCadEmulation::HceService>();
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_hasHceCapability(hasHceCapability);
TH_EXPORT_CPP_API_isDefaultService(isDefaultService);
TH_EXPORT_CPP_API_getPaymentServices(getPaymentServices);
TH_EXPORT_CPP_API_MakeHceService(MakeHceService);
// NOLINTEND
