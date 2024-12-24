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
#include "nfc_service.h"
#include <unistd.h>
#include "app_data_parser.h"
#include "infc_controller_callback.h"
#include "iservice_registry.h"
#include "loghelper.h"
#include "nfc_preferences.h"
#include "nfc_event_handler.h"
#include "nfc_event_publisher.h"
#include "nfc_hisysevent.h"
#include "nfc_polling_params.h"
#include "nfc_sdk_common.h"
#include "nfc_timer.h"
#include "nfc_watch_dog.h"
#include "tag_session.h"
#include "external_deps_proxy.h"
#include "want.h"
#include "nci_nfcc_proxy.h"
#include "nci_tag_proxy.h"
#include "nci_ce_proxy.h"
#include "hce_session.h"

namespace OHOS {
namespace NFC {
const std::u16string NFC_SERVICE_NAME = OHOS::to_utf16("ohos.nfc.service");
uint32_t NfcService::unloadStaSaTimerId{0};

NfcService::NfcService()
    : eventHandler_(nullptr),
    tagDispatcher_(nullptr),
    nfcControllerImpl_(nullptr),
    nfcState_(KITS::STATE_OFF)
{
}

NfcService::~NfcService()
{
    nfcControllerImpl_ = nullptr;
    nfcPollingManager_ = nullptr;
    nfcRoutingManager_ = nullptr;
}

std::weak_ptr<NfcService> NfcService::GetInstance() const
{
    return nfcService_;
}

std::weak_ptr<NCI::INciTagInterface> NfcService::GetNciTagProxy(void)
{
    return nciTagProxy_;
}

std::weak_ptr<NfcPollingManager> NfcService::GetNfcPollingManager()
{
    return nfcPollingManager_;
}

std::weak_ptr<NfcRoutingManager> NfcService::GetNfcRoutingManager()
{
    return nfcRoutingManager_;
}

std::weak_ptr<CeService> NfcService::GetCeService()
{
    return ceService_;
}

std::string NfcService::GetSimVendorBundleName()
{
    return nciCeProxy_->GetSimVendorBundleName();
}

bool NfcService::Initialize()
{
    nfcService_ = shared_from_this();
    InfoLog("Nfc service initialize.");
    nciNfccProxy_ = std::make_shared<NFC::NCI::NciNfccProxy>();
    nciTagProxy_ = std::make_shared<NFC::NCI::NciTagProxy>();
    nciCeProxy_ = std::make_shared<NFC::NCI::NciCeProxy>();
    nciTagProxy_->SetTagListener(nfcService_);
    nciCeProxy_->SetCeHostListener(nfcService_);

    // inner message handler, used by other modules as initialization parameters
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("nfcservice::EventRunner");
    eventHandler_ = std::make_shared<NfcEventHandler>(runner, shared_from_this());
    nfcSwitchHandler_ = std::make_shared<NfcSwitchEventHandler>(
        AppExecFwk::EventRunner::Create("NfcSwitchHandler", AppExecFwk::ThreadMode::FFRT), shared_from_this());
    tagDispatcher_ = std::make_shared<TAG::TagDispatcher>(shared_from_this());
    ceService_ = std::make_shared<CeService>(shared_from_this(), nciCeProxy_);

    nfcPollingManager_ = std::make_shared<NfcPollingManager>(shared_from_this(), nciNfccProxy_, nciTagProxy_);
    nfcRoutingManager_ = std::make_shared<NfcRoutingManager>(eventHandler_, nciNfccProxy_,
    nciCeProxy_, shared_from_this());
    tagSessionIface_ = new TAG::TagSession(shared_from_this());
    hceSessionIface_ = new HCE::HceSession(shared_from_this());

    // used by NfcSaManager::Init(), to public for the proxy.
    nfcControllerImpl_ = new NfcControllerImpl(shared_from_this());
    nfcPollingManager_->ResetCurrPollingParams();

    runner->Run();
    // NFC ROOT
    ExecuteTask(KITS::TASK_INITIALIZE);
    return true;
}

void NfcService::UnloadNfcSa()
{
#ifndef DTFUZZ_TEST // not for fuzz
    InfoLog("%{public}s enter, systemAbilityId = [%{public}d] unloading", __func__, KITS::NFC_MANAGER_SYS_ABILITY_ID);
    if (nfcState_ != KITS::STATE_OFF) {
        InfoLog("%{public}s nfc state = [%{public}d] skip unload", __func__, nfcState_);
        return;
    }
    sptr<ISystemAbilityManager> samgr =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        ErrorLog("%{public}s: get system ability manager failed!", __func__);
        return;
    }
    int32_t ret = samgr->UnloadSystemAbility(KITS::NFC_MANAGER_SYS_ABILITY_ID);
    if (ret != ERR_NONE) {
        ErrorLog("%{public}s: Failed to unload system ability, SA Id = [%{public}d], ret = [%{public}d].",
            __func__, KITS::NFC_MANAGER_SYS_ABILITY_ID, ret);
    }
#endif
}

std::weak_ptr<TAG::TagDispatcher> NfcService::GetTagDispatcher()
{
    return tagDispatcher_;
}

OHOS::sptr<IRemoteObject> NfcService::GetTagServiceIface()
{
    return tagSessionIface_;
}

void NfcService::OnTagDiscovered(uint32_t tagDiscId)
{
    InfoLog("NfcService::OnTagDiscovered tagDiscId %{public}d", tagDiscId);
    eventHandler_->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_TAG_FOUND), tagDiscId, 0);
    InfoLog("NfcService::OnTagDiscovered end");
}

