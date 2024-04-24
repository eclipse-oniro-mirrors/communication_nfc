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

#include "nfc_notification.h"

#include <map>

#include "cJSON.h"
#include "file_ex.h"
#include "locale_config.h"
#include "locale_info.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "securec.h"
#include "want_agent_helper.h"
#include "want_agent_info.h"

#ifdef DEBUG
#undef DEBUG
#endif
#include "notification_helper.h"

namespace OHOS {
namespace NFC {
namespace TAG {
static std::string g_sysLanguage = "";
static std::map<std::string, std::string> g_resourceMap;
static std::mutex g_callbackMutex {};
static NfcNtfCallback g_ntfCallback = nullptr;

class NfcNotificationSubscriber : public Notification::NotificationSubscriber {
    void OnConnected() {}
    void OnDisconnected() {}
    void OnUpdate(const std::shared_ptr<Notification::NotificationSortingMap> &sortingMap) {}
    void OnDoNotDisturbDateChange(const std::shared_ptr<Notification::NotificationDoNotDisturbDate> &date) {}
    void OnEnabledNotificationChanged(
        const std::shared_ptr<Notification::EnabledNotificationCallbackData> &callbackData) {}
    void OnDied() {}
    void OnCanceled(const std::shared_ptr<OHOS::Notification::Notification> &request,
        const std::shared_ptr<Notification::NotificationSortingMap> &sortingMap, int deleteReason)
    {
        int creatorUid = request->GetUid();
        int notificationId = request->GetId();
        InfoLog("Oncanceled, creatorUid = %{public}d, notificationId = %{public}d, deleteReason = %{public}d",
            creatorUid, notificationId, deleteReason);

        if (deleteReason == Notification::NotificationConstant::CLICK_REASON_DELETE) {
            std::lock_guard<std::mutex> lock(g_callbackMutex);
            g_ntfCallback(notificationId);
        }
    }
    void OnConsumed(const std::shared_ptr<OHOS::Notification::Notification> &notification,
        const std::shared_ptr<Notification::NotificationSortingMap> &sortingMap) {}
    void OnBadgeChanged(const std::shared_ptr<Notification::BadgeNumberCallbackData> &badgeData) {}
    void OnBadgeEnabledChanged(const sptr<Notification::EnabledNotificationCallbackData> &callbackData) {}
    void OnBatchCanceled(const std::vector<std::shared_ptr<OHOS::Notification::Notification>> &requestList,
        const std::shared_ptr<Notification::NotificationSortingMap> &sortingMap, int32_t deleteReason) {}
};

static const auto NOTIFICATION_SUBSCRIBER = NfcNotificationSubscriber();

static void UpdateResourceMap(const std::string &resourcePath)
{
    InfoLog("Reading resource string from json config.");

    std::string content;
    LoadStringFromFile(resourcePath, content);
    cJSON *json = cJSON_Parse(content.c_str());
    if (json == nullptr) {
        ErrorLog("json nullptr.");
        return;
    }

    cJSON *resJson = cJSON_GetObjectItemCaseSensitive(json, KEY_STRING);
    if (resJson == nullptr || cJSON_GetArraySize(resJson) > MAX_RES_VEC_LEN) {
        ErrorLog("fail to parse res json");
        cJSON_Delete(json);
        return;
    }

    g_resourceMap.clear();
    cJSON *resJsonEach = nullptr;
    cJSON_ArrayForEach(resJsonEach, resJson) {
        cJSON *key = cJSON_GetObjectItemCaseSensitive(resJsonEach, KEY_NAME);
        if (key == nullptr || !cJSON_IsString(key)) {
            ErrorLog("json param not string");
            cJSON_Delete(json);
            return;
        }

        cJSON *value = cJSON_GetObjectItemCaseSensitive(resJsonEach, KEY_VALUE);
        if (value == nullptr || !cJSON_IsString(value)) {
            ErrorLog("json param not string");
            cJSON_Delete(json);
            return;
        }

        g_resourceMap.insert(std::pair<std::string, std::string>(key->valuestring, value->valuestring));
    }
    cJSON_Delete(json);
}

static void UpdateResourceMapByLanguage()
{
    std::string curSysLanguage = "zh";
    OHOS::Global::I18n::LocaleInfo locale(Global::I18n::LocaleConfig::GetSystemLocale());
    curSysLanguage = locale.GetLanguage();
    if (g_sysLanguage == curSysLanguage) {
        DebugLog("same language environment, no need to update resource map.");
        return;
    }

    InfoLog("current system language[%{public}s] changes, should update resource map", curSysLanguage.c_str());
    g_sysLanguage = curSysLanguage;

    if (g_sysLanguage == "en") {
        UpdateResourceMap(NFC_RES_EN_JSON_FILEPATH);
    } else {
        UpdateResourceMap(NFC_RES_DEFAULT_JSON_FILEPATH);
    }
}

static std::string GetTrafficCardNotificationText(const std::string &cardName, int balance)
{
    char buf[MAX_BUFF_LEN] = {0};
    int ret = sprintf_s(buf, MAX_BUFF_LEN, g_resourceMap[KEY_TRANSPORT_CARD_NTF_TEXT].c_str(),
        g_resourceMap[cardName].c_str(), static_cast<float>(balance) / NFC_UNIT_CHANGE_CONSTANT);
    if (ret <= 0) {
        ErrorLog("sprintf_s failed, ret[%{public}d]", ret);
        return "";
    }

    return std::string(buf);
}

#ifdef NDEF_WIFI_ENABLED
static std::string GetWifiNotificationText(const std::string &ssid)
{
    char buf[MAX_BUFF_LEN] = {0};
    int ret = sprintf_s(buf, MAX_BUFF_LEN, g_resourceMap[KEY_NFC_WIFI_NTF_TEXT].c_str(), ssid.c_str());
    if (ret <= 0) {
        ErrorLog("sprintf_s failed, ret[%{public}d]", ret);
        return "";
    }

    return std::string(buf);
}
#endif

#ifdef NDEF_BT_ENABLED
static std::string GetBtNotificationText(const std::string &name)
{
    char buf[MAX_BUFF_LEN] = {0};
    int ret = sprintf_s(buf, MAX_BUFF_LEN, g_resourceMap[KEY_NFC_BT_NTF_TEXT].c_str(), name.c_str());
    if (ret <= 0) {
        ErrorLog("sprintf_s failed, ret[%{public}d]", ret);
        return "";
    }

    return std::string(buf);
}
#endif

static bool SetTitleAndTextForOtherNotificationId(int notificationId,
    std::shared_ptr<Notification::NotificationNormalContent> nfcContent, const std::string &name, int balance)
{
    switch (notificationId) {
        case NFC_TAG_DEFAULT_NOTIFICATION_ID:
            if (g_resourceMap.find(KEY_TAG_DEFAULT_NTF_TITLE) != g_resourceMap.end() &&
                g_resourceMap.find(KEY_TAG_DEFAULT_NTF_TEXT) != g_resourceMap.end()) {
                nfcContent->SetTitle(g_resourceMap[KEY_TAG_DEFAULT_NTF_TITLE]);
                nfcContent->SetText(g_resourceMap[KEY_TAG_DEFAULT_NTF_TEXT]);
            }
            break;
        case NFC_BROWSER_NOTIFICATION_ID:
            if (g_resourceMap.find(KEY_TAG_DEFAULT_NTF_TITLE) != g_resourceMap.end()) {
                nfcContent->SetTitle(g_resourceMap[KEY_TAG_DEFAULT_NTF_TITLE]);
                nfcContent->SetText(NFC_OPEN_LINK_TEXT_HEAD + name);
            }
            break;
        case NFC_HCE_AID_CONFLICTED_ID:
            if (g_resourceMap.find(KEY_HCE_AID_CONFLICTED_TITLE) != g_resourceMap.end() &&
                g_resourceMap.find(KEY_HCE_AID_CONFLICTED_TEXT) != g_resourceMap.end()) {
                nfcContent->SetTitle(g_resourceMap[KEY_HCE_AID_CONFLICTED_TITLE]);
                nfcContent->SetText(g_resourceMap[KEY_HCE_AID_CONFLICTED_TEXT]);
            }
            break;
        case NFC_NO_HAP_SUPPORTED_NOTIFICATION_ID:
            if (g_resourceMap.find(KEY_NO_HAP_TITLE) != g_resourceMap.end() &&
                g_resourceMap.find(KEY_NO_HAP_TEXT) != g_resourceMap.end()) {
                nfcContent->SetTitle(g_resourceMap[KEY_NO_HAP_TITLE]);
                nfcContent->SetText(g_resourceMap[KEY_NO_HAP_TEXT]);
            }
            break;
        default:
            WarnLog("unknown notification ID");
            return false;
    }
    return true;
}

static bool SetTitleAndText(int notificationId,
    std::shared_ptr<Notification::NotificationNormalContent> nfcContent, const std::string &name, int balance)
{
    if (nfcContent == nullptr) {
        ErrorLog("notification normal content nullptr");
        return false;
    }
    UpdateResourceMapByLanguage();

    switch (notificationId) {
        case NFC_TRANSPORT_CARD_NOTIFICATION_ID:
            if (g_resourceMap.find(KEY_TRANSPORT_CARD_NTF_TITLE) != g_resourceMap.end() &&
                g_resourceMap.find(KEY_TRANSPORT_CARD_NTF_TEXT) != g_resourceMap.end() &&
                g_resourceMap.find(name) != g_resourceMap.end()) {
                nfcContent->SetTitle(g_resourceMap[KEY_TRANSPORT_CARD_NTF_TITLE]);
                nfcContent->SetText(GetTrafficCardNotificationText(name, balance));
            }
            break;
        case NFC_WIFI_NOTIFICATION_ID:
#ifdef NDEF_WIFI_ENABLED
            if (g_resourceMap.find(KEY_NFC_WIFI_NTF_TITLE) != g_resourceMap.end() &&
                g_resourceMap.find(KEY_NFC_WIFI_NTF_TEXT) != g_resourceMap.end()) {
                nfcContent->SetTitle(g_resourceMap[KEY_NFC_WIFI_NTF_TITLE]);
                nfcContent->SetText(GetWifiNotificationText(name));
            }
            break;
#else
            ErrorLog("nfc wifi notification not supported");
            return false;
#endif
        case NFC_BT_NOTIFICATION_ID:
#ifdef NDEF_BT_ENABLED
            if (g_resourceMap.find(KEY_NFC_BT_NTF_TITLE) != g_resourceMap.end() &&
                g_resourceMap.find(KEY_NFC_BT_NTF_TEXT) != g_resourceMap.end()) {
                nfcContent->SetTitle(g_resourceMap[KEY_NFC_BT_NTF_TITLE]);
                nfcContent->SetText(GetBtNotificationText(name));
            }
            break;
#else
            ErrorLog("nfc bt notification not supported");
            return false;
#endif
        default:
            return SetTitleAndTextForOtherNotificationId(notificationId, nfcContent, name, balance);
    }
    return true;
}

static std::string GetButtonName(int notificationId)
{
    switch (notificationId) {
        case NFC_BT_NOTIFICATION_ID:
            if (g_resourceMap.find(KEY_NFC_BT_BUTTON_NAME) != g_resourceMap.end()) {
                return g_resourceMap[KEY_NFC_BT_BUTTON_NAME];
            }
            return "";
        case NFC_WIFI_NOTIFICATION_ID:
            if (g_resourceMap.find(KEY_NFC_WIFI_BUTTON_NAME) != g_resourceMap.end()) {
                return g_resourceMap[KEY_NFC_WIFI_BUTTON_NAME];
            }
            return "";
        case NFC_BROWSER_NOTIFICATION_ID:
            return NFC_OPEN_LINK_BUTTON_NAME;
        case NFC_NO_HAP_SUPPORTED_NOTIFICATION_ID:
            return "";
        default:
            if (g_resourceMap.find(KEY_ACTION_BUTTON_NAME) != g_resourceMap.end()) {
                return g_resourceMap[KEY_ACTION_BUTTON_NAME];
            }
            return "";
    }
}

static void SetActionButton(const std::string& buttonName, Notification::NotificationRequest& request)
{
    auto want = std::make_shared<AAFwk::Want>();
    std::vector<std::shared_ptr<AAFwk::Want>> wants;
    wants.push_back(want);
    std::vector<AbilityRuntime::WantAgent::WantAgentConstant::Flags> flags;
    flags.push_back(AbilityRuntime::WantAgent::WantAgentConstant::Flags::CONSTANT_FLAG);
    AbilityRuntime::WantAgent::WantAgentInfo wantAgentInfo(
        0, AbilityRuntime::WantAgent::WantAgentConstant::OperationType::UNKNOWN_TYPE,
        flags, wants, nullptr
    );
    auto wantAgentDeal = AbilityRuntime::WantAgent::WantAgentHelper::GetWantAgent(wantAgentInfo);
    std::shared_ptr<Notification::NotificationActionButton> actionButtonDeal =
        Notification::NotificationActionButton::Create(nullptr, buttonName, wantAgentDeal);
    if (actionButtonDeal == nullptr) {
        ErrorLog("get notification actionButton nullptr");
        return;
    }
    request.AddActionButton(actionButtonDeal);
}

void NfcNotification::GetPixelMap(const std::string &path)
{
    if (nfcIconPixelMap_ != nullptr) {
        InfoLog("nfc icon pixel map already exists.");
        return;
    }

    if (!std::filesystem::exists(path)) {
        ErrorLog("nfc icon file path not exists.");
        nfcIconPixelMap_ = nullptr;
        return;
    }
    uint32_t errorCode = 0;
    Media::SourceOptions opts;
    opts.formatHint = "image/png";
    std::unique_ptr<Media::ImageSource> imageSource = Media::ImageSource::CreateImageSource(path, opts, errorCode);
    if (imageSource == nullptr) {
        ErrorLog("imageSource nullptr");
        nfcIconPixelMap_ = nullptr;
        return;
    }
    Media::DecodeOptions decodeOpts;
    std::unique_ptr<Media::PixelMap> pixelMap = imageSource->CreatePixelMap(decodeOpts, errorCode);
    nfcIconPixelMap_ = std::move(pixelMap);
}

NfcNotification& NfcNotification::GetInstance()
{
    static NfcNotification instance;
    return instance;
}

NfcNotification::NfcNotification()
{
    InfoLog("NfcNotification constructor enter.");
    std::lock_guard<std::mutex> lock(mutex_);
    // only need to subscribe notification once
    int result = Notification::NotificationHelper::SubscribeNotification(NOTIFICATION_SUBSCRIBER);
    if (result != ERR_OK) {
        ErrorLog("fail to subscribe notification");
    }
    UpdateResourceMapByLanguage();
}

NfcNotification::~NfcNotification()
{
    InfoLog("NfcNotification destructor enter.");
    // no operation to unsubscribe notification
}

void NfcNotification::PublishNfcNotification(int notificationId, const std::string &name, int balance)
{
    InfoLog("Publishing nfc tag notification, id [%{public}d]", notificationId);
    std::shared_ptr<Notification::NotificationNormalContent> nfcContent =
        std::make_shared<Notification::NotificationNormalContent>();
    if (nfcContent == nullptr) {
        ErrorLog("get notification normal content nullptr");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (!SetTitleAndText(notificationId, nfcContent, name, balance)) {
        ErrorLog("error setting title and text");
        return;
    }

    std::shared_ptr<Notification::NotificationContent> content =
        std::make_shared<Notification::NotificationContent>(nfcContent);
    if (content == nullptr) {
        ErrorLog("get notification content nullptr");
        return;
    }

    Notification::NotificationRequest request;
    request.SetNotificationId(static_cast<int>(notificationId));
    request.SetContent(content);
    request.SetCreatorUid(KITS::NFC_MANAGER_SYS_ABILITY_ID);
    request.SetCreatorBundleName(KITS::NFC_MANAGER_SYS_ABILITY_NAME);
    request.SetAutoDeletedTime(NTF_AUTO_DELETE_TIME);
    request.SetTapDismissed(true);
    request.SetSlotType(OHOS::Notification::NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    request.SetNotificationControlFlags(NFC_NTF_CONTROL_FLAG);

    GetPixelMap(NFC_ICON_PATH);
    if (nfcIconPixelMap_ != nullptr) {
        request.SetLittleIcon(nfcIconPixelMap_);
        request.SetBadgeIconStyle(Notification::NotificationRequest::BadgeStyle::LITTLE);
    }

    std::string buttonName = GetButtonName(notificationId);
    if (!buttonName.empty()) {
        SetActionButton(buttonName, request);
    }
    int ret = Notification::NotificationHelper::PublishNotification(request);
    InfoLog("NFC service publish notification result = %{public}d", ret);
}

void NfcNotification::RegNotificationCallback(NfcNtfCallback callback)
{
    std::lock_guard<std::mutex> lock(g_callbackMutex);
    g_ntfCallback = callback;
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS

void RegNotificationCallback(NfcNtfCallback callback)
{
    OHOS::NFC::TAG::NfcNotification::GetInstance().RegNotificationCallback(callback);
}

void PublishNfcNotification(int notificationId, const std::string &name, int balance)
{
    OHOS::NFC::TAG::NfcNotification::GetInstance().PublishNfcNotification(notificationId, name, balance);
}