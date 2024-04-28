/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "nfc_event_publisher.h"
#include "nfc_sdk_common.h"
#include "loghelper.h"
#include "want.h"
#include "common_event_manager.h"

namespace OHOS {
namespace NFC {
void NfcEventPublisher::PublishNfcStateChanged(int newState)
{
    // notify the common event for nfc state changed.
    AAFwk::Want want;
    want.SetAction(KITS::COMMON_EVENT_NFC_ACTION_STATE_CHANGED);
    want.SetParam(KITS::NFC_EXTRA_STATE, newState);
    EventFwk::CommonEventData data;
    data.SetWant(want);
    data.SetCode(newState);
    EventFwk::CommonEventManager::PublishCommonEvent(data);
}

void NfcEventPublisher::PublishNfcFieldStateChanged(bool isFieldOn)
{
    // notify the common event for field on/off.
    AAFwk::Want want;
    if (isFieldOn) {
        want.SetAction(KITS::COMMON_EVENT_NFC_ACTION_RF_FIELD_ON_DETECTED);
    } else {
        want.SetAction(KITS::COMMON_EVENT_NFC_ACTION_RF_FIELD_OFF_DETECTED);
    }
    EventFwk::CommonEventData data;
    data.SetWant(want);
    EventFwk::CommonEventManager::PublishCommonEvent(data);
}
} // NFC
} // OHOS