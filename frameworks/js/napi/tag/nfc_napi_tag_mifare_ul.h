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

#ifndef NFC_NAPI_TAG_MIFARE_UL_H_
#define NFC_NAPI_TAG_MIFARE_UL_H_

#include <locale>
#include "mifare_ultralight_tag.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nfc_napi_tag_sesstion.h"
#include "nfc_napi_tag_utils.h"

namespace OHOS {
namespace NFC {
namespace KITS {
struct NapiMifareUltralightTag : public NapiNfcTagSession {
    static napi_value ReadMultiplePages(napi_env env, napi_callback_info info);
    static napi_value WriteSinglePage(napi_env env, napi_callback_info info);
    static napi_value GetType(napi_env env, napi_callback_info info);
};

template<typename T, typename D>
struct MifareUltralightContext : BaseContext {
    T value;
    D *objectInfo;
    int pageIndex;
    std::string data;
};
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif