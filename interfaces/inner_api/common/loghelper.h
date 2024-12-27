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

#ifdef LOG_DOMAIN
#undef LOG_DOMAIN
#endif
#define LOG_DOMAIN 0xD000301

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "Nfc_Core"

#define FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define FatalLog(fmt, ...) HILOG_FATAL( \
    LOG_CORE, "[%{public}s(%{public}s:%{public}d)]" fmt, FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define ErrorLog(fmt, ...) HILOG_ERROR( \
    LOG_CORE, "[%{public}s(%{public}s:%{public}d)]" fmt, FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WarnLog(fmt, ...) HILOG_WARN(  \
    LOG_CORE, "[%{public}s(%{public}s:%{public}d)]" fmt, FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define InfoLog(fmt, ...) HILOG_INFO(  \
    LOG_CORE, "[%{public}s(%{public}s:%{public}d)]" fmt, FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define DebugLog(fmt, ...) HILOG_DEBUG( \
    LOG_CORE, "[%{public}s(%{public}s:%{public}d)]" fmt, FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else

#define FatalLog(...)
#define ErrorLog(...)
#define WarnLog(...)
#define InfoLog(...)
#define DebugLog(...)
#endif  // DEBUG

#endif // LOG_HELPER_H
