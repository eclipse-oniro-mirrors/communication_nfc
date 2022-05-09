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
#ifndef SYNCHRONIZE_EVENT_H
#define SYNCHRONIZE_EVENT_H
#include <pthread.h>

namespace OHOS {
namespace NFC {
class SynchronizeEvent {
public:
    SynchronizeEvent();
    ~SynchronizeEvent();

    void Start();
    void End();
    void Wait();
    bool Wait(long ms);
    void NotifyOne();

private:
    pthread_mutex_t lock_;
    pthread_cond_t cond_;
};

class SynchronizeGuard {
public:
    explicit SynchronizeGuard(SynchronizeEvent& event) : syncEvent_(event)
    {
        event.Start();
    };

    ~SynchronizeGuard()
    {
        syncEvent_.End();
    };

private:
    SynchronizeEvent& syncEvent_;
};
}  // namespace NFC
}  // namespace OHOS
#endif  // SYNCHRONIZE_EVENT_H
