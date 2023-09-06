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

#include "accesstoken_kit.h"
#include "common_event_handler.h"
#include "ability_manager_client.h"
#include "system_ability_definition.h"
#include "iservice_registry.h"
#include "app_mgr_interface.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {

const int FIELD_COMMON_EVENT_INTERVAL = 1000;
const int DEACTIVATE_TIMEOUT = 6000;
const std::string COMMON_EVENT_NFC_ACTION_RF_FIELD_ON_DETECTED = "usual.event.nfc.action.RF_FIELD_ON_DETECTED";
const std::string COMMON_EVENT_NFC_ACTION_RF_FIELD_OFF_DETECTED = "usual.event.nfc.action.RF_FIELD_OFF_DETECTED";
const std::string ACTION_WALLET_SWIPE_CARD = "action.com.huawei.hmos.wallet.SWIPE_CARD";
const std::string WALLET_BUNDLE_NAME = "com.huawei.hmos.wallet";

CeService::CeService(std::weak_ptr<NfcService> nfcService) : nfcService_(nfcService)
{
}

CeService::~CeService()
{
}

void CeService::PublishFieldOnOrOffCommonEvent(bool isFieldOn)
{
    AAFwk::Want want;
    if (isFieldOn) {
        want.SetAction(COMMON_EVENT_NFC_ACTION_RF_FIELD_ON_DETECTED);
    } else {
        want.SetAction(COMMON_EVENT_NFC_ACTION_RF_FIELD_OFF_DETECTED);
    }
    EventFwk::CommonEventData data;
    data.SetWant(want);
    EventFwk::CommonEventManager::PublishCommonEvent(data);
}

bool CeService::IsWalletProcessExist()
{
    sptr<ISystemAbilityManager> samgrClient = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrClient == nullptr) {
        ErrorLog("samgrClient is null");
        return false;
    }

    sptr<AppExecFwk::IAppMgr> appMgrProxy =
        iface_cast<AppExecFwk::IAppMgr>(samgrClient->GetSystemAbility(APP_MGR_SERVICE_ID));
    if (appMgrProxy == nullptr) {
        ErrorLog("appMgrProxy is null");
        return false;
    }

    std::vector<AppExecFwk::RunningProcessInfo> runningList;
    int result = appMgrProxy->GetAllRunningProcesses(runningList);
    if (result != ERR_OK) {
        ErrorLog("GetAllRunningProcesses failed");
        return false;
    }

    for (AppExecFwk::RunningProcessInfo info : runningList) {
        for (std::string bundleName : info.bundleNames) {
            if (bundleName == WALLET_BUNDLE_NAME) {
                return true;
            }
        }
    }
    return false;
}

void CeService::NotifyWalletFieldEvent(std::string event)
{
    InfoLog("%{public}s", event.c_str());
    if (IsWalletProcessExist()) {
        InfoLog("Wallet Exist, return");
        return;
    }

    AAFwk::Want want;
    want.SetAction(ACTION_WALLET_SWIPE_CARD);
    want.SetParam("event", event);
    want.SetParam("ability.params.backToOtherMissionStack", true);

    if (AAFwk::AbilityManagerClient::GetInstance() == nullptr) {
        ErrorLog("AbilityManagerClient is null");
        return;
    }
    AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    InfoLog("call wallet StartAbility end");
}

void CeService::HandleFieldActivated()
{
    if (nfcService_.expired() || nfcService_.lock()->eventHandler_ == nullptr) {
        return;
    }
    nfcService_.lock()->eventHandler_->RemoveEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF));
    nfcService_.lock()->eventHandler_->RemoveEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF_TIMEOUT));
    nfcService_.lock()->eventHandler_->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF_TIMEOUT),
        DEACTIVATE_TIMEOUT);

    uint64_t currentTime = KITS::NfcSdkCommon::GetCurrentTime();
    if (currentTime - lastFieldOnTime_ > FIELD_COMMON_EVENT_INTERVAL) {
        lastFieldOnTime_ = currentTime;
        nfcService_.lock()->eventHandler_->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_ON));
        NotifyWalletFieldEvent(COMMON_EVENT_NFC_ACTION_RF_FIELD_ON_DETECTED);
    }
}

void CeService::HandleFieldDeactivated()
{
    if (nfcService_.expired() || nfcService_.lock()->eventHandler_ == nullptr) {
        return;
    }
    nfcService_.lock()->eventHandler_->RemoveEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF_TIMEOUT));
    nfcService_.lock()->eventHandler_->RemoveEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF));

    uint64_t currentTime = KITS::NfcSdkCommon::GetCurrentTime();
    if (currentTime - lastFieldOffTime_ > FIELD_COMMON_EVENT_INTERVAL) {
        lastFieldOffTime_ = currentTime;
        nfcService_.lock()->eventHandler_->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_NOTIFY_FIELD_OFF),
            FIELD_COMMON_EVENT_INTERVAL);
    }
}
} // NFC
} // OHOS