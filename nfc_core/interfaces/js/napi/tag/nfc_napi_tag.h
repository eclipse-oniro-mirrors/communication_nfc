/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef NFC_NAPI_TAG_H_
#define NFC_NAPI_TAG_H_

#include "nfc_napi_tag_sesstion.h"
#include "taginfo.h"

namespace OHOS {
namespace NFC {
namespace KITS {
napi_value RegisternfcATagObject(napi_env env, napi_value exports);
napi_value GetNfcATag(napi_env env, napi_callback_info info);
napi_value JS_Constructor(napi_env env, napi_callback_info cbinfo);
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif
