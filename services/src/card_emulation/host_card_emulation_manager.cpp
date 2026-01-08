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
#include "ability_manager_service.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "nfc_ability_connection_callback.h"
#include "ability_info.h"

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
//ISO 7816: P2 is 0x0c when no response data if the Le field absent, or proprietary if Le field present
const uint8_t SELECT_P2_0C = 0x0c;
const uint32_t INDEX_CLASS_BYTE = 0;
const uint32_t INDEX_CHAIN_INSTRUCTION = 1;
const uint32_t INDEX_P1 = 2;
const uint32_t INDEX_3 = 3;
const uint32_t INDEX_AID_LEN = 4;
const int32_t USERID = 100;
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
    WarnLog("~HostCardEmulationManager");
    // both hceState_ and bundleNameToHceCmdRegData_ must be protected in destructing
    std::lock_guard<std::mutex> lock(hceStateMutex_);
    std::lock_guard<std::mutex> lockRegInfo(regInfoMutex_);
    hceState_ = HostCardEmulationManager::INITIAL_STATE;
    queueHceData_.clear();
    abilityConnection_ = nullptr;
    bundleNameToHceCmdRegData_.clear();
}

sptr<AppExecFwk::IBundleMgr> HostCardEmulationManager::NfcGetBundleMgrProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        ErrorLog("NfcGetBundleMgrProxy, systemAbilityManager is null");
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        ErrorLog("NfcGetBundleMgrProxy, remoteObject is null");
        return nullptr;
    }
    return iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
}

bool HostCardEmulationManager::IsFaModeApplication(ElementName& elementName)
{
    sptr<AppExecFwk::IBundleMgr> bundleMgrProxy = NfcGetBundleMgrProxy();
    if (bundleMgrProxy == nullptr) {
        ErrorLog("IsFaModeApplication, bundleMgrProxy is nullptr.");
        return false;
    }

    constexpr auto flag = AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_DEFAULT;
    AppExecFwk::AbilityInfo hceAbilityInfo;
    AAFwk::Want want;
    want.SetElement(elementName);

    if (!bundleMgrProxy->QueryAbilityInfo(want, flag, USERID, hceAbilityInfo)) {
        ErrorLog("IsFaModeApplication QueryAbilityInfo fail!");
        return false;
    }
    InfoLog("IsFaModeApplication QueryAbilityInfo bundleName=[%{public}s], isStageBasedModel=[%{public}d]",
        hceAbilityInfo.bundleName.c_str(), hceAbilityInfo.isStageBasedModel);
    if (hceAbilityInfo.isStageBasedModel) {
        return false;
    }
    return true;
}

/* Handle received APDU data for FA Model Application */
void HostCardEmulationManager::HandleDataForFaApplication(const std::string& aid,
    ElementName& aidElement, const std::vector<uint8_t>& data)
{
    InfoLog("HandleDataForFaApplication hce state is %{public}d.", hceState_);
    switch (hceState_) {
        case HostCardEmulationManager::INITIAL_STATE: {
            InfoLog("got data on state fa INITIAL_STATE");
            return;
        }
        case HostCardEmulationManager::WAIT_FOR_SELECT: {
            InfoLog("got data on state fa WAIT_FOR_SELECT");
            HandleDataOnW4SelectForFa(aid, aidElement, data);
            break;
        }
        case HostCardEmulationManager::WAIT_FOR_SERVICE: {
            InfoLog("got data on state fa w4 service");
            return;
        }
        case HostCardEmulationManager::DATA_TRANSFER: {
            InfoLog("got data on state fa DATA_TRANSFER");
            HandleDataOnDataTransferForFa(aid, aidElement, data);
            break;
        }
        case HostCardEmulationManager::WAIT_FOR_DEACTIVATE: {
            InfoLog("got data on state fa w4 deactivate");
            return;
        }
        default: break;
    }
}
/* Handle received APDU data for Stage Model Application */
void HostCardEmulationManager::HandleDataForStageApplication(const std::string& aid,
    ElementName& aidElement, const std::vector<uint8_t>& data)
{
    InfoLog("HandleDataForStageApplication hce state is %{public}d.", hceState_);
    switch (hceState_) {
        case HostCardEmulationManager::INITIAL_STATE: {
            InfoLog("got data on state stage INITIAL_STATE");
            return;
        }
        case HostCardEmulationManager::WAIT_FOR_SELECT: {
            InfoLog("got data on state stage WAIT_FOR_SELECT");
            HandleDataOnW4Select(aid, aidElement, data);
            break;
        }
        case HostCardEmulationManager::WAIT_FOR_SERVICE: {
            InfoLog("got data on state stage w4 service");
            return;
        }
        case HostCardEmulationManager::DATA_TRANSFER: {
            InfoLog("got data on state stage DATA_TRANSFER");
            HandleDataOnDataTransfer(aid, aidElement, data);
            break;
        }
        case HostCardEmulationManager::WAIT_FOR_DEACTIVATE: {
            InfoLog("got data on state stage w4 deactivate");
            return;
        }
        default: break;
    }
}

#ifdef VENDOR_APPLICATIONS_ENABLED
bool HostCardEmulationManager::IsForegroundApp(const std::string &appBundleName)
{
    std::vector<AppExecFwk::AbilityStateData> list{};
    int ret = AAFwk::AbilityManagerClient::GetInstance()->GetForegroundUIAbilities(list);
    if (ret != ERR_NONE) {
        ErrorLog("GetForegroundUIAbilities failed: %{public}d", ret);
        return false;
    }
    for (const auto &abilityStateData : list) {
        std::string bundleName = abilityStateData.bundleName;
        std::string abilityName = abilityStateData.abilityName;
        if (abilityStateData.abilityState == static_cast<int32_t>(AAFwk::AbilityState::FOREGROUND)) {
            InfoLog("fg element: %{public}s/%{public}s", bundleName.c_str(), abilityName.c_str());
            if (appBundleName == bundleName) {
                return true;
            }
        }
    }
    return false;
}

bool HostCardEmulationManager::ShouldVendorHandleHce(const std::string &aid, const ElementName &aidElement)
{
    if (aid.empty()) {
        return false;
    }
    std::vector<AppDataParser::HceAppAidInfo> vendorHceApps;
    ExternalDepsProxy::GetInstance().GetVendorHceAppsByAid(aid, vendorHceApps);
    if (!aidElement.GetBundleName().empty()) {
        // no vendor hce
        if (vendorHceApps.empty()) {
            InfoLog("only local hce app bundle name: %{public}s", aidElement.GetBundleName().c_str());
            return false;
        }
    } else {
        InfoLog("no local hce app");
        return true;
    }

    // if local and vendor hce have same aid, resolve conflicts
    // local hce is foreground, resolved
    if (IsForegroundApp(aidElement.GetBundleName())) {
        InfoLog("local foreground hce app bundle name: %{public}s", aidElement.GetBundleName().c_str());
        return false;
    }

    auto ceService = ceService_.lock();
    if (!ceService) {
        ErrorLog("ce service is null.");
        return true;
    }

    // local hce is default payment, resolved
    if (ceService->IsDefaultService(aidElement, KITS::TYPE_PAYMENT)) {
        InfoLog("local default hce bundle name: %{public}s", aidElement.GetBundleName().c_str());
        return false;
    }

    // vendor hce is default payment, resolved
    for (const auto &hceApp : vendorHceApps) {
        if (ceService->IsDefaultService(hceApp.element, KITS::TYPE_PAYMENT)) {
            InfoLog("vendor default hce bundle name: %{public}s", hceApp.element.GetBundleName().c_str());
            return true;
        }
    }
    return false;
}

bool HostCardEmulationManager::IsVendorHandleHce(const std::string &aid)
{
    std::lock_guard<std::mutex> lock(hceStateMutex_);
    if (!aid.empty()) {
        shouldVendorHandleHce_ = ShouldVendorHandleHce(aid, aidElement_);
        InfoLog("vendor handle hce: %{public}d", shouldVendorHandleHce_);
    }
    return shouldVendorHandleHce_;
}

bool HostCardEmulationManager::IsVendorCeActivated()
{
    std::lock_guard<std::mutex> lock(hceStateMutex_);
    return isVendorCeActivated_;
}

