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

uint16_t TagDispatcher::HandleNdefDispatch(uint32_t tagDiscId, std::string &msg)
{
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if (nciTagProxyPtr == nullptr) {
        ErrorLog("HandleNdefDispatch, nciTagProxy_ is nullptr");
        return DISPATCH_UNKNOWN;
    }
    std::string tagUid = nciTagProxyPtr->GetTagUid(tagDiscId);
    int msgType = NDEF_TYPE_NORMAL;
    std::string ndef = msg;
    std::string vendorPayload = "";
#ifdef NDEF_BT_ENABLED
    std::shared_ptr<BtData> btData = NdefBtDataParser::CheckBtRecord(msg);
    if (btData && btData->isValid_) {
        msgType = NDEF_TYPE_BT;
        if (!btData->vendorPayload_.empty() && NdefBtDataParser::IsVendorPayloadValid(btData->vendorPayload_)) {
            // Bt msg for NdefMsg Callback: bt payload len | bt payload | mac addr | dev name
            vendorPayload = NfcSdkCommon::IntToHexString(btData->vendorPayload_.length() / HEX_BYTE_LEN);
            vendorPayload.append(btData->vendorPayload_);
            vendorPayload.append(btData->macAddrOrg_);
            vendorPayload.append(NfcSdkCommon::StringToHexString(btData->name_));
        } else {
            InfoLog("BT vendor payload invalid");
        }
    }
#endif
#ifdef NDEF_WIFI_ENABLED
    std::shared_ptr<WifiData> wifiData;
    if (msgType == NDEF_TYPE_NORMAL) {
        wifiData = NdefWifiDataParser::CheckWifiRecord(msg);
        if (wifiData && wifiData->isValid_) {
            msgType = NDEF_TYPE_WIFI;
            vendorPayload = wifiData->vendorPayload_;
        }
    }
#endif
    InfoLog("HandleNdefDispatch, tagUid = %{public}s, msgType = %{public}d",
        KITS::NfcSdkCommon::CodeMiddlePart(tagUid).c_str(), msgType);
    HandleOnNdefMsgDiscovered(tagUid, ndef, vendorPayload, msgType, tagDiscId);
    if (ndefCbRes_) {
        InfoLog("HandleNdefDispatch, is dispatched by ndefMsg callback");
        return DISPATCH_CALLBACK;
    }
    if (msg.empty()) {
        ErrorLog("HandleNdefDispatch, ndef msg is empty");
        return DISPATCH_UNKNOWN;
    }
#ifdef NDEF_BT_ENABLED
    if (msgType == NDEF_TYPE_BT) {
        BtConnectionManager::GetInstance().Initialize(nfcService_);
        BtConnectionManager::GetInstance().TryPairBt(btData);
        return DISPATCH_BT;
    }
#endif
#ifdef NDEF_WIFI_ENABLED
    if (msgType == NDEF_TYPE_WIFI) {
        WifiConnectionManager::GetInstance().Initialize(nfcService_);
        WifiConnectionManager::GetInstance().TryConnectWifi(wifiData);
        return DISPATCH_WIFI;
    }
#endif
    std::shared_ptr<KITS::TagInfo> tagInfo = GetTagInfoFromTag(tagDiscId);
    uint16_t dispatchRes = NdefHarDataParser::GetInstance().TryNdef(msg, tagInfo);
    if (dispatchRes != DISPATCH_UNKNOWN) {
        return dispatchRes;
    }
    return DISPATCH_UNKNOWN;
}

void TagDispatcher::HandleOnNdefMsgDiscovered(const std::string &tagUid, const std::string &ndef,
    const std::string &payload, int ndefMsgType, uint32_t tagDiscId)
{
    if (ndefCb_ != nullptr) {
        KITS::TagInfoParcelable* tagInfoParcel = GetTagInfoParcelableFromTag(tagDiscId);
        if (tagInfoParcel != nullptr) {
            ndefCbRes_ = ndefCb_->OnNdefMsgDiscovered(tagUid, ndef, payload, ndefMsgType, tagInfoParcel);
            delete tagInfoParcel;
            tagInfoParcel = nullptr;
        } else {
            ErrorLog("tagInfoParcel is nullptr");
        }
    }
}

