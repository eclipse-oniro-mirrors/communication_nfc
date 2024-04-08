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

#ifndef NFC_NOTIFICATION_PUBLISHER_H
#define NFC_NOTIFICATION_PUBLISHER_H

#include <mutex>
#include <string>

#include "nfc_service.h"

namespace OHOS {
namespace NFC {
namespace TAG {
enum NfcNotificationId : int {
    NFC_TAG_DEFAULT_NOTIFICATION_ID = 114000,
    NFC_BT_NOTIFICATION_ID = 114001,
    NFC_WIFI_NOTIFICATION_ID = 114002,
    NFC_TRANSPORT_CARD_NOTIFICATION_ID = 114003,
    NFC_BROWSER_NOTIFICATION_ID = 114004,
    NFC_HCE_AID_CONFLICTED_ID = 114005,
};

typedef void (NfcNtfCallback)(int notificationId);

struct NfcNtfInterface {
    void (*publishNotification)(int notificationId, const std::string &name, int balance);
    void (*regNtfCallback)(NfcNtfCallback *callback);
};

class NfcNotificationPublisher {
public:
    static NfcNotificationPublisher& GetInstance(void);

    void PublishNfcNotification(int notificationId, const std::string &name, int balance);
    void RegNotificationCallback(std::weak_ptr<NfcService> nfcService);
    void OnNotificationButtonClicked(int notificationId);

private:
    NfcNotificationPublisher();
    ~NfcNotificationPublisher();
    NfcNotificationPublisher(const NfcNotificationPublisher&) = delete;
    NfcNotificationPublisher& operator=(const NfcNotificationPublisher&) = delete;

    void UnloadNfcNtfLib();
    void InitNfcNtfLib();

    const std::string NFC_NTF_LIB_PATH = "libnfc_notification.z.so";
    const std::string REG_NFC_CALLBACK_FUNC_NAME = "RegNotificationCallback";
    const std::string PUBLISH_NTF_FUNC_NAME = "PublishNfcNotification";

    std::mutex mutex_ {};
    bool isNtfLibLoaded_ = false;
    void *nfcNtfHandle_ {};
    NfcNtfInterface nfcNtfInf_ {};
    bool isInitialized_ = false;
    std::weak_ptr<NfcService> nfcService_;
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_NOTIFICATION_PUBLISHER_H