void HostCardEmulationManager::SetVendorCeActivated(bool isActivated)
{
    std::lock_guard<std::mutex> lock(hceStateMutex_);
    isVendorCeActivated_ = isActivated;
}
#endif

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
    std::string aid = ParseSelectAid(data);
    InfoLog("onHostCardEmulationDataNfcA: selectAid = %{public}s, state %{public}d", aid.c_str(), hceState_);
    ElementName aidElement;
    auto ceServicePtr = ceService_.lock();
    if (ceServicePtr == nullptr) {
        ErrorLog("ce service expired.");
        return;
    }
    ceServicePtr->SearchElementByAid(aid, aidElement);
    /* check aid */
    if (!aid.empty() && !aidElement.GetBundleName().empty()) {
        std::lock_guard<std::mutex> lock(hceStateMutex_);
        aidElement_ = aidElement;
    }

#ifdef VENDOR_APPLICATIONS_ENABLED
    sptr<IOnCardEmulationNotifyCb> notifyApduDataCallback =
        ExternalDepsProxy::GetInstance().GetNotifyCardEmulationCallback();
    if ((notifyApduDataCallback != nullptr) && IsVendorHandleHce(aid)) {
        if (!IsVendorCeActivated()) {
            InfoLog("send vendor ce activated");
            std::string data{};
            notifyApduDataCallback->OnCardEmulationNotify(CODE_SEND_FIELD_ACTIVATE, data);
            SetVendorCeActivated(true);
        }
        if (notifyApduDataCallback->OnCardEmulationNotify(CODE_SEND_APDU_DATA, dataStr)) {
            InfoLog("send ce data to vendor");
            return;
        }
    }
#endif

    std::lock_guard<std::mutex> lock(hceStateMutex_);

    if (IsFaModeApplication(aidElement_)) {
        HandleDataForFaApplication(aid, aidElement_, data);
    } else {
        HandleDataForStageApplication(aid, aidElement_, data);
    }
}

void HostCardEmulationManager::OnCardEmulationActivated()
{
    InfoLog("OnCardEmulationActivated: state %{public}d", hceState_);
    std::lock_guard<std::mutex> lock(hceStateMutex_);
    hceState_ = HostCardEmulationManager::WAIT_FOR_SELECT;
    InfoLog("hce state is %{public}d.", hceState_);

#ifdef VENDOR_APPLICATIONS_ENABLED
    shouldVendorHandleHce_ = false;
    isVendorCeActivated_ = false;
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
    if ((notifyApduDataCallback != nullptr) && isVendorCeActivated_) {
        std::string data{};
        notifyApduDataCallback->OnCardEmulationNotify(CODE_SEND_FIELD_DEACTIVATE, data);
    }
    shouldVendorHandleHce_ = false;
    isVendorCeActivated_ = false;
#endif

    queueHceData_.clear();
    /* clear aidElement_ status */
    aidElement_.SetBundleName("");
    if (abilityConnection_ == nullptr) {
        ErrorLog("OnCardEmulationDeactivated abilityConnection_ nullptr.");
        return;
    }
    ErrCode releaseCallRet = AAFwk::AbilityManagerClient::GetInstance()->ReleaseCall(
        abilityConnection_, abilityConnection_->GetConnectedElement());
    InfoLog("Release call end. ret = %{public}d", releaseCallRet);
}

void HostCardEmulationManager::HandleDataOnW4Select(const std::string& aid, ElementName& aidElement,
                                                    const std::vector<uint8_t>& data)
{
    bool existService = ExistService(aidElement);
    if (!aid.empty()) {
        if (existService) {
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
    } else if (existService) {
        InfoLog("HandleDataOnW4Select: existing service, try to send data "
                "directly.");
        hceState_ = HostCardEmulationManager::DATA_TRANSFER;
        SendDataToService(data);
        return;
    } else {
        InfoLog("no aid got");
        std::string unknowError = "6F00";
        if (nciCeProxy_.expired()) {
            ErrorLog("HandleDataOnW4Select: nciCeProxy_ is nullptr.");
            return;
        }
        nciCeProxy_.lock()->SendRawFrame(unknowError);
    }
}

void HostCardEmulationManager::HandleDataOnW4SelectForFa(const std::string& aid, ElementName& aidElement,
    const std::vector<uint8_t>& data)
{
    /* check aidElement.BundleName */
    bool existService = IsFaServiceConnected(aidElement);
    if (!aid.empty()) {
        if (existService) {
            InfoLog("HandleDataOnW4SelectForFa: existing service, try to send data "
                    "directly.");
            hceState_ = HostCardEmulationManager::DATA_TRANSFER;
            InfoLog("hce state is %{public}d.", hceState_);
            SendDataToFaService(data, aidElement.GetBundleName());
            return;
        } else {
            InfoLog("HandleDataOnW4SelectForFa: try to connect service.");
            queueHceData_ = std::move(data);
            DispatchAbilitySingleAppForFaModel(aidElement);
            return;
        }
    } else if (existService) {
        InfoLog("HandleDataOnW4SelectForFa: existing service, try to send data "
                "directly.");
        hceState_ = HostCardEmulationManager::DATA_TRANSFER;
        SendDataToFaService(data, aidElement.GetBundleName());
        return;
    } else {
        InfoLog("no aid got");
        std::string unknowError = "6F00";
        auto nciCeProxyPtr = nciCeProxy_.lock();
        if (nciCeProxyPtr == nullptr) {
            ErrorLog("HandleDataOnW4SelectForFa: nciCeProxy_ is nullptr.");
            return;
        }
        nciCeProxyPtr->SendRawFrame(unknowError);
    }
}

void HostCardEmulationManager::HandleDataOnDataTransfer(const std::string& aid, ElementName& aidElement,
                                                        const std::vector<uint8_t>& data)
{
    bool existService = ExistService(aidElement);
    if (!aid.empty()) {
        if (existService) {
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
    } else if (existService) {
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

void HostCardEmulationManager::HandleDataOnDataTransferForFa(const std::string& aid, ElementName& aidElement,
    const std::vector<uint8_t>& data)
{
    /* check aidElement.BundleName */
    bool existService = IsFaServiceConnected(aidElement);
    if (!aid.empty()) {
        if (existService) {
            InfoLog("HandleDataOnDataTransferforFa: existing service, try to send "
                    "data directly.");
            hceState_ = HostCardEmulationManager::DATA_TRANSFER;
            InfoLog("hce state is %{public}d.", hceState_);
            SendDataToFaService(data, aidElement.GetBundleName());
            return;
        } else {
            InfoLog("HandleDataOnDataTransferforFa: existing service, try to "
                    "connect service.");
            queueHceData_ = std::move(data);
            DispatchAbilitySingleAppForFaModel(aidElement);
            return;
        }
    } else if (existService) {
        InfoLog("HandleDataOnDataTransferforFa: existing service, try to send data "
                "directly.");
        hceState_ = HostCardEmulationManager::DATA_TRANSFER;
        InfoLog("hce state is %{public}d.", hceState_);
        SendDataToFaService(data, aidElement.GetBundleName());
        return;
    } else {
        InfoLog("no service, drop apdu data.");
    }
}

bool HostCardEmulationManager::IsFaServiceConnected(ElementName& aidElement)
{
    std::string bundleName = aidElement.GetBundleName();
    std::lock_guard<std::mutex> lock(regInfoMutex_);
    auto it = bundleNameToHceCmdRegData_.find(bundleName);
    if (it == bundleNameToHceCmdRegData_.end()) {
        InfoLog("IsFaServiceConnected not register data for %{public}s", bundleName.c_str());
        return false;
    }
    InfoLog("IsFaServiceConnected is Connected:%{public}s", bundleName.c_str());
    return true;
}

bool HostCardEmulationManager::ExistService(ElementName& aidElement)
{
    if (abilityConnection_ == nullptr || (!abilityConnection_->ServiceConnected())) {
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
        if (data[INDEX_3] != SELECT_00 && data[INDEX_3] != SELECT_P2_0C) {
            InfoLog("not supported aid");
            return "";
        }

        uint8_t aidLength = data[INDEX_AID_LEN];
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
    auto nfcServicePtr = nfcService_.lock();
    if (nfcServicePtr == nullptr) {
        ErrorLog("RegHceCmdCallback: nfcService_ is nullptr.");
        return false;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("RegHceCmdCallback: NFC not enabled, do not set");
        return false;
    }
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
    {
        std::lock_guard<std::mutex> lock(regInfoMutex_);
        InfoLog("RegHceCmdCallback start, register size =%{public}zu.", bundleNameToHceCmdRegData_.size());
        if (bundleNameToHceCmdRegData_.find(hapTokenInfo.bundleName) != bundleNameToHceCmdRegData_.end()) {
            InfoLog("override the register data for %{public}s", hapTokenInfo.bundleName.c_str());
        }
        bundleNameToHceCmdRegData_[hapTokenInfo.bundleName] = regData;
        InfoLog("RegHceCmdCallback end, register size =%{public}zu.", bundleNameToHceCmdRegData_.size());
    }
    /* If there is APDU data and the application is the fa model, the data will be sent to the application */
    ElementName aidElement;
    std::string abilityName = "";
    std::vector<AppDataParser::HceAppAidInfo> hceApps;
    ExternalDepsProxy::GetInstance().GetHceApps(hceApps);
    for (const AppDataParser::HceAppAidInfo &appAidInfo : hceApps) {
        if (appAidInfo.element.GetBundleName() == hapTokenInfo.bundleName) {
            abilityName = appAidInfo.element.GetAbilityName();
            InfoLog("RegHceCmdCallback: abilityName = [%{public}s]", abilityName.c_str());
            break;
        }
    }

    if (abilityName.empty()) {
        ErrorLog("RegHceCmdCallback: abilityName is not find");
        return false;
    }
    aidElement.SetBundleName(hapTokenInfo.bundleName);
    aidElement.SetAbilityName(abilityName);
    if (IsFaModeApplication(aidElement)) {
        HandleQueueDataForFa(aidElement.GetBundleName());
    }
    return true;
}

bool HostCardEmulationManager::SendHostApduData(std::string hexCmdData, bool raw, std::string& hexRespData,
                                                Security::AccessToken::AccessTokenID callerToken)
{
    auto nfcServicePtr = nfcService_.lock();
    if (nfcServicePtr == nullptr) {
        ErrorLog("SendHostApduData: nfcService_ is nullptr.");
        return false;
    }
    if (!nfcServicePtr->IsNfcEnabled()) {
        ErrorLog("SendHostApduData: NFC not enabled, do not send.");
        return false;
    }

    ElementName aidElement;
    {
        std::lock_guard<std::mutex> lock(hceStateMutex_);
        aidElement = aidElement_;
    }
    if (IsFaModeApplication(aidElement)) {
        if (!IsFaServiceConnected(aidElement)) {
            ErrorLog("SendHostApduData fa: not the connected app, do not send.");
            return false;
        }
    } else {
        if (!IsCorrespondentService(callerToken)) {
            ErrorLog("SendHostApduData stage: not the connected app, do not send.");
            return false;
        }
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
    if (abilityConnection_ == nullptr) {
        ErrorLog("IsCorrespondentService abilityConnection_ is null");
        return false;
    }
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
    std::lock_guard<std::mutex> lock(hceStateMutex_);
    bool shouldSendQueueData = hceState_ == HostCardEmulationManager::WAIT_FOR_SERVICE && !queueHceData_.empty();

    std::string queueData = KITS::NfcSdkCommon::BytesVecToHexString(&queueHceData_[0], queueHceData_.size());
    if (abilityConnection_ == nullptr) {
        ErrorLog("HandleQueueData abilityConnection_ is null");
        return;
    }
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

void HostCardEmulationManager::HandleQueueDataForFa(const std::string &bundleName)
{
    std::lock_guard<std::mutex> lock(hceStateMutex_);
    if (queueHceData_.size() == 0) {
        WarnLog("HandleQueueDataForFa queueHceData is null");
        return;
    }
    if (abilityConnection_ == nullptr) {
        ErrorLog("HandleQueueDataForFa abilityConnection_ is null");
        return;
    }
    std::string queueData = KITS::NfcSdkCommon::BytesVecToHexString(&queueHceData_[0], queueHceData_.size());
    InfoLog("RegHceCmdCallback queue data for fa %{public}s, hceState= %{public}d, "
            "service connected= %{public}d",
            queueData.c_str(), hceState_, abilityConnection_->ServiceConnected());
    hceState_ = HostCardEmulationManager::WAIT_FOR_SERVICE;
    bool shouldSendQueueData = hceState_ == HostCardEmulationManager::WAIT_FOR_SERVICE && !queueHceData_.empty();
    if (shouldSendQueueData) {
        InfoLog("RegHceCmdCallback should send queue data");
        hceState_ = HostCardEmulationManager::DATA_TRANSFER;
        InfoLog("hce state is %{public}d.", hceState_);
        SendDataToFaService(queueHceData_, bundleName);
        queueHceData_.clear();
        return;
    }
    WarnLog("HandleQueueDataForFa can not send the data.");
}

void HostCardEmulationManager::SendDataToService(const std::vector<uint8_t>& data)
{
    if (abilityConnection_ == nullptr) {
        ErrorLog("SendDataToService abilityConnection_ is null");
        return;
    }
    std::string bundleName = abilityConnection_->GetConnectedElement().GetBundleName();

    std::lock_guard<std::mutex> lock(regInfoMutex_);
    InfoLog("SendDataToService register size = %{public}zu.", bundleNameToHceCmdRegData_.size());
    auto it = bundleNameToHceCmdRegData_.find(bundleName);
    if (it == bundleNameToHceCmdRegData_.end()) {
        ErrorLog("no register data for %{public}s", abilityConnection_->GetConnectedElement().GetURI().c_str());
        ExternalDepsProxy::GetInstance().WriteNfcHceCmdCbHiSysEvent(bundleName, SubErrorCode::HCE_CMD_CB_NOT_EXIST);
        return;
    }
    if (it->second.callback_ == nullptr) {
        ErrorLog("callback is null");
        ExternalDepsProxy::GetInstance().WriteNfcHceCmdCbHiSysEvent(bundleName, SubErrorCode::HCE_CMD_CB_NULL);
        return;
    }
    it->second.callback_->OnCeApduData(data);
    ExternalDepsProxy::GetInstance().WriteNfcHceCmdCbHiSysEvent(bundleName, SubErrorCode::HCE_CMD_CB_EXIST);
}

void HostCardEmulationManager::SendDataToFaService(const std::vector<uint8_t>& data, const std::string &bundleName)
{
    std::lock_guard<std::mutex> lock(regInfoMutex_);
    InfoLog("SendDataToFaService register size = %{public}zu.", bundleNameToHceCmdRegData_.size());
    if (abilityConnection_ == nullptr) {
        ErrorLog("SendDataToFaService abilityConnection_ is null");
        return;
    }
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
    if (abilityConnection_ == nullptr) {
        ErrorLog("DispatchAbilitySingleApp abilityConnection_ is null");
        return false;
    }
    abilityConnection_->SetHceManager(shared_from_this());
    if (element.GetBundleName().empty() && !nciCeProxy_.expired()) {
        ErrorLog("DispatchAbilitySingleApp element empty");
        std::string aidNotFound = "6A82";
        nciCeProxy_.lock()->SendRawFrame(aidNotFound);
        return false;
    }

    InfoLog("DispatchAbilitySingleApp for element %{public}s", element.GetURI().c_str());
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

bool HostCardEmulationManager::DispatchAbilitySingleAppForFaModel(ElementName& element)
{
    auto nciCeProxyPtr = nciCeProxy_.lock();
    if (nciCeProxyPtr == nullptr) {
        ErrorLog("nciCeProxy is nullptr");
        return false;
    }
    if (element.GetBundleName().empty() && !nciCeProxy_.expired()) {
        ErrorLog("DispatchAbilitySingleAppForFaModel element empty");
        std::string aidNotFound = "6A82";
        nciCeProxyPtr->SendRawFrame(aidNotFound);
        return false;
    }

    InfoLog("DispatchAbilitySingleAppForFaModel for element %{public}s", element.GetURI().c_str());
    AAFwk::Want want;
    want.SetElement(element);

    if (AAFwk::AbilityManagerClient::GetInstance() == nullptr) {
        ErrorLog("DispatchAbilitySingleAppForFaModel AbilityManagerClient is null");
        return false;
    }
    ErrCode err = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    InfoLog("DispatchAbilitySingleAppForFaModel call StartAbility end. ret = %{public}d", err);
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
    InfoLog("EraseHceCmdCallback start, register size =%{public}zu.", bundleNameToHceCmdRegData_.size());
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