void NfcService::OnTagLost(uint32_t tagDiscId)
{
    InfoLog("NfcService::OnTagLost tagDiscId %{public}d", tagDiscId);
    eventHandler_->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_TAG_LOST), tagDiscId, 0);
}

void NfcService::FieldActivated()
{
    InfoLog("NfcService::FieldActivated");
    eventHandler_->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_FIELD_ACTIVATED));
}

void NfcService::FieldDeactivated()
{
    InfoLog("NfcService::FieldDeactivated");
    eventHandler_->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_FIELD_DEACTIVATED));
}

#ifdef VENDOR_APPLICATIONS_ENABLED
void NfcService::OnVendorEvent(int eventType, int arg1, std::string arg2)
{
    InfoLog("NfcService::OnVendorEvent");
    eventHandler_->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_VENDOR_EVENT), eventType, 0);
}
#endif

void NfcService::OnCardEmulationData(const std::vector<uint8_t> &data)
{
    InfoLog("NfcService::OnCardEmulationData");
    ceService_->OnCardEmulationData(data);
}

void NfcService::OnCardEmulationActivated()
{
    InfoLog("NfcService::OnCardEmulationActivated");
    ceService_->OnCardEmulationActivated();
}

OHOS::sptr<IRemoteObject> NfcService::GetHceServiceIface()
{
    return hceSessionIface_;
}

void NfcService::OnCardEmulationDeactivated()
{
    InfoLog("NfcService::OnCardEmulationDeactivated");
    ceService_->OnCardEmulationDeactivated();
}

int NfcService::ExecuteTask(KITS::NfcTask param)
{
    if (nfcSwitchHandler_ == nullptr) {
        ErrorLog("eventhandler nullptr.");
        return KITS::ERR_NFC_STATE_INVALID;
    }
    InfoLog("executing task [%{public}d]", param);
    nfcSwitchHandler_->RemoveAllEvents();
    nfcSwitchHandler_->SendEvent(param);
    return ERR_NONE;
}

NfcService::NfcSwitchEventHandler::NfcSwitchEventHandler(
    const std::shared_ptr<AppExecFwk::EventRunner>& runner, std::weak_ptr<NfcService> service)
    : EventHandler(runner), nfcService_(service)
{
}

NfcService::NfcSwitchEventHandler::~NfcSwitchEventHandler()
{
}

bool NfcService::NfcSwitchEventHandler::CheckNfcState(int param)
{
    int nfcState = nfcService_.lock()->GetNfcState();
    if (nfcState == KITS::STATE_TURNING_OFF || nfcState == KITS::STATE_TURNING_ON) {
        WarnLog("Execute task %{public}d from bad state %{public}d", param, nfcState);
        return false;
    }
    if (param == KITS::TASK_TURN_ON && nfcState == KITS::STATE_ON) {
        WarnLog("NFC Turn On, already On");
        ExternalDepsProxy::GetInstance().UpdateNfcState(KITS::STATE_ON);
        return false;
    }
    if (param == KITS::TASK_TURN_OFF && nfcState == KITS::STATE_OFF) {
        WarnLog("NFC Turn Off, already Off");
        ExternalDepsProxy::GetInstance().UpdateNfcState(KITS::STATE_OFF);
        return false;
    }
    return true;
}

void NfcService::NfcSwitchEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer& event)
{
    if (event == nullptr) {
        ErrorLog("event nullptr.");
        return;
    }
    if (nfcService_.expired()) {
        ErrorLog("nfc service expired.");
        return;
    }
    int eventId = static_cast<int>(event->GetInnerEventId());
    InfoLog("process eventid = [%{public}d]", eventId);
    if (!CheckNfcState(eventId)) {
        return;
    }
    switch (eventId) {
        case KITS::TASK_INITIALIZE:
            nfcService_.lock()->DoInitialize();
            break;
        case KITS::TASK_TURN_ON:
            nfcService_.lock()->DoTurnOn();
            break;
        case KITS::TASK_TURN_OFF:
            nfcService_.lock()->DoTurnOff();
            break;
        default:
            WarnLog("ProcessEvent, unknown eventId %{public}d", eventId);
            break;
    }
    InfoLog("process eventid finished.");
}

bool NfcService::DoTurnOn()
{
    InfoLog("Nfc do turn on: current state %{public}d", nfcState_);

    CancelUnloadNfcSaTimer();
    UpdateNfcState(KITS::STATE_TURNING_ON);
    NotifyMessageToVendor(KITS::NFC_SWITCH_KEY, std::to_string(KITS::STATE_TURNING_ON));
    NfcWatchDog nfcWatchDog("DoTurnOn", WAIT_MS_INIT, nciNfccProxy_);
    nfcWatchDog.Run();
    // Routing WakeLock acquire
    if (!nciNfccProxy_->Initialize()) {
        ErrorLog("Nfc do turn on err");
        UpdateNfcState(KITS::STATE_OFF);
        // Routing Wake Lock release
        nfcWatchDog.Cancel();
        // Do turn on failed, openRequestCnt and openFailedCnt = 1, others = 0
        ExternalDepsProxy::GetInstance().WriteOpenAndCloseHiSysEvent(DEFAULT_COUNT, DEFAULT_COUNT,
            NOT_COUNT, NOT_COUNT);
        // Record failed event
        ExternalDepsProxy::GetInstance().WriteNfcFailedHiSysEvent(MainErrorCode::NFC_OPEN_FAILED,
            SubErrorCode::NCI_RESP_ERROR);
        NotifyMessageToVendor(KITS::NFC_SWITCH_KEY, std::to_string(KITS::STATE_OFF));
        return false;
    }
    // Routing Wake Lock release
    nfcWatchDog.Cancel();

    nciVersion_ = nciNfccProxy_->GetNciVersion();
    InfoLog("Get nci version: ver %{public}d", nciVersion_);

    UpdateNfcState(KITS::STATE_ON);

    NfcWatchDog nfcRoutingManagerDog("RoutingManager", WAIT_ROUTING_INIT, nciNfccProxy_);
    nfcRoutingManagerDog.Run();
    screenState_ = (int)eventHandler_->CheckScreenState();
    nciNfccProxy_->SetScreenStatus(screenState_);

    /* Start polling loop */
    nfcPollingManager_->StartPollingLoop(true);
    ceService_->Initialize();
    ceService_->InitConfigAidRouting(true);

    nfcRoutingManager_->ComputeRoutingParams(ceService_->GetDefaultPaymentType());
    nfcRoutingManager_->CommitRouting();
    nfcRoutingManagerDog.Cancel();
    // Do turn on success, openRequestCnt = 1, others = 0
    ExternalDepsProxy::GetInstance().WriteOpenAndCloseHiSysEvent(DEFAULT_COUNT, NOT_COUNT, NOT_COUNT, NOT_COUNT);
    // Record success event
    ExternalDepsProxy::GetInstance().WriteNfcFailedHiSysEvent(
        MainErrorCode::NFC_OPEN_SUCCEED, SubErrorCode::DEFAULT_ERR_DEF);
    NotifyMessageToVendor(KITS::NFC_SWITCH_KEY, std::to_string(KITS::STATE_ON));
    return true;
}

