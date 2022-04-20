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
#include "nfc_watch_dog.h"

#include <chrono>

#include "loghelper.h"

namespace OHOS {
namespace NFC {
NfcWatchDog::NfcWatchDog(const std::string& threadName, int timeout, std::weak_ptr<NCI::INfccHost> nfccHost)
    : threadName_(threadName), timeout_(timeout), canceled_(false), thread_(nullptr), nfccHost_(nfccHost)
{
}

NfcWatchDog::~NfcWatchDog()
{
    if (thread_ && thread_->joinable()) {
        conditionVariable_.notify_one();
        thread_->join();
    }
}

void NfcWatchDog::MainLoop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    conditionVariable_.wait_for(lock, std::chrono::milliseconds(timeout_), [this] { return canceled_; });
    if (canceled_) {
        return;
    }
    // If Routing Wake Lock is held, Routing Wake Lock release. Watchdog triggered, release lock before aborting.
    InfoLog("Watchdog triggered, aborting.");
    nfccHost_.lock()->Abort();
}

void NfcWatchDog::Run()
{
    thread_ = std::make_unique<std::thread>(&NfcWatchDog::MainLoop, this);
}

void NfcWatchDog::Cancel()
{
    std::unique_lock<std::mutex> lock(mutex_);
    canceled_ = true;
    conditionVariable_.notify_one();
}
}  // namespace NFC
}  // namespace OHOS
