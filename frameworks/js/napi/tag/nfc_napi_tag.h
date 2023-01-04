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

#ifndef NFC_NAPI_TAG_H_
#define NFC_NAPI_TAG_H_

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nfc_napi_tag_sesstion.h"
#include "nfc_napi_taga.h"
#include "nfc_napi_tagb.h"
#include "nfc_napi_tagf.h"
#include "nfc_napi_tagv.h"
#include "nfc_napi_tag_isodep.h"
#include "nfc_napi_tag_mifare_classic.h"
#include "nfc_napi_tag_mifare_ul.h"
#include "nfc_napi_tag_ndef.h"
#include "nfc_napi_tag_ndef_formatable.h"
#include "nfc_napi_tag_utils.h"
#include "taginfo.h"

namespace OHOS {
namespace NFC {
namespace KITS {
napi_value RegisternfcATagObject(napi_env env, napi_value exports);
napi_value GetSpecificTagObj(napi_env env, napi_callback_info info, napi_ref ref);
napi_value RegisternfcBTagObject(napi_env env, napi_value exports);
napi_value RegisternfcFTagObject(napi_env env, napi_value exports);
napi_value RegisternfcVTagObject(napi_env env, napi_value exports);
napi_value RegisterIsoDepTagObject(napi_env env, napi_value exports);
napi_value RegisterNdefTagObject(napi_env env, napi_value exports);
napi_value RegisterMifareClassicTagObject(napi_env env, napi_value exports);
napi_value RegisterMifareUltralightTagObject(napi_env env, napi_value exports);
napi_value RegisterNdefFormatableTagObject(napi_env env, napi_value exports);
napi_value GetNfcATag(napi_env env, napi_callback_info info);
napi_value GetNfcBTag(napi_env env, napi_callback_info info);
napi_value GetNfcFTag(napi_env env, napi_callback_info info);
napi_value GetNfcVTag(napi_env env, napi_callback_info info);
napi_value GetIsoDepTag(napi_env env, napi_callback_info info);
napi_value GetNdefTag(napi_env env, napi_callback_info info);
napi_value GetMifareClassicTag(napi_env env, napi_callback_info info);
napi_value GetMifareUltralightTag(napi_env env, napi_callback_info info);
napi_value GetNdefFormatableTag(napi_env env, napi_callback_info info);
napi_value JS_Constructor(napi_env env, napi_callback_info cbinfo);
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif
