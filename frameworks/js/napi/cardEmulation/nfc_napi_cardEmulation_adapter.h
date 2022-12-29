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

#ifndef NFC_NAPI_CARDEMULATION_H
#define NFC_NAPI_CARDEMULATION_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace NFC {
namespace KITS {

napi_value IsSupported(napi_env env, napi_callback_info info);
napi_value HasHceCapability(napi_env env, napi_callback_info info);
napi_value IsDefaultService(napi_env env, napi_callback_info info);

}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif