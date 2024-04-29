/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "loghelper.h"
#include "app_data_parser.h"
#include "external_deps_proxy.h"
#include "host_card_emulation_manager.h"
#include "ability_manager_client.h"
#include "nfc_sdk_common.h"
#include "accesstoken_kit.h"
#include "hap_token_info.h"

namespace OHOS {
namespace NFC {
#ifdef VENDOR_APPLICATIONS_ENABLED
static const int CODE_SEND_FIELD_DEACTIVATE = 0;
static const int CODE_SEND_FIELD_ACTIVATE = 1;
static const int CODE_SEND_APDU_DATA = 2;
#endif
const uint32_t SELECT_APDU_HDR_LENGTH = 5;
const uint8_t INSTR_SELECT = 0xA4;
const uint32_t MINIMUM_AID_LENGTH = 5;
const uint8_t SELECT_00 = 0x00;
const uint8_t SELECT_P1 = 0x04;
const uint32_t INDEX_CLASS_BYTE = 0;
const uint32_t INDEX_CHAIN_INSTRUCTION = 1;
const uint32_t INDEX_P1 = 2;
const uint32_t INDEX_3 = 3;
const uint32_t INDEX_AID_LEN = 4;
using OHOS::AppExecFwk::ElementName;
HostCardEmulationManager::HostCardEmulationManager(std::weak_ptr<NfcService> nfcService,
                                                   std::weak_ptr<NCI::INciCeInterface> nciCeProxy,
                                                   std::weak_ptr<CeService> ceService)
    : nfcService_(nfcService), nciCeProxy_(nciCeProxy), ceService_(ceService)
{
    hceState_ = HostCardEmulationManager::INITIAL_STATE;
    queueHceData_.clear();
    abilityConnection_ = new (std::nothrow) NfcAbilityConnectionCallback();
}
HostCardEmulationManager::~HostCardEmulationManager()
{
    hceState_ = HostCardEmulationManager::INITIAL_STATE;
    queueHceData_.clear();
    abilityConnection_ = nullptr;
    bundleNameToHceCmdRegData_.clear();
}

void HostCardEmulationManager::OnHostCardEmulationDataNfcA(const std::vector<uint8_t>& data)
{
    if (data.empty()) {
        InfoLog("onHostCardEmulationDataNfcA: no data");
        return;
    }
    std::string dataStr = KITS::NfcSdkCommon::BytesVecToHexString(&data[0], data.size());
    InfoLog("onHostCardEmulationDataNfcA: Data Length = %{public}zu; Data as "
            "String = %{public}s",
            data.size(), dataStr.c_str());

#ifdef VENDOR_APPLICATIONS_ENABLED
    // send data to vendor
    sptr<IOnCardEmulationNotifyCb> notifyApduDataCallback =
        ExternalDepsProxy::GetInstance().GetNotifyCardEmulationCallback();
    if (notifyApduDataCallback != nullptr) {
        notifyApduDataCallback->OnCardEmulationNotify(CODE_SEND_APDU_DATA, dataStr);
    }
#endif

    std::string aid = ParseSelectAid(data);
    InfoLog("selectAid = %{public}s", aid.c_str());
    InfoLog("onHostCardEmulationDataNfcA: state %{public}d", hceState_);
    ElementName aidElement;
    if (ceService_.expired()) {
        ErrorLog("ce service expired.");
        return;
    }
    ceService_.lock()->SearchElementByAid(aid, aidElement);

    std::lock_guard<std::mutex> lock(hceStateMutex_);
    switch (hceState_) {
        case HostCardEmulationManager::INITIAL_STATE: {
            InfoLog("got data on state INITIAL_STATE");
            return;
        }
        case HostCardEmulationManager::WAIT_FOR_SELECT: {
            HandleDataOnW4Select(aid, aidElement, data);
            break;
        }
        case HostCardEmulationManager::WAIT_FOR_SERVICE: {
            InfoLog("got data on state w4 service");
            return;
        }
        case HostCardEmulationManager::DATA_TRANSFER: {
            HandleDataOnDataTransfer(aid, aidElement, data);
            break;
        }
        case HostCardEmulationManager::WAIT_FOR_DEACTIVATE: {
            InfoLog("got data on state w4 deactivate");
            return;
        }
        default: break;
    }
}

void HostCardEmulationManager::OnCardEmulationActivated()
{
    InfoLog("OnCardEmulationActivated: state %{public}d", hceState_);
    std::lock_guard<std::mutex> lock(hceStateMutex_);
    hceState_ = HostCardEmulationManager::WAIT_FOR_SELECT;
    InfoLog("hce state is %{public}d.", hceState_);

#ifdef VENDOR_APPLICATIONS_ENABLED
    // send data to vendor
    sptr<IOnCardEmulationNotifyCb> notifyApduDataCallback =
        ExternalDepsProxy::GetInstance().GetNotifyCardEmulationCallback();
    if (notifyApduDataCallback != nullptr) {
        std::string data{};
        notifyApduDataCallback->OnCardEmulationNotify(CODE_SEND_FIELD_ACTIVATE, data);
    }
#endif

    queueHceData_.clear();
}

void HostCardEmulationManager::OnCardEmulationDeactivated()
{
    InfoLog("OnCardEmulationDeactivated: state %{public}d", hceState_);
    std::lock_guard<std::mutex> lock(hceStateMutex_);
    hceState_ = HostCardEmulationManager::INITIAL_STATE;
    InfoLog("hce state is %{public}d.", hceState_);

#ifdef VENDOR_APPLICATIONS_ENABLED
    // send data to vendor
    sptr<IOnCardEmulationNotifyCb> notifyApduDataCallback =
        ExternalDepsProxy::GetInstance().GetNotifyCardEmulationCallback();
    if (notifyApduDataCallback != nullptr) {
        std::string data{};
        notifyApduDataCallback->OnCardEmulationNotify(CODE_SEND_FIELD_DEACTIVATE, data);
    }
#endif

    queueHceData_.clear();
    ErrCode releaseCallRet = AAFwk::AbilityManagerClient::GetInstance()->ReleaseCall(
        abilityConnection_, abilityConnection_->GetConnectedElement());
    InfoLog("Release call end. ret = %{public}d", releaseCallRet);
}

void HostCardEmulationManager::HandleDataOnW4Select(const std::string& aid, ElementName& aidElement,
                                                    const std::vector<uint8_t>& data)
{
    bool exitService = ExistService(aidElement);
    if (!aid.empty()) {
        if (exitService) {
            InfoLog("HandleDataOnW4Select: existing service, try to send data "
                    "directly.");
            hceState_ = HostCardEmulationManager::DATA_TRANSFER;
            InfoLog("hce state is %{public}d.", hceState_);
            SendDataToService(data);
            return;
        } else {
            InfoLog("HandleDataOnW4Select: try to connect service.");
            queueHceData_ = std::move(data);
            DispatchAbilitySingleApp(aidElement);
            return;
        }
    } else if (exitService) {
        InfoLog("HandleDataOnW4Select: existing service, try to send data "
                "directly.");
        hceState_ = HostCardEmulationManager::DATA_TRANSFER;
        SendDataToService(data);
        return;
    } else {
        InfoLog("no aid got");
        std::string unknowError = "6F00";
        nciCeProxy_.lock()->SendRawFrame(unknowError);
    }
}

void HostCardEmulationManager::HandleDataOnDataTransfer(const std::string& aid, ElementName& aidElement,
                                                        const std::vector<uint8_t>& data)
{
    bool exitService = ExistService(aidElement);
    if (!aid.empty()) {
        if (exitService) {
            InfoLog("HandleDataOnDataTransfer: existing service, try to send "
                    "data directly.");
            hceState_ = HostCardEmulationManager::DATA_TRANSFER;
            InfoLog("hce state is %{public}d.", hceState_);
            SendDataToService(data);
            return;
        } else {
            InfoLog("HandleDataOnDataTransfer: existing service, try to "
                    "connect service.");
            queueHceData_ = std::move(data);
            DispatchAbilitySingleApp(aidElement);
            return;
        }
    } else if (exitService) {
        InfoLog("HandleDataOnDataTransfer: existing service, try to send data "
                "directly.");
        hceState_ = HostCardEmulationManager::DATA_TRANSFER;
        InfoLog("hce state is %{public}d.", hceState_);
        SendDataToService(data);
        return;
    } else {
        InfoLog("no service, drop apdu data.");
    }
}
bool HostCardEmulationManager::ExistService(ElementName& aidElement)
{
    if (!abilityConnection_->ServiceConnected()) {
        InfoLog("no service connected.");
        return false;
    }
    std::string bundleName = abilityConnection_->GetConnectedElement().GetBundleName();
    std::lock_guard<std::mutex> lock(regInfoMutex_);
    auto it = bundleNameToHceCmdRegData_.find(bundleName);
    if (it == bundleNameToHceCmdRegData_.end()) {
        ErrorLog("no register data for %{public}s", abilityConnection_->GetConnectedElement().GetURI().c_str());
        return false;
    }
    if (it->second.callback_ == nullptr) {
        ErrorLog("callback is null");
        return false;
    }

    if (aidElement.GetBundleName().empty()) {
        InfoLog("aid is empty.");
        // normal data not select data
        return true;
    }
    // only verify the element name for select data
    if (aidElement.GetBundleName() == abilityConnection_->GetConnectedElement().GetBundleName() &&
        aidElement.GetAbilityName() == abilityConnection_->GetConnectedElement().GetAbilityName()) {
        InfoLog("ability is already connected.");
        return true;
    } else {
        WarnLog("not the same element");
        return false;
    }
}

std::string HostCardEmulationManager::ParseSelectAid(const std::vector<uint8_t>& data)
{
    if (data.empty() || data.size() < SELECT_APDU_HDR_LENGTH + MINIMUM_AID_LENGTH) {
        InfoLog("invalid data. Data size less than hdr length plus minumum length.");
        return "";
    }

    if (data[INDEX_CLASS_BYTE] == SELECT_00 && data[INDEX_CHAIN_INSTRUCTION] == INSTR_SELECT &&
        data[INDEX_P1] == SELECT_P1) {
        if (data[INDEX_3] != SELECT_00) {
            InfoLog("not supported aid");
            return "";
        }

        int aidLength = data[INDEX_AID_LEN];
        if (data.size() < SELECT_APDU_HDR_LENGTH + aidLength) {
            InfoLog("invalid data. Data size less than hdr length plus aid declared length.");
            return "";
        }

        std::vector<uint8_t> aidVec(data.begin() + SELECT_APDU_HDR_LENGTH,
                                    data.begin() + SELECT_APDU_HDR_LENGTH + aidLength);
        return KITS::NfcSdkCommon::BytesVecToHexString(&aidVec[0], aidVec.size());
    }

    return "";
}

bool HostCardEmulationManager::RegHceCmdCallback(const sptr<KITS::IHceCmdCallback>& callback,
                                                 const std::string& type,
                                                 Security::AccessToken::AccessTokenID callerToken)
{
    if (nfcService_.expired()) {
        ErrorLog("RegHceCmdCallback: nfcService_ is nullptr.");
        return false;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("RegHceCmdCallback: NFC not enabled, do not set ");
        return false;
    }
    InfoLog("RegHceCmdCallback start, register size =%{public}zu.", bundleNameToHceCmdRegData_.size());
    Security::AccessToken::HapTokenInfo hapTokenInfo;
    int result = Security::AccessToken::AccessTokenKit::GetHapTokenInfo(callerToken, hapTokenInfo);

    InfoLog("get hap token info, result = %{public}d", result);
    if (result) {
        return false;
    }
    if (hapTokenInfo.bundleName.empty()) {
        ErrorLog("RegHceCmdCallback: not got bundle name");
        return false;
    }
    HostCardEmulationManager::HceCmdRegistryData regData;

    regData.callback_ = callback;
    regData.callerToken_ = callerToken;
    std::lock_guard<std::mutex> lock(regInfoMutex_);
    if (bundleNameToHceCmdRegData_.find(hapTokenInfo.bundleName) != bundleNameToHceCmdRegData_.end()) {
        InfoLog("override the register data for  %{public}s", hapTokenInfo.bundleName.c_str());
    }
    bundleNameToHceCmdRegData_[hapTokenInfo.bundleName] = regData;

    InfoLog("RegHceCmdCallback end, register size =%{public}zu.", bundleNameToHceCmdRegData_.size());
    return true;
}

bool HostCardEmulationManager::SendHostApduData(std::string hexCmdData, bool raw, std::string& hexRespData,
                                                Security::AccessToken::AccessTokenID callerToken)
{
    if (nfcService_.expired()) {
        ErrorLog("SendHostApduData: nfcService_ is nullptr.");
        return false;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("SendHostApduData: NFC not enabled, do not send.");
        return false;
    }
    if (!IsCorrespondentService(callerToken)) {
        ErrorLog("SendHostApduData: not the connected app, do not send.");
        return false;
    }

    return nciCeProxy_.lock()->SendRawFrame(hexCmdData);
}
bool HostCardEmulationManager::IsCorrespondentService(Security::AccessToken::AccessTokenID callerToken)
{
    Security::AccessToken::HapTokenInfo hapTokenInfo;
    int result = Security::AccessToken::AccessTokenKit::GetHapTokenInfo(callerToken, hapTokenInfo);

    InfoLog("get hap token info, result = %{public}d", result);
#ifdef VENDOR_APPLICATIONS_ENABLED
    if (result) {
        WarnLog("vendor application, allow to send raw frame.");
        return true;
    }
#endif
    if (!hapTokenInfo.bundleName.empty() &&
        hapTokenInfo.bundleName == abilityConnection_->GetConnectedElement().GetBundleName()) {
        return true;
    }
    ErrorLog("SendHostApduData: diff app, the call app %{public}s , the connected app %{public}s",
             hapTokenInfo.bundleName.c_str(), abilityConnection_->GetConnectedElement().GetBundleName().c_str());
    return false;
}

void HostCardEmulationManager::HandleQueueData()
{
    bool shouldSendQueueData = hceState_ == HostCardEmulationManager::WAIT_FOR_SERVICE && !queueHceData_.empty();

    std::string queueData = KITS::NfcSdkCommon::BytesVecToHexString(&queueHceData_[0], queueHceData_.size());
    InfoLog("RegHceCmdCallback queue data %{public}s, hceState= %{public}d, "
            "service connected= %{public}d",
            queueData.c_str(), hceState_, abilityConnection_->ServiceConnected());
    if (shouldSendQueueData) {
        InfoLog("RegHceCmdCallback should send queue data");
        hceState_ = HostCardEmulationManager::DATA_TRANSFER;
        InfoLog("hce state is %{public}d.", hceState_);
        SendDataToService(queueHceData_);
        queueHceData_.clear();
        return;
    }
    WarnLog("HandleQueueData can not send the data.");
}

void HostCardEmulationManager::SendDataToService(const std::vector<uint8_t>& data)
{
    std::string bundleName = abilityConnection_->GetConnectedElement().GetBundleName();
    InfoLog("SendDataToService register size =%{public}zu.", bundleNameToHceCmdRegData_.size());
    std::lock_guard<std::mutex> lock(regInfoMutex_);
    auto it = bundleNameToHceCmdRegData_.find(bundleName);
    if (it == bundleNameToHceCmdRegData_.end()) {
        ErrorLog("no register data for %{public}s", abilityConnection_->GetConnectedElement().GetURI().c_str());
        return;
    }
    if (it->second.callback_ == nullptr) {
        ErrorLog("callback is null");
        return;
    }
    it->second.callback_->OnCeApduData(data);
}

bool HostCardEmulationManager::DispatchAbilitySingleApp(ElementName& element)
{
    abilityConnection_->SetHceManager(shared_from_this());
    if (element.GetBundleName().empty()) {
        ErrorLog("DispatchAbilitySingleApp element empty");
        std::string aidNotFound = "6A82";
        nciCeProxy_.lock()->SendRawFrame(aidNotFound);
        return false;
    }

    InfoLog("DispatchAbilitySingleApp for element  %{public}s", element.GetURI().c_str());
    AAFwk::Want want;
    want.SetElement(element);

    if (AAFwk::AbilityManagerClient::GetInstance() == nullptr) {
        ErrorLog("DispatchAbilitySingleApp AbilityManagerClient is null");
        return false;
    }
    ErrCode err = AAFwk::AbilityManagerClient::GetInstance()->StartAbilityByCall(want, abilityConnection_);
    InfoLog("DispatchAbilitySingleApp call StartAbility end. ret = %{public}d", err);
    if (err == ERR_NONE) {
        hceState_ = HostCardEmulationManager::WAIT_FOR_SERVICE;
        InfoLog("hce state is %{public}d.", hceState_);
        ExternalDepsProxy::GetInstance().WriteHceSwipeResultHiSysEvent(element.GetBundleName(), DEFAULT_COUNT);
        
        NfcFailedParams params;
        ExternalDepsProxy::GetInstance().BuildFailedParams(params, MainErrorCode::HCE_SWIPE_CARD,
                                                           SubErrorCode::DEFAULT_ERR_DEF);
        params.appPackageName = element.GetBundleName();
        ExternalDepsProxy::GetInstance().WriteNfcFailedHiSysEvent(&params);
        return true;
    }
    return false;
}
bool HostCardEmulationManager::UnRegHceCmdCallback(const std::string& type,
                                                   Security::AccessToken::AccessTokenID callerToken)
{
    return EraseHceCmdCallback(callerToken);
}
bool HostCardEmulationManager::EraseHceCmdCallback(Security::AccessToken::AccessTokenID callerToken)
{
    InfoLog("EraseHceCmdCallback start, register size =%{public}zu.", bundleNameToHceCmdRegData_.size());
    Security::AccessToken::HapTokenInfo hapTokenInfo;
    int result = Security::AccessToken::AccessTokenKit::GetHapTokenInfo(callerToken, hapTokenInfo);

    InfoLog("get hap token info, result = %{public}d", result);
    if (result) {
        return false;
    }
    if (hapTokenInfo.bundleName.empty()) {
        ErrorLog("EraseHceCmdCallback: not got bundle name");
        return false;
    }
    std::lock_guard<std::mutex> lock(regInfoMutex_);

    if (bundleNameToHceCmdRegData_.find(hapTokenInfo.bundleName) != bundleNameToHceCmdRegData_.end()) {
        InfoLog("unregister data for  %{public}s", hapTokenInfo.bundleName.c_str());
    }
    bundleNameToHceCmdRegData_.erase(hapTokenInfo.bundleName);
    InfoLog("EraseHceCmdCallback end, register size =%{public}zu.", bundleNameToHceCmdRegData_.size());
    return true;
}

bool HostCardEmulationManager::UnRegAllCallback(Security::AccessToken::AccessTokenID callerToken)
{
    return EraseHceCmdCallback(callerToken);
}
} // namespace NFC
} // namespace OHOS