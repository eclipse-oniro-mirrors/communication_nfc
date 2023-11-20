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
#include "nfc_controller.h"

#include "loghelper.h"
#include "nfc_controller_callback_stub.h"
#include "nfc_sa_client.h"
#include "nfc_sdk_common.h"
#include "indef_msg_callback.h"
#include "infc_controller_callback.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "nfc_state_change_callback.h"

namespace OHOS {
namespace NFC {
namespace KITS {
std::shared_ptr<OHOS::NFC::NfcControllerProxy> NfcController::nfcControllerProxy_;
std::weak_ptr<INfcControllerService> NfcController::nfcControllerService_;
sptr<IRemoteObject::DeathRecipient> NfcController::deathRecipient_;
sptr<IRemoteObject> NfcController::remote_;
bool NfcController::initialized_ = false;
bool NfcController::remoteDied_ = true;
std::mutex NfcController::mutex_;
static sptr<NfcStateChangeCallback> dataRdbObserver_;

NfcController::NfcController()
{
    DebugLog("[NfcController::NfcController] new ability manager");
    deathRecipient_ = new (std::nothrow) NfcServiceDeathRecipient(*this);
}

NfcController::~NfcController()
{
    DebugLog("destruct NfcController");
}

void NfcController::InitNfcRemoteSA()
{
    DebugLog("NfcController::%{public}s in, initialized_ = %{public}d, nfcControllerService_ = %{public}d",
        __func__, initialized_, nfcControllerService_.expired());
    std::lock_guard<std::mutex> guard(mutex_);
    if (!initialized_ || nfcControllerService_.expired() || remoteDied_) {
        remote_ = NfcSaClient::GetInstance().LoadNfcSa(NFC_MANAGER_SYS_ABILITY_ID);
        if (remote_ == nullptr) {
            ErrorLog("Nfc Controller Is Unexist.");
            return;
        }
        if (deathRecipient_ == nullptr) {
            WarnLog("deathRecipient_ is nullptr!");
        }
        remote_->AddDeathRecipient(deathRecipient_);
        InfoLog("%{public}s:add remote death lister", __func__);
        nfcControllerProxy_ = std::make_shared<NfcControllerProxy>(remote_);
        nfcControllerService_ = nfcControllerProxy_;

        initialized_ = true;
        remoteDied_ = false;
    }
    DebugLog("NfcController::%{public}s success.", __func__);
}

NfcController &NfcController::GetInstance()
{
    DebugLog("NfcController::GetInstance in.");
    static NfcController instance;
    return instance;
}

void NfcController::OnRemoteDied(const wptr<IRemoteObject> &remoteObject)
{
    WarnLog("%{public}s:Remote service is died!", __func__);
    std::lock_guard<std::mutex> lock(mutex_);
    remoteDied_ = true;
    initialized_ = false;
    if (deathRecipient_ == nullptr || remoteObject == nullptr) {
        ErrorLog("deathRecipient_ is nullptr!");
        return;
    }
    if (remote_ == nullptr) {
        ErrorLog("remote_ is nullptr!");
        return;
    }
    remote_->RemoveDeathRecipient(deathRecipient_);

    nfcControllerService_.reset();
    nfcControllerProxy_ = nullptr;
    remote_ = nullptr;
}

// Open NFC
int NfcController::TurnOn()
{
    InitNfcRemoteSA();
    if (nfcControllerService_.expired()) {
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    return nfcControllerService_.lock()->TurnOn();
}

// Close NFC
int NfcController::TurnOff()
{
    InitNfcRemoteSA();
    if (nfcControllerService_.expired()) {
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    return nfcControllerService_.lock()->TurnOff();
}

// get NFC state
int NfcController::GetNfcState()
{
    int state = NfcState::STATE_OFF;
    Uri nfcEnableUri(NFC_DATA_URI);
    DelayedSingleton<NfcDataShareImpl>::GetInstance()->
        GetValue(nfcEnableUri, DATA_SHARE_KEY_STATE, state);
    if (state == NfcState::STATE_ON) {
        InfoLog("%{public}s: nfc is On, reInitNfcRemoteSA.", __func__);
        InitNfcRemoteSA();
    }
    return state;
}

// check whether NFC is supported
bool NfcController::IsNfcAvailable()
{
    return true;
}

// check whether NFC is enabled
int NfcController::IsNfcOpen(bool &isOpen)
{
    isOpen = (GetNfcState() == NfcState::STATE_ON);
    return ErrorCode::ERR_NONE;
}

// register NFC state change callback
ErrorCode NfcController::RegListener(const sptr<INfcControllerCallback> &callback,
    const std::string& type)
{
    DebugLog("NfcController::RegListener");
    Uri nfcEnableUri(NFC_DATA_URI);
    if (dataRdbObserver_ == nullptr) {
        dataRdbObserver_ = sptr<NfcStateChangeCallback>(new (std::nothrow) NfcStateChangeCallback(callback));
    }
    return DelayedSingleton<NfcDataShareImpl>::GetInstance()->RegisterDataObserver(nfcEnableUri, dataRdbObserver_);
}

// unregister NFC state change
ErrorCode NfcController::UnregListener(const std::string& type)
{
    DebugLog("NfcController::UnregListener");
    if (dataRdbObserver_ == nullptr) {
        ErrorLog("NfcController::UnregListener dataRdbObserver_ is nullptr.");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    Uri nfcEnableUri(NFC_DATA_URI);
    return DelayedSingleton<NfcDataShareImpl>::GetInstance()->UnregisterDataObserver(nfcEnableUri, dataRdbObserver_);
}

OHOS::sptr<IRemoteObject> NfcController::GetTagServiceIface()
{
    InitNfcRemoteSA();
    return nfcControllerService_.lock()->GetTagServiceIface();
}

ErrorCode NfcController::RegNdefMsgCb(const sptr<INdefMsgCallback> &callback)
{
    DebugLog("NfcController::RegNdefMsgCb");
    InitNfcRemoteSA();
    if (nfcControllerService_.expired()) {
        ErrorLog("NfcController::RegNdefMsgCb nfcControllerService_ expired");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    return nfcControllerService_.lock()->RegNdefMsgCb(callback);
}

ErrorCode NfcController::RegQueryApplicationCb(QueryApplicationByVendor callback)
{
    DebugLog("NfcController::RegQueryApplicationCb");
    InitNfcRemoteSA();
    if (nfcControllerService_.expired()) {
        ErrorLog("NfcController::RegQueryApplicationCb nfcControllerService_ expired");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    return nfcControllerService_.lock()->RegQueryApplicationCb(callback);
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS