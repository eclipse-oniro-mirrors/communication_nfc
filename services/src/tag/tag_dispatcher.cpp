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
        if (nciTagProxy_.expired()) {
            ErrorLog("TagDispatcher, nciTagProxy_ expired");
            return;
        }
        isodepCardHandler_ = std::make_shared<IsodepCardHandler>(nciTagProxy_);
        nciNfccProxy_ = nfcService_->GetNciNfccProxy();
        if (nciNfccProxy_.expired()) {
            ErrorLog("TagDispatcher, nciNfccProxy_ expired");
            return;
        }
        NdefHarDataParser::GetInstance().Initialize(nfcService_, nciTagProxy_, nciNfccProxy_);
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
    if (nciTagProxy_.expired()) {
        ErrorLog("HandleNdefDispatch, nciTagProxy_ expired");
        return false;
    }
    std::string tagUid = nciTagProxy_.lock()->GetTagUid(tagDiscId);
    int msgType = NDEF_TYPE_NORMAL;
    std::string ndef = msg;
    if (ndefCb_ != nullptr) {
        ndefCbRes_ = ndefCb_->OnNdefMsgDiscovered(tagUid, ndef, msgType);
    }
    if (ndefCbRes_) {
        InfoLog("HandleNdefDispatch, is dispatched by ndefMsg callback");
        return true;
    }
#ifdef NDEF_BT_ENABLED
    std::shared_ptr<BtData> btData = NdefBtDataParser::CheckBtRecord(msg);
    if (btData && btData->isValid_) {
        msgType = NDEF_TYPE_BT;
        if (!btData->vendorPayload_.empty() && NdefBtDataParser::IsVendorPayloadValid(btData->vendorPayload_)) {
            // Bt msg for NdefMsg Callback: bt payload len | bt payload | mac addr | dev name
            ndef = NfcSdkCommon::IntToHexString(btData->vendorPayload_.length() / HEX_BYTE_LEN);
            ndef.append(btData->vendorPayload_);
            ndef.append(btData->macAddrOrg_);
            ndef.append(NfcSdkCommon::StringToHexString(btData->name_));
        } else {
            InfoLog("BT vendor payload invalid");
            ndef = "";
        }
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
    InfoLog("HandleNdefDispatch, tagUid = %{public}s, msgType = %{public}d",
        KITS::NfcSdkCommon::CodeMiddlePart(tagUid).c_str(), msgType);
    if (ndefCb_ != nullptr) {
        ndefCbRes_ = ndefCb_->OnNdefMsgDiscovered(tagUid, ndef, msgType);
    }
    if (ndefCbRes_) {
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
    if (NdefHarDataParser::GetInstance().TryNdef(msg, tagInfo)) {
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
    if (static_cast<int>(nciTagProxy_.lock()->GetConnectedTech(tagDiscId)) ==
        static_cast<int>(TagTechnology::NFC_ISODEP_TECH)) {
        fieldOnCheckInterval_ = DEFAULT_ISO_DEP_FIELD_ON_CHECK_DURATION;
        isIsoDep = true;
    }
    ndefCbRes_ = false;
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
        break;
    } while (0);
    if (tagInfo != nullptr) {
        delete tagInfo;
        tagInfo = nullptr;
    }
#ifndef NFC_VIBRATOR_DISABLED
    StartVibratorOnce();
#endif
    // Record types of read tags.
    std::vector<int> techList = nciTagProxy_.lock()->GetTechList(tagDiscId);
    ExternalDepsProxy::GetInstance().WriteTagFoundHiSysEvent(techList);
}

void TagDispatcher::StartVibratorOnce()
{
    if (!ndefCbRes_) {
        ExternalDepsProxy::GetInstance().StartVibratorOnce();
    }
}
void TagDispatcher::HandleTagLost(uint32_t tagDiscId)
{
    InfoLog("HandleTagLost, tagDiscId: %{public}d", tagDiscId);
}

std::shared_ptr<KITS::TagInfo> TagDispatcher::GetTagInfoFromTag(uint32_t tagDiscId)
{
    if (nciTagProxy_.expired()) {
        ErrorLog("nciTagProxy_ nullptr");
        return nullptr;
    }
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
    if (nciTagProxy_.expired()) {
        ErrorLog("nciTagProxy_ nullptr");
        return nullptr;
    }
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

void TagDispatcher::HandleTagDebounce()
{
    DebugLog("HandleTagDebounce, unimplemented...");
}

void TagDispatcher::OnNotificationButtonClicked(int notificationId)
{
    InfoLog("notificationId[%{public}d]", notificationId);
    switch (notificationId) {
        case NFC_TRANSPORT_CARD_NOTIFICATION_ID: {
            // start application ability for tag found.
            if (nfcService_) {
                ExternalDepsProxy::GetInstance().DispatchTagAbility(tagInfo_, nfcService_->GetTagServiceIface());
                nfcService_->NotifyMessageToVendor(KITS::TAG_DISPATCH_KEY, "");
            }
            break;
        }
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
            if (nfcService_) {
                ExternalDepsProxy::GetInstance().DispatchTagAbility(tagInfo_, nfcService_->GetTagServiceIface());
                nfcService_->NotifyMessageToVendor(KITS::TAG_DISPATCH_KEY, "");
            }
            break;
        case NFC_NO_HAP_SUPPORTED_NOTIFICATION_ID:
            // start AppGallery
            if (!nciTagProxy_.expired() && nfcService_) {
                std::string appGalleryBundleName = nciTagProxy_.lock()->GetVendorAppGalleryBundleName();
                ExternalDepsProxy::GetInstance().DispatchAppGallery(nfcService_->GetTagServiceIface(),
                                                                    appGalleryBundleName);
            }
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
