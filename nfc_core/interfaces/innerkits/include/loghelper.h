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
#ifndef LOG_HELPER_H
#define LOG_HELPER_H

#ifdef DEBUG
#include "hilog/log.h"

#ifdef FatalLog
#undef FatalLog
#endif

#ifdef ErrorLog
#undef ErrorLog
#endif

#ifdef WarnLog
#undef WarnLog
#endif

#ifdef InfoLog
#undef InfoLog
#endif

#ifdef DebugLog
#undef DebugLog
#endif

#ifndef NFC_LOG_DOMAIN
#define NFC_LOG_DOMAIN 0xD000301
#endif

#ifndef NFC_LOG_TAG
#define NFC_LOG_TAG "Nfc_Core"
#endif

#ifdef LOG_LABEL
#undef LOG_LABEL
#endif

static constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = {LOG_CORE, NFC_LOG_DOMAIN, NFC_LOG_TAG};

#define FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define FatalLog(fmt, ...)               \
    (void)OHOS::HiviewDFX::HiLog::Fatal( \
        LOG_LABEL, "[%{public}s(%{public}s:%{public}d)]" fmt, FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define ErrorLog(fmt, ...)               \
    (void)OHOS::HiviewDFX::HiLog::Error( \
        LOG_LABEL, "[%{public}s(%{public}s:%{public}d)]" fmt, FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WarnLog(fmt, ...)                \
    (void)OHOS::HiviewDFX::HiLog::Warn(  \
        LOG_LABEL, "[%{public}s(%{public}s:%{public}d)]" fmt, FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define InfoLog(fmt, ...)                \
    (void)OHOS::HiviewDFX::HiLog::Info(  \
        LOG_LABEL, "[%{public}s(%{public}s:%{public}d)]" fmt, FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define DebugLog(fmt, ...)               \
    (void)OHOS::HiviewDFX::HiLog::Debug( \
        LOG_LABEL, "[%{public}s(%{public}s:%{public}d)]" fmt, FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else

#define FatalLog(...)
#define ErrorLog(...)
#define WarnLog(...)
#define InfoLog(...)
#define DebugLog(...)
#endif  // DEBUG

#endif // LOG_HELPER_H
