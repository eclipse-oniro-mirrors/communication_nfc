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
#include "synchronize_event.h"

#include <cerrno>
#include <cstring>

#include "loghelper.h"

namespace OHOS {
namespace NFC {
static const int MILLISECOND_PER_SECOND = 1000;
static const int NANOSECOND_PER_MILLISECOND = 1000000;
static const int NANOSECOND_PER_SECOND = 1000000000;

SynchronizeEvent::SynchronizeEvent()
{
    pthread_mutex_init(&lock_, nullptr);
    pthread_condattr_t attr;
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    pthread_cond_init(&cond_, &attr);
}

SynchronizeEvent::~SynchronizeEvent()
{
    pthread_mutex_destroy(&lock_);
    pthread_cond_destroy(&cond_);
}

void SynchronizeEvent::Start()
{
    int res = pthread_mutex_lock(&lock_);
    if (res != 0) {
        DebugLog("SynchronizeEvent::start: fail lock; error=0x%{public}X", res);
    }
}

void SynchronizeEvent::End()
{
    int res = pthread_mutex_unlock(&lock_);
    if (res != 0) {
        DebugLog("SynchronizeEvent::end: fail unlock; error=0x%{public}X", res);
    }
}

void SynchronizeEvent::Wait()
{
    int const res = pthread_cond_wait(&cond_, &lock_);
    if (res) {
        DebugLog("CondVar::wait: fail wait; error=0x%{public}X", res);
    }
}

bool SynchronizeEvent::Wait(long ms)
{
    bool retVal = false;
    struct timespec absoluteTime;

    if (clock_gettime(CLOCK_MONOTONIC, &absoluteTime) == -1) {
        DebugLog("SynchronizeEvent::wait: fail get time");
    } else {
        absoluteTime.tv_sec += ms / MILLISECOND_PER_SECOND;
        long ns = absoluteTime.tv_nsec + ((ms % MILLISECOND_PER_SECOND) * NANOSECOND_PER_MILLISECOND);
        if (ns > NANOSECOND_PER_SECOND) {
            absoluteTime.tv_sec++;
            absoluteTime.tv_nsec = ns - NANOSECOND_PER_SECOND;
        } else {
            absoluteTime.tv_nsec = ns;
        }
    }

    int waitResult = pthread_cond_timedwait(&cond_, &lock_, &absoluteTime);
    if ((waitResult != 0) && (waitResult != ETIMEDOUT)) {
        DebugLog("SynchronizeEvent::wait: fail timed wait; error=0x%{public}X", waitResult);
    }
    retVal = (waitResult == 0);  // waited successfully
    return retVal;
}

void SynchronizeEvent::NotifyOne()
{
    int const res = pthread_cond_signal(&cond_);
    if (res) {
        DebugLog("SynchronizeEvent::notifyOne: fail signal; error=0x%{public}X", res);
    }
}
}  // namespace NFC
}  // namespace OHOS
