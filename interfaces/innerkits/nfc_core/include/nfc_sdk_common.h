/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#ifndef NFC_SDK_COMMON_H
#define NFC_SDK_COMMON_H

#include <string>

namespace OHOS {
namespace NFC {
namespace KITS {
enum NfcErrorCode : const int {
    NFC_SUCCESS = 0,

    // SDK ERROR CODE
    NFC_SDK_ERROR_NOT_INITIALIZED = 0x00000100,
    NFC_SDK_ERROR_INVALID_PARAM,
    NFC_SDK_ERROR_TAG_NOT_CONNECT,
    NFC_SDK_ERROR_TAG_INVALID,
    NFC_SDK_ERROR_NOT_NDEF_TAG,
    NFC_SDK_ERROR_NFC_STATE_INVALID,
    NFC_SDK_ERROR_DEVICE_NOT_SUPPORT_NFC,
    NFC_SDK_ERROR_DISABLE_MAKE_READONLY,
    NFC_SDK_ERROR_UNKOWN,
    NFC_SDK_ERROR_PERMISSION,
    NFC_SDK_ERROR_CARDEMULATION_CONTROLLER_NOT_INITIALIZED,
    NFC_SDK_ERROR_CARDEMULATION_HANDLER_IS_NULL,

    // SERVICE ERROR CODE
    NFC_SER_ERROR_NOT_INITIALIZED = 0x00000200,
    NFC_SER_ERROR_DISCONNECT,
    NFC_SER_ERROR_IO,
    NFC_SER_ERROR_INVALID_PARAM
};

enum NfcState { STATE_OFF = 1, STATE_TURNING_ON = 2, STATE_ON = 3, STATE_TURNING_OFF = 4 };
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_SDK_COMMON_H
