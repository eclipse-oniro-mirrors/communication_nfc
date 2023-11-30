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

namespace OHOS {
namespace NFC {
const int FIELD_COMMON_EVENT_INTERVAL = 1000;
const int DEACTIVATE_TIMEOUT = 6000;
static const int DEFAULT_HOST_ROUTE_DEST = 0x00;
static const int PWR_STA_SWTCH_ON_SCRN_UNLCK = 0x01;
static const int PWR_STA_SWTCH_ON_SCRN_LOCK = 0x10;
static const int DEFAULT_PWR_STA_HOST =
    PWR_STA_SWTCH_ON_SCRN_UNLCK | PWR_STA_SWTCH_ON_SCRN_LOCK;

CeService::CeService(std::weak_ptr<NfcService> nfcService,
                     std::weak_ptr<NCI::INciCeInterface> nciCeProxy)
    : nfcService_(nfcService), nciCeProxy_(nciCeProxy)
{
    hostCardEmulationManager_ =
        std::make_shared<HostCardEmulationManager>(nfcService, nciCeProxy);
}

CeService::~CeService() { hostCardEmulationManager_ = nullptr; }

void CeService::PublishFieldOnOrOffCommonEvent(bool isFieldOn)
{
    ExternalDepsProxy::GetInstance().PublishNfcFieldStateChanged(isFieldOn);
}

bool CeService::RegHceCmdCallback(const sptr<KITS::IHceCmdCallback> &callback,
                                  const std::string &type)
{
    return hostCardEmulationManager_->RegHceCmdCallback(callback, type);
}

bool CeService::SendHostApduData(std::string hexCmdData, bool raw,
                                 std::string &hexRespData)
{
    return hostCardEmulationManager_->SendHostApduData(hexCmdData, raw,
                                                       hexRespData);
}

void CeService::InitConfigAidRouting()
{
    DebugLog("AddAidRoutingHceOtherAids: start");
    std::vector<AppDataParser::HceAppAidInfo> hceApps;
    AppDataParser::GetInstance().GetHceApps(hceApps);
    if (hceApps.empty()) {
        InfoLog("AddAidRoutingHceOtherAids: no hce apps");
        return;
    }
    std::vector<AidEntry> aidEntries;
    for (const AppDataParser::HceAppAidInfo &appAidInfo : hceApps) {
        for (const AppDataParser::AidInfo &aidInfo : appAidInfo.customDataAid) {
            if (KITS::KEY_OHTER_AID == aidInfo.name) {
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
        static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF_TIMEOUT),
        DEACTIVATE_TIMEOUT);

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
            static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF),
            FIELD_COMMON_EVENT_INTERVAL);
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
} // namespace NFC
} // namespace OHOS