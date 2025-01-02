/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef NFC_NAPI_TAG_BARCODE_H
#define NFC_NAPI_TAG_BARCODE_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nfc_napi_tag_session.h"
#include "nfc_napi_common_utils.h"

namespace OHOS {
namespace NFC {
namespace KITS {
struct NapiTagBarcode : public NapiNfcTagSession {
    static napi_value GetBarcode(napi_env env, napi_callback_info info);
};

template<typename T, typename D>
struct TagBarcodeContext : BaseContext {
    T value;
    D *objectInfo;
    std::vector<unsigned char> barcodeDataBytes;
};
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif // NFC_NAPI_TAG_BARCODE_H