void TagDispatcher::HandleTagFound(uint32_t tagDiscId)
{
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if (nciTagProxyPtr == nullptr) {
        ErrorLog("nciTagProxy_ is nullptr");
        return;
    }
    long tagFoundTime = static_cast<long>(KITS::NfcSdkCommon::GetCurrentTime());
    ndefCbRes_ = false;
    isIsoDep_ = false;
    if (static_cast<int>(nciTagProxyPtr->GetConnectedTech(tagDiscId)) ==
        static_cast<int>(TagTechnology::NFC_ISODEP_TECH)) {
        isIsoDep_ = true;
    }
    std::string ndefMsg = nciTagProxyPtr->FindNdefTech(tagDiscId);
    long readFinishTime = static_cast<long>(KITS::NfcSdkCommon::GetCurrentTime());
    std::shared_ptr<KITS::NdefMessage> ndefMessage = KITS::NdefMessage::GetNdefMessage(ndefMsg);
    KITS::TagInfoParcelable* tagInfo = nullptr;
    bool isNtfPublished = false;
    uint16_t dispatchResult = HandleTagDispatch(ndefMsg, ndefMessage, tagInfo, tagDiscId, isNtfPublished);
    if (tagInfo != nullptr) {
        delete tagInfo;
        tagInfo = nullptr;
    }
    long dispatchFinishTime = static_cast<long>(KITS::NfcSdkCommon::GetCurrentTime());
    SendTagInfoToVendor(tagFoundTime, readFinishTime, dispatchFinishTime, ndefMessage, dispatchResult);
    NdefHarDataParser::GetInstance().ClearRecord0Uri();

#ifndef NFC_VIBRATOR_DISABLED
    StartVibratorOnce(isNtfPublished);
#endif
    // Record types of read tags.
    std::vector<int> techList = nciTagProxyPtr->GetTechList(tagDiscId);
    ExternalDepsProxy::GetInstance().WriteTagFoundHiSysEvent(techList);
}

uint16_t TagDispatcher::HandleTagDispatch(std::string &ndefMsg, std::shared_ptr<KITS::NdefMessage> ndefMessage,
    KITS::TagInfoParcelable* tagInfo, uint32_t tagDiscId, bool &isNtfPublished)
{
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if (nfcService_ == nullptr || nciTagProxyPtr == nullptr) {
        ErrorLog("nfcService_ or nciTagProxy_ is nullptr");
        return DISPATCH_UNKNOWN;
    }
    if (ndefMessage == nullptr) {
        if (!nciTagProxyPtr->Reconnect(tagDiscId)) {
            nciTagProxyPtr->Disconnect(tagDiscId);
            ErrorLog("HandleTagFound bad connection, tag disconnected");
            nfcService_->NotifyMessageToVendor(KITS::NOTIFY_TAG_DISCONNECT, "");
            return DISPATCH_UNKNOWN;
        }
    }
    lastNdefMsg_ = ndefMsg;
    int fieldOnCheckInterval = GetFieldOnCheckInterval();
    InfoLog("fieldOnCheckInterval = %{public}d", fieldOnCheckInterval);
    nciTagProxyPtr->StartFieldOnChecking(tagDiscId, fieldOnCheckInterval);
    tagInfo = GetTagInfoParcelableFromTag(tagDiscId);
    auto pollingMgr = nfcService_->GetNfcPollingManager().lock();
    if (pollingMgr != nullptr && pollingMgr->IsReaderModeEnabled()) {
        pollingMgr->SendTagToReaderApp(tagInfo);
        return DISPATCH_READERMODE;
    }
    if (pollingMgr != nullptr && pollingMgr->IsForegroundEnabled()) {
        pollingMgr->SendTagToForeground(tagInfo);
        return DISPATCH_FOREGROUND;
    }
    ExternalDepsProxy::GetInstance().RegNotificationCallback(nfcService_);
    uint16_t dispatchResult = HandleNdefDispatch(tagDiscId, ndefMsg);
    if (dispatchResult != DISPATCH_UNKNOWN) {
        return dispatchResult;
    }
    isNtfPublished = true;
    return PublishTagNotification(tagDiscId, isIsoDep_);
}

