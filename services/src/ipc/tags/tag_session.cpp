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
#include "loghelper.h"
#include "app_state_observer.h"

namespace OHOS {
namespace NFC {
namespace TAG {
using OHOS::AppExecFwk::ElementName;
const std::string DUMP_LINE = "---------------------------";
const std::string DUMP_END = "\n";

// NFC_A = 1 ~ NDEF_FORMATABLE = 10
const int MAX_TECH = 12;
int g_techTimeout[MAX_TECH] = {0};
int g_maxTransLength[MAX_TECH] = {0, 253, 253, 261, 255, 253, 0, 0, 253, 253, 0, 0};
std::shared_ptr<AppStateObserver> g_appStateObserver = nullptr;

TagSession::TagSession(std::shared_ptr<NfcService> service)
    : nfcService_(service)
{
    if (service) {
        nciTagProxy_ = service->GetNciTagProxy();
        nfcPollingManager_ = service->GetNfcPollingManager();
    }
    g_appStateObserver = std::make_shared<AppStateObserver>(this);
}

TagSession::~TagSession()
{
}

/**
 * @brief To connect the tagRfDiscId by technology.
 * @param tagRfDiscId the rf disc id of tag
 * @param technology the tag technology
 * @return the result to connect the tag
 */
int TagSession::Connect(int tagRfDiscId, int technology)
{
    if (technology < 0 || technology >= MAX_TECH) {
        ErrorLog("Connect, invalid technology %{public}d", technology);
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    if (nfcService_.expired() || nciTagProxy_.expired()) {
        ErrorLog("Connect, expired");
        return NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("Connect, IsNfcEnabled error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }
    if (!nciTagProxy_.lock()->IsTagFieldOn(tagRfDiscId)) {
        ErrorLog("Connect, IsTagFieldOn error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_LOST;
    }

    if (nciTagProxy_.lock()->Connect(tagRfDiscId, technology)) {
        return NFC::KITS::ErrorCode::ERR_NONE;
    } else {
        ErrorLog("Connect, unallowd call error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_IO_FAILED;
    }
}
/**
 * @brief To reconnect the tagRfDiscId.
 * @param tagRfDiscId the rf disc id of tag
 * @return the result to reconnect the tag
 */
int TagSession::Reconnect(int tagRfDiscId)
{
    // Check if NFC is enabled
    if (nfcService_.expired() || nciTagProxy_.expired()) {
        ErrorLog("Reconnect, expired");
        return NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("Reconnect, IsNfcEnabled error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }

    if (nciTagProxy_.lock()->Reconnect(tagRfDiscId)) {
        return NFC::KITS::ErrorCode::ERR_NONE;
    } else {
        ErrorLog("Reconnect, unallowd call error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_IO_FAILED;
    }
}
/**
 * @brief To disconnect the tagRfDiscId.
 * @param tagRfDiscId the rf disc id of tag
 */
void TagSession::Disconnect(int tagRfDiscId)
{
    // Check if NFC is enabled
    if (nfcService_.expired() || nciTagProxy_.expired() || !nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("Disconnect, IsTagFieldOn error");
        return;
    }

    nciTagProxy_.lock()->Disconnect(tagRfDiscId);
}

int TagSession::SetTimeout(int tagRfDiscId, int timeout, int technology)
{
    if (technology < 0 || technology >= MAX_TECH) {
        ErrorLog("SetTimeout, invalid technology %{public}d", technology);
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    // Check if NFC is enabled
    if (nfcService_.expired() || nciTagProxy_.expired()) {
        ErrorLog("SetTimeout, expired");
        return NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("SetTimeout, IsNfcEnabled error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }

    nciTagProxy_.lock()->SetTimeout(tagRfDiscId, timeout, technology);
    return NFC::KITS::ErrorCode::ERR_NONE;
}

int TagSession::GetTimeout(int tagRfDiscId, int technology, int &timeout)
{
    if (technology < 0 || technology >= MAX_TECH) {
        ErrorLog("GetTimeout, invalid technology %{public}d", technology);
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    // Check if NFC is enabled
    if (nfcService_.expired() || nciTagProxy_.expired()) {
        ErrorLog("GetTimeout, expired");
        return NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("GetTimeout, IsNfcEnabled error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }

    uint32_t timeoutTemp = 0;
    nciTagProxy_.lock()->GetTimeout(tagRfDiscId, timeoutTemp, technology);
    timeout = timeoutTemp;
    return NFC::KITS::ErrorCode::ERR_NONE;
}

void TagSession::ResetTimeout(int tagRfDiscId)
{
    if (nfcService_.expired() || nciTagProxy_.expired()) {
        ErrorLog("ResetTimeout, expired");
        return;
    }
    nciTagProxy_.lock()->ResetTimeout(tagRfDiscId);
    return;
}

/**
 * @brief Get the TechList of the tagRfDiscId.
 * @param tagRfDiscId the rf disc id of tag
 * @return TechList
 */
std::vector<int> TagSession::GetTechList(int tagRfDiscId)
{
    std::vector<int> techList;
    // Check if NFC is enabled
    if (nfcService_.expired() || nciTagProxy_.expired() || !nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("GetTechList, IsTagFieldOn error");
        return techList;
    }

    return nciTagProxy_.lock()->GetTechList(tagRfDiscId);
}
/**
 * @brief Checking the tagRfDiscId is present.
 * @param tagRfDiscId the rf disc id of tag
 * @return true - Presnet; the other - No Presnet
 */
bool TagSession::IsTagFieldOn(int tagRfDiscId)
{
    // Check if NFC is enabled
    if (nfcService_.expired() || nciTagProxy_.expired() || !nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("IsTagFieldOn, IsTagFieldOn error");
        return false;
    }

    return nciTagProxy_.lock()->IsTagFieldOn(tagRfDiscId);
}
/**
 * @brief Checking the tagRfDiscId is a Ndef Tag.
 * @param tagRfDiscId the rf disc id of tag
 * @return true - Ndef Tag; the other - No Ndef Tag
 */
bool TagSession::IsNdef(int tagRfDiscId)
{
    // Check if NFC is enabled
    if (nfcService_.expired() || nciTagProxy_.expired() || !nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("IsNdef, IsTagFieldOn error");
        return false;
    }

    std::vector<int> ndefInfo;
    return nciTagProxy_.lock()->DetectNdefInfo(tagRfDiscId, ndefInfo);
}

int TagSession::SendRawFrame(const int tagRfDiscId, std::string hexCmdData, bool raw, std::string &hexRespData)
{
    DebugLog("Send Raw(%{public}d) Frame", raw);
    // Check if NFC is enabled
    if (nfcService_.expired() || nciTagProxy_.expired()) {
        ErrorLog("SendRawFrame, expired");
        return NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("SendRawFrame, IsNfcEnabled error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }

    // Check if length is within limits
    int maxSize = 0;
    GetMaxTransceiveLength(nciTagProxy_.lock()->GetConnectedTech(tagRfDiscId), maxSize);
    if (KITS::NfcSdkCommon::GetHexStrBytesLen(hexCmdData) > static_cast<uint32_t>(maxSize)) {
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }

    int result = nciTagProxy_.lock()->Transceive(tagRfDiscId, hexCmdData, hexRespData);
    DebugLog("TagSession::SendRawFrame, result = 0x%{public}X", result);
    if ((result == 0) && (!hexRespData.empty())) {
        return NFC::KITS::ErrorCode::ERR_NONE;
    } else if (result == 1) {  // result == 1 means that Tag lost
        ErrorLog("TagSession::SendRawFrame: tag lost.");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_LOST;
    }
    ErrorLog("TagSession::SendRawFrame: result failed.");
    return NFC::KITS::ErrorCode::ERR_TAG_STATE_IO_FAILED;
}
/**
 * @brief Reading from the host tag
 * @param tagRfDiscId the rf disc id of tag
 * @return the read data
 */
std::string TagSession::NdefRead(int tagRfDiscId)
{
    // Check if NFC is enabled
    if (nfcService_.expired() || nciTagProxy_.expired() || !nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("NdefRead, IsTagFieldOn error");
        return "";
    }

    return nciTagProxy_.lock()->ReadNdef(tagRfDiscId);
}
/**
 * @brief Writing the data into the host tag.
 * @param tagRfDiscId the rf disc id of tag
 * @param msg the wrote data
 * @return the Writing Result
 */
int TagSession::NdefWrite(int tagRfDiscId, std::string msg)
{
    // Check if NFC is enabled
    if (nfcService_.expired() || nciTagProxy_.expired()) {
        ErrorLog("NdefWrite, expired");
        return NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("NdefWrite, IsNfcEnabled error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }

    if (msg.empty()) {
        ErrorLog("NdefWrite, msg.empty error");
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }

    if (nciTagProxy_.lock()->WriteNdef(tagRfDiscId, msg)) {
        return NFC::KITS::ErrorCode::ERR_NONE;
    }
    return NFC::KITS::ErrorCode::ERR_TAG_STATE_IO_FAILED;
}
/**
 * @brief Making the host tag to read only.
 * @param tagRfDiscId the rf disc id of tag
 * @return the making result
 */
int TagSession::NdefMakeReadOnly(int tagRfDiscId)
{
    // Check if NFC is enabled
    if (nfcService_.expired() || nciTagProxy_.expired()) {
        ErrorLog("NdefMakeReadOnly, expired");
        return NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("NdefMakeReadOnly, IsNfcEnabled error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }

    if (nciTagProxy_.lock()->SetNdefReadOnly(tagRfDiscId)) {
        return NFC::KITS::ErrorCode::ERR_NONE;
    }
    return NFC::KITS::ErrorCode::ERR_TAG_STATE_IO_FAILED;
}
/**
 * @brief format the tag by Ndef
 * @param tagRfDiscId the rf disc id of tag
 * @param key the format key
 * @return the format result
 */
int TagSession::FormatNdef(int tagRfDiscId, const std::string& key)
{
    // Check if NFC is enabled
    if (nfcService_.expired() || nciTagProxy_.expired()) {
        ErrorLog("FormatNdef, expired");
        return NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("FormatNdef, IsNfcEnabled error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }

    if (nciTagProxy_.lock()->FormatNdef(tagRfDiscId, key)) {
        return NFC::KITS::ErrorCode::ERR_NONE;
    }
    return NFC::KITS::ErrorCode::ERR_TAG_STATE_IO_FAILED;
}

int TagSession::CanMakeReadOnly(int ndefType, bool &canSetReadOnly)
{
    if (nfcService_.expired() || nciTagProxy_.expired()) {
        ErrorLog("CanMakeReadOnly, expired");
        return NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    canSetReadOnly = nciTagProxy_.lock()->CanMakeReadOnly(ndefType);
    return NFC::KITS::ErrorCode::ERR_NONE;
}
/**
 * @brief Get Max Transceive Length
 * @param technology the tag technology
 * @return Max Transceive Length
 */
int TagSession::GetMaxTransceiveLength(int technology, int &maxSize)
{
    if (technology < 0 || technology >= MAX_TECH) {
        ErrorLog("GetMaxTransceiveLength, technology not support");
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    maxSize = g_maxTransLength[technology];
    return NFC::KITS::ErrorCode::ERR_NONE;
}

int TagSession::IsSupportedApdusExtended(bool &isSupported)
{
    if (nfcService_.expired() || nciTagProxy_.expired()) {
        ErrorLog("IsSupportedApdusExtended, expired");
        return NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    isSupported = nciTagProxy_.lock()->IsExtendedLengthApduSupported();
    return NFC::KITS::ErrorCode::ERR_NONE;
}

uint16_t TagSession::GetFgDataVecSize()
{
    std::unique_lock<std::shared_mutex> guard(fgMutex_);
    return fgDataVec_.size();
}

uint16_t TagSession::GetReaderDataVecSize()
{
    std::unique_lock<std::shared_mutex> guard(fgMutex_);
    return readerDataVec_.size();
}

void TagSession::CheckFgAppStateChanged(const std::string &bundleName, const std::string &abilityName,
    int abilityState)
{
    std::unique_lock<std::shared_mutex> guard(fgMutex_);
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
    std::unique_lock<std::shared_mutex> guard(fgMutex_);
    for (auto readerData = readerDataVec_.begin(); readerData != readerDataVec_.end(); readerData++) {
        ElementName element = readerData->element_;
        if (element.GetBundleName() == bundleName && element.GetAbilityName() == abilityName) {
            // app changes to foreground, RegReaderModeInner.
            if (abilityState == static_cast<int32_t>(AppExecFwk::AbilityState::ABILITY_STATE_FOREGROUND) &&
                !readerData->isEnabled_) {
                InfoLog("app changes to foreground, RegReaderModeInner");
                RegReaderModeInner(element, readerData->techs_, readerData->cb_);
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

KITS::ErrorCode TagSession::RegForegroundDispatch(ElementName &element, std::vector<uint32_t> &discTech,
    const sptr<KITS::IForegroundCallback> &callback)
{
    std::unique_lock<std::shared_mutex> guard(fgMutex_);
    return RegForegroundDispatchInner(element, discTech, callback);
}

KITS::ErrorCode TagSession::RegForegroundDispatchInner(ElementName &element, const std::vector<uint32_t> &discTech,
    const sptr<KITS::IForegroundCallback> &callback)
{
    if (IsFgRegistered(element, discTech, callback)) {
        WarnLog("%{public}s already RegForegroundDispatch", element.GetBundleName().c_str());
        return KITS::ERR_NONE;
    }
    InfoLog("RegForegroundDispatch: bundleName = %{public}s, abilityName = %{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    if (nfcPollingManager_.expired()) {
        ErrorLog("RegForegroundDispatch, expired");
        return NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    if (nfcPollingManager_.lock()->EnableForegroundDispatch(element, discTech, callback)) {
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

KITS::ErrorCode TagSession::UnregForegroundDispatch(ElementName &element)
{
    std::unique_lock<std::shared_mutex> guard(fgMutex_);
    return UnregForegroundDispatchInner(element, true);
}

KITS::ErrorCode TagSession::UnregForegroundDispatchInner(const ElementName &element, bool isAppUnregister)
{
    if (IsFgUnregistered(element, isAppUnregister)) {
        WarnLog("%{public}s already UnregForegroundDispatch", element.GetBundleName().c_str());
        return KITS::ERR_NONE;
    }
    InfoLog("UnregForegroundDispatchInner: bundleName = %{public}s, abilityName = %{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    if (nfcPollingManager_.expired()) {
        ErrorLog("UnregForegroundDispatch, expired");
        return NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND;
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
    const sptr<KITS::IReaderModeCallback> &callback)
{
    for (ReaderData &readerData : readerDataVec_) {
        ElementName readerElement = readerData.element_;
        if (IsSameAppAbility(element, readerElement)) {
            if (readerData.isEnabled_) {
                return true;
            }
            InfoLog("Enable ReaderData: bundleName = %{public}s, abilityName = %{public}s",
                readerElement.GetBundleName().c_str(), readerElement.GetAbilityName().c_str());
            readerData.isEnabled_ = true;
            return false;
        }
    }
    ReaderData readerData(true, element, discTech, callback);
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

KITS::ErrorCode TagSession::RegReaderModeInner(ElementName &element, std::vector<uint32_t> &discTech,
    const sptr<KITS::IReaderModeCallback> &callback)
{
    if (IsReaderRegistered(element, discTech, callback)) {
        WarnLog("%{public}s already RegReaderMode", element.GetBundleName().c_str());
        return KITS::ERR_NONE;
    }
    InfoLog("RegReaderModeInner: bundleName = %{public}s, abilityName = %{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    if (nfcPollingManager_.expired()) {
        ErrorLog("RegReaderModeInner, expired");
        return NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    if (nfcPollingManager_.lock()->EnableReaderMode(element, discTech, callback)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_NFC_PARAMETERS;
}

KITS::ErrorCode TagSession::UnregReaderModeInner(ElementName &element, bool isAppUnregister)
{
    if (IsReaderUnregistered(element, isAppUnregister)) {
        WarnLog("%{public}s already UnregReaderMode", element.GetBundleName().c_str());
        return KITS::ERR_NONE;
    }
    InfoLog("UnregReaderModeInner: bundleName = %{public}s, abilityName = %{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    if (nfcPollingManager_.expired()) {
        ErrorLog("UnregReaderMode, expired");
        return NFC::KITS::ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    if (nfcPollingManager_.lock()->DisableReaderMode(element)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_NFC_PARAMETERS;
}

KITS::ErrorCode TagSession::RegReaderMode(ElementName &element, std::vector<uint32_t> &discTech,
    const sptr<KITS::IReaderModeCallback> &callback)
{
    if (!g_appStateObserver->IsForegroundApp(element.GetBundleName())) {
        return KITS::ERR_TAG_APP_NOT_FOREGROUND;
    }
    std::unique_lock<std::shared_mutex> guard(fgMutex_);
    return RegReaderModeInner(element, discTech, callback);
}

KITS::ErrorCode TagSession::UnregReaderMode(ElementName &element)
{
    std::unique_lock<std::shared_mutex> guard(fgMutex_);
    return UnregReaderModeInner(element, true);
}

int32_t TagSession::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    std::string info = GetDumpInfo();
    int ret = dprintf(fd, "%s\n", info.c_str());
    if (ret < 0) {
        ErrorLog("TagSession Dump ret = %{public}d", ret);
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    return NFC::KITS::ErrorCode::ERR_NONE;
}

std::string TagSession::GetDumpInfo()
{
    std::string info;
    if (nfcService_.expired()) {
        return info;
    }

    return info.append(DUMP_LINE)
        .append(" TAG DUMP ")
        .append(DUMP_LINE)
        .append(DUMP_END)
        .append("NFC_STATE          : ")
        .append(std::to_string(nfcService_.lock()->GetNfcState()))
        .append(DUMP_END)
        .append("SCREEN_STATE       : ")
        .append(std::to_string(nfcService_.lock()->GetScreenState()))
        .append(DUMP_END)
        .append("NCI_VERSION        : ")
        .append(std::to_string(nfcService_.lock()->GetNciVersion()))
        .append(DUMP_END);
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
