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
#include "nfc_controller_proxy.h"
#include "nfc_sa_client.h"
#include "nfc_sdk_common.h"
#include "indef_msg_callback.h"
#include "infc_controller_callback.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace NFC {
namespace KITS {
static const uint8_t MAX_RETRY_TIMES = 3;

sptr<IRemoteObject::DeathRecipient> NfcController::deathRecipient_;
sptr<IRemoteObject> NfcController::remote_;
bool NfcController::initialized_ = false;
bool NfcController::remoteDied_ = true;
std::mutex NfcController::mutex_;

static sptr<NfcControllerCallBackStub> g_nfcControllerCallbackStub =
    sptr<NfcControllerCallBackStub>(new NfcControllerCallBackStub());
static sptr<NdefMsgCallbackStub> g_ndefMsgCallbackStub =
    sptr<NdefMsgCallbackStub>(new NdefMsgCallbackStub());

#ifdef VENDOR_APPLICATIONS_ENABLED
static sptr<QueryAppInfoCallbackStub> g_queryAppInfoCallbackStub =
    sptr<QueryAppInfoCallbackStub>(new QueryAppInfoCallbackStub());
static sptr<OnCardEmulationNotifyCbStub> g_onCardEmulationNotifyCbStub =
    sptr<OnCardEmulationNotifyCbStub>(new OnCardEmulationNotifyCbStub());
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
    DebugLog("initialized_ = %{public}d, remote_ = %{public}d", initialized_, remote_ == nullptr);
    if (!initialized_ || remote_ == nullptr || remoteDied_) {
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
    remote_ = nullptr;
}

// Open NFC
int NfcController::TurnOn()
{
    std::lock_guard<std::mutex> guard(mutex_);
    InitNfcRemoteSA();
    sptr<INfcController> controllerProxy = iface_cast<INfcController>(remote_);
    if (controllerProxy == nullptr || controllerProxy->AsObject() == nullptr) {
        ErrorLog("nfc controller proxy nullptr.");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    ErrCode errCode = controllerProxy->TurnOn();
    InfoLog("errCode = %{public}d", errCode);
    return static_cast<int>(errCode);
}

// Close NFC
int NfcController::TurnOff()
{
    std::lock_guard<std::mutex> guard(mutex_);
    InitNfcRemoteSA();
    sptr<INfcController> controllerProxy = iface_cast<INfcController>(remote_);
    if (controllerProxy == nullptr || controllerProxy->AsObject() == nullptr) {
        ErrorLog("nfc controller proxy nullptr.");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    ErrCode errCode = controllerProxy->TurnOff();
    InfoLog("errCode = %{public}d", errCode);
    return static_cast<int>(errCode);
}

// Restart NFC
int NfcController::RestartNfc()
{
    std::lock_guard<std::mutex> guard(mutex_);
    InitNfcRemoteSA();
    sptr<INfcController> controllerProxy = iface_cast<INfcController>(remote_);
    if (controllerProxy == nullptr || controllerProxy->AsObject() == nullptr) {
        ErrorLog("nfc controller proxy nullptr.");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    ErrCode errCode = controllerProxy->RstartNfc();
    InfoLog("errCode = %{public}d", errCode);
    return static_cast<int>(errCode);
}

// get NFC state
int NfcController::GetNfcState()
{
    int state = NfcState::STATE_OFF;
    std::lock_guard<std::mutex> guard(mutex_);
    if (!NfcSaClient::GetInstance().CheckNfcSystemAbility()) {
        WarnLog("Nfc SA not started yet.");
        return state;
    }
    InitNfcRemoteSA();
    sptr<INfcController> controllerProxy = iface_cast<INfcController>(remote_);
    if (controllerProxy == nullptr || controllerProxy->AsObject() == nullptr) {
        ErrorLog("nfc controller proxy nullptr.");
        return state;
    }
    controllerProxy->GetState(state);
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
    if (g_nfcControllerCallbackStub == nullptr) {
        ErrorLog("g_nfcControllerCallbackStub is nullptr");
        return KITS::ERR_NFC_PARAMETERS;
    }
    std::lock_guard<std::mutex> guard(mutex_);
    g_nfcControllerCallbackStub->RegisterCallBack(callback);
    InitNfcRemoteSA();
    sptr<INfcController> controllerProxy = iface_cast<INfcController>(remote_);
    if (controllerProxy == nullptr || controllerProxy->AsObject() == nullptr) {
        ErrorLog("nfc controller proxy nullptr.");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    return static_cast<ErrorCode>(controllerProxy->RegisterNfcStatusCallBack(g_nfcControllerCallbackStub, type));
}

// unregister NFC state change
ErrorCode NfcController::UnregListener(const std::string& type)
{
    InfoLog("NfcController::UnregListener");
    if (!NfcSaClient::GetInstance().CheckNfcSystemAbility()) {
        WarnLog("nfc SA not started yet.");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    std::lock_guard<std::mutex> guard(mutex_);
    InitNfcRemoteSA();
    sptr<INfcController> controllerProxy = iface_cast<INfcController>(remote_);
    if (controllerProxy == nullptr || controllerProxy->AsObject() == nullptr) {
        ErrorLog("nfc controller proxy nullptr.");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    return static_cast<ErrorCode>(controllerProxy->UnregisterNfcStatusCallBack(type));
}

OHOS::sptr<IRemoteObject> NfcController::GetTagServiceIface()
{
    std::lock_guard<std::mutex> guard(mutex_);
    InitNfcRemoteSA();
    sptr<INfcController> controllerProxy = iface_cast<INfcController>(remote_);
    if (controllerProxy == nullptr || controllerProxy->AsObject() == nullptr) {
        ErrorLog("nfc controller proxy nullptr.");
        return nullptr;
    }
    OHOS::sptr<IRemoteObject> remoteObj = nullptr;
    controllerProxy->GetTagServiceIface(remoteObj);
    return remoteObj;
}

ErrorCode NfcController::RegNdefMsgCb(const sptr<INdefMsgCallback> &callback)
{
    DebugLog("NfcController::RegNdefMsgCb");
    std::lock_guard<std::mutex> guard(mutex_);
    if (g_ndefMsgCallbackStub == nullptr) {
        ErrorLog("g_ndefMsgCallbackStub is nullptr");
        return KITS::ERR_NFC_PARAMETERS;
    }
    g_ndefMsgCallbackStub->RegisterCallback(callback);

    InitNfcRemoteSA();
    sptr<INfcController> controllerProxy = iface_cast<INfcController>(remote_);
    if (controllerProxy == nullptr || controllerProxy->AsObject() == nullptr) {
        ErrorLog("nfc controller proxy nullptr.");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    return static_cast<ErrorCode>(controllerProxy->RegNdefMsgCb(g_ndefMsgCallbackStub));
}

#ifdef VENDOR_APPLICATIONS_ENABLED
ErrorCode NfcController::RegQueryApplicationCb(const std::string& type,
    QueryApplicationByVendor tagCallback, QueryHceAppByVendor hceCallback)
{
    DebugLog("NfcController::RegQueryApplicationCb");
    if (g_queryAppInfoCallbackStub == nullptr) {
        ErrorLog("g_queryAppInfoCallbackStub is nullptr");
        return KITS::ERR_NFC_PARAMETERS;
    }

    std::lock_guard<std::mutex> guard(mutex_);
    InitNfcRemoteSA();
    sptr<INfcController> controllerProxy = iface_cast<INfcController>(remote_);
    if (controllerProxy == nullptr || controllerProxy->AsObject() == nullptr) {
        ErrorLog("nfc controller proxy nullptr.");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    if (type.compare(KEY_TAG_APP) == 0) {
        g_queryAppInfoCallbackStub->RegisterQueryTagAppCallback(tagCallback);
    } else if (type.compare(KEY_HCE_APP) == 0) {
        g_queryAppInfoCallbackStub->RegisterQueryHceAppCallback(hceCallback);
    }
    return static_cast<ErrorCode>(controllerProxy->RegQueryApplicationCb(g_queryAppInfoCallbackStub));
}

ErrorCode NfcController::RegCardEmulationNotifyCb(OnCardEmulationNotifyCb callback)
{
    DebugLog("NfcController::RegCardEmulationNotifyCb");
    std::lock_guard<std::mutex> guard(mutex_);
    if (g_onCardEmulationNotifyCbStub == nullptr) {
        ErrorLog("g_onCardEmulationNotifyCbStub is nullptr");
        return KITS::ERR_NFC_PARAMETERS;
    }

    InitNfcRemoteSA();
    sptr<INfcController> controllerProxy = iface_cast<INfcController>(remote_);
    if (controllerProxy == nullptr || controllerProxy->AsObject() == nullptr) {
        ErrorLog("nfc controller proxy nullptr.");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    g_onCardEmulationNotifyCbStub->RegisterCallback(callback);
    return static_cast<ErrorCode>(controllerProxy->RegCardEmulationNotifyCb(g_onCardEmulationNotifyCbStub));
}

ErrorCode NfcController::NotifyEventStatus(int eventType, int arg1, std::string arg2)
{
    DebugLog("NfcController::NotifyEventStatus");
    std::lock_guard<std::mutex> guard(mutex_);
    InitNfcRemoteSA();
    sptr<INfcController> controllerProxy = iface_cast<INfcController>(remote_);
    if (controllerProxy == nullptr || controllerProxy->AsObject() == nullptr) {
        ErrorLog("nfc controller proxy nullptr.");
        return ErrorCode::ERR_NFC_STATE_UNBIND;
    }
    return static_cast<ErrorCode>(controllerProxy->NotifyEventStatus(eventType, arg1, arg2));
}
#endif // VENDOR_APPLICATIONS_ENABLED

OHOS::sptr<IRemoteObject> NfcController::GetHceServiceIface(int32_t &res)
{
    std::lock_guard<std::mutex> guard(mutex_);
    InitNfcRemoteSA();
    sptr<INfcController> controllerProxy = iface_cast<INfcController>(remote_);
    if (controllerProxy == nullptr || controllerProxy->AsObject() == nullptr) {
        ErrorLog("nfc controller proxy nullptr.");
        return nullptr;
    }
    OHOS::sptr<IRemoteObject> remoteObj = nullptr;
    controllerProxy->GetHceServiceIface(remoteObj);
    return remoteObj;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS