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

#include "nfc_napi_cardEmulation_adapter.h"
#include "cardEmulation.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {

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
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS