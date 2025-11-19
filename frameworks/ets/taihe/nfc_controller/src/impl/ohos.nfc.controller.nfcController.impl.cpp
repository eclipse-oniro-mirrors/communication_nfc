/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ohos.nfc.controller.nfcController.proj.hpp"
#include "ohos.nfc.controller.nfcController.impl.hpp"
#include "taihe/runtime.hpp"

#include "loghelper.h"
#include "nfc_controller.h"
#include "nfc_taihe_controller_event.h"

using namespace taihe;
using namespace ohos::nfc::controller::nfcController;

namespace {
void onNfcStateChange(callback_view<void(NfcState)> callback)
{
    InfoLog("onNfcStateChange, enter");
    OHOS::NFC::KITS::NfcStateEventRegister::GetInstance().Register(callback);
}

void offNfcStateChange(optional_view<callback<void(NfcState)>> callback)
{
    InfoLog("offNfcStateChange, enter");
    OHOS::NFC::KITS::NfcStateEventRegister::GetInstance().Unregister();
}

void enableNfc()
{
    InfoLog("enableNfc, enter");
    OHOS::NFC::KITS::NfcController::GetInstance().TurnOn();
}

void disableNfc()
{
    InfoLog("disableNfc, enter");
    OHOS::NFC::KITS::NfcController::GetInstance().TurnOff();
}

bool isNfcOpen()
{
    InfoLog("isNfcOpen, enter");
    bool isOpen = false;
    int statusCode = OHOS::NFC::KITS::NfcController::GetInstance().IsNfcOpen(isOpen);
    if (statusCode != OHOS::NFC::KITS::ERR_NONE) {
        ErrorLog("isNfcOpen, statusCode = %{public}d", statusCode);
    }
    return isOpen;
}

NfcState getNfcState()
{
    InfoLog("getNfcState, enter");
    int nfcState = OHOS::NFC::KITS::NfcController::GetInstance().GetNfcState();
    return NfcState::from_value(nfcState);
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_onNfcStateChange(onNfcStateChange);
TH_EXPORT_CPP_API_offNfcStateChange(offNfcStateChange);
TH_EXPORT_CPP_API_enableNfc(enableNfc);
TH_EXPORT_CPP_API_disableNfc(disableNfc);
TH_EXPORT_CPP_API_isNfcOpen(isNfcOpen);
TH_EXPORT_CPP_API_getNfcState(getNfcState);
// NOLINTEND
 