/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#ifndef NFC_TIMER_H
#define NFC_TIMER_H
#include <functional>
#include "common_timer_errors.h"
#include "timer.h"

namespace OHOS {
namespace NFC {
using TimeOutCallback = std::function<void()>;
const uint32_t TIMEOUT_UNLOAD_NFC_SA = 5 * 60 * 1000; // ms
const uint32_t TIMEOUT_UNLOAD_NFC_SA_AFTER_GET_STATE = 10 * 1000; // ms

class NfcTimer {
public:
    using TimerCallback = std::function<void()>;
    static constexpr uint32_t DEFAULT_TIMEROUT = 10000; // ms
    static NfcTimer *GetInstance(void);

    NfcTimer();
    ~NfcTimer();

    void Register(const TimerCallback &callback, uint32_t &outTimerId, uint32_t interval = DEFAULT_TIMEROUT);
    void UnRegister(uint32_t timerId);

private:
    std::unique_ptr<Utils::Timer> timer_{nullptr};
};
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_TIMER_H