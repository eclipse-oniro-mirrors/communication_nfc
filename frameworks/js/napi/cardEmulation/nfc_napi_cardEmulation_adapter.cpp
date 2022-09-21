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

#include "nfc_napi_cardEmulation_adapter.h"

#include <vector>

#include "loghelper.h"
#include "nfc_controller.h"
#include "nfc_sdk_common.h"
#include "cardEmulation.h"

namespace OHOS {
namespace NFC {
namespace KITS {

napi_value IsSupported(napi_env env, napi_callback_info info)
{
    DebugLog("nfc_napi_cardEmulation_adapter::Issupported");
    bool ispt = false;
    napi_value result;
    napi_get_boolean(env, ispt, &result);
    return result;
}

}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
