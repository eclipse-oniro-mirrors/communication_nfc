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
#include "ce_service.h"
#include "nfc_event_publisher.h"
#include "nfc_event_handler.h"
#include "external_deps_proxy.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "setting_data_share_impl.h"

namespace OHOS {
namespace NFC {
const int FIELD_COMMON_EVENT_INTERVAL = 1000;
const int DEACTIVATE_TIMEOUT = 6000;
static const int DEFAULT_HOST_ROUTE_DEST = 0x00;
static const int PWR_STA_SWTCH_ON_SCRN_UNLCK = 0x01;
static const int PWR_STA_SWTCH_ON_SCRN_LOCK = 0x10;
static const int DEFAULT_PWR_STA_HOST = PWR_STA_SWTCH_ON_SCRN_UNLCK | PWR_STA_SWTCH_ON_SCRN_LOCK;

CeService::CeService(std::weak_ptr<NfcService> nfcService, std::weak_ptr<NCI::INciCeInterface> nciCeProxy)
    : nfcService_(nfcService), nciCeProxy_(nciCeProxy)
{
    hostCardEmulationManager_ = std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy);

    Uri nfcDefaultPaymentApp(KIST::NFC_DATA_URI_PAYMENT_DEFAULT_APP);
    DelayedSingleton<SettingDataShareImpl>::GetInstance()->GetElementName(
        nfcDefaultPaymentApp, KIST::DATA_SHARE_KEY_NFC_PAYMENT_DEFAULT_APP, defaultPaymentElement_);
    DebugLog("CeService constructor end");
}

CeService::~CeService()
{
    hostCardEmulationManager_ = nullptr;
    DebugLog("CeService deconstructor end");
}

void CeService::PublishFieldOnOrOffCommonEvent(bool isFieldOn)
{
    ExternalDepsProxy::GetInstance().PublishNfcFieldStateChanged(isFieldOn);
}

bool CeService::RegHceCmdCallback(const sptr<KITS::IHceCmdCallback> &callback, const std::string &type)
{
    return hostCardEmulationManager_->RegHceCmdCallback(callback, type);
}

bool CeService::SendHostApduData(std::string hexCmdData, bool raw, std::string &hexRespData)
{
    return hostCardEmulationManager_->SendHostApduData(hexCmdData, raw, hexRespData);
}

void CeService::InitConfigAidRouting()
{
    DebugLog("AddAidRoutingHceOtherAids: start");
    std::vector<AppDataParser::HceAppAidInfo> hceApps;
    ExternalDepsProxy::GetInstance().GetHceApps(hceApps);
    if (hceApps.empty()) {
        InfoLog("AddAidRoutingHceOtherAids: no hce apps");
        return;
    }
    std::vector<AidEntry> aidEntries;
    for (const AppDataParser::HceAppAidInfo &appAidInfo : hceApps) {
        bool isDefaultPayment = appAidInfo.element.GetBundleName() == defaultPaymentElement_.GetBundleName() &&
                                appAidInfo.element.GetAbilityName() == defaultPaymentElement_.GetAbilityName();
        for (const AppDataParser::AidInfo &aidInfo : appAidInfo.customDataAid) {
            bool shouldAdd = KITS::KEY_OHTER_AID == aidInfo.name || isDefaultPayment;
            if (shouldAdd) {
                AidEntry aidEntry;
                aidEntry.aid = aidInfo.value;
                aidEntry.aidInfo = 0;
                aidEntry.power = DEFAULT_PWR_STA_HOST;
                aidEntry.route = DEFAULT_HOST_ROUTE_DEST;
                aidEntries.push_back(aidEntry);
            }
        }
    }
    for (const AidEntry &entry : aidEntries) {
        std::string aid = entry.aid;
        int aidInfo = entry.aidInfo;
        int power = entry.power;
        int route = entry.route;
        InfoLog("AddAidRoutingHceOtherAids: aid= %{public}s, aidInfo= "
                "0x%{public}x, route=0x%{public}x, power=0x%{public}x",
                aid.c_str(), aidInfo, route, power);
        nciCeProxy_.lock()->AddAidRouting(aid, route, aidInfo, power);
    }
    DebugLog("AddAidRoutingHceOtherAids: end");
}

void CeService::OnDefaultPaymentServiceChange()
{
    ElementName newElement;
    Uri nfcDefaultPaymentApp(KIST::NFC_DATA_URI_PAYMENT_DEFAULT_APP);
    DelayedSingleton<SettingDataShareImpl>::GetInstance()->GetElementName(
        nfcDefaultPaymentApp, KIST::DATA_SHARE_KEY_NFC_PAYMENT_DEFAULT_APP, newElement);
    if (newElement.GetURI() == defaultPaymentElement_.GetURI()) {
        InfoLog("OnDefaultPaymentServiceChange: payment service not change");
        return;
    }
    defaultPaymentElement_ = newElement;
    InitConfigAidRouting();
    if (nfcService_.expired()) {
        ErrorLog("OnDefaultPaymentServiceChange: nfc service is null");
        return;
    }
    std::weak_ptr<NfcRoutingManager> routingManager = nfcService_.lock()->GetNfcRoutingManager();
    if (routingManager.expired()) {
        ErrorLog("OnDefaultPaymentServiceChange: routing manager is null");
        return;
    }
    routingManager.lock()->ComputeRoutingParams();
    routingManager.lock()->CommitRouting();
}

void CeService::HandleFieldActivated()
{
    if (nfcService_.expired() || nfcService_.lock()->eventHandler_ == nullptr) {
        return;
    }
    nfcService_.lock()->eventHandler_->RemoveEvent(
        static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF));
    nfcService_.lock()->eventHandler_->RemoveEvent(
        static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF_TIMEOUT));
    nfcService_.lock()->eventHandler_->SendEvent(
        static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF_TIMEOUT), DEACTIVATE_TIMEOUT);

    uint64_t currentTime = KITS::NfcSdkCommon::GetCurrentTime();
    if (currentTime - lastFieldOnTime_ > FIELD_COMMON_EVENT_INTERVAL) {
        lastFieldOnTime_ = currentTime;
        nfcService_.lock()->eventHandler_->SendEvent(
            static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_ON));
    }
}

void CeService::HandleFieldDeactivated()
{
    if (nfcService_.expired() || nfcService_.lock()->eventHandler_ == nullptr) {
        return;
    }
    nfcService_.lock()->eventHandler_->RemoveEvent(
        static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF_TIMEOUT));
    nfcService_.lock()->eventHandler_->RemoveEvent(
        static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF));

    uint64_t currentTime = KITS::NfcSdkCommon::GetCurrentTime();
    if (currentTime - lastFieldOffTime_ > FIELD_COMMON_EVENT_INTERVAL) {
        lastFieldOffTime_ = currentTime;
        nfcService_.lock()->eventHandler_->SendEvent(
            static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF), FIELD_COMMON_EVENT_INTERVAL);
    }
}
void CeService::OnCardEmulationData(const std::vector<uint8_t> &data)
{
    hostCardEmulationManager_->OnHostCardEmulationDataNfcA(data);
}
void CeService::OnCardEmulationActivated()
{
    hostCardEmulationManager_->OnCardEmulationActivated();
}
void CeService::OnCardEmulationDeactivated()
{
    hostCardEmulationManager_->OnCardEmulationDeactivated();
}
OHOS::sptr<OHOS::IRemoteObject> CeService::AsObject()
{
    return nullptr;
}
void CeService::Initialize()
{
    DebugLog("CeService Initialize start");
    dataRdbObserver_ = sptr<DefaultPaymentServiceChangeCallback>(
        new (std::nothrow) DefaultPaymentServiceChangeCallback(shared_from_this()));
    DelayedSingleton<SettingDataShareImpl>::GetInstance()->RegisterDataObserver(nfcDefaultPaymentApp,
                                                                                dataRdbObserver_);
    DebugLog("CeService Initialize end");
}
void CeService::Deinitialize()
{
    DebugLog("CeService Deinitialize start");
    Uri nfcDefaultPaymentApp(KIST::NFC_DATA_URI_PAYMENT_DEFAULT_APP);
    DelayedSingleton<SettingDataShareImpl>::GetInstance()->ReleaseDataObserver(nfcDefaultPaymentApp,
                                                                               dataRdbObserver_);
    DebugLog("CeService Deinitialize end");
}
} // namespace NFC
} // namespace OHOS