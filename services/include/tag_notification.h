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

#ifndef TAG_NOTIFICATION_H
#define TAG_NOTIFICATION_H

#include <string>

namespace OHOS {
namespace NFC {
namespace TAG {
enum NfcNotificationId {
    NFC_TAG_NOTIFICATION_ID = 114000,
    NFC_BT_NOTIFICATION_ID = 114001,
    NFC_WIFI_NOTIFICATION_ID = 114002,
    NFC_TRAFFIC_CARD_NOTIFICATION_ID = 114003,
};

static const int NFC_UNIT_CHANGE_CONSTANT = 100;
static const int NTF_AUTO_DELETE_TIME = 1000;

static const std::string NFC_TAG_DEFAULT_NTF_TITLE  = "NFC found tag, click to start app";
static const std::string NFC_TAG_DEFAULT_NTF_TEXT   = "Some apps need to turn on nfc inner switch";
static const std::string NFC_TRAFFIC_CARD_NTF_TITLE = "NFC found traffic card";

static const std::string NFC_ACTION_BUTTON_NAME     = "Deal";

class TagNotification {
public:
    static TagNotification& GetInstance(void);

    void PublishTagNotification(NfcNotificationId notificationId, std::string name, int balance);

private:
    TagNotification();
    ~TagNotification();
    TagNotification(const TagNotification&) = delete;
    TagNotification& operator=(const TagNotification&) = delete;

    bool GetNeedActionButton(NfcNotificationId notificationId);

    static const int NFC_SERVICE_SA_ID = 1140;
    static const int NFC_NTF_CONTROL_FLAG = 0;  

    const std::string NFC_SERVICE_NAME = "nfc_service"; 
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_NOTIFICATION_H