int TagDispatcher::GetFieldOnCheckInterval()
{
    if (fieldOnCheckInterval_ != 0) {
        return fieldOnCheckInterval_;
    } else if (isIsoDep_) {
        return DEFAULT_ISO_DEP_FIELD_ON_CHECK_DURATION;
    } else {
        return DEFAULT_FIELD_ON_CHECK_DURATION;
    }
}

std::string TagDispatcher::ParseNdefInfo(std::shared_ptr<KITS::NdefMessage> ndefMessage)
{
    std::string ndefInfo = "";
    if (ndefMessage == nullptr) {
        return ndefInfo;
    }
    std::vector<std::shared_ptr<NdefRecord>> records = ndefMessage->GetNdefRecords();
    uint16_t recordNum = records.size();
    if (recordNum == 0) {
        return ndefInfo;
    }
    for (uint16_t i = 0; i < recordNum; i++) {
        std::string tagType = NfcSdkCommon::HexStringToAsciiString(records[i]->tagRtdType_);
        std::string payload = NfcSdkCommon::HexStringToAsciiString(records[i]->payload_);
        // control the length of ndefInfo
        uint8_t maxInfoLen = 50;
        if (payload.length() > maxInfoLen) {
            payload = payload.substr(0, maxInfoLen);
        }
        ndefInfo = ndefInfo + tagType + "=" + payload + "|";
    }
    ndefInfo = std::to_string(recordNum) + "|" + ndefInfo;
    return ndefInfo;
}

void TagDispatcher::SendTagInfoToVendor(long tagFoundStartTime, long readFinishTime, long dispatchFinishTime,
    std::shared_ptr<KITS::NdefMessage> ndefMessage, uint16_t dispatchResult)
{
    std::string ndefInfo = ParseNdefInfo(ndefMessage);
    if (nfcService_ == nullptr) {
        ErrorLog("nfcService is nullptr");
        return;
    }
    nfcService_->NotifyMessageToVendor(KITS::NOTIFY_NDEF_INFO_EVENT, ndefInfo);
    std::string readTagInfo = "startTime:" + std::to_string(tagFoundStartTime) + "|readFinishTime:" +
        std::to_string(readFinishTime) + "|dispatchTime:" + std::to_string(dispatchFinishTime) + "|dispatchResult:" +
        std::to_string(dispatchResult);
    auto pollingMgr = nfcService_->GetNfcPollingManager().lock();
    if (dispatchResult == DISPATCH_FOREGROUND && pollingMgr) {
        std::shared_ptr<NfcPollingManager::ForegroundRegistryData> foregroundData = pollingMgr->GetForegroundData();
        if (foregroundData != nullptr) {
            std::string foregroundBundle = foregroundData->element_.GetBundleName();
            readTagInfo = readTagInfo + "|foregroundBundle:" + foregroundBundle;
        }
    }
    if (dispatchResult == DISPATCH_READERMODE && pollingMgr) {
        std::shared_ptr<NfcPollingManager::ReaderModeRegistryData> readerModeData = pollingMgr->GetReaderModeData();
        if (readerModeData != nullptr) {
            std::string readerModeBundle = readerModeData->element_.GetBundleName();
            readTagInfo = readTagInfo + "|readerModeBundle:" + readerModeBundle;
        }
    }
    nfcService_->NotifyMessageToVendor(KITS::NOTIFY_READ_TAG_EVENT, readTagInfo);
}

