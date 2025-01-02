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

#include "nfc_napi_tag_barcode.h"

#include "barcode_tag.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
static void NativeGetBarcode(napi_env env, void *data)
{
    auto context = static_cast<TagBarcodeContext<std::string, NapiTagBarcode> *>(data);
    context->value = "";
    context->errorCode = ERR_NONE;

    BarcodeTag *barcodeTagPtr = static_cast<BarcodeTag *>(static_cast<void *>(context->objectInfo->tagSession.get()));
    if (barcodeTagPtr != nullptr) {
        std::string barcodeData = barcodeTagPtr->GetBarcode();
        std::vector<unsigned char> barcodeDataBytes;
        NfcSdkCommon::HexStringToBytes(barcodeData, barcodeDataBytes);
        context->barcodeDataBytes = barcodeDataBytes;
    } else {
        ErrorLog("tagSession nullptr.");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_TAG_STATE_INVALID,
            BuildErrorMessage(BUSI_ERR_TAG_STATE_INVALID, "", "", "", "")));
        return;
    }
    context->resolved = true;
}

static void GetBarcodeCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<TagBarcodeContext<std::string, NapiTagBarcode> *>(data);
    napi_value arrBuffer = nullptr;
    void *buffer = nullptr;
    napi_create_arraybuffer(env, context->barcodeDataBytes.size() * sizeof(int32_t), &buffer, &arrBuffer);
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE && buffer != nullptr) {
        int32_t *i32Buffer = reinterpret_cast<int32_t *>(buffer);
        for (uint8_t i = 0; i < context->barcodeDataBytes.size(); i++) {
            i32Buffer[i] = context->barcodeDataBytes[i];
        }
        DoAsyncCallbackOrPromise(env, context, arrBuffer);
    } else {
        int errCode = BuildOutputErrorCode(context->errorCode);
        std::string msg = BuildErrorMessage(errCode, "GetBarcode", TAG_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, msg);
    }
}

napi_value NapiTagBarcode::GetBarcode(napi_env env, napi_callback_info info)
{
    InfoLog("NapiGetBarcode start.");
    napi_value thisVar = nullptr;
    std::size_t argc = ARGV_NUM_0;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr));
    NapiTagBarcode *objectInfo = nullptr;

    // unwrap from thisVar to retrieve the native instance
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&objectInfo));
    if (status != napi_ok || objectInfo == nullptr || objectInfo->tagSession == nullptr) {
        ErrorLog("napi_unwrap failed, object is null.");
        return CreateUndefined(env);
    }
    auto context = std::make_unique<TagBarcodeContext<std::string, NapiTagBarcode>>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        ErrorLog("context nullptr.");
        return CreateUndefined(env);
    }
    context->objectInfo = objectInfo;
    napi_value result = HandleAsyncWork(env, context, "GetBarcode", NativeGetBarcode, GetBarcodeCallback);
    return result;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
