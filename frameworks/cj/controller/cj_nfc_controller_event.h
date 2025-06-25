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

#ifndef CJ_NFC_CONTROLLER_EVENT_H
#define CJ_NFC_CONTROLLER_EVENT_H

#include <map>
#include <set>
#include <shared_mutex>
#include <string>
#include <vector>

#include "infc_controller_callback.h"
#include "nfc_sdk_common.h"
#include "singleton.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class EventRegister : public OHOS::DelayedSingleton<EventRegister> {
public:
    EventRegister() {}
    ~EventRegister() {}

    void Register(const std::string& type, int64_t callbackId);
    void Unregister(const std::string& type, int64_t callbackId);
    void Unregister(const std::string& type);
    ErrorCode RegisterNfcStateChangedEvents(const std::string& type);

private:
    ErrorCode UnRegisterNfcEvents(const std::string& type);
    bool IsEventSupport(const std::string& type);
};

void OnStateChange(int64_t callbackId);
void OffStateChange(int64_t callbackId);
void OffAllStateChange();
} // namespace KITS
} // namespace NFC
} // namespace OHOS

#endif
