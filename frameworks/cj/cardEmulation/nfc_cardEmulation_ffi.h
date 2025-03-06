/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#ifndef NFC_CARDEMULATION_FFI_H
#define NFC_CARDEMULATION_FFI_H

#include <cstdint>

#include "cj_ffi/cj_common_ffi.h"

namespace OHOS {
namespace NFC {
namespace KITS {
const int32_t ERR_NO_MEMORY = -2;

struct CArrUI8 {
    uint8_t* head;
    int64_t size;
};

extern "C" {
FFI_EXPORT int32_t FfiNfcCardEmulationisDefaultService(
    char* cBundleName, char* cAbilityName, char* cModuleName, char* cardTypeName, bool* ret);

FFI_EXPORT int32_t FfiNfcCardEmulationstart(
    char* cBundleName, char* cAbilityName, char* cModuleName, CArrString cAidList);

FFI_EXPORT int32_t FfiNfcCardEmulationOn(int8_t eventType, int64_t id);

FFI_EXPORT int32_t FfiNfcCardEmulationstop(char* cBundleName, char* cAbilityName, char* cModuleName);

FFI_EXPORT int32_t FfiNfcCardEmulationTransmit(CArrUI8 cResponseApdu);
}

} // namespace KITS
} // namespace NFC
} // namespace OHOS

#endif