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
#include "cardEmulation.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace KITS {

CardEmulation::CardEmulation()
{
    DebugLog("[cardEmulation::cardEmulation] new ability manager");
}

CardEmulation::~CardEmulation()
{
    DebugLog("destruct cardEmulation");
}

CardEmulation &CardEmulation::GetInstance()
{
    static CardEmulation instance;
    return instance;
}

bool CardEmulation::IsSupported(FeatureType feature)
{
    DebugLog("cardEmulation::IsSupported in.");
    switch (feature) {
        case HCE: {
            DebugLog("cardEmulation::HCE card emulation is supported.");
            break;
        }
        case UICC: {
            DebugLog("cardEmulation::UICC card emulation is supported.");
            break;
        }
        case ESE: {
            DebugLog("cardEmulation::ESE card emulation is supported.");
            break;
        }
        default:
            DebugLog("cardEmulation:: card emulation is not supported.");
            return false;
    }
    return true;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS