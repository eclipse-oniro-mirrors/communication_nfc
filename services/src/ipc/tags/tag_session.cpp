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
#include "tag_session.h"

#include "app_state_observer.h"
#include "external_deps_proxy.h"
#include "foreground_death_recipient.h"
#include "ipc_skeleton.h"
#include "loghelper.h"
#include "reader_mode_death_recipient.h"

namespace OHOS {
namespace NFC {
namespace TAG {
using OHOS::AppExecFwk::ElementName;

// NFC_A = 1 ~ NDEF_FORMATABLE = 10
const int MAX_TECH = 12;
int g_techTimeout[MAX_TECH] = {0};
int g_maxTransLength[MAX_TECH] = {0, 253, 253, 0xFEFF, 255, 253, 0, 0, 253, 253, 0, 0};
std::shared_ptr<AppStateObserver> g_appStateObserver = nullptr;

TagSession::TagSession(std::shared_ptr<NfcService> service)
    : nfcService_(service)
{
    if (service) {
        nciTagProxy_ = service->GetNciTagProxy();
        nfcPollingManager_ = service->GetNfcPollingManager();
        tagDispatcher_ = service->GetTagDispatcher();
    }
    g_appStateObserver = std::make_shared<AppStateObserver>(this);
}

TagSession::~TagSession()
{
}

int32_t TagSession::CallbackEnter(uint32_t code)
{
    InfoLog("TagSession, code[%{public}u]", code);
    return ERR_NONE;
}

int32_t TagSession::CallbackExit(uint32_t code, int32_t result)
{
    InfoLog("TagSession, code[%{public}u], result[%{public}d]", code, result);
    return ERR_NONE;
}

/**
 * @brief To connect the tagRfDiscId by technology.
 * @param tagRfDiscId the rf disc id of tag
 * @param technology the tag technology
 * @return the result to connect the tag
 */
ErrCode TagSession::Connect(int32_t tagRfDiscId, int32_t technology)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("Connect ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    if (technology < 0 || technology >= MAX_TECH) {
        ErrorLog("Connect, invalid technology %{public}d", technology);
        return KITS::ERR_TAG_PARAMETERS;
    }
    auto nfcServicePtr = nfcService_.lock();
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if ((nfcServicePtr == nullptr) || (nciTagProxyPtr == nullptr)) {
        ErrorLog("Connect, nfcService or nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("Connect, IsNfcEnabled error");
        return KITS::ERR_TAG_STATE_NFC_CLOSED;
    }
    if (!nciTagProxyPtr->IsTagFieldOn(tagRfDiscId)) {
        ErrorLog("Connect, IsTagFieldOn error");
        return KITS::ERR_TAG_STATE_LOST;
    }

    if (nciTagProxyPtr->Connect(tagRfDiscId, technology)) {
        return KITS::ERR_NONE;
    } else {
        ErrorLog("Connect, call error");
        return KITS::ERR_TAG_STATE_IO_FAILED;
    }
}

/**
 * @brief To get connection status of tag.
 * @param tagRfDiscId the rf disc id of tag
 * @param isConnected the connection status of tag
 * @return the result to get connection status of the tag
 */
ErrCode TagSession::IsConnected(int32_t tagRfDiscId, bool& isConnected)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("IsConnected, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    auto nfcServicePtr = nfcService_.lock();
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if ((nfcServicePtr == nullptr) || (nciTagProxyPtr == nullptr)) {
        ErrorLog("IsConnected, nfcService or nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("IsConnected, IsNfcEnabled error");
        return KITS::ERR_TAG_STATE_NFC_CLOSED;
    }
    isConnected = nciTagProxyPtr->IsTagFieldOn(tagRfDiscId);
    return KITS::ERR_NONE;
}

/**
 * @brief To reconnect the tagRfDiscId.
 * @param tagRfDiscId the rf disc id of tag
 * @return the result to reconnect the tag
 */
ErrCode TagSession::Reconnect(int32_t tagRfDiscId)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("Reconnect, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    // Check if NFC is enabled
    auto nfcServicePtr = nfcService_.lock();
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if ((nfcServicePtr == nullptr) || (nciTagProxyPtr == nullptr)) {
        ErrorLog("Reconnect, nfcService or nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("Reconnect, IsNfcEnabled error");
        return KITS::ERR_TAG_STATE_NFC_CLOSED;
    }

    if (nciTagProxyPtr->Reconnect(tagRfDiscId)) {
        return KITS::ERR_NONE;
    } else {
        ErrorLog("Reconnect, call error");
        return KITS::ERR_TAG_STATE_IO_FAILED;
    }
}

/**
 * @brief To disconnect the tagRfDiscId.
 * @param tagRfDiscId the rf disc id of tag
 */
ErrCode TagSession::Disconnect(int32_t tagRfDiscId)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("Disconnect, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    // Check if NFC is enabled
    auto nfcServicePtr = nfcService_.lock();
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if ((nfcServicePtr == nullptr) || (nciTagProxyPtr == nullptr)) {
        ErrorLog("Disconnect nfcService or nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("Disconnect, IsNfcEnabled error");
        return KITS::ERR_TAG_STATE_NFC_CLOSED;
    }

    nciTagProxyPtr->Disconnect(tagRfDiscId);
    return KITS::ERR_NONE;
}

ErrCode TagSession::SetTimeout(int32_t tagRfDiscId, int32_t timeout, int32_t technology)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("SetTimeout, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    if (technology < 0 || technology >= MAX_TECH) {
        ErrorLog("SetTimeout, invalid technology %{public}d", technology);
        return KITS::ERR_TAG_PARAMETERS;
    }
    // Check if NFC is enabled
    auto nfcServicePtr = nfcService_.lock();
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if ((nfcServicePtr == nullptr) || (nciTagProxyPtr == nullptr)) {
        ErrorLog("SetTimeout nfcService or nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("SetTimeout, IsNfcEnabled error");
        return KITS::ERR_TAG_STATE_NFC_CLOSED;
    }

    nciTagProxyPtr->SetTimeout(tagRfDiscId, timeout, technology);
    return KITS::ERR_NONE;
}

ErrCode TagSession::GetTimeout(int32_t tagRfDiscId, int32_t technology, int32_t& timeout)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("GetTimeout, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    if (technology < 0 || technology >= MAX_TECH) {
        ErrorLog("GetTimeout, invalid technology %{public}d", technology);
        return KITS::ERR_TAG_PARAMETERS;
    }
    // Check if NFC is enabled
    auto nfcServicePtr = nfcService_.lock();
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if ((nfcServicePtr == nullptr) || (nciTagProxyPtr == nullptr)) {
        ErrorLog("GetTimeout nfcService or nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("GetTimeout, IsNfcEnabled error");
        return KITS::ERR_TAG_STATE_NFC_CLOSED;
    }

    uint32_t timeoutTemp = 0;
    nciTagProxyPtr->GetTimeout(tagRfDiscId, timeoutTemp, technology);
    timeout = static_cast<int>(timeoutTemp);
    return KITS::ERR_NONE;
}

ErrCode TagSession::ResetTimeout(int32_t tagRfDiscId)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("ResetTimeout, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    auto nfcServicePtr = nfcService_.lock();
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if ((nfcServicePtr == nullptr) || (nciTagProxyPtr == nullptr)) {
        ErrorLog("ResetTimeout nfcService or nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("ResetTimeout, IsNfcEnabled error");
        return KITS::ERR_TAG_STATE_NFC_CLOSED;
    }
    nciTagProxyPtr->ResetTimeout(tagRfDiscId);
    return KITS::ERR_NONE;
}

/**
 * @brief Get the TechList of the tagRfDiscId.
 * @param tagRfDiscId the rf disc id of tag
 * @return TechList
 */
ErrCode TagSession::GetTechList(int32_t tagRfDiscId, std::vector<int32_t>& funcResult)
{
    funcResult = std::vector<int32_t>();
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("GetTechList, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    // Check if NFC is enabled
    auto nfcServicePtr = nfcService_.lock();
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if ((nfcServicePtr == nullptr) || (nciTagProxyPtr == nullptr)) {
        ErrorLog("GetTechList nfcService or nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("GetTechList, IsNfcEnabled error");
        return KITS::ERR_TAG_STATE_NFC_CLOSED;
    }

    funcResult = nciTagProxyPtr->GetTechList(tagRfDiscId);
    return KITS::ERR_NONE;
}

/**
 * @brief Checking the tagRfDiscId is present.
 * @param tagRfDiscId the rf disc id of tag
 * @return true - Presnet; the other - No Presnet
 */
ErrCode TagSession::IsTagFieldOn(int32_t tagRfDiscId, bool& funcResult)
{
    funcResult = false;
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("IsTagFieldOn, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    // Check if NFC is enabled
    auto nfcServicePtr = nfcService_.lock();
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if ((nfcServicePtr == nullptr) || (nciTagProxyPtr == nullptr)) {
        ErrorLog("IsTagFieldOn nfcService or nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("IsTagFieldOn, IsNfcEnabled error");
        return KITS::ERR_TAG_STATE_NFC_CLOSED;
    }

    funcResult = nciTagProxyPtr->IsTagFieldOn(tagRfDiscId);
    return KITS::ERR_NONE;
}

/**
 * @brief Checking the tagRfDiscId is a Ndef Tag.
 * @param tagRfDiscId the rf disc id of tag
 * @return true - Ndef Tag; the other - No Ndef Tag
 */
ErrCode TagSession::IsNdef(int32_t tagRfDiscId, bool& funcResult)
{
    funcResult = false;
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("IsNdef, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    // Check if NFC is enabled
    auto nfcServicePtr = nfcService_.lock();
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if ((nfcServicePtr == nullptr) || (nciTagProxyPtr == nullptr)) {
        ErrorLog("IsNdef nfcService or nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("IsNdef, IsNfcEnabled error");
        return KITS::ERR_TAG_STATE_NFC_CLOSED;
    }

    std::vector<int> ndefInfo;
    funcResult = nciTagProxyPtr->DetectNdefInfo(tagRfDiscId, ndefInfo);
    return KITS::ERR_NONE;
}

ErrCode TagSession::SendRawFrame(int32_t tagRfDiscId, const std::string& hexCmdData, bool raw, std::string& hexRespData)
{
    DebugLog("Send Raw(%{public}d) Frame", raw);
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("SendRawFrame, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    // Check if NFC is enabled
    auto nfcServicePtr = nfcService_.lock();
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if ((nfcServicePtr == nullptr) || (nciTagProxyPtr == nullptr)) {
        ErrorLog("SendRawFrame nfcService or nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("SendRawFrame, IsNfcEnabled error");
        return KITS::ERR_TAG_STATE_NFC_CLOSED;
    }

    // Check if length is within limits
    int maxSize = 0;
    GetMaxTransceiveLength(nciTagProxyPtr->GetConnectedTech(tagRfDiscId), maxSize);
    if (KITS::NfcSdkCommon::GetHexStrBytesLen(hexCmdData) > static_cast<uint32_t>(maxSize)) {
        ErrorLog("hexCmdData exceed max size.");
        return KITS::ERR_TAG_PARAMETERS;
    }

    int result = nciTagProxyPtr->Transceive(tagRfDiscId, hexCmdData, hexRespData);
    DebugLog("TagSession::SendRawFrame, result = 0x%{public}X", result);
    if ((result == 0) && (!hexRespData.empty())) {
        return KITS::ERR_NONE;
    } else if (result == 1) {  // result == 1 means that Tag lost
        ErrorLog("TagSession::SendRawFrame: tag lost.");
        return KITS::ERR_TAG_STATE_LOST;
    }
    ErrorLog("TagSession::SendRawFrame: result failed.");
    return KITS::ERR_TAG_STATE_IO_FAILED;
}

/**
 * @brief Reading from the host tag
 * @param tagRfDiscId the rf disc id of tag
 * @return the read data
 */
ErrCode TagSession::NdefRead(int32_t tagRfDiscId, std::string& ndefMessage)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("NdefRead, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    // Check if NFC is enabled
    auto nfcServicePtr = nfcService_.lock();
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if ((nfcServicePtr == nullptr) || (nciTagProxyPtr == nullptr)) {
        ErrorLog("NdefRead nfcService or nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("NdefRead, IsNfcEnabled error");
        return KITS::ERR_TAG_STATE_NFC_CLOSED;
    }

    ndefMessage = nciTagProxyPtr->ReadNdef(tagRfDiscId);
    return KITS::ERR_NONE;
}

/**
 * @brief Writing the data into the host tag.
 * @param tagRfDiscId the rf disc id of tag
 * @param msg the wrote data
 * @return the Writing Result
 */
ErrCode TagSession::NdefWrite(int32_t tagRfDiscId, const std::string& msg)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("NdefWrite, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    // Check if NFC is enabled
    auto nfcServicePtr = nfcService_.lock();
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if ((nfcServicePtr == nullptr) || (nciTagProxyPtr == nullptr)) {
        ErrorLog("NdefWrite nfcService or nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("NdefWrite, IsNfcEnabled error");
        return KITS::ERR_TAG_STATE_NFC_CLOSED;
    }

    if (msg.empty()) {
        ErrorLog("NdefWrite, msg.empty error");
        return KITS::ERR_TAG_PARAMETERS;
    }

    if (nciTagProxyPtr->WriteNdef(tagRfDiscId, msg)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_TAG_STATE_IO_FAILED;
}

/**
 * @brief Making the host tag to read only.
 * @param tagRfDiscId the rf disc id of tag
 * @return the making result
 */
ErrCode TagSession::NdefMakeReadOnly(int32_t tagRfDiscId)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("NdefMakeReadOnly, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    // Check if NFC is enabled
    auto nfcServicePtr = nfcService_.lock();
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if ((nfcServicePtr == nullptr) || (nciTagProxyPtr == nullptr)) {
        ErrorLog("NdefMakeReadOnly nfcService or nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("NdefMakeReadOnly, IsNfcEnabled error");
        return KITS::ERR_TAG_STATE_NFC_CLOSED;
    }

    if (nciTagProxyPtr->SetNdefReadOnly(tagRfDiscId)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_TAG_STATE_IO_FAILED;
}

/**
 * @brief format the tag by Ndef
 * @param tagRfDiscId the rf disc id of tag
 * @param key the format key
 * @return the format result
 */
ErrCode TagSession::FormatNdef(int32_t tagRfDiscId, const std::string& key)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("FormatNdef, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    // Check if NFC is enabled
    auto nfcServicePtr = nfcService_.lock();
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if ((nfcServicePtr == nullptr) || (nciTagProxyPtr == nullptr)) {
        ErrorLog("FormatNdef nfcService or nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("FormatNdef, IsNfcEnabled error");
        return KITS::ERR_TAG_STATE_NFC_CLOSED;
    }

    if (nciTagProxyPtr->FormatNdef(tagRfDiscId, key)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_TAG_STATE_IO_FAILED;
}

ErrCode TagSession::CanMakeReadOnly(int32_t ndefType, bool& canSetReadOnly)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("CanMakeReadOnly, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    if (nfcService_.expired()) {
        ErrorLog("CanMakeReadOnly, expired");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if (nciTagProxyPtr == nullptr) {
        ErrorLog("nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    canSetReadOnly = nciTagProxyPtr->CanMakeReadOnly(ndefType);
    return KITS::ERR_NONE;
}

/**
 * @brief Get Max Transceive Length
 * @param technology the tag technology
 * @return Max Transceive Length
 */
ErrCode TagSession::GetMaxTransceiveLength(int32_t technology, int32_t& maxSize)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("GetMaxTransceiveLength, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    if (technology < 0 || technology >= MAX_TECH) {
        ErrorLog("GetMaxTransceiveLength, technology not support");
        return KITS::ERR_TAG_PARAMETERS;
    }
    maxSize = g_maxTransLength[technology];
    return KITS::ERR_NONE;
}

ErrCode TagSession::IsSupportedApdusExtended(bool& isSupported)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("IsSupportedApdusExtended, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    if (nfcService_.expired()) {
        ErrorLog("IsSupportedApdusExtended, expired");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    auto nciTagProxyPtr = nciTagProxy_.lock();
    if (nciTagProxyPtr == nullptr) {
        ErrorLog("IsSupportedApdusExtended nciTagProxy is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    isSupported = nciTagProxyPtr->IsExtendedLengthApduSupported();
    return KITS::ERR_NONE;
}

uint16_t TagSession::GetFgDataVecSize()
{
    std::lock_guard<std::mutex> guard(mutex_);
    return fgDataVec_.size();
}

uint16_t TagSession::GetReaderDataVecSize()
{
    std::lock_guard<std::mutex> guard(mutex_);
    return readerDataVec_.size();
}

void TagSession::CheckFgAppStateChanged(const std::string &bundleName, const std::string &abilityName,
    int abilityState)
{
    std::lock_guard<std::mutex> guard(mutex_);
    for (auto fgData = fgDataVec_.begin(); fgData != fgDataVec_.end(); fgData++) {
        ElementName element = fgData->element_;
        if (element.GetBundleName() == bundleName && element.GetAbilityName() == abilityName) {
            // app changes to foreground, RegForegroundDispatch.
            if (abilityState == static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND) &&
                !fgData->isEnableForeground_) {
                InfoLog("app changes to foreground, RegForegroundDispatchInner");
                RegForegroundDispatchInner(element, fgData->techs_, fgData->cb_);
                return;
            }
            // app changes to background, UnregForegroundDispatchInner.
            if (abilityState == static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND) &&
                fgData->isEnableForeground_) {
                InfoLog("app changes to background, UnregForegroundDispatchInner");
                UnregForegroundDispatchInner(element, false);
                return;
            }
            // app death, UnregForegroundDispatchInner and erase from fgDtataVec_.
            if (abilityState == static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_TERMINATED)) {
                InfoLog("app died, unregForegroundDispatchInner and erase fgData");
                UnregForegroundDispatchInner(element, false);
                fgDataVec_.erase(fgData);
                return;
            }
        }
    }
}

void TagSession::CheckReaderAppStateChanged(const std::string &bundleName, const std::string &abilityName,
    int abilityState)
{
    std::lock_guard<std::mutex> guard(mutex_);
    for (auto readerData = readerDataVec_.begin(); readerData != readerDataVec_.end(); readerData++) {
        ElementName element = readerData->element_;
        if (element.GetBundleName() != bundleName || element.GetAbilityName() != abilityName) {
            continue;
        }
        // app changes to foreground, RegReaderModeInner.
        if (abilityState == static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND) &&
            !readerData->isEnabled_) {
            InfoLog("app changes to foreground, RegReaderModeInner");
            if (readerData->interval_ > 0) {
                RegReaderModeInnerWithIntvl(element, readerData->techs_, readerData->cb_, readerData->interval_);
            } else {
                RegReaderModeInner(element, readerData->techs_, readerData->cb_);
            }
            return;
        }
        // app changes to background, UnregReaderModeInner.
        if (abilityState == static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_BACKGROUND) &&
            readerData->isEnabled_) {
            InfoLog("app changes to background, UnregReaderModeInner");
            UnregReaderModeInner(element, false);
            return;
        }
        // app death, UnregReaderModeInner and erase from readerDataVec_.
        if (abilityState == static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_TERMINATED)) {
            InfoLog("app died, UnregReaderModeInner and erase readerData");
            UnregReaderModeInner(element, false);
            readerDataVec_.erase(readerData);
            return;
        }
    }
}

void TagSession::HandleAppStateChanged(const std::string &bundleName, const std::string &abilityName,
    int abilityState)
{
    if (GetFgDataVecSize() == 0 && GetReaderDataVecSize() == 0) {
        return;
    }
    InfoLog("HandleAppStateChanged: bundleName = %{public}s, abilityName = %{public}s, abilityState = %{public}d",
        bundleName.c_str(), abilityName.c_str(), abilityState);
    CheckFgAppStateChanged(bundleName, abilityName, abilityState);
    CheckReaderAppStateChanged(bundleName, abilityName, abilityState);
}

bool TagSession::IsSameAppAbility(const ElementName &element, const ElementName &fgElement)
{
    if (element.GetBundleName() == fgElement.GetBundleName() &&
        element.GetAbilityName() == fgElement.GetAbilityName()) {
        return true;
    }
    return false;
}

bool TagSession::IsSameDiscoveryPara(const std::vector<uint32_t> &discoveryPara, const std::vector<uint32_t> &discTech)
{
    std::set<uint32_t> discoveryParaSet = {};
    std::set<uint32_t> discTechSet = {};
    for (uint32_t it : discoveryPara) {
        discoveryParaSet.insert(it);
    }
    for (uint32_t it : discTech) {
        discTechSet.insert(it);
    }
    bool isSameDiscoveryPara = (discoveryParaSet.size() == discTechSet.size()) &&
        std::equal(discoveryParaSet.begin(), discoveryParaSet.end(), discTechSet.begin());
    InfoLog("IsSameDiscoveryPara? %{public}d", isSameDiscoveryPara);
    return isSameDiscoveryPara;
}

#ifdef VENDOR_APPLICATIONS_ENABLED
bool TagSession::IsVendorProcess()
{
    auto tag = nciTagProxy_.lock();
    if (tag) {
        return tag->IsVendorProcess();
    }
    ErrorLog("IsVendorProcess: tag proxy null");
    return false;
}
#endif

ErrCode TagSession::RegForegroundDispatch(
    const ElementName& element, const std::vector<uint32_t>& discTech, const sptr<IForegroundCallback>& cb)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("RegForegroundDispatch, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }
    if (cb == nullptr || cb->AsObject() == nullptr) {
        ErrorLog("callback nullptr.");
        return KITS::ERR_TAG_PARAMETERS;
    }

    std::unique_ptr<ForegroundDeathRecipient> recipient
        = std::make_unique<ForegroundDeathRecipient>(this, IPCSkeleton::GetCallingTokenID());
    sptr<IRemoteObject::DeathRecipient> dr(recipient.release());
    if (!cb->AsObject()->AddDeathRecipient(dr)) {
        ErrorLog("Failed to add death recipient");
        return KITS::ERR_TAG_PARAMETERS;
    }

    std::lock_guard<std::mutex> guard(mutex_);
    foregroundDeathRecipient_ = dr;
    foregroundCallback_ = cb;
    bool isVendorApp = false;
    if (!g_appStateObserver->IsForegroundApp(element.GetBundleName())) {
#ifdef VENDOR_APPLICATIONS_ENABLED
        if (!IsVendorProcess()) {
            ErrorLog("not foreground app.");
            return KITS::ERR_NONE;
        } else {
            InfoLog("is vendor app");
            isVendorApp = true;
        }
#else
        ErrorLog("not foreground app.");
        return KITS::ERR_NONE;
#endif
    }
    return RegForegroundDispatchInner(element, discTech, cb, isVendorApp);
}

int TagSession::RegForegroundDispatchInner(const ElementName &element, const std::vector<uint32_t> &discTech,
    const sptr<KITS::IForegroundCallback> &callback, bool isVendorApp)
{
    if (IsFgRegistered(element, discTech, callback)) {
        WarnLog("%{public}s already RegForegroundDispatch", element.GetBundleName().c_str());
        return KITS::ERR_NONE;
    }
    InfoLog("RegForegroundDispatch: bundleName = %{public}s, abilityName = %{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    auto nfcPollingManagerPtr = nfcPollingManager_.lock();
    if (nfcPollingManagerPtr == nullptr) {
        ErrorLog("RegForegroundDispatch nfcPollingManager is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (nfcPollingManagerPtr->EnableForegroundDispatch(element, discTech, callback, isVendorApp)) {
        bool isFgAbility = nfcPollingManagerPtr->
            CheckForegroundAbility(element.GetBundleName(), element.GetAbilityName());
        SubErrorCode subErrorCode = isFgAbility ?
            SubErrorCode::REG_FOREGROUND_DISPATCH : SubErrorCode::REG_FOREGROUND_DISPATCH_ABILITY_INVALID;
        ExternalDepsProxy::GetInstance().WriteAppBehaviorHiSysEvent(
            subErrorCode, element.GetBundleName());
        return KITS::ERR_NONE;
    }
    return KITS::ERR_NFC_PARAMETERS;
}

bool TagSession::IsFgRegistered(const ElementName &element, const std::vector<uint32_t> &discTech,
    const sptr<KITS::IForegroundCallback> &callback)
{
    for (FgData &fgData : fgDataVec_) {
        ElementName fgElement = fgData.element_;
        if (IsSameAppAbility(element, fgElement)) {
            if (fgData.isEnableForeground_) {
                return true;
            }
            InfoLog("Enable FgData: bundleName = %{public}s, abilityName = %{public}s",
                fgElement.GetBundleName().c_str(), fgElement.GetAbilityName().c_str());
            fgData.isEnableForeground_ = true;
            return false;
        }
    }
    FgData fgData(true, element, discTech, callback);
    fgDataVec_.push_back(fgData);
    InfoLog("Add new FgData to vector: %{public}s, %{public}s", element.GetBundleName().c_str(),
        element.GetAbilityName().c_str());
    return false;
}

ErrCode TagSession::UnregForegroundDispatch(const ElementName& element)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("UnregForegroundDispatch, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    std::lock_guard<std::mutex> guard(mutex_);
    return UnregForegroundDispatchInner(element, true);
}

int TagSession::UnregForegroundDispatchInner(const ElementName &element, bool isAppUnregister)
{
    if (IsFgUnregistered(element, isAppUnregister)) {
        WarnLog("%{public}s already UnregForegroundDispatch", element.GetBundleName().c_str());
        return KITS::ERR_NONE;
    }
    InfoLog("UnregForegroundDispatchInner: bundleName = %{public}s, abilityName = %{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    if (nfcPollingManager_.expired()) {
        ErrorLog("UnregForegroundDispatch, expired");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (nfcPollingManager_.lock()->DisableForegroundDispatch(element)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_NFC_PARAMETERS;
}

bool TagSession::IsFgUnregistered(const ElementName &element, bool isAppUnregister)
{
    if (fgDataVec_.size() == 0) {
        return true;
    }
    bool isUnregistered = false;
    for (auto fgData = fgDataVec_.begin(); fgData != fgDataVec_.end(); fgData++) {
        if (IsSameAppAbility(element, fgData->element_)) {
            // isEnableForeground_ is false => is already unregistered.
            if (!fgData->isEnableForeground_) {
                isUnregistered = true;
            }
            fgData->isEnableForeground_ = false;
            // app unregister, delete record
            // background unregister, retain record, re-register when switching to foreground
            if (isAppUnregister) {
                InfoLog("isAppUnregister: erase fgData");
                fgDataVec_.erase(fgData);
            }
            return isUnregistered;
        }
    }
    // No record, indicating has not registered(or IsFgUnregistered).
    return true;
}

bool TagSession::IsReaderRegistered(const ElementName &element, const std::vector<uint32_t> &discTech,
    const sptr<KITS::IReaderModeCallback> &callback, int interval)
{
    for (ReaderData &readerData : readerDataVec_) {
        ElementName readerElement = readerData.element_;
        if (IsSameAppAbility(element, readerElement)) {
            if (readerData.isEnabled_ &&
                IsSameDiscoveryPara(readerData.techs_, discTech) && readerData.interval_ == interval) {
                return true;
            }
            InfoLog("Enable ReaderData: bundleName = %{public}s, abilityName = %{public}s",
                readerElement.GetBundleName().c_str(), readerElement.GetAbilityName().c_str());
            readerData.isEnabled_ = true;
            readerData.techs_ = discTech;
            readerData.interval_ = interval;
            return false;
        }
    }
    ReaderData readerData(true, element, discTech, callback, interval);
    readerDataVec_.push_back(readerData);
    InfoLog("Add new ReaderData to vector: %{public}s, %{public}s", element.GetBundleName().c_str(),
        element.GetAbilityName().c_str());
    return false;
}

bool TagSession::IsReaderUnregistered(const ElementName &element, bool isAppUnregistered)
{
    if (readerDataVec_.size() == 0) {
        return true;
    }
    bool isUnregistered = false;
    for (auto readerData = readerDataVec_.begin(); readerData != readerDataVec_.end(); readerData++) {
        if (IsSameAppAbility(element, readerData->element_)) {
            // isEnabled_ is false => is already unregistered.
            if (!readerData->isEnabled_) {
                isUnregistered = true;
            }
            readerData->isEnabled_ = false;
            // app unregister, delete record
            // background unregister, retain record, re-register when switching to foreground
            if (isAppUnregistered) {
                InfoLog("isAppUnregister: erase readerData");
                readerDataVec_.erase(readerData);
            }
            return isUnregistered;
        }
    }
    // No record, indicating has not registered(or IsReaderUnregistered).
    return true;
}

int TagSession::RegReaderModeInner(const ElementName &element, const std::vector<uint32_t> &discTech,
    const sptr<KITS::IReaderModeCallback> &callback, bool isVendorApp)
{
    if (IsReaderRegistered(element, discTech, callback, 0)) {
        WarnLog("%{public}s already RegReaderMode", element.GetBundleName().c_str());
        return KITS::ERR_NONE;
    }
    InfoLog("RegReaderModeInner: bundleName = %{public}s, abilityName = %{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    auto nfcPollingManagerPtr = nfcPollingManager_.lock();
    if (nfcPollingManagerPtr == nullptr) {
        ErrorLog("RegReaderModeInner nfcPollingManager_ is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (nfcPollingManagerPtr->EnableReaderMode(element, discTech, callback, isVendorApp)) {
        bool isFgAbility = nfcPollingManagerPtr->
            CheckForegroundAbility(element.GetBundleName(), element.GetAbilityName());
        SubErrorCode subErrorCode = isFgAbility ?
            SubErrorCode::REG_READERMODE : SubErrorCode::REG_READERMODE_ABILITY_INVALID;
        ExternalDepsProxy::GetInstance().WriteAppBehaviorHiSysEvent(
            subErrorCode, element.GetBundleName());
        return KITS::ERR_NONE;
    }
    return KITS::ERR_NFC_PARAMETERS;
}

int TagSession::UnregReaderModeInner(const ElementName &element, bool isAppUnregister)
{
    if (IsReaderUnregistered(element, isAppUnregister)) {
        WarnLog("%{public}s already UnregReaderMode", element.GetBundleName().c_str());
        return KITS::ERR_NONE;
    }
    InfoLog("UnregReaderModeInner: bundleName = %{public}s, abilityName = %{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    if (nfcPollingManager_.expired()) {
        ErrorLog("UnregReaderMode, expired");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    SetFieldCheckInterval(0);
    if (nfcPollingManager_.lock()->DisableReaderMode(element)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_NFC_PARAMETERS;
}

ErrCode TagSession::RegReaderMode(
    const ElementName& element, const std::vector<uint32_t>& discTech, const sptr<IReaderModeCallback>& cb)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("RegReaderMode, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }
    if (cb == nullptr || cb->AsObject() == nullptr) {
        ErrorLog("callback nullptr.");
        return KITS::ERR_NFC_PARAMETERS;
    }

    std::unique_ptr<ReaderModeDeathRecipient> recipient
        = std::make_unique<ReaderModeDeathRecipient>(this, IPCSkeleton::GetCallingTokenID());
    sptr<IRemoteObject::DeathRecipient> dr(recipient.release());
    if (!cb->AsObject()->AddDeathRecipient(dr)) {
        ErrorLog("Failed to add death recipient");
        return KITS::ERR_NFC_PARAMETERS;
    }

    std::lock_guard<std::mutex> guard(mutex_);
    readerModeDeathRecipient_ = dr;
    readerModeCallback_ = cb;
    bool isVendorApp = false;
    if (!g_appStateObserver->IsForegroundApp(element.GetBundleName())) {
#ifdef VENDOR_APPLICATIONS_ENABLED
        if (!IsVendorProcess()) {
            ErrorLog("not foreground app.");
            return KITS::ERR_TAG_APP_NOT_FOREGROUND;
        } else {
            InfoLog("is vendor app");
            isVendorApp = true;
        }
#else
        ErrorLog("not foreground app.");
        return KITS::ERR_TAG_APP_NOT_FOREGROUND;
#endif
    }
    return RegReaderModeInner(element, discTech, cb, isVendorApp);
}

int TagSession::RegReaderModeInnerWithIntvl(const ElementName &element, const std::vector<uint32_t> &discTech,
    const sptr<KITS::IReaderModeCallback> &callback, bool isVendorApp, int interval)
{
    if (IsReaderRegistered(element, discTech, callback, interval)) {
        WarnLog("%{public}s already RegReaderMode", element.GetBundleName().c_str());
        return KITS::ERR_NONE;
    }
    InfoLog("bundleName = %{public}s, abilityName = %{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    auto nfcPollingManagerPtr = nfcPollingManager_.lock();
    if (nfcPollingManagerPtr == nullptr) {
        ErrorLog("RegReaderModeInnerWithIntvl nfcPollingManager_ is nullptr");
        return KITS::ERR_TAG_STATE_UNBIND;
    }
    if (nfcPollingManagerPtr->EnableReaderMode(element, discTech, callback, isVendorApp)) {
        bool isFgAbility = nfcPollingManagerPtr->
            CheckForegroundAbility(element.GetBundleName(), element.GetAbilityName());
        SubErrorCode subErrorCode = isFgAbility ?
            SubErrorCode::REG_READERMODE : SubErrorCode::REG_READERMODE_ABILITY_INVALID;
        ExternalDepsProxy::GetInstance().WriteAppBehaviorHiSysEvent(
            subErrorCode, element.GetBundleName());
        if (interval > 0) {
            SetFieldCheckInterval(interval);
        }
        return KITS::ERR_NONE;
    }
    return KITS::ERR_NFC_PARAMETERS;
}

ErrCode TagSession::RegReaderModeWithIntvl(const ElementName& element, const std::vector<uint32_t>& discTech,
    const sptr<IReaderModeCallback>& cb, int interval)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }
    if (cb == nullptr || cb->AsObject() == nullptr) {
        ErrorLog("callback nullptr.");
        return KITS::ERR_NFC_PARAMETERS;
    }

    std::unique_ptr<ReaderModeDeathRecipient> recipient
        = std::make_unique<ReaderModeDeathRecipient>(this, IPCSkeleton::GetCallingTokenID());
    sptr<IRemoteObject::DeathRecipient> dr(recipient.release());
    if (!cb->AsObject()->AddDeathRecipient(dr)) {
        ErrorLog("Failed to add death recipient");
        return KITS::ERR_NFC_PARAMETERS;
    }

    std::lock_guard<std::mutex> guard(mutex_);
    readerModeDeathRecipient_ = dr;
    readerModeCallback_ = cb;
    bool isVendorApp = false;
    if (!g_appStateObserver->IsForegroundApp(element.GetBundleName())) {
#ifdef VENDOR_APPLICATIONS_ENABLED
        if (!IsVendorProcess()) {
            ErrorLog("not foreground app.");
            return KITS::ERR_TAG_APP_NOT_FOREGROUND;
        } else {
            InfoLog("is vendor app");
            isVendorApp = true;
        }
#else
        ErrorLog("not foreground app.");
        return KITS::ERR_TAG_APP_NOT_FOREGROUND;
#endif
    }
    return RegReaderModeInnerWithIntvl(element, discTech, cb, isVendorApp, interval);
}

ErrCode TagSession::UnregReaderMode(const ElementName& element)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("UnregReaderMode, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }
    std::lock_guard<std::mutex> guard(mutex_);
    return UnregReaderModeInner(element, true);
}

void TagSession::SetFieldCheckInterval(int interval)
{
    auto tagDispatcherPtr = tagDispatcher_.lock();
    if (tagDispatcherPtr == nullptr) {
        ErrorLog("tagDispatcher_ is nullptr.");
        return;
    }
    tagDispatcherPtr->SetFieldCheckInterval(interval);
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