bool NfcService::DoTurnOff()
{
    InfoLog("Nfc do turn off: current state %{public}d", nfcState_);
    UpdateNfcState(KITS::STATE_TURNING_OFF);
    NotifyMessageToVendor(KITS::NFC_SWITCH_KEY, std::to_string(KITS::STATE_TURNING_OFF));

    /* WatchDog to monitor for Deinitialize failed */
    NfcWatchDog nfcWatchDog("DoTurnOff", WAIT_MS_SET_ROUTE, nciNfccProxy_);
    nfcWatchDog.Run();

    bool result = nciNfccProxy_->Deinitialize();
    InfoLog("Nfcc deinitialize result %{public}d", result);

    nfcWatchDog.Cancel();

    nfcPollingManager_->ResetCurrPollingParams();
    ceService_->Deinitialize();
    UpdateNfcState(KITS::STATE_OFF);

    // Do turn off success, closeRequestCnt = 1, others = 0
    ExternalDepsProxy::GetInstance().WriteOpenAndCloseHiSysEvent(NOT_COUNT, NOT_COUNT, DEFAULT_COUNT, NOT_COUNT);
    // Record success event
    ExternalDepsProxy::GetInstance().WriteNfcFailedHiSysEvent(
        MainErrorCode::NFC_CLOSE_SUCCEED, SubErrorCode::DEFAULT_ERR_DEF);
    NotifyMessageToVendor(KITS::NFC_SWITCH_KEY, std::to_string(KITS::STATE_OFF));
    return result;
}

void NfcService::DoInitialize()
{
    eventHandler_->Intialize(tagDispatcher_, ceService_, nfcPollingManager_, nfcRoutingManager_, nciNfccProxy_);
    ExternalDepsProxy::GetInstance().InitAppList();

    int nfcStateFromPref = ExternalDepsProxy::GetInstance().NfcDataGetInt(PREF_KEY_STATE);
    int nfcStateFromParam = ExternalDepsProxy::GetInstance().GetNfcStateFromParam();
    if (nfcStateFromPref == KITS::STATE_ON || nfcStateFromParam == KITS::STATE_ON) {
        InfoLog("should turn nfc on.");
        ExecuteTask(KITS::TASK_TURN_ON);
    } else {
        // 5min later unload nfc_service, if nfc state is off
        SetupUnloadNfcSaTimer(true);
    }
    ExternalDepsProxy::GetInstance().NfcDataClear(); // delete nfc state xml
}

int NfcService::SetRegisterCallBack(const sptr<INfcControllerCallback> &callback,
    const std::string& type, Security::AccessToken::AccessTokenID callerToken)
{
    InfoLog("NfcService SetRegisterCallBack");
    if (callback == nullptr) {
        ErrorLog("register callback is nullptr");
        return KITS::ERR_NFC_PARAMETERS;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    bool isExist = false;
    NfcStateRegistryRecord record;
    InfoLog("RecordsSize=%{public}zu,isExist=%{public}d,type=%{public}s",
        stateRecords_.size(), isExist, type.c_str());
    for (size_t i = 0; i < stateRecords_.size(); i++) {
        record = stateRecords_[i];
        InfoLog("record.type_=%{public}s", record.type_.c_str());
        if (record.type_.compare(type) == 0 && record.callerToken_ == callerToken) {
            isExist = true;
            break;
        }
    }
    InfoLog("isExist=%{public}d", isExist);
    if (!isExist) {
        record.type_ = type;
        record.callerToken_ = callerToken;
        record.nfcStateChangeCallback_ = callback;
        stateRecords_.push_back(record);
    }
    return KITS::ERR_NONE;
}

int NfcService::RemoveRegisterCallBack(const std::string& type,
    Security::AccessToken::AccessTokenID callerToken)
{
    InfoLog("NfcService RemoveRegisterCallBack");
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t result = KITS::ERR_NFC_PARAMETERS;
    std::vector<NfcStateRegistryRecord>::iterator it;
    for (it = stateRecords_.begin(); it != stateRecords_.end(); ++it) {
        if (it->type_.compare(type) == 0 && it->callerToken_ == callerToken) {
            InfoLog("NfcService RemoveRegisterCallBack success.");
            stateRecords_.erase(it);
            result = KITS::ERR_NONE;
            break;
        }
    }
    return result;
}

int NfcService::RemoveAllRegisterCallBack(Security::AccessToken::AccessTokenID callerToken)
{
    InfoLog("NfcService RemoveAllRegisterCallBack");
    std::lock_guard<std::mutex> lock(mutex_);
    int32_t result = KITS::ERR_NFC_PARAMETERS;
    std::vector<NfcStateRegistryRecord>::iterator it;
    for (it = stateRecords_.begin(); it != stateRecords_.end(); ++it) {
        if (it->callerToken_ == callerToken) {
            InfoLog("NfcService RemoveAllRegisterCallBack success.");
            stateRecords_.erase(it);
            result = KITS::ERR_NONE;
            break;
        }
    }
    return result;
}

void NfcService::UpdateNfcState(int newState)
{
    InfoLog("Update nfc state: oldState %{public}d, newState %{public}d", nfcState_, newState);
    std::lock_guard<std::mutex> lock(mutex_);
    if (newState == nfcState_) {
        return;
    }
    nfcState_ = newState;

    ExternalDepsProxy::GetInstance().UpdateNfcState(newState);
    ExternalDepsProxy::GetInstance().PublishNfcStateChanged(newState);
    InfoLog("Update nfc state: nfcState_ %{public}d, newState %{public}d succ", nfcState_, newState);

    // notify the nfc state changed by callback to JS APP
    InfoLog("stateRecords_.size[%{public}zu]", stateRecords_.size());
    for (size_t i = 0; i < stateRecords_.size(); i++) {
        NfcStateRegistryRecord record = stateRecords_[i];
        DebugLog("stateRecords_[%{public}d]:type_=%{public}s ",
            (int)i, record.type_.c_str());
        if (record.nfcStateChangeCallback_ != nullptr) {
            record.nfcStateChangeCallback_->OnNfcStateChanged(newState);
        }
    }
    if (nfcState_ == KITS::STATE_OFF) {
        // 5min later unload nfc_service, if nfc state is off
        SetupUnloadNfcSaTimer(true);
    } else {
        CancelUnloadNfcSaTimer();
    }
}

int NfcService::GetNfcState()
{
    InfoLog("start to get nfc state.");
    std::lock_guard<std::mutex> lock(mutex_);
    // 5min later unload nfc_service, if nfc state is off
    if (nfcState_ == KITS::STATE_OFF) {
        SetupUnloadNfcSaTimer(false);
    }
    InfoLog("get nfc state[%{public}d]", nfcState_);
    return nfcState_;
}

int NfcService::GetScreenState()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return screenState_;
}

int NfcService::GetNciVersion()
{
    return nciVersion_;
}

bool NfcService::IsNfcEnabled()
{
    InfoLog("IsNfcEnabled, nfcState_=%{public}d", nfcState_);
    return (nfcState_ == KITS::STATE_ON);
}

void NfcService::HandleShutdown()
{
    std::lock_guard<std::mutex> lock(mutex_);
    ExternalDepsProxy::GetInstance().UpdateNfcState(nfcState_);
    InfoLog("device is shutting down, nfcState_ = %{public}d", nfcState_);
    nciNfccProxy_->Shutdown();
}

bool NfcService::RegNdefMsgCb(const sptr<INdefMsgCallback> &callback)
{
    DebugLog("NfcService::RegNdefMsgCb");
    tagDispatcher_->RegNdefMsgCb(callback);
    return true;
}

void NfcService::SetupUnloadNfcSaTimer(bool shouldRestartTimer)
{
    TimeOutCallback timeoutCallback = [this]() { UnloadNfcSa(); };
    if (unloadStaSaTimerId != 0) {
        if (!shouldRestartTimer) {
            InfoLog("timer already started.");
            return;
        }
        NfcTimer::GetInstance()->UnRegister(unloadStaSaTimerId);
        unloadStaSaTimerId = 0;
    }
    NfcTimer::GetInstance()->Register(timeoutCallback, unloadStaSaTimerId, TIMEOUT_UNLOAD_NFC_SA);
}

void NfcService::CancelUnloadNfcSaTimer()
{
    if (unloadStaSaTimerId != 0) {
        NfcTimer::GetInstance()->UnRegister(unloadStaSaTimerId);
        unloadStaSaTimerId = 0;
    }
}

void NfcService::NotifyMessageToVendor(int key, const std::string &value)
{
    if (nciNfccProxy_ == nullptr) {
        ErrorLog("nciNfccProxy_ nullptr.");
        return;
    }
    nciNfccProxy_->NotifyMessageToVendor(key, value);
}
}  // namespace NFC
}  // namespace OHOS