void TagDispatcher::StartVibratorOnce(bool isNtfPublished)
{
    if (!ndefCbRes_) {
        ExternalDepsProxy::GetInstance().StartVibratorOnce(isNtfPublished);
    }
}
void TagDispatcher::HandleTagLost(uint32_t tagDiscId)
{
    InfoLog("HandleTagLost, tagDiscId: %{public}d", tagDiscId);
}

std::shared_ptr<KITS::TagInfo> TagDispatcher::GetTagInfoFromTag(uint32_t tagDiscId)
{
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if (nciTagProxyPtr == nullptr) {
        ErrorLog("nciTagProxy is nullptr");
        return nullptr;
    }
    std::vector<int> techList = nciTagProxyPtr->GetTechList(tagDiscId);
    std::string tagUid = nciTagProxyPtr->GetTagUid(tagDiscId);
    std::vector<AppExecFwk::PacMap> tagTechExtras = nciTagProxyPtr->GetTechExtrasData(tagDiscId);
    DebugLog("GetTagInfoFromTag: tag uid = %{public}s, techListLen = %{public}zu, extrasLen = %{public}zu,"
        "rfID = %{public}d", KITS::NfcSdkCommon::CodeMiddlePart(tagUid).c_str(),
        techList.size(), tagTechExtras.size(), tagDiscId);
    return std::make_shared<KITS::TagInfo>(techList, tagTechExtras, tagUid, tagDiscId,
        nfcService_->GetTagServiceIface());
}

KITS::TagInfoParcelable* TagDispatcher::GetTagInfoParcelableFromTag(uint32_t tagDiscId)
{
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if (nciTagProxyPtr == nullptr) {
        ErrorLog("nciTagProxy is nullptr");
        return nullptr;
    }
    std::vector<int> techList = nciTagProxyPtr->GetTechList(tagDiscId);
    std::string tagUid = nciTagProxyPtr->GetTagUid(tagDiscId);
    std::vector<AppExecFwk::PacMap> tagTechExtras = nciTagProxyPtr->GetTechExtrasData(tagDiscId);
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
            HandleNoHapSupportId();
            break;
        case NFC_TEXT_NOTIFICATION_ID:
            HandleTextId();
            break;
        default:
            WarnLog("unknown notification Id");
            break;
    }
}

void TagDispatcher::HandleNoHapSupportId()
{
    if (!nciTagProxy_.expired() && nfcService_) {
        auto tagProxy = nciTagProxy_.lock();
        if (tagProxy) {
            std::string appGalleryBundleName = tagProxy->GetVendorInfo(VendorInfoType::HAP_NAME_GALLERY);
            ExternalDepsProxy::GetInstance().DispatchAppGallery(nfcService_->GetTagServiceIface(),
                                                                appGalleryBundleName);
        }
    }
}

void TagDispatcher::HandleTextId()
{
    if (!nciTagProxy_.expired()) {
        auto tagProxy = nciTagProxy_.lock();
        if (tagProxy) {
            std::string notepadBundleName = tagProxy->GetVendorInfo(VendorInfoType::HAP_NAME_NOTEPAD);
            ExternalDepsProxy::GetInstance().StartNotepadAbility(notepadBundleName);
        }
    }
}

uint16_t TagDispatcher::PublishTagNotification(uint32_t tagDiscId, bool isIsoDep)
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
    tagInfo_ = GetTagInfoFromTag(tagDiscId);
    return notificationId == NFC_TRANSPORT_CARD_NOTIFICATION_ID ? DISPATCH_TRAFFIC : DISPATCH_UNKNOWN_TAG;
}

void TagDispatcher::SetFieldCheckInterval(int interval)
{
    InfoLog("interval = %{public}d", interval);
    fieldOnCheckInterval_ = interval;
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
