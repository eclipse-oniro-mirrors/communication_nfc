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
#include "nfc_database_helper.h"
#include "nfc_event_handler.h"
#include "nfc_event_publisher.h"
#include "nfc_hisysevent.h"
#include "nfc_polling_params.h"
#include "nfc_sdk_common.h"
#include "nfc_timer.h"
#include "nfc_watch_dog.h"
#include "tag_session.h"
#include "run_on_demaind_manager.h"
#include "want.h"

#ifdef USE_VENDOR_NCI_NATIVE
#include "nci_ce_impl_vendor.h"
#include "nci_nfcc_impl_vendor.h"
#include "nci_tag_impl_vendor.h"
#else
#include "nci_ce_impl_default.h"
#include "nci_nfcc_impl_default.h"
#include "nci_tag_impl_default.h"
#endif

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
    if (task_ && task_->joinable()) {
        task_->join();
    }
    if (rootTask_ && rootTask_->joinable()) {
        rootTask_->join();
    }
}

std::weak_ptr<NfcService> NfcService::GetInstance() const
{
    return nfcService_;
}

std::shared_ptr<NCI::INciNfccInterface> NfcService::GetNciNfccInterface(void)
{
#ifdef USE_VENDOR_NCI_NATIVE
    return std::make_shared<NCI::NciNfccImplVendor>();
#else
    return std::make_shared<NCI::NciNfccImplDefault>();
#endif
}

std::shared_ptr<NCI::INciTagInterface> NfcService::GetNciTagInterface(void)
{
#ifdef USE_VENDOR_NCI_NATIVE
    return std::make_shared<NCI::NciTagImplVendor>();
#else
    return std::make_shared<NCI::NciTagImplDefault>();
#endif
}

std::shared_ptr<NCI::INciCeInterface> NfcService::GetNciCeInterface(void)
{
#ifdef USE_VENDOR_NCI_NATIVE
    return std::make_shared<NCI::NciCeImplVendor>();
#else
    return std::make_shared<NCI::NciCeImplDefault>();
#endif
}

std::weak_ptr<NCI::NciTagProxy> NfcService::GetNciTagProxy(void)
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

bool NfcService::Initialize()
{
    nfcService_ = shared_from_this();
    InfoLog("Nfc service initialize.");
    nciNfccProxy_ = std::make_shared<NFC::NCI::NciNfccProxy>(GetNciNfccInterface());
    nciTagProxy_ = std::make_shared<NFC::NCI::NciTagProxy>(GetNciTagInterface());
    nciCeProxy_ = std::make_shared<NFC::NCI::NciCeProxy>(GetNciCeInterface());
    nciTagProxy_->SetTagListener(nfcService_);
    nciCeProxy_->SetCeHostListener(nfcService_);

    // inner message handler, used by other modules as initialization parameters
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("nfcservice::EventRunner");
    eventHandler_ = std::make_shared<NfcEventHandler>(runner, shared_from_this());
    tagDispatcher_ = std::make_shared<TAG::TagDispatcher>(shared_from_this());
    ceService_ = std::make_shared<CeService>(shared_from_this());

    nfcPollingManager_ = std::make_shared<NfcPollingManager>(shared_from_this(), nciNfccProxy_, nciTagProxy_);
    nfcRoutingManager_ = std::make_shared<NfcRoutingManager>(eventHandler_, nciCeProxy_, shared_from_this());
    tagSessionIface_ = new TAG::TagSession(shared_from_this());

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
    DebugLog("%{public}s enter, systemAbilityId = [%{public}d] unloading", __func__, KITS::NFC_MANAGER_SYS_ABILITY_ID);
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
    InfoLog("NfcService::FiledDeactivated");
    eventHandler_->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_FIELD_DEACTIVATED));
}

bool NfcService::IsNfcTaskReady(std::future<int>& future) const
{
    if (future.valid()) {
        if (std::future_status::ready != future.wait_for(std::chrono::seconds(1))) {
            return false;
        }
    }
    return true;
}

void NfcService::ExecuteTask(KITS::NfcTask param)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (nfcState_ == KITS::STATE_TURNING_OFF || nfcState_ == KITS::STATE_TURNING_ON) {
        WarnLog("Execute task %{public}d from bad state %{public}d", param, nfcState_);
        return;
    }

    // Check the current state
    if (param == KITS::TASK_TURN_ON && nfcState_ == KITS::STATE_ON) {
        WarnLog("NFC Turn On, already On");
        return;
    }
    if (param == KITS::TASK_TURN_OFF && nfcState_ == KITS::STATE_OFF) {
        WarnLog("NFC Turn Off, already Off");
        return;
    }

    std::promise<int> promise;
    if (rootTask_) {
        if (!IsNfcTaskReady(future_)) {
            WarnLog("ExecuteTask, IsNfcTaskReady is false.");
            return;
        }
        if (task_ && task_->joinable()) {
            task_->join();
        }
        future_ = promise.get_future();
        task_ = std::make_unique<std::thread>(&NfcService::NfcTaskThread, this, param, std::move(promise));
    } else {
        rootTask_ = std::make_unique<std::thread>(&NfcService::NfcTaskThread, this, param, std::move(promise));
    }
}

void NfcService::NfcTaskThread(KITS::NfcTask params, std::promise<int> promise)
{
    InfoLog("Nfc task thread params %{public}d", params);
    switch (params) {
        case KITS::TASK_TURN_ON:
            DoTurnOn();
            break;
        case KITS::TASK_TURN_OFF:
            DoTurnOff();
            break;
        case KITS::TASK_INITIALIZE:
            DoInitialize();
            break;
        default:
            break;
    }
    promise.set_value_at_thread_exit(0);
    return;
}

bool NfcService::DoTurnOn()
{
    InfoLog("Nfc do turn on: current state %{public}d", nfcState_);
    UpdateNfcState(KITS::STATE_TURNING_ON);

    NfcWatchDog nfcWatchDog("DoTurnOn", WAIT_MS_INIT, nciNfccProxy_);
    nfcWatchDog.Run();
    // Routing WakeLock acquire
    if (!nciNfccProxy_->Initialize()) {
        ErrorLog("Nfc do turn on err");
        UpdateNfcState(KITS::STATE_OFF);
        // Routing Wake Lock release
        nfcWatchDog.Cancel();
        // Do turn on failed, openRequestCnt and openFailedCnt = 1, others = 0
        RunOnDemaindManager::GetInstance().WriteOpenAndCloseHiSysEvent(DEFAULT_COUNT, DEFAULT_COUNT,
            NOT_COUNT, NOT_COUNT);
        // Record failed event
        NfcFailedParams* nfcFailedParams = RunOnDemaindManager::GetInstance().BuildFailedParams(
            MainErrorCode::NFC_OPEN_FAILED, SubErrorCode::NCI_RESP_ERROR);
        RunOnDemaindManager::GetInstance().WriteNfcFailedHiSysEvent(nfcFailedParams);
        return false;
    }
    // Routing Wake Lock release
    nfcWatchDog.Cancel();

    nciVersion_ = nciNfccProxy_->GetNciVersion();
    InfoLog("Get nci version: ver %{public}d", nciVersion_);

    UpdateNfcState(KITS::STATE_ON);

    if (unloadStaSaTimerId != 0) {
        NfcTimer::GetInstance()->UnRegister(unloadStaSaTimerId);
        unloadStaSaTimerId = 0;
    }

    nciNfccProxy_->SetScreenStatus(screenState_);

    /* Start polling loop */
    nfcPollingManager_->StartPollingLoop(true);

    nfcRoutingManager_->ComputeRoutingParams();
    nfcRoutingManager_->CommitRouting();
    // Do turn on success, openRequestCnt = 1, others = 0
    RunOnDemaindManager::GetInstance().WriteOpenAndCloseHiSysEvent(DEFAULT_COUNT, NOT_COUNT, NOT_COUNT, NOT_COUNT);
    return true;
}

bool NfcService::DoTurnOff()
{
    InfoLog("Nfc do turn off: current state %{public}d", nfcState_);
    UpdateNfcState(KITS::STATE_TURNING_OFF);

    /* WatchDog to monitor for Deinitialize failed */
    NfcWatchDog nfcWatchDog("DoTurnOff", WAIT_MS_SET_ROUTE, nciNfccProxy_);
    nfcWatchDog.Run();

    bool result = nciNfccProxy_->Deinitialize();
    InfoLog("Nfcc deinitialize result %{public}d", result);

    nfcWatchDog.Cancel();

    nfcPollingManager_->ResetCurrPollingParams();

    if (nfcPollingManager_->IsForegroundEnabled()) {
        nfcPollingManager_->DisableForegroundDispatch(nfcPollingManager_->GetForegroundData()->element_);
    }

    UpdateNfcState(KITS::STATE_OFF);
    TimeOutCallback timeoutCallback = std::bind(NfcService::UnloadNfcSa);
    if (unloadStaSaTimerId != 0) {
        NfcTimer::GetInstance()->UnRegister(unloadStaSaTimerId);
        unloadStaSaTimerId = 0;
    }
    NfcTimer::GetInstance()->Register(timeoutCallback, unloadStaSaTimerId, TIMEOUT_UNLOAD_NFC_SA);
    // Do turn off success, closeRequestCnt = 1, others = 0
    RunOnDemaindManager::GetInstance().WriteOpenAndCloseHiSysEvent(NOT_COUNT, NOT_COUNT, DEFAULT_COUNT, NOT_COUNT);
    return result;
}

void NfcService::DoInitialize()
{
    eventHandler_->Intialize(tagDispatcher_, ceService_, nfcPollingManager_, nfcRoutingManager_);
    RunOnDemaindManager::GetInstance().InitAppList();

    int lastState = RunOnDemaindManager::GetInstance().NfcDataGetInt(PREF_KEY_STATE);
    if (lastState == KITS::STATE_ON) {
        ExecuteTask(KITS::TASK_TURN_ON);
    }
}

int NfcService::SetRegisterCallBack(const sptr<INfcControllerCallback> &callback,
    const std::string& type, Security::AccessToken::AccessTokenID callerToken)
{
    InfoLog("NfcService SetRegisterCallBack");
    std::lock_guard<std::mutex> lock(mutex_);
    bool isExist = false;
    NfcStateRegistryRecord record;
    InfoLog("RecordsSize=%{public}zu,isExist=%{public}d,type=%{public}s,callerToken=%{public}d",
        stateRecords_.size(), isExist, type.c_str(), callerToken);
    for (size_t i = 0; i < stateRecords_.size(); i++) {
        record = stateRecords_[i];
        InfoLog("record.type_=%{public}s,record.callerToken=%{public}d",
            record.type_.c_str(), record.callerToken_);
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
    DebugLog("Update nfc state: oldState %{public}d, newState %{public}d", nfcState_, newState);
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (newState == nfcState_) {
            return;
        }
        nfcState_ = newState;
    }
    RunOnDemaindManager::GetInstance().UpdateNfcState(newState);
    RunOnDemaindManager::GetInstance().PublishNfcStateChanged(newState);

    // notify the nfc state changed by callback to JS APP
    std::lock_guard<std::mutex> lock(mutex_);
    DebugLog("stateRecords_.size[%{public}zu]", stateRecords_.size());
    for (size_t i = 0; i < stateRecords_.size(); i++) {
        NfcStateRegistryRecord record = stateRecords_[i];
        DebugLog("stateRecords_[%{public}d]:type_=%{public}s,callerToken=%{public}d",
            (int)i, record.type_.c_str(), record.callerToken_);
        if (record.nfcStateChangeCallback_ != nullptr) {
            InfoLog("UpdateNfcState, OnNfcStateChanged = %{public}d", newState);
            record.nfcStateChangeCallback_->OnNfcStateChanged(newState);
        }
    }
}

int NfcService::GetNfcState()
{
    std::lock_guard<std::mutex> lock(mutex_);
    // 5min later unload nfc_service, if nfc state is off
    if (nfcState_ == KITS::STATE_OFF) {
        TimeOutCallback timeoutCallback = std::bind(NfcService::UnloadNfcSa);
        if (unloadStaSaTimerId != 0) {
            NfcTimer::GetInstance()->UnRegister(unloadStaSaTimerId);
            unloadStaSaTimerId = 0;
        }
        NfcTimer::GetInstance()->Register(timeoutCallback, unloadStaSaTimerId, TIMEOUT_UNLOAD_NFC_SA);
    }
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
    std::lock_guard<std::mutex> lock(mutex_);
    DebugLog("IsNfcEnabled, nfcState_=%{public}d", nfcState_);
    return (nfcState_ == KITS::STATE_ON);
}

void NfcService::HandleShutdown()
{
    std::lock_guard<std::mutex> lock(mutex_);
    DebugLog("device is shutting down");
    nciNfccProxy_->Shutdown();
}
}  // namespace NFC
}  // namespace OHOS
