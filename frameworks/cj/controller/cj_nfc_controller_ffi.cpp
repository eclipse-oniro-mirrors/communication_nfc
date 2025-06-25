/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "cj_nfc_controller.h"
#include "cj_nfc_controller_event.h"
#include "ffi_remote_data.h"

namespace OHOS {
namespace NFC {
namespace KITS {
extern "C" {
FFI_EXPORT bool FfiOHOSNfcControllerIsNfcOpen()
{
    return IsNfcOpen();
}

FFI_EXPORT int32_t FfiOHOSNfcControllerGetNfcState()
{
    return GetNfcState();
}

FFI_EXPORT void FfiOHOSNfcControllerOnStateChange(int64_t callbackId)
{
    return OnStateChange(callbackId);
}

FFI_EXPORT void FfiOHOSNfcControllerOffStateChange(int64_t callbackId)
{
    return OffStateChange(callbackId);
}

FFI_EXPORT void FfiOHOSNfcControllerOffAllStateChange()
{
    return OffAllStateChange();
}
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
