/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef NFC_NOTIFICATION_H
#define NFC_NOTIFICATION_H

#include <mutex>
#include <string>

#include "image_source.h"
#include "pixel_map.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef void (*NfcNtfCallback)(int notificationId);
void RegNotificationCallback(NfcNtfCallback callback);
void PublishNfcNotification(bool isNfcNotDisturb, int notificationId, const std::string &name, int balance);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

namespace OHOS {
namespace NFC {
namespace TAG {
class NfcNotification {
public:
    static NfcNotification& GetInstance(void);

    void PublishNfcNotification(bool isNfcNotDisturb, int notificationId, const std::string &name, int balance);
    void RegNotificationCallback(NfcNtfCallback callback);

private:
    NfcNotification();
    ~NfcNotification();
    NfcNotification(const NfcNotification&) = delete;
    NfcNotification& operator=(const NfcNotification&) = delete;

    void GetPixelMap(const std::string &path);

    std::mutex mutex_ {};
    std::shared_ptr<Media::PixelMap> nfcIconPixelMap_ {};
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_NOTIFICATION_H
