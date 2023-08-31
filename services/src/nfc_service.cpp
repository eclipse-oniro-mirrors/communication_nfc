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
#include "common_event_handler.h"
#include "loghelper.h"
#include "nfc_controller.h"
#include "nfc_polling_params.h"
#include "nfc_sdk_common.h"
#include "nfc_watch_dog.h"
#include "nfcc_host.h"
#include "nfc_timer.h"
#include "want.h"
#include "utils/preferences/nfc_pref_impl.h"
#include "tag_session.h"
#include "iservice_registry.h"

namespace OHOS {
namespace NFC {
const std::u16string NFC_SERVICE_NAME = OHOS::to_utf16("ohos.nfc.service");
const int ROUTING_DELAY_TIME = 500; // ms
uint32_t NfcService::unloadStaSaTimerId{0};

NfcService::NfcService(std::unique_ptr<NFC::NCI::INfccHost> nfccHost)
    : nfccHost_(std::move(nfccHost)),
    nfcControllerImpl_(nullptr),
    eventHandler_(nullptr),
    tagDispatcher_(nullptr),
    nfcState_(KITS::STATE_OFF)
{
}

NfcService::~NfcService()
{
    nfcControllerImpl_ = nullptr;
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

bool NfcService::Initialize()
{
    nfcService_ = shared_from_this();
    InfoLog("Nfc service initialize.");
    if (nfccHost_) {
        nfccHost_->SetNfccHostListener(nfcService_);
    } else {
        nfccHost_ = std::make_shared<NFC::NCI::NfccHost>(nfcService_);
    }

    // inner message handler, used by other modules as initialization parameters
    std::shared_ptr<AppExecFwk::EventRunner> runner = AppExecFwk::EventRunner::Create("nfcservice::EventRunner");
    eventHandler_ = std::make_shared<CommonEventHandler>(runner, shared_from_this());
    tagDispatcher_ = std::make_shared<TAG::TagDispatcher>(shared_from_this());
    tagSessionIface_ = new TAG::TagSession(shared_from_this());
    ceService_ = std::make_shared<CeService>(shared_from_this());

    // To be structured after Tag and HCE, the controller module is the controller of tag and HCE module
    nfcControllerImpl_ = new NfcControllerImpl(shared_from_this());

    currPollingParams_ = NfcPollingParams::GetNfcOffParameters();

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

void NfcService::OnTagDiscovered(std::shared_ptr<NCI::ITagHost> tagHost)
{
    InfoLog("NfcService::OnTagDiscovered");
    eventHandler_->SendEvent<NCI::ITagHost>(static_cast<uint32_t>(NfcCommonEvent::MSG_TAG_FOUND), tagHost);
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

    NfcWatchDog nfcWatchDog("DoTurnOn", WAIT_MS_INIT, nfccHost_);
    nfcWatchDog.Run();
    // Routing WakeLock acquire
    if (!nfccHost_->Initialize()) {
        ErrorLog("Nfc do turn on err");
        UpdateNfcState(KITS::STATE_OFF);
        // Routing Wake Lock release
        nfcWatchDog.Cancel();
        return false;
    }
    // Routing Wake Lock release
    nfcWatchDog.Cancel();

    nciVersion_ = nfccHost_->GetNciVersion();
    InfoLog("Get nci version: ver %{public}d", nciVersion_);

    UpdateNfcState(KITS::STATE_ON);

    if (unloadStaSaTimerId != 0) {
        NfcTimer::GetInstance()->UnRegister(unloadStaSaTimerId);
        unloadStaSaTimerId = 0;
    }

    nfccHost_->SetScreenStatus(screenState_);

    /* Start polling loop */
    StartPollingLoop(true);

    ComputeRoutingParams();
    CommitRouting();

    return true;
}

bool NfcService::DoTurnOff()
{
    InfoLog("Nfc do turn off: current state %{public}d", nfcState_);
    UpdateNfcState(KITS::STATE_TURNING_OFF);

    /* WatchDog to monitor for Deinitialize failed */
    NfcWatchDog nfcWatchDog("DoTurnOff", WAIT_MS_SET_ROUTE, nfccHost_);
    nfcWatchDog.Run();

    bool result = nfccHost_->Deinitialize();
    InfoLog("NfccHost deinitialize result %{public}d", result);

    nfcWatchDog.Cancel();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        currPollingParams_ = NfcPollingParams::GetNfcOffParameters();
    }

    if (foregroundData_.isEnable_) {
        DisableForegroundDispatch(foregroundData_.element_);
    }

    UpdateNfcState(KITS::STATE_OFF);
    TimeOutCallback timeoutCallback = std::bind(NfcService::UnloadNfcSa);
    if (unloadStaSaTimerId != 0) {
        NfcTimer::GetInstance()->UnRegister(unloadStaSaTimerId);
        unloadStaSaTimerId = 0;
    }
    NfcTimer::GetInstance()->Register(timeoutCallback, unloadStaSaTimerId, TIMEOUT_UNLOAD_NFC_SA);
    return result;
}

void NfcService::DoInitialize()
{
    // delay 5s to wait for bundle manager ready when device reboot
    sleep(5);
    eventHandler_->Intialize(tagDispatcher_, ceService_);
    AppDataParser::GetInstance().InitAppList();

    DebugLog("DoInitialize start FactoryReset");
    nfccHost_->FactoryReset();

    int lastState = NfcPrefImpl::GetInstance().GetInt(PREF_KEY_STATE);
    if (lastState == KITS::STATE_ON) {
        DoTurnOn();
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
    NfcPrefImpl::GetInstance().SetInt(PREF_KEY_STATE, newState);

    // noitfy the common event for nfc state changed.
    AAFwk::Want want;
    want.SetAction(KITS::COMMON_EVENT_NFC_ACTION_STATE_CHANGED);
    want.SetParam(KITS::NFC_EXTRA_STATE, newState);
    EventFwk::CommonEventData data;
    data.SetWant(want);
    EventFwk::CommonEventManager::PublishCommonEvent(data);

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

void NfcService::StartPollingLoop(bool force)
{
    InfoLog("StartPollingLoop force = %{public}d", force);
    if (!IsNfcEnabled()) {
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);

    NfcWatchDog pollingWatchDog("StartPollingLoop", WAIT_MS_SET_ROUTE, nfccHost_);
    pollingWatchDog.Run();
    // Compute new polling parameters
    std::shared_ptr<NfcPollingParams> newParams = GetPollingParameters(screenState_);
    InfoLog("newParams: %{public}s", newParams->ToString().c_str());
    InfoLog("currParams: %{public}s", currPollingParams_->ToString().c_str());
    if (force || !(newParams == currPollingParams_)) {
        if (newParams->ShouldEnablePolling()) {
            bool shouldRestart = currPollingParams_->ShouldEnablePolling();
            InfoLog("StartPollingLoop shouldRestart = %{public}d", shouldRestart);

            nfccHost_->EnableDiscovery(newParams->GetTechMask(),
                                       newParams->ShouldEnableReaderMode(),
                                       newParams->ShouldEnableHostRouting(),
                                       shouldRestart || force);
        } else {
            nfccHost_->DisableDiscovery();
        }
        currPollingParams_ = newParams;
    } else {
        InfoLog("StartPollingLoop: polling params equal, not updating");
    }
    pollingWatchDog.Cancel();
}

std::shared_ptr<NfcPollingParams> NfcService::GetPollingParameters(int screenState)
{
    // Recompute polling parameters based on screen state
    std::shared_ptr<NfcPollingParams> params = std::make_shared<NfcPollingParams>();

    if (foregroundData_.isEnable_) {
        params->SetTechMask(foregroundData_.techMask_);
        params->SetEnableReaderMode(true);
    } else {
        params->SetTechMask(NfcPollingParams::NFC_POLL_DEFAULT);
        params->SetEnableReaderMode(false);
    }
    return params;
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

void NfcService::HandleScreenChanged(int screenState)
{
    std::lock_guard<std::mutex> lock(mutex_);
    screenState_ = screenState;
    DebugLog("Screen changed screenState %{public}d", screenState_);
    nfccHost_->SetScreenStatus(screenState_);
}

void NfcService::HandlePackageUpdated(std::shared_ptr<EventFwk::CommonEventData> data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::string action = data->GetWant().GetAction();
    if (action.empty()) {
        ErrorLog("action is empty");
        return;
    }
    if ((action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED) ||
        (action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED)) {
        AppDataParser::GetInstance().HandleAppAddOrChangedEvent(data);
    } else if (action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED) {
        AppDataParser::GetInstance().HandleAppRemovedEvent(data);
    } else {
        DebugLog("not need event.");
    }
}

void NfcService::CommitRouting()
{
    eventHandler_->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_COMMIT_ROUTING), ROUTING_DELAY_TIME);
}

void NfcService::HandleCommitRouting()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (nfcState_ == KITS::STATE_OFF || nfcState_ == KITS::STATE_TURNING_OFF) {
        DebugLog("NOT Handle CommitRouting in state off or turning off.");
        return;
    }
    if (currPollingParams_->ShouldEnablePolling()) {
        bool result = nfccHost_->CommitRouting();
        DebugLog("HandleCommitRouting result = %{public}d", result);
    } else {
        DebugLog("NOT Handle CommitRouting when polling not enabled.");
    }
}

void NfcService::ComputeRoutingParams()
{
    eventHandler_->SendEvent(static_cast<uint32_t>(NfcCommonEvent::MSG_COMPUTE_ROUTING_PARAMS), ROUTING_DELAY_TIME);
}

void NfcService::HandleComputeRoutingParams()
{
    if (!IsNfcEnabled()) {
        ErrorLog("HandleComputeRoutingParams: NFC not enabled, do not Compute Routing Params");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    bool result = nfccHost_->ComputeRoutingParams();
    DebugLog("HandleComputeRoutingParams result = %{public}d", result);
}

uint16_t NfcService::GetTechMaskFromTechList(std::vector<uint32_t> &discTech)
{
    uint16_t techMask = 0;
    for (uint16_t i = 0; i < sizeof(discTech); i++) {
        switch (discTech[i]) {
            case static_cast<int32_t>(KITS::TagTechnology::NFC_A_TECH):
                techMask |= NFA_TECHNOLOGY_MASK_A;
                break;
            case static_cast<int32_t>(KITS::TagTechnology::NFC_B_TECH):
                techMask |= NFA_TECHNOLOGY_MASK_B;
                break;
            case static_cast<int32_t>(KITS::TagTechnology::NFC_F_TECH):
                techMask |= NFA_TECHNOLOGY_MASK_F;
                break;
            case static_cast<int32_t>(KITS::TagTechnology::NFC_V_TECH):
                techMask |= NFA_TECHNOLOGY_MASK_V;
                break;
            default:
                break;
        }
    }
    return techMask;
}

bool NfcService::EnableForegroundDispatch(AppExecFwk::ElementName element, std::vector<uint32_t> &discTech,
    const sptr<KITS::IForegroundCallback> &callback)
{
    if (!IsNfcEnabled()) {
        ErrorLog("EnableForegroundDispatch: NFC not enabled, do not set foreground");
        return false;
    }
    bool isDisablePolling = (discTech.size() == 0);
    DebugLog("EnableForegroundDispatch: element: %{public}s/%{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    if (!isDisablePolling) {
        foregroundData_.isEnable_ = true;
        foregroundData_.techMask_ = GetTechMaskFromTechList(discTech);
        foregroundData_.element_ = element;
        foregroundData_.callback_ = callback;
    }
    StartPollingLoop(true);
    return true;
}

bool NfcService::DisableForegroundDispatch(AppExecFwk::ElementName element)
{
    DebugLog("DisableForegroundDispatch: element: %{public}s/%{public}s",
        element.GetBundleName().c_str(), element.GetAbilityName().c_str());
    foregroundData_.isEnable_ = false;
    foregroundData_.techMask_ = 0xFFFF;
    foregroundData_.callerToken_ = 0;
    foregroundData_.callback_ = nullptr;

    StartPollingLoop(true);
    return true;
}

bool NfcService::DisableForegroundByDeathRcpt()
{
    return DisableForegroundDispatch(foregroundData_.element_);
}

bool NfcService::IsForegroundEnabled()
{
    return foregroundData_.isEnable_;
}

void NfcService::SendTagToForeground(KITS::TagInfoParcelable tagInfo)
{
    if (!IsForegroundEnabled() || foregroundData_.callback_ == nullptr) {
        ErrorLog("SendTagToForeground: invalid foreground state");
        return;
    }
    DebugLog("SendTagToForeground: OnTagDiscovered, tagInfo = %{public}s", tagInfo.ToString().c_str());
    foregroundData_.callback_->OnTagDiscovered(tagInfo);
}
}  // namespace NFC
}  // namespace OHOS
