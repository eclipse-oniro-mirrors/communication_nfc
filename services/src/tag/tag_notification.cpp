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

#include "tag_notification.h"

#include "loghelper.h"
#include "want_agent_helper.h"
#include "want_agent_info.h"

#ifdef DEBUG
#undef DEBUG
#include "notification_helper.h"
#endif

namespace OHOS {
namespace NFC {
namespace TAG {
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
    }
    void OnConsumed(const std::shared_ptr<OHOS::Notification::Notification> &notification,
        const std::shared_ptr<Notification::NotificationSortingMap> &sortingMap) {}
    void OnBadgeChanged(const std::shared_ptr<Notification::BadgeNumberCallbackData> &badgeData) {}
    void OnBadgeEnabledChanged(const sptr<Notification::EnabledNotificationCallbackData> &callbackData) {}
    void OnBatchCanceled(const std::vector<std::shared_ptr<OHOS::Notification::Notification>> &requestList,
        const std::shared_ptr<Notification::NotificationSortingMap> &sortingMap, int32_t deleteReason) {}
};

static const auto NOTIFICATION_SUBSCRIBER = NfcNotificationSubscriber();

static std::string GetTrafficCardNotificationText(std::string cardName, int balance)
{
    return ("found " + cardName + ", current balance " + std::to_string(balance / NFC_UNIT_CHANGE_CONSTANT)
            + "." + std::to_string(balance % NFC_UNIT_CHANGE_CONSTANT) + " yuan");
}

static bool SetTitleAndText(NfcNotificationId notificationId,
    std::shared_ptr<Notification::NotificationNormalContent> nfcContent, std::string name, int balance)
{
    if (nfcContent == nullptr) {
        ErrorLog("notification normal content nullptr");
        return false;
    }
    switch (notificationId) {
        case NFC_TRAFFIC_CARD_NOTIFICATION_ID:
            nfcContent->SetTitle(NFC_TRAFFIC_CARD_NTF_TITLE);
            nfcContent->SetText(GetTrafficCardNotificationText(name, balance));
            break;
        case NFC_WIFI_NOTIFICATION_ID:
            break;
        case NFC_BT_NOTIFICATION_ID:
            break;
        case NFC_TAG_NOTIFICATION_ID:
            nfcContent->SetTitle(NFC_TAG_DEFAULT_NTF_TITLE);
            nfcContent->SetText(NFC_TAG_DEFAULT_NTF_TEXT);
            break;
        default:
            WarnLog("unknown notification ID");
            return false;
    }
    return true;
}

TagNotification& TagNotification::GetInstance()
{
    static TagNotification instance;
    return instance;
}

TagNotification::TagNotification()
{
    InfoLog("TagNotification constructor enter.");
    // only need to subscribe notification once
    int result = Notification::NotificationHelper::SubscribeNotification(NOTIFICATION_SUBSCRIBER);
    if (result != ERR_OK) {
        ErrorLog("fail to subscribe notification");
    }
}

TagNotification::~TagNotification()
{
    InfoLog("TagNotification destructor enter.");
    // no operation to unsubscribe notification
}

void TagNotification::PublishTagNotification(NfcNotificationId notificationId, std::string name, int balance)
{
    InfoLog("Publishing nfc tag notification, id [%{public}d]", notificationId);
    std::shared_ptr<Notification::NotificationNormalContent> nfcContent =
        std::make_shared<Notification::NotificationNormalContent>();
    if (nfcContent == nullptr) {
        ErrorLog("get notification normal content nullptr");
        return;
    }
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
    request.SetCreatorUid(NFC_SERVICE_SA_ID);
    request.SetCreatorBundleName(NFC_SERVICE_NAME);
    request.SetAutoDeletedTime(NTF_AUTO_DELETE_TIME);
    request.SetTapDismissed(true);
    request.SetSlotType(OHOS::Notification::NotificationConstant::SlotType::SOCIAL_COMMUNICATION);
    request.SetNotificationControlFlags(NFC_NTF_CONTROL_FLAG);

    if (GetNeedActionButton(notificationId)) {
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
            Notification::NotificationActionButton::Create(nullptr, NFC_ACTION_BUTTON_NAME, wantAgentDeal);
        if (actionButtonDeal == nullptr) {
            ErrorLog("get notification actionButton nullptr");
            return;
        }
        request.AddActionButton(actionButtonDeal);
    }

    int ret = Notification::NotificationHelper::PublishNotification(request);
    InfoLog("NFC service publish notification result = %{public}d", ret);
}

bool TagNotification::GetNeedActionButton(NfcNotificationId notificationId)
{
    return (notificationId > NFC_TAG_NOTIFICATION_ID && notificationId <= NFC_TRAFFIC_CARD_NOTIFICATION_ID);
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
