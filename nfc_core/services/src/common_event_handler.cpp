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
#include "common_event_handler.h"

#include "common_event_support.h"
#include "itag_host.h"
#include "loghelper.h"
#include "want.h"

namespace OHOS {
namespace NFC {
class CommonEventHandler::ScreenChangedReceiver : public EventFwk::CommonEventSubscriber {
public:
    explicit ScreenChangedReceiver(std::weak_ptr<NfcService> nfcService,
        const EventFwk::CommonEventSubscribeInfo& subscribeInfo);
    ~ScreenChangedReceiver()
    {
    }
    void OnReceiveEvent(const EventFwk::CommonEventData& data) override;

private:
    std::weak_ptr<NfcService> nfcService_ {};
    std::weak_ptr<AppExecFwk::EventHandler> eventHandler_ {};
};

CommonEventHandler::ScreenChangedReceiver::ScreenChangedReceiver(std::weak_ptr<NfcService> nfcService,
    const EventFwk::CommonEventSubscribeInfo& subscribeInfo)
    : EventFwk::CommonEventSubscriber(subscribeInfo),
    nfcService_(nfcService),
    eventHandler_(nfcService.lock()->eventHandler_)
{
}

void CommonEventHandler::ScreenChangedReceiver::OnReceiveEvent(const EventFwk::CommonEventData& data)
{
    std::string action = data.GetWant().GetAction();
    if (action.empty()) {
        ErrorLog("action is empty");
        return;
    }

    ScreenState screenState = ScreenState::SCREEN_STATE_UNKNOWN;
    if (action.compare(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON) == 0) {
        screenState = ScreenState::SCREEN_STATE_ON_UNLOCKED;
    } else if (action.compare(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF) == 0) {
        screenState = ScreenState::SCREEN_STATE_OFF_UNLOCKED;
    } else {
        ErrorLog("Screen changed receiver event:unknown");
        return;
    }
    eventHandler_.lock()->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_SCREEN_CHANGED),
        static_cast<int64_t>(screenState), static_cast<int64_t>(0));
}

class CommonEventHandler::PackageChangedReceiver : public EventFwk::CommonEventSubscriber {
public:
    explicit PackageChangedReceiver(std::weak_ptr<NfcService> nfcService,
        const EventFwk::CommonEventSubscribeInfo& subscribeInfo);
    ~PackageChangedReceiver()
    {
    }
    void OnReceiveEvent(const EventFwk::CommonEventData& data) override;

private:
    std::weak_ptr<NfcService> nfcService_ {};
    std::weak_ptr<AppExecFwk::EventHandler> eventHandler_ {};
};

CommonEventHandler::PackageChangedReceiver::PackageChangedReceiver(std::weak_ptr<NfcService> nfcService,
    const EventFwk::CommonEventSubscribeInfo& subscribeInfo)
    : EventFwk::CommonEventSubscriber(subscribeInfo),
    nfcService_(nfcService),
    eventHandler_(nfcService.lock()->eventHandler_)
{
}

void CommonEventHandler::PackageChangedReceiver::OnReceiveEvent(const EventFwk::CommonEventData& data)
{
    DebugLog("CommonEventHandler::PackageChangedReceiver");
    std::string action = data.GetWant().GetAction();
    if (action.empty()) {
        ErrorLog("action is empty");
        return;
    }
    const std::shared_ptr<EventFwk::CommonEventData> mdata =
        std::make_shared<EventFwk::CommonEventData> (data);
    if (action.compare(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED) == 0 ||
        action.compare(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED) == 0 ||
        action.compare(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED) == 0) {
        eventHandler_.lock()->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_PACKAGE_UPDATED),
            mdata, static_cast<int64_t>(0));
    }
}

CommonEventHandler::CommonEventHandler(const std::shared_ptr<AppExecFwk::EventRunner>& runner,
                                       std::weak_ptr<NfcService> servcie)
    : EventHandler(runner), nfcService_(servcie)
{
}

CommonEventHandler::~CommonEventHandler()
{
    EventFwk::CommonEventManager::UnSubscribeCommonEvent(screenSubscriber_);
    EventFwk::CommonEventManager::UnSubscribeCommonEvent(pkgSubscriber_);
}

void CommonEventHandler::Intialize(std::weak_ptr<TAG::TagDispatcher> tagDispatcher)
{
    DebugLog("CommonEventHandler::Intialize");
    tagDispatcher_ = tagDispatcher;

    SubscribeScreenChangedEvent();
    SubscribePackageChangedEvent();
}

void CommonEventHandler::SubscribeScreenChangedEvent()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    screenSubscriber_ = std::make_shared<ScreenChangedReceiver>(nfcService_, subscribeInfo);
    if (screenSubscriber_ == nullptr) {
        ErrorLog("Create screen changed subscriber failed");
        return;
    }

    if (!EventFwk::CommonEventManager::SubscribeCommonEvent(screenSubscriber_)) {
        ErrorLog("Subscribe screen changed event fail");
    }
}

void CommonEventHandler::SubscribePackageChangedEvent()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    pkgSubscriber_ = std::make_shared<PackageChangedReceiver>(nfcService_, subscribeInfo);
    if (pkgSubscriber_ == nullptr) {
        ErrorLog("Create package changed subscriber failed");
        return;
    }

    if (!EventFwk::CommonEventManager::SubscribeCommonEvent(pkgSubscriber_)) {
        ErrorLog("Subscribe package changed event fail");
    }
}

void CommonEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event)
{
    if (event == nullptr) {
        ErrorLog("event is nullptr");
        return;
    }
    NfcCommonEvent eventId = static_cast<NfcCommonEvent>(event->GetInnerEventId());
    DebugLog("NFC common event handler receive a message of %{public}d", eventId);
    switch (eventId) {
        case NfcCommonEvent::MSG_TAG_FOUND:
            tagDispatcher_.lock()->HandleTagFound(event->GetSharedObject<NCI::ITagHost>());
            break;
        case NfcCommonEvent::MSG_TAG_DEBOUNCE:
            tagDispatcher_.lock()->HandleTagDebounce();
            break;
        case NfcCommonEvent::MSG_SCREEN_CHANGED: {
            nfcService_.lock()->HandleScreenChanged(event->GetParam());
            break;
        }
        case NfcCommonEvent::MSG_PACKAGE_UPDATED: {
            nfcService_.lock()->HandlePackageUpdated(event->GetSharedObject<EventFwk::CommonEventData>());
            break;
        }
        default:
            ErrorLog("Unknown message received: id %{public}d", eventId);
            break;
    }
}
}  // namespace NFC
}  // namespace OHOS
