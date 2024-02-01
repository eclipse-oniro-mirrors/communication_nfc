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
#include "nfc_napi_common_utils.h"
#include "element_name.h"


namespace OHOS {
namespace NFC {
namespace KITS {
using AppExecFwk::AbilityInfo;
using AppExecFwk::ElementName;
napi_value IsSupported(napi_env env, napi_callback_info cbinfo)
{
    bool isSupported = false;
    size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
    napi_value thisVar = 0;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    int32_t type;
    if (!CheckArgCountAndThrow(env, argc, ARGV_NUM_1) || !ParseInt32(env, type, argv[ARGV_INDEX_0])) {
        ErrorLog("IsSupported: parse args failed");
        return CreateUndefined(env);
    }
    isSupported = type == FeatureType::HCE;
    napi_value result;
    napi_get_boolean(env, isSupported, &result);
    return result;
}

napi_value HasHceCapability(napi_env env, napi_callback_info info)
{
    bool hasHceCapability = true;
    napi_value result;
    napi_get_boolean(env, hasHceCapability, &result);
    return result;
}

napi_value IsDefaultService(napi_env env, napi_callback_info cbinfo)
{
    bool isDefaultService = false;

    size_t argc = ARGV_NUM_2;
    napi_value argv[ARGV_NUM_2] = {0};
    napi_value thisVar = 0;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    ElementName element;
    std::string type;
    if (!CheckArgCountAndThrow(env, argc, ARGV_NUM_2) || !ParseElementName(env, element, argv[ARGV_INDEX_0]) ||
        !ParseString(env, type, argv[ARGV_INDEX_1])) {
        ErrorLog("IsDefaultService: parse args failed");
        return CreateUndefined(env);
    }
    if (type != KITS::TYPE_PAYMENT) {
        ErrorLog("IsDefaultService: unsupported card type");
        return CreateUndefined(env);
    }

    HceService hceService = HceService::GetInstance();
    int statusCode = hceService.IsDefaultService(element, type, isDefaultService);
    if (!CheckHceStatusCodeAndThrow(env, statusCode, "isDefaultService")) {
        ErrorLog("IsDefaultService, statusCode = %{public}d", statusCode);
        return CreateUndefined(env);
    }
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
    napi_create_string_utf8(env, abilityInfo.name.c_str(), NAPI_AUTO_LENGTH, &name);
    napi_set_named_property(env, result, "name", name);

    napi_value labelId;
    napi_create_int32(env, abilityInfo.labelId, &labelId);
    napi_set_named_property(env, result, "labelId", labelId);

    napi_value bundleName;
    napi_create_string_utf8(env, abilityInfo.bundleName.c_str(), NAPI_AUTO_LENGTH, &bundleName);
    napi_set_named_property(env, result, "bundleName", bundleName);

    napi_value iconId;
    napi_create_int32(env, abilityInfo.iconId, &iconId);
    napi_set_named_property(env, result, "iconId", iconId);
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
    int statusCode = hceService.GetPaymentServices(paymentAbilityInfos);
    DebugLog("GetPaymentServices ability size %{public}zu.", paymentAbilityInfos.size());

    if (!CheckHceStatusCodeAndThrow(env, statusCode, "getPaymentServices")) {
        ErrorLog("GetPaymentServices, statusCode = %{public}d", statusCode);
        return CreateUndefined(env);
    }
    napi_value result = nullptr;
    ConvertAbilityInfoVectorToJS(env, result, paymentAbilityInfos);
    DebugLog("GetPaymentServices ability end.");
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS