/*
* Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "nfc_napi_cardEmulation_adapter.h"
#include "cardEmulation.h"
#include "loghelper.h"
#include "hce_service.h"
#include "ability_info.h"

namespace OHOS {
namespace NFC {
namespace KITS {
using AppExecFwk::AbilityInfo;
napi_value IsSupported(napi_env env, napi_callback_info info)
{
    bool isSupported = false;
    napi_value result;
    napi_get_boolean(env, isSupported, &result);
    return result;
}

napi_value HasHceCapability(napi_env env, napi_callback_info info)
{
    bool hasHceCapability = false;
    napi_value result;
    napi_get_boolean(env, hasHceCapability, &result);
    return result;
}

napi_value IsDefaultService(napi_env env, napi_callback_info info)
{
    bool isDefaultService = false;
    napi_value result;
    napi_get_boolean(env, isDefaultService, &result);
    return result;
}

void ConvertAbilityInfoToJS(napi_env env, napi_value &result, AbilityInfo &abilityInfo)
{
    // std::string name;  // ability name, only the main class name
    // std::string label;
    // std::string bundleName;
    // std::string iconPath;
    napi_create_object(env, &result);

    napi_value name;
    napi_create_string_utf8(env, abilityInfo.name.c_ctr(), NAPI_AUTO_LENGTH, &name);
    napi_set_named_property(env, result, "name", name);

    napi_value label;
    napi_create_string_utf8(env, abilityInfo.label.c_ctr(), NAPI_AUTO_LENGTH, &label);
    napi_set_named_property(env, result, "label", label);

    napi_value bundleName;
    napi_create_string_utf8(env, abilityInfo.bundleName.c_ctr(), NAPI_AUTO_LENGTH, &bundleName);
    napi_set_named_property(env, result, "bundleName", bundleName);

    napi_value iconPath;
    napi_create_string_utf8(env, abilityInfo.iconPath.c_ctr(), NAPI_AUTO_LENGTH, &iconPath);
    napi_set_named_property(env, result, "icon", iconPath);
}

void ConvertAbilityInfoVectorToJS(napi_env env, napi_value &result, std::vector<AbilityInfo> &paymentAbilityInfos)
{
    napi_create_array(env, &result);
    if (paymentAbilityInfos.empty()) {
        WarnLog("ConvertAbilityInfoVectorToJS ability infos is empty.");
        return;
    }
    size_t idx = 0;
    for (auto &abilityInfo : paymentAbilityInfos) {
        napi_value obj = nullptr;
        ConvertAbilityInfoToJS(env, obj, abilityInfo);
        napi_set_element(env, result, idx, obj);
        idx++;
    }
}
napi_value GetPaymentServices(napi_env env, napi_callback_info info)
{
    DebugLog("GetPaymentServices ability start.");
    HceService hceService = HceService::GetInstance();
    std::vector<AbilityInfo> paymentAbilityInfos;
    hceService.GetPaymentServices(paymentAbilityInfos);
    DebugLog("GetPaymentServices ability size %{public}zu.", paymentAbilityInfos.size());

    napi_value result = nullptr;
    ConvertAbilityInfoVectorToJS(env, result, paymentAbilityInfos);
    DebugLog("GetPaymentServices ability end.");
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS