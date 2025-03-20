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
#ifdef VENDOR_APPLICATIONS_ENABLED
#include "on_card_emulation_notify_cb_stub.h"
#include "query_app_info_callback_stub.h"
#endif

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
#ifdef VENDOR_APPLICATIONS_ENABLED
static sptr<QueryAppInfoCallbackStub> g_queryAppInfoCallbackStub =
    sptr<QueryAppInfoCallbackStub>(new (std::nothrow) QueryAppInfoCallbackStub());
static sptr<OnCardEmulationNotifyCbStub> g_onCardEmulationNotifyCbStub =
    sptr<OnCardEmulationNotifyCbStub>(new (std::nothrow) OnCardEmulationNotifyCbStub());
#endif

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
        for (uint8_t i = 0; i < MAX_RETRY_TIMES; ++i) {
            remote_ = NfcSaClient::GetInstance().LoadNfcSa(NFC_MANAGER_SYS_ABILITY_ID);
            if (remote_ == nullptr) {
                ErrorLog("Nfc Controller Is Unexist...retrying...");
                sleep(1);
                continue;
            }
            break;
        }
        if (remote_ == nullptr) {
            ErrorLog("Nfc Controller Is Unexist.");
            return;
        }
        if (deathRecipient_ == nullptr) {
            WarnLog("deathRecipient_ is nullptr!");
        }
        remote_->AddDeathRecipient(deathRecipient_);
        InfoLog("%{public}s:add remote death listener", __func__);
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
    if (!NfcSaClient::GetInstance().CheckNfcSystemAbility()) {
        WarnLog("Nfc SA not started yet.");
        return state;
    }
    InitNfcRemoteSA();
    if (nfcControllerService_.expired()) {
        ErrorLog("Nfc controller service expired.");
        return state;
    }
    state = nfcControllerService_.lock()->GetState();
    InfoLog("nfc state: %{public}d.", state);
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
    InfoLog("NfcController::RegListener");
    if (!NfcSaClient::GetInstance().CheckNfcSystemAbility()) {
        WarnLog("nfc SA not started yet.");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    InitNfcRemoteSA();
    if (nfcControllerService_.expired()) {
        ErrorLog("nfcControllerService_ expired.");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    return nfcControllerService_.lock()->RegisterCallBack(callback, type);
}

// unregister NFC state change
ErrorCode NfcController::UnregListener(const std::string& type)
{
    InfoLog("NfcController::UnregListener");
    if (!NfcSaClient::GetInstance().CheckNfcSystemAbility()) {
        WarnLog("nfc SA not started yet.");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    InitNfcRemoteSA();
    if (nfcControllerService_.expired()) {
        ErrorLog("nfcControllerService_ expired.");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    return nfcControllerService_.lock()->UnRegisterCallBack(type);
}

OHOS::sptr<IRemoteObject> NfcController::GetTagServiceIface()
{
    InitNfcRemoteSA();
    if (nfcControllerService_.expired()) {
        ErrorLog("NfcController::GetTagServiceIface nfcControllerService_ expired");
        return nullptr;
    }
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

#ifdef VENDOR_APPLICATIONS_ENABLED
ErrorCode NfcController::RegQueryApplicationCb(const std::string& type,
    QueryApplicationByVendor tagCallback, QueryHceAppByVendor hceCallback)
{
    DebugLog("NfcController::RegQueryApplicationCb");
    InitNfcRemoteSA();
    if (nfcControllerService_.expired()) {
        ErrorLog("NfcController::RegQueryApplicationCb nfcControllerService_ expired");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    if (type.compare(KEY_TAG_APP) == 0) {
        g_queryAppInfoCallbackStub->RegisterQueryTagAppCallback(tagCallback);
    } else if (type.compare(KEY_HCE_APP) == 0) {
        g_queryAppInfoCallbackStub->RegisterQueryHceAppCallback(hceCallback);
    }
    return nfcControllerService_.lock()->RegQueryApplicationCb(g_queryAppInfoCallbackStub);
}

ErrorCode NfcController::RegCardEmulationNotifyCb(OnCardEmulationNotifyCb callback)
{
    DebugLog("NfcController::RegCardEmulationNotifyCb");
    InitNfcRemoteSA();
    if (nfcControllerService_.expired()) {
        ErrorLog("NfcController::RegCardEmulationNotifyCb nfcControllerService_ expired");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    g_onCardEmulationNotifyCbStub->RegisterCallback(callback);
    return nfcControllerService_.lock()->RegCardEmulationNotifyCb(g_onCardEmulationNotifyCbStub);
}
ErrorCode NfcController::NotifyEventStatus(int eventType, int arg1, std::string arg2)
{
    DebugLog("NfcController::NotifyEventStatus");
    InitNfcRemoteSA();
    if (nfcControllerService_.expired()) {
        ErrorLog("NfcController::NotifyEventStatus nfcControllerService_ expired");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    return nfcControllerService_.lock()->NotifyEventStatus(eventType, arg1, arg2);
}
#endif

OHOS::sptr<IRemoteObject> NfcController::GetHceServiceIface(int32_t &res)
{
    InitNfcRemoteSA();
    if (nfcControllerService_.expired()) {
        ErrorLog("NfcController::GetHceServiceIface nfcControllerService_ expired");
        return nullptr;
    }
    return nfcControllerService_.lock()->GetHceServiceIface(res);
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS