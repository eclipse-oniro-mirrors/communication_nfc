/*
* Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
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
#include "nfc_napi_controller_adapter.h"
#include "nfc_napi_controller_event.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace KITS {
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

static napi_value CreateEnumNfcState(napi_env env, napi_value exports)
{
    napi_value state_off = nullptr;
    napi_value state_turning_on = nullptr;
    napi_value state_on = nullptr;
    napi_value state_turning_off = nullptr;
    napi_create_int32(env, static_cast<int32_t>(NfcState::STATE_OFF), &state_off);
    napi_create_int32(env, static_cast<int32_t>(NfcState::STATE_TURNING_OFF), &state_turning_off);
    napi_create_int32(env, static_cast<int32_t>(NfcState::STATE_ON), &state_on);
    napi_create_int32(env, static_cast<int32_t>(NfcState::STATE_TURNING_ON), &state_turning_on);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("STATE_OFF", state_off),
        DECLARE_NAPI_STATIC_PROPERTY("STATE_TURNING_OFF", state_turning_off),
        DECLARE_NAPI_STATIC_PROPERTY("STATE_ON", state_on),
        DECLARE_NAPI_STATIC_PROPERTY("STATE_TURNING_ON", state_turning_on),
    };
    napi_value result = nullptr;
    napi_define_class(env, "NfcState", NAPI_AUTO_LENGTH, CreateEnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "NfcState", result);
    return exports;
}

static napi_value InitJs(napi_env env, napi_value exports)
{
    DebugLog("Init, nfc_napi_controller");
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("openNfc", OpenNfc), // @deprecated since 9
        DECLARE_NAPI_FUNCTION("enableNfc", EnableNfc),
        DECLARE_NAPI_FUNCTION("closeNfc", CloseNfc), // @deprecated since 9
        DECLARE_NAPI_FUNCTION("disableNfc", DisableNfc),
        DECLARE_NAPI_FUNCTION("getNfcState", GetNfcState),
        DECLARE_NAPI_FUNCTION("isNfcAvailable", IsNfcAvailable),
        DECLARE_NAPI_FUNCTION("isNfcOpen", IsNfcOpen),
        DECLARE_NAPI_FUNCTION("on", On),
        DECLARE_NAPI_FUNCTION("off", Off),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(napi_property_descriptor), desc));
    CreateEnumNfcState(env, exports);
    return exports;
}

static napi_module nfcControllerModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = NULL,
    .nm_register_func = InitJs,
    .nm_modname = "nfc.controller",
    .nm_priv = ((void *)0),
    .reserved = { 0 }
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&nfcControllerModule);
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
