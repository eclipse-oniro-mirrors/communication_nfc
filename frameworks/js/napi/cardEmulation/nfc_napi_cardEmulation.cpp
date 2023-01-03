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
#include "loghelper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nfc_napi_cardEmulation_adapter.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace KITS {
const std::string FEATURE_TYPE = "FeatureType";
const std::string CARD_TYPE = "CardType";

/*
 * Module initialization function
 */
static napi_value CreateEnumConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisArg = nullptr;
    void *data = nullptr;

    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);
    napi_value global = nullptr;
    napi_get_global(env, &global);
    return thisArg;
}

// @deprecated since 9
static napi_value CreateEnumFeatureType(napi_env env, napi_value exports)
{
    napi_value hce = nullptr;
    napi_value uicc = nullptr;
    napi_value ese = nullptr;
    napi_create_int32(env, static_cast<int32_t>(FeatureType::HCE), &hce);
    napi_create_int32(env, static_cast<int32_t>(FeatureType::UICC), &uicc);
    napi_create_int32(env, static_cast<int32_t>(FeatureType::ESE), &ese);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("HCE", hce),
        DECLARE_NAPI_STATIC_PROPERTY("UICC", uicc),
        DECLARE_NAPI_STATIC_PROPERTY("ESE", ese),
    };
    napi_value result = nullptr;
    napi_define_class(env, FEATURE_TYPE.c_str(), NAPI_AUTO_LENGTH, CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, FEATURE_TYPE.c_str(), result);
    return exports;
}

static napi_value CreateEnumCardType(napi_env env, napi_value exports)
{
    napi_value payment = nullptr;
    napi_value other = nullptr;
    napi_create_string_utf8(env, KITS::TYPE_PAYMENT.c_str(), KITS::TYPE_PAYMENT.length(), &payment);
    napi_create_string_utf8(env, KITS::TYPE_OHTER.c_str(), KITS::TYPE_OHTER.length(), &other);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("PAYMENT", payment),
        DECLARE_NAPI_STATIC_PROPERTY("OTHER", other),
    };
    napi_value result = nullptr;
    napi_define_class(env, CARD_TYPE.c_str(), NAPI_AUTO_LENGTH, CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(desc[0]), desc, &result);
    napi_set_named_property(env, exports, CARD_TYPE.c_str(), result);
    return exports;
}

static napi_value InitJs(napi_env env, napi_value exports)
{
    DebugLog("Init, nfc_napi_cardEmulation");
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("isSupported", IsSupported),
        DECLARE_NAPI_FUNCTION("hasHceCapability", HasHceCapability),
        DECLARE_NAPI_FUNCTION("isDefaultService", IsDefaultService),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(napi_property_descriptor), desc));
    CreateEnumFeatureType(env, exports);
    CreateEnumCardType(env, exports);
    return exports;
}

static napi_module cardEmulationModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = NULL,
    .nm_register_func = InitJs,
    .nm_modname = "nfc.cardEmulation",
    .nm_priv = ((void *)0),
    .reserved = { 0 }
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&cardEmulationModule);
}

}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS

