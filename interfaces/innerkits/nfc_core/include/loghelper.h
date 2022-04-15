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

#include <cstdio>
#include <ctime>

#define GET_TIME()                                                \
    do {                                                          \
        struct timespec xTime;                                    \
        clock_gettime(CLOCK_REALTIME, &xTime);                    \
        printf("%lld%06ld ", xTime.tv_sec, xTime.tv_nsec / 1000); \
    } while (0)
#ifdef DEBUG
#define InfoLog(format, ...)                                                   \
    do {                                                                       \
        GET_TIME();                                                            \
        printf(__FILE__ "(%05d) INFO: " format "\n", __LINE__, ##__VA_ARGS__); \
    } while (0)
#define DebugLog(format, ...)                                                   \
    do {                                                                        \
        GET_TIME();                                                             \
        printf(__FILE__ "(%05d) DEBUG: " format "\n", __LINE__, ##__VA_ARGS__); \
    } while (0)
#define WarnLog(format, ...)                                                   \
    do {                                                                       \
        GET_TIME();                                                            \
        printf(__FILE__ "(%05d) WARN: " format "\n", __LINE__, ##__VA_ARGS__); \
    } while (0)
#define ErrorLog(format, ...)                                                   \
    do {                                                                        \
        GET_TIME();                                                             \
        printf(__FILE__ "(%05d) ERROR: " format "\n", __LINE__, ##__VA_ARGS__); \
    } while (0)
#else
#define InfoLog(format, ...)                                                   \
    do {                                                                       \
        GET_TIME();                                                            \
        printf(__FILE__ "(%05d) INFO: " format "\n", __LINE__, ##__VA_ARGS__); \
    } while (0)
#define DebugLog(format, ...)
#define WarnLog(format, ...)
#define ErrorLog(format, ...)                                                   \
    do {                                                                        \
        GET_TIME();                                                             \
        printf(__FILE__ "(%05d) ERROR: " format "\n", __LINE__, ##__VA_ARGS__); \
    } while (0)
#endif
#endif // LOG_HELPER_H
