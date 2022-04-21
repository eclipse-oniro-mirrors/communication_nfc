/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "nfc_napi_adapter.h"
#include "nfc_napi_event.h"
#include "log.h"

namespace OHOS {
namespace ConnectedTag {
/*
 * Module initialization function
 */
static napi_value InitJs(napi_env env, napi_value exports)
{
    HILOGI("Init, nfc_napi_entry");

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("init", Init),
        DECLARE_NAPI_FUNCTION("uninit", Uninit),
        DECLARE_NAPI_FUNCTION("readNdefTag", ReadNdefTag),
        DECLARE_NAPI_FUNCTION("writeNdefTag", WriteNdefTag),
        DECLARE_NAPI_FUNCTION("on", On),
        DECLARE_NAPI_FUNCTION("off", Off),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(napi_property_descriptor), desc));
    return exports;
}

static napi_module nfcConnectedTagModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = NULL,
    .nm_register_func = InitJs,
    .nm_modname = "connectedTag",
    .nm_priv = ((void *)0),
    .reserved = { 0 }
};

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&nfcConnectedTagModule);
}
}  // namespace ConnectedTag
}  // namespace OHOS
