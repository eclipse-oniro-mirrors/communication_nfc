/*
 * Copyright (C) 2023-2023 Huawei Device Co., Ltd.
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
#include "host_card_emulation_manager.h"
#include "ability_manager_client.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
using OHOS::AppExecFwk::ElementName;
HostCardEmulationManager::HostCardEmulationManager(
    std::weak_ptr<NfcService> nfcService,
    std::weak_ptr<NCI::INciCeInterface> nciCeProxy)
    : nfcService_(nfcService), nciCeProxy_(nciCeProxy)
{
    hceCmdRegistryData_ =
        std::make_shared<HostCardEmulationManager::HceCmdRegistryData>();
    hceState_ = HostCardEmulationManager::IDLE;
    queueHceData_.clear();
    connect_ = new (std::nothrow) NfcAbilityConnectionCallback();
}
HostCardEmulationManager::~HostCardEmulationManager()
{
    hceCmdRegistryData_ = nullptr;
    hceState_ = HostCardEmulationManager::IDLE;
    queueHceData_.clear();
    connect_ = nullptr;
}

void HostCardEmulationManager::OnHostCardEmulationDataNfcA(
    const std::vector<uint8_t>& data)
{
    if (data.empty()) {
        InfoLog("onHostCardEmulationDataNfcA: no data");
        return;
    }
    std::string dataStr =
        KITS::NfcSdkCommon::BytesVecToHexString(&data[0], data.size());
    InfoLog("onHostCardEmulationDataNfcA: Data Length = %{public}zu; Data as "
            "String = %{public}s",
            data.size(), dataStr.c_str());
    std::string aid = FindSelectAid(data);
    InfoLog("selectAid = %{public}s", aid.c_str());
    InfoLog("onHostCardEmulationDataNfcA: state %{public}d", hceState_);

    switch (hceState_) {
        case HostCardEmulationManager::IDLE: {
            InfoLog("got data on state idle");
            return;
        }
        case HostCardEmulationManager::W4_SELECT: {
            HandleDataOnW4Select(aid, data);
            break;
        }
        case HostCardEmulationManager::W4_SERVICE: {
            InfoLog("got data on state w4 service");
            return;
        }
        case HostCardEmulationManager::DATA_TRANSFER: {
            HandleDataOnDataTransfer(aid, data);
            break;
        }
        case HostCardEmulationManager::W4_DEACTIVATE: {
            InfoLog("got data on state w4 deactivate");
            return;
        }
        default: break;
    }
}

void HostCardEmulationManager::OnCardEmulationActivated()
{
    InfoLog("OnCardEmulationActivated: state %{public}d", hceState_);
    hceState_ = HostCardEmulationManager::W4_SELECT;
    queueHceData_.clear();
}

void HostCardEmulationManager::OnCardEmulationDeactivated()
{
    InfoLog("OnCardEmulationDeactivated: state %{public}d", hceState_);
    hceState_ = HostCardEmulationManager::IDLE;
    queueHceData_.clear();
    hceCmdRegistryData_->isEnabled_ = false;
    hceCmdRegistryData_->callerToken_ = 0;
    hceCmdRegistryData_->element_.SetBundleName("");
    hceCmdRegistryData_->element_.SetAbilityName("");
    hceCmdRegistryData_->element_.SetDeviceID("");
    hceCmdRegistryData_->element_.SetModuleName("");

    hceCmdRegistryData_->callback_ = nullptr;
}

void HostCardEmulationManager::HandleDataOnW4Select(
    const std::string aid, const std::vector<uint8_t>& data)
{
    bool exitService = ExistService();
    if (!aid.empty()) {
        if (exitService) {
            InfoLog("HandleDataOnW4Select: existing service, try to send data "
                    "directly.");
            hceState_ = HostCardEmulationManager::DATA_TRANSFER;
            SendDataToService(data);
            return;
        }
        else {
            InfoLog("HandleDataOnW4Select: try to connect service.");
            queueHceData_ = std::move(data);
            bool startService = DispatchAbilitySingleApp(aid);
            if (startService) {
                hceState_ = HostCardEmulationManager::W4_SERVICE;
            }
            return;
        }
    }
    else if (exitService) {
        InfoLog("HandleDataOnW4Select: existing service, try to send data "
                "directly.");
        hceState_ = HostCardEmulationManager::DATA_TRANSFER;
        SendDataToService(data);
        return;
    }
    else {
        InfoLog("no aid got");
    }
}

void HostCardEmulationManager::HandleDataOnDataTransfer(
    const std::string aid, const std::vector<uint8_t>& data)
{
    bool exitService = ExistService();
    if (!aid.empty()) {
        if (exitService) {
            InfoLog("HandleDataOnDataTransfer: existing service, try to send "
                    "data directly.");
            hceState_ = HostCardEmulationManager::DATA_TRANSFER;
            SendDataToService(data);
            return;
        }
        else {
            InfoLog("HandleDataOnDataTransfer: existing service, try to "
                    "connect service.");
            queueHceData_ = std::move(data);
            bool startService = DispatchAbilitySingleApp(aid);
            if (startService) {
                hceState_ = HostCardEmulationManager::W4_SERVICE;
            }
            return;
        }
    }
    else if (exitService) {
        InfoLog("HandleDataOnDataTransfer: existing service, try to send data "
                "directly.");
        hceState_ = HostCardEmulationManager::DATA_TRANSFER;
        SendDataToService(data);
        return;
    }
    else {
        InfoLog("no service, drop apdu data.");
    }
}
bool HostCardEmulationManager::ExistService()
{
    if (hceCmdRegistryData_->callback_ == nullptr) {
        InfoLog("no callback info.");
        return false;
    }
    if (!connect_->ServiceConnected()) {
        InfoLog("no service connected.");
        return false;
    }
    return true;
}

const uint32_t SELECT_APDU_HDR_LENGTH = 5;
const uint8_t INSTR_SELECT = 0xA4;
const uint32_t MINIMUM_AID_LENGTH = 5;

std::string HostCardEmulationManager::FindSelectAid(
    const std::vector<uint8_t>& data)
{
    if (data.empty() ||
        data.size() < SELECT_APDU_HDR_LENGTH + MINIMUM_AID_LENGTH) {
        InfoLog("Data size too small for SELECT APDU");
        return "";
    }

    // To accept a SELECT AID for dispatch, we require the following:
    // Class byte must be 0x00: logical channel set to zero, no secure
    // messaging, no chaining Instruction byte must be 0xA4: SELECT instruction
    // P1: must be 0x04: select by application identifier
    // P2: File control information is only relevant for higher-level
    // application, and we only support "first or only occurrence."
    if (data[0] == 0x00 && data[1] == INSTR_SELECT && data[2] == 0x04) {
        if (data[3] != 0x00) {
            InfoLog("Selecting next, last, or previous AID occurrence is not "
                    "supported");
            return "";
        }

        int aidLength = data[4];
        if (data.size() < SELECT_APDU_HDR_LENGTH + aidLength) {
            return "";
        }

        std::vector<uint8_t> aidVec(
            data.begin() + SELECT_APDU_HDR_LENGTH,
            data.begin() + SELECT_APDU_HDR_LENGTH + aidLength);
        return KITS::NfcSdkCommon::BytesVecToHexString(&aidVec[0],
                                                       aidVec.size());
    }

    return "";
}

bool HostCardEmulationManager::RegHceCmdCallback(
    const sptr<KITS::IHceCmdCallback>& callback, const std::string& ype)
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
    bool shouldSendQueueData =
        hceState_ == HostCardEmulationManager::W4_SERVICE &&
        !queueHceData_.empty();

    std::string queueData = KITS::NfcSdkCommon::BytesVecToHexString(
        &queueHceData_[0], queueHceData_.size());
    InfoLog("RegHceCmdCallback queue data %{public}s, hceState= %{public}d, "
            "service connected= %{public}d",
            queueData.c_str(), hceState_, connect_->ServiceConnected());
    if (shouldSendQueueData) {
        DebugLog("RegHceCmdCallback should send queue data");
        hceState_ = HostCardEmulationManager::DATA_TRANSFER;
        SendDataToService(queueHceData_);
        queueHceData_.clear();
    }
    DebugLog("RegHceCmdCallback success ");
    return true;
}

bool HostCardEmulationManager::SendHostApduData(std::string hexCmdData,
                                                bool raw,
                                                const std::string& hexRespData)
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

void HostCardEmulationManager::SendDataToService(
    const std::vector<uint8_t>& data)
{
    if (hceCmdRegistryData_->callback_ == nullptr) {
        ErrorLog("callback is null");
        return;
    }
    hceCmdRegistryData_->callback_->OnCeApduData(data);
}

bool HostCardEmulationManager::DispatchAbilitySingleApp(const std::string aid)
{
    std::vector<ElementName> searchElementNames;
    AppDataParser::GetInstance().GetHceAppsByAid(aid, searchElementNames);
    if (searchElementNames.empty()) {
        InfoLog("No applications found");
        return false;
    }
    if (searchElementNames.size() > 1) {
        InfoLog("Found too many applications");
    }
    for (const ElementName& elementName : searchElementNames) {
        InfoLog("ElementName: %{public}s", elementName.GetBundleName().c_str());
        InfoLog("ElementValue: %{public}s",
                elementName.GetAbilityName().c_str());
    }
    ElementName element = searchElementNames[0];
    if (element.GetBundleName().empty()) {
        ErrorLog("DispatchAbilitySingleApp element empty");
        return false;
    }

    InfoLog("DispatchAbilitySingleApp for app %{public}s, ability = %{public}s",
            element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    AAFwk::Want want;
    want.SetElement(element);

    if (AAFwk::AbilityManagerClient::GetInstance() == nullptr) {
        ErrorLog("DispatchAbilitySingleApp AbilityManagerClient is null");
        return false;
    }
    ErrCode err =
        AAFwk::AbilityManagerClient::GetInstance()->StartAbilityByCall(
            want, connect_);
    InfoLog("DispatchAbilitySingleApp call StartAbility end. ret = %{public}d",
            err);
    if (err == ERR_NONE) {
        return true;
    }
    return false;
}
} // namespace NFC
} // namespace OHOS