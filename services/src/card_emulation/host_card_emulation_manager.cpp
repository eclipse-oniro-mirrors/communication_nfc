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

namespace OHOS {
namespace NFC {
static const int CODE_SEND_APDU_DATA = 2;
using OHOS::AppExecFwk::ElementName;
HostCardEmulationManager::HostCardEmulationManager(std::weak_ptr<NfcService> nfcService,
                                                   std::weak_ptr<NCI::INciCeInterface> nciCeProxy)
    : nfcService_(nfcService), nciCeProxy_(nciCeProxy)
{
    hceCmdRegistryData_ = std::make_shared<HostCardEmulationManager::HceCmdRegistryData>();
    hceState_ = HostCardEmulationManager::INITIAL_STATE;
    queueHceData_.clear();
    connect_ = new (std::nothrow) NfcAbilityConnectionCallback();
}
HostCardEmulationManager::~HostCardEmulationManager()
{
    hceCmdRegistryData_ = nullptr;
    hceState_ = HostCardEmulationManager::INITIAL_STATE;
    queueHceData_.clear();
    connect_ = nullptr;
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

    // send data to vendor
    sptr<IOnCardEmulationNotifyCb> notifyApduDataCallback =
        ExternalDepsProxy::GetInstance().GetNotifyCardEmulationCallback();
    if (notifyApduDataCallback != nullptr) {
        notifyApduDataCallback->OnCardEmulationNotify(CODE_SEND_APDU_DATA, dataStr);
    }

    std::string aid = ParseSelectAid(data);
    InfoLog("selectAid = %{public}s", aid.c_str());
    InfoLog("onHostCardEmulationDataNfcA: state %{public}d", hceState_);
    ElementName aidElement;
    SearchElementByAid(aid, aidElement);

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

void HostCardEmulationManager::SearchElementByAid(const std::string aid, ElementName& aidElement)
{
    if (aid.empty()) {
        InfoLog("aid is empty");
        return;
    }
    std::vector<ElementName> searchElementNames;
    ExternalDepsProxy::GetInstance().GetHceAppsByAid(aid, searchElementNames);
    if (searchElementNames.empty()) {
        InfoLog("No applications found");
        return;
    }
    if (searchElementNames.size() > 1) {
        InfoLog("Found too many applications");
    }
    for (const ElementName& elementName : searchElementNames) {
        InfoLog("ElementName: %{public}s", elementName.GetBundleName().c_str());
        InfoLog("ElementValue: %{public}s", elementName.GetAbilityName().c_str());
    }
    ElementName element = searchElementNames[0];
    aidElement.SetBundleName(element.GetBundleName());
    aidElement.SetAbilityName(element.GetAbilityName());
    aidElement.SetDeviceID(element.GetDeviceID());
    aidElement.SetModuleName(element.GetModuleName());
}

void HostCardEmulationManager::OnCardEmulationActivated()
{
    InfoLog("OnCardEmulationActivated: state %{public}d", hceState_);
    hceState_ = HostCardEmulationManager::WAIT_FOR_SELECT;

    // send data to vendor
    sptr<IOnCardEmulationNotifyCb> notifyApduDataCallback =
        ExternalDepsProxy::GetInstance().GetNotifyCardEmulationCallback();
    if (notifyApduDataCallback != nullptr) {
        std::string data{};
        notifyApduDataCallback->OnCardEmulationNotify(1, data);
    }

    queueHceData_.clear();
}

void HostCardEmulationManager::OnCardEmulationDeactivated()
{
    InfoLog("OnCardEmulationDeactivated: state %{public}d", hceState_);
    hceState_ = HostCardEmulationManager::INITIAL_STATE;

    // send data to vendor
    sptr<IOnCardEmulationNotifyCb> notifyApduDataCallback =
        ExternalDepsProxy::GetInstance().GetNotifyCardEmulationCallback();
    if (notifyApduDataCallback != nullptr) {
        std::string data{};
        notifyApduDataCallback->OnCardEmulationNotify(0, data);
    }

    queueHceData_.clear();
    ErrCode releaseCallRet =
        AAFwk::AbilityManagerClient::GetInstance()->ReleaseCall(connect_, connect_.GetConnectedElement());
    InfoLog("Release call end. ret = %{public}d", releaseCallRet);
}

void HostCardEmulationManager::HandleDataOnW4Select(const std::string aid, ElementName& aidElement,
                                                    const std::vector<uint8_t>& data)
{
    bool exitService = ExistService(aidElement);
    if (!aid.empty()) {
        if (exitService) {
            InfoLog("HandleDataOnW4Select: existing service, try to send data "
                    "directly.");
            hceState_ = HostCardEmulationManager::DATA_TRANSFER;
            SendDataToService(data);
            return;
        } else {
            InfoLog("HandleDataOnW4Select: try to connect service.");
            queueHceData_ = std::move(data);
            bool startService = DispatchAbilitySingleApp(aidElement);
            if (startService) {
                hceState_ = HostCardEmulationManager::WAIT_FOR_SERVICE;
            }
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
    }
}

void HostCardEmulationManager::HandleDataOnDataTransfer(const std::string aid, ElementName& aidElement,
                                                        const std::vector<uint8_t>& data)
{
    bool exitService = ExistService(aidElement);
    if (!aid.empty()) {
        if (exitService) {
            InfoLog("HandleDataOnDataTransfer: existing service, try to send "
                    "data directly.");
            hceState_ = HostCardEmulationManager::DATA_TRANSFER;
            SendDataToService(data);
            return;
        } else {
            InfoLog("HandleDataOnDataTransfer: existing service, try to "
                    "connect service.");
            queueHceData_ = std::move(data);
            bool startService = DispatchAbilitySingleApp(aidElement);
            if (startService) {
                hceState_ = HostCardEmulationManager::WAIT_FOR_SERVICE;
            }
            return;
        }
    } else if (exitService) {
        InfoLog("HandleDataOnDataTransfer: existing service, try to send data "
                "directly.");
        hceState_ = HostCardEmulationManager::DATA_TRANSFER;
        SendDataToService(data);
        return;
    } else {
        InfoLog("no service, drop apdu data.");
    }
}
bool HostCardEmulationManager::ExistService(ElementName& aidElement)
{
    if (hceCmdRegistryData_->callback_ == nullptr) {
        InfoLog("no callback info.");
        return false;
    }
    if (!connect_->ServiceConnected()) {
        InfoLog("no service connected.");
        return false;
    }
    if (aidElement.GetBundleName().empty()) {
        InfoLog("aid is empty.");
        // normal data not select data
        return true;
    }
    if (aidElement.GetBundleName() == connect_->GetConnectedElement().GetBundleName() &&
        aidElement.GetAbilityName() == connect_->GetConnectedElement().GetAbilityName()) {
        InfoLog("ability is already connected.");
        return true;
    } else {
        WarnLog("not the same element");
        return false;
    }
}

const uint32_t SELECT_APDU_HDR_LENGTH = 5;
const uint8_t INSTR_SELECT = 0xA4;
const uint32_t MINIMUM_AID_LENGTH = 5;
const uint8_t SELECT_00 = 0x00;
const uint8_t SELECT_P1 = 0x04;
const uint32_t INDEX_CLASS_BYTE = 0;
const uint32_t INDEX_CHAIN_INSTRUCTION = 1;
const uint32_t INDEX_P1 = 2;
const uint32_t INDEX_3 = 3;

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

        int aidLength = data[4];
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
                                                 const std::string& type)
{
    if (nfcService_.expired()) {
        ErrorLog("RegHceCmdCallback: nfcService_ is nullptr.");
        return false;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("RegHceCmdCallback: NFC not enabled, do not set ");
        return false;
    }
    hceCmdRegistryData_->callback_ = callback;
    DebugLog("RegHceCmdCallback success ");
    return true;
}

bool HostCardEmulationManager::SendHostApduData(std::string hexCmdData, bool raw, const std::string& hexRespData)
{
    if (nfcService_.expired()) {
        ErrorLog("RegHceCmdCallback: nfcService_ is nullptr.");
        return false;
    }
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("RegHceCmdCallback: NFC not enabled, do not set ");
        return false;
    }
    return nciCeProxy_.lock()->SendRawFrame(hexCmdData);
}

void HostCardEmulationManager::HandleQueueData()
{
    bool shouldSendQueueData = hceState_ == HostCardEmulationManager::WAIT_FOR_SERVICE && !queueHceData_.empty();

    std::string queueData = KITS::NfcSdkCommon::BytesVecToHexString(&queueHceData_[0], queueHceData_.size());
    InfoLog("RegHceCmdCallback queue data %{public}s, hceState= %{public}d, "
            "service connected= %{public}d",
            queueData.c_str(), hceState_, connect_->ServiceConnected());
    if (shouldSendQueueData) {
        DebugLog("RegHceCmdCallback should send queue data");
        hceState_ = HostCardEmulationManager::DATA_TRANSFER;
        SendDataToService(queueHceData_);
        queueHceData_.clear();
    }
}

void HostCardEmulationManager::SendDataToService(const std::vector<uint8_t>& data)
{
    if (hceCmdRegistryData_->callback_ == nullptr) {
        ErrorLog("callback is null");
        return;
    }
    hceCmdRegistryData_->callback_->OnCeApduData(data);
}

bool HostCardEmulationManager::DispatchAbilitySingleApp(ElementName& element)
{
    connect_->SetHceManager(shared_from_this());
    if (element.GetBundleName().empty()) {
        ErrorLog("DispatchAbilitySingleApp element empty");
        return false;
    }

    InfoLog("DispatchAbilitySingleApp for element  %{public}s", element.GetURI().c_str());
    AAFwk::Want want;
    want.SetElement(element);

    if (AAFwk::AbilityManagerClient::GetInstance() == nullptr) {
        ErrorLog("DispatchAbilitySingleApp AbilityManagerClient is null");
        return false;
    }
    ErrCode err = AAFwk::AbilityManagerClient::GetInstance()->StartAbilityByCall(want, connect_);
    InfoLog("DispatchAbilitySingleApp call StartAbility end. ret = %{public}d", err);
    if (err == ERR_NONE) {
        ExternalDepsProxy::GetInstance().WriteHceSwipeResultHiSysEvent(element.GetBundleName(), DEFAULT_COUNT);
        return true;
    }
    return false;
}
} // namespace NFC
} // namespace OHOS