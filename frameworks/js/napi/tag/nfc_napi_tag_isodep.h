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

#ifndef NFC_NAPI_TAG_ISODEP_H
#define NFC_NAPI_TAG_ISODEP_H

#include "isodep_tag.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nfc_napi_tag_sesstion.h"
#include "nfc_napi_tag_utils.h"

namespace OHOS {
namespace NFC {
namespace KITS {
struct NapiIsoDepTag : public NapiNfcTagSession {
    static napi_value GetHistoricalBytes(napi_env env, napi_callback_info info);
    static napi_value GetHiLayerResponse(napi_env env, napi_callback_info info);
    static napi_value IsExtendedApduSupported(napi_env env, napi_callback_info info);
};
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif