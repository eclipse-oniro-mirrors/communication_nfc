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

#include "loghelper.h"
#include "nfc_controller.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace KITS {
bool IsNfcOpen()
{
    NfcController nfcCtrl = OHOS::NFC::KITS::NfcController::GetInstance();
    bool isOpen = false;
    int statusCode = nfcCtrl.IsNfcOpen(isOpen);
    if (statusCode != KITS::ERR_NONE) {
        ErrorLog("IsNfcOpen, statusCode = %{public}d", statusCode);
    }
    return isOpen;
}

int32_t GetNfcState()
{
    NfcController nfcCtrl = OHOS::NFC::KITS::NfcController::GetInstance();
    return nfcCtrl.GetNfcState();
}

} // namespace KITS
} // namespace NFC
} // namespace OHOS
