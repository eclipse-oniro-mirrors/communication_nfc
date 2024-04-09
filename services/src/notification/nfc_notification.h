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

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef void (*NfcNtfCallback)(int notificationId);
void RegNotificationCallback(NfcNtfCallback callback);
void PublishNfcNotification(int notificationId, const std::string &name, int balance);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

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

const int MAX_BUFF_LEN = 100;
const int NFC_UNIT_CHANGE_CONSTANT = 100;
const int NTF_AUTO_DELETE_TIME = 10;
const int MAX_RES_VEC_LEN = 100;

const std::string NFC_RES_DEFAULT_JSON_FILEPATH = "system/etc/nfc/string_zh.json";
const std::string NFC_RES_EN_JSON_FILEPATH = "system/etc/nfc/string_en.json";

const std::string KEY_STRING = "string";
const std::string KEY_NAME = "name";
const std::string KEY_VALUE = "value";

const std::string KEY_TAG_DEFAULT_NTF_TITLE    = "DefaultTitle";
const std::string KEY_TAG_DEFAULT_NTF_TEXT     = "DefaultText";
const std::string KEY_TRANSPORT_CARD_NTF_TITLE = "TransportCardTitle";
const std::string KEY_TRANSPORT_CARD_NTF_TEXT  = "TransportCardText";
const std::string KEY_NFC_WIFI_NTF_TITLE       = "NfcWifiNtfTitle";
const std::string KEY_NFC_WIFI_NTF_TEXT        = "NfcWifiNtfText";
const std::string KEY_ACTION_BUTTON_NAME       = "ActionButtonName";
const std::string KEY_NFC_WIFI_BUTTON_NAME     = "NfcWifiButtonName";
const std::string KEY_NFC_BT_NTF_TITLE         = "NfcBtNtfTitle";
const std::string KEY_NFC_BT_NTF_TEXT          = "NfcBtNtfText";
const std::string KEY_NFC_BT_BUTTON_NAME       = "NfcBtButtonName";
const std::string NFC_OPEN_LINK_BUTTON_NAME    = "Open Link";
const std::string NFC_OPEN_LINK_TEXT_HEAD      = "Open link: ";

const std::string KEY_HCE_AID_CONFLICTED_TITLE = "NfcHceAidConflictedTitle";
const std::string KEY_HCE_AID_CONFLICTED_TEXT = "NfcHceAidConflictedText";

class NfcNotification {
public:
    static NfcNotification& GetInstance(void);

    void PublishNfcNotification(int notificationId, const std::string &name, int balance);
    void RegNotificationCallback(NfcNtfCallback callback);

private:
    NfcNotification();
    ~NfcNotification();
    NfcNotification(const NfcNotification&) = delete;
    NfcNotification& operator=(const NfcNotification&) = delete;

    std::mutex mutex_ {};

    static const int NFC_NTF_CONTROL_FLAG = 0;
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_NOTIFICATION_H
