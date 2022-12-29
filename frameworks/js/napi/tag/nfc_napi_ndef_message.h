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

#ifndef NFC_NAPI_NDEF_MESSAGE_H_
#define NFC_NAPI_NDEF_MESSAGE_H_

#include <locale>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "ndef_tag.h"
#include "ndef_message.h"
#include "nfc_napi_tag_sesstion.h"
#include "nfc_napi_tag_utils.h"

namespace OHOS {
namespace NFC {
namespace KITS {
struct NapiNdefMessage {
    static napi_value GetNdefRecords(napi_env env, napi_callback_info info);
    static napi_value MakeUriRecord(napi_env env, napi_callback_info info);
    static napi_value MakeTextRecord(napi_env env, napi_callback_info info);
    static napi_value MakeMimeRecord(napi_env env, napi_callback_info info);
    static napi_value MakeExternalRecord(napi_env env, napi_callback_info info);
    static napi_value MessageToBytes(napi_env env, napi_callback_info info);
    std::shared_ptr<NdefMessage> ndefMessage = nullptr;
};
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif