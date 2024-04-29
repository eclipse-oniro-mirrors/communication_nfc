/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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
#include "tag_dispatcher.h"

#include <functional>

#include "app_data_parser.h"
#include "external_deps_proxy.h"
#include "loghelper.h"
#include "ndef_har_data_parser.h"
#include "ndef_har_dispatch.h"
#include "ndef_message.h"
#include "nfc_hisysevent.h"
#include "nfc_sdk_common.h"
#include "tag_ability_dispatcher.h"

#ifdef NDEF_WIFI_ENABLED
#include "ndef_wifi_data_parser.h"
#include "wifi_connection_manager.h"
#endif

#ifdef NDEF_BT_ENABLED
#include "ndef_bt_data_parser.h"
#include "bt_connection_manager.h"
#endif

namespace OHOS {
namespace NFC {
namespace TAG {
#define NDEF_TYPE_NORMAL  1
#define NDEF_TYPE_BT      2
#define NDEF_TYPE_WIFI    3

using OHOS::NFC::KITS::TagTechnology;

TagDispatcher::TagDispatcher(std::shared_ptr<NFC::NfcService> nfcService)
    : nfcService_(nfcService),
    lastNdefMsg_(""),
    ndefCb_(nullptr)
{
    if (nfcService_) {
        nciTagProxy_ = nfcService_->GetNciTagProxy();
        if (!nciTagProxy_.expired()) {
            isodepCardHandler_ = std::make_shared<IsodepCardHandler>(nciTagProxy_);
            ndefHarDataParser_ = std::make_shared<NdefHarDataParser>(nciTagProxy_);
        }
    }
}

TagDispatcher::~TagDispatcher()
{
}

void TagDispatcher::RegNdefMsgCb(const sptr<INdefMsgCallback> &callback)
{
    ndefCb_ = callback;
}

bool TagDispatcher::HandleNdefDispatch(uint32_t tagDiscId, std::string &msg)
{
    int msgType = NDEF_TYPE_NORMAL;
    std::string ndef = msg;
#ifdef NDEF_BT_ENABLED
    std::shared_ptr<BtData> btData = NdefBtDataParser::CheckBtRecord(msg);
    if (btData && btData->isValid_) {
        msgType = NDEF_TYPE_BT;
        ndef = btData->vendorPayload_;
    }
#endif
#ifdef NDEF_WIFI_ENABLED
    std::shared_ptr<WifiData> wifiData;
    if (msgType == NDEF_TYPE_NORMAL) {
        wifiData = NdefWifiDataParser::CheckWifiRecord(msg);
        if (wifiData && wifiData->isValid_) {
            msgType = NDEF_TYPE_WIFI;
            ndef = wifiData->vendorPayload_;
        }
    }
#endif
    std::string tagUid = nciTagProxy_.lock()->GetTagUid(tagDiscId);
    InfoLog("HandleNdefDispatch, tagUid = %{public}s, msgType = %{public}d",
        KITS::NfcSdkCommon::CodeMiddlePart(tagUid).c_str(), msgType);
    bool ndefCbRes = false;
    if (ndefCb_ != nullptr) {
        ndefCbRes = ndefCb_->OnNdefMsgDiscovered(tagUid, ndef, msgType);
    }
    if (ndefCbRes) {
        InfoLog("HandleNdefDispatch, is dispatched by ndefMsg callback");
        return true;
    }
    if (msg.empty()) {
        ErrorLog("HandleNdefDispatch, ndef msg is empty");
        return false;
    }
#ifdef NDEF_BT_ENABLED
    if (msgType == NDEF_TYPE_BT) {
        BtConnectionManager::GetInstance().Initialize(nfcService_);
        BtConnectionManager::GetInstance().TryPairBt(btData);
        return true;
    }
#endif
#ifdef NDEF_WIFI_ENABLED
    if (msgType == NDEF_TYPE_WIFI) {
        WifiConnectionManager::GetInstance().Initialize(nfcService_);
        WifiConnectionManager::GetInstance().TryConnectWifi(wifiData);
        return true;
    }
#endif
    std::shared_ptr<KITS::TagInfo> tagInfo = GetTagInfoFromTag(tagDiscId);
    if (ndefHarDataParser_ != nullptr && ndefHarDataParser_->TryNdef(msg, tagInfo)) {
        return true;
    }
    return false;
}

void TagDispatcher::HandleTagFound(uint32_t tagDiscId)
{
    if (nfcService_ == nullptr || nciTagProxy_.expired() || nfcService_->GetNfcPollingManager().expired()) {
        ErrorLog("HandleTagFound, invalid state.");
        return;
    }

    bool isIsoDep = false;
    int fieldOnCheckInterval_ = DEFAULT_FIELD_ON_CHECK_DURATION;
    if (nciTagProxy_.lock()->GetConnectedTech(tagDiscId) == static_cast<int>(TagTechnology::NFC_ISODEP_TECH)) {
        fieldOnCheckInterval_ = DEFAULT_ISO_DEP_FIELD_ON_CHECK_DURATION;
        isIsoDep = true;
    }
    DebugLog("HandleTagFound fieldOnCheckInterval_ = %{public}d", fieldOnCheckInterval_);

    std::string ndefMsg = nciTagProxy_.lock()->FindNdefTech(tagDiscId);
    std::shared_ptr<KITS::NdefMessage> ndefMessage = KITS::NdefMessage::GetNdefMessage(ndefMsg);
    KITS::TagInfoParcelable* tagInfo = nullptr;
    do {
        if (ndefMessage == nullptr) {
            if (!nciTagProxy_.lock()->Reconnect(tagDiscId)) {
                nciTagProxy_.lock()->Disconnect(tagDiscId);
                ErrorLog("HandleTagFound bad connection, tag disconnected");
                break;
            }
        }
        lastNdefMsg_ = ndefMsg;
        nciTagProxy_.lock()->StartFieldOnChecking(tagDiscId, fieldOnCheckInterval_);
        tagInfo = GetTagInfoParcelableFromTag(tagDiscId);
        if (nfcService_->GetNfcPollingManager().lock()->IsReaderModeEnabled()) {
            nfcService_->GetNfcPollingManager().lock()->SendTagToReaderApp(tagInfo);
            break;
        }
        if (nfcService_->GetNfcPollingManager().lock()->IsForegroundEnabled()) {
            nfcService_->GetNfcPollingManager().lock()->SendTagToForeground(tagInfo);
            break;
        }
        ExternalDepsProxy::GetInstance().RegNotificationCallback(nfcService_);
        if (HandleNdefDispatch(tagDiscId, ndefMsg)) {
            break;
        }
        PublishTagNotification(tagDiscId, isIsoDep);
        DispatchTag(tagDiscId);
        break;
    } while (0);
    if (tagInfo != nullptr) {
        delete tagInfo;
        tagInfo = nullptr;
    }
    ExternalDepsProxy::GetInstance().StartVibratorOnce();
}

void TagDispatcher::HandleTagLost(uint32_t tagDiscId)
{
    InfoLog("HandleTagLost, tagDiscId: %{public}d", tagDiscId);
}

std::shared_ptr<KITS::TagInfo> TagDispatcher::GetTagInfoFromTag(uint32_t tagDiscId)
{
    std::vector<int> techList = nciTagProxy_.lock()->GetTechList(tagDiscId);
    std::string tagUid = nciTagProxy_.lock()->GetTagUid(tagDiscId);
    std::vector<AppExecFwk::PacMap> tagTechExtras = nciTagProxy_.lock()->GetTechExtrasData(tagDiscId);
    DebugLog("GetTagInfoFromTag: tag uid = %{public}s, techListLen = %{public}zu, extrasLen = %{public}zu,"
        "rfID = %{public}d", KITS::NfcSdkCommon::CodeMiddlePart(tagUid).c_str(),
        techList.size(), tagTechExtras.size(), tagDiscId);
    return std::make_shared<KITS::TagInfo>(techList, tagTechExtras, tagUid, tagDiscId,
        nfcService_->GetTagServiceIface());
}

KITS::TagInfoParcelable* TagDispatcher::GetTagInfoParcelableFromTag(uint32_t tagDiscId)
{
    std::vector<int> techList = nciTagProxy_.lock()->GetTechList(tagDiscId);
    std::string tagUid = nciTagProxy_.lock()->GetTagUid(tagDiscId);
    std::vector<AppExecFwk::PacMap> tagTechExtras = nciTagProxy_.lock()->GetTechExtrasData(tagDiscId);
    DebugLog("GetTagInfoParcelableFromTag: tag uid = %{public}s, techListLen = %{public}zu, extrasLen = %{public}zu,"
        "rfID = %{public}d", KITS::NfcSdkCommon::CodeMiddlePart(tagUid).c_str(),
        techList.size(), tagTechExtras.size(), tagDiscId);

    // tagInfo should be deleted at where it is used (HandleTagFound)
    KITS::TagInfoParcelable* tagInfo = new (std::nothrow) KITS::TagInfoParcelable(techList, tagTechExtras,
        tagUid, tagDiscId, nfcService_->GetTagServiceIface());
    return tagInfo;
}

void TagDispatcher::DispatchTag(uint32_t tagDiscId)
{
    tagInfo_ = GetTagInfoFromTag(tagDiscId);
    if (tagInfo_ == nullptr) {
        ErrorLog("DispatchTag: taginfo is null");
        return;
    }

    // select the matched applications, try start ability
    std::vector<int> techList = nciTagProxy_.lock()->GetTechList(tagDiscId);
    // Record types of read tags.
    ExternalDepsProxy::GetInstance().WriteTagFoundHiSysEvent(techList);
}

void TagDispatcher::HandleTagDebounce()
{
    DebugLog("HandleTagDebounce, unimplemented...");
}

void TagDispatcher::OnNotificationButtonClicked(int notificationId)
{
    InfoLog("notificationId[%{public}d]", notificationId);
    switch (notificationId) {
        case NFC_TRANSPORT_CARD_NOTIFICATION_ID:
            // start application ability for tag found.
            ExternalDepsProxy::GetInstance().DispatchTagAbility(tagInfo_, nfcService_->GetTagServiceIface());
            break;
        case NFC_WIFI_NOTIFICATION_ID: {
#ifdef NDEF_WIFI_ENABLED
            if (nfcService_ && nfcService_->eventHandler_) {
                nfcService_->eventHandler_->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_WIFI_NTF_CLICKED));
            }
#endif
            break;
        }
        case NFC_BT_NOTIFICATION_ID: {
#ifdef NDEF_BT_ENABLED
            if (nfcService_ && nfcService_->eventHandler_) {
                nfcService_->eventHandler_->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_BT_NTF_CLICKED));
            }
#endif
            break;
        }
        case NFC_TAG_DEFAULT_NOTIFICATION_ID:
            // start application ability for tag found.
            ExternalDepsProxy::GetInstance().DispatchTagAbility(tagInfo_, nfcService_->GetTagServiceIface());
            break;
        case NFC_BROWSER_NOTIFICATION_ID:
            NdefHarDispatch::GetInstance().OnBrowserOpenLink();
            break;
        default:
            WarnLog("unknown notification Id");
            break;
    }
}

void TagDispatcher::PublishTagNotification(uint32_t tagDiscId, bool isIsoDep)
{
    NfcNotificationId notificationId = NFC_TAG_DEFAULT_NOTIFICATION_ID;
    std::string cardName = "";
    uint8_t cardIndex = INVALID_CARD_INDEX;
    int balance = INVALID_BALANCE;
    if (isIsoDep && isodepCardHandler_ != nullptr) {
        isodepCardHandler_->InitTransportCardInfo();
        if (isodepCardHandler_->IsSupportedTransportCard(tagDiscId, cardIndex)) {
            isodepCardHandler_->GetBalance(tagDiscId, cardIndex, balance);
            if (balance < 0) {
                WarnLog("failed to get card balance.");
            } else {
                isodepCardHandler_->GetCardName(cardIndex, cardName);
                notificationId = NFC_TRANSPORT_CARD_NOTIFICATION_ID;
            }
        }
    }
    ExternalDepsProxy::GetInstance().PublishNfcNotification(notificationId, cardName, balance);
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
