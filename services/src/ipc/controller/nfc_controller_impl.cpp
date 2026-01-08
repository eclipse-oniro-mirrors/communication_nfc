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
#include "nfc_controller_impl.h"

#include "ipc_skeleton.h"
#include "nfc_controller_death_recipient.h"
#include "nfc_sdk_common.h"
#include "nfc_service.h"
#include "loghelper.h"
#include "external_deps_proxy.h"
#include "parameter.h"

namespace OHOS {
namespace NFC {
NfcControllerImpl::NfcControllerImpl(std::weak_ptr<NfcService> nfcService)
    : NfcControllerStub(), nfcService_(nfcService)
{
}

NfcControllerImpl::~NfcControllerImpl()
{
}

int32_t NfcControllerImpl::CallbackEnter(uint32_t code)
{
    InfoLog("NfcControllerImpl, code[%{public}u]", code);
    return ERR_NONE;
}

int32_t NfcControllerImpl::CallbackExit(uint32_t code, int32_t result)
{
    InfoLog("NfcControllerImpl, code[%{public}u], result[%{public}d]", code, result);
    return ERR_NONE;
}

ErrCode NfcControllerImpl::GetState(int32_t& funcResult)
{
    funcResult = KITS::STATE_OFF;
    auto nfcServicePtr = nfcService_.lock();
    if (nfcServicePtr == nullptr) {
        ErrorLog("nfcService_ is nullptr");
        return KITS::ERR_NFC_PARAMETERS;
    }
    funcResult = nfcServicePtr->GetNfcState();
    return KITS::ERR_NONE;
}

inline bool IsNfcEdmDisallowed()
{
    const char* nfcEdmKey = "persist.edm.nfc_disable";
    const uint32_t paramTrueLen = 4; // "true" 4 bytes
    const uint32_t paramFalseLen = 5; // "false" 5 bytes
    char result[paramFalseLen + 1] = {0};
    // Returns the number of bytes of the system parameter if the operation is successful.
    int len = GetParameter(nfcEdmKey, "false", result, paramFalseLen + 1);
    if (len != paramFalseLen && len != paramTrueLen) {
        ErrorLog("GetParameter edm len is invalid.");
        return false;
    }
    if (strncmp(result, "true", paramTrueLen) == 0) {
        WarnLog("nfc is prohibited by EDM. You won't be able to turn on nfc!");
        return true;
    }
    return false;
}

ErrCode NfcControllerImpl::TurnOn()
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::SYS_PERM)) {
        ErrorLog("TurnOn no permission");
        return KITS::ERR_NO_PERMISSION;
    }
    std::string appPackageName = ExternalDepsProxy::GetInstance().GetBundleNameByUid(IPCSkeleton::GetCallingUid());
    ExternalDepsProxy::GetInstance().WriteAppBehaviorHiSysEvent(SubErrorCode::TURN_ON_NFC, appPackageName);

    if (IsNfcEdmDisallowed()) {
        ErrorLog("nfc edm disallowed");
        return KITS::ERR_NFC_EDM_DISALLOWED;
    }
    if (nfcService_.expired()) {
        ErrorLog("nfcService_ expired.");
        return KITS::ERR_NFC_PARAMETERS;
    }
    return nfcService_.lock()->ExecuteTask(KITS::TASK_TURN_ON);
}

ErrCode NfcControllerImpl::TurnOff()
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::SYS_PERM)) {
        ErrorLog("TurnOff no permission");
        return KITS::ERR_NO_PERMISSION;
    }
    std::string appPackageName = ExternalDepsProxy::GetInstance().GetBundleNameByUid(IPCSkeleton::GetCallingUid());
    ExternalDepsProxy::GetInstance().WriteAppBehaviorHiSysEvent(SubErrorCode::TURN_OFF_NFC, appPackageName);

    if (nfcService_.expired()) {
        ErrorLog("nfcService_ expired.");
        return KITS::ERR_NFC_PARAMETERS;
    }
    return nfcService_.lock()->ExecuteTask(KITS::TASK_TURN_OFF);
}

ErrCode NfcControllerImpl::RestartNfc()
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::SYS_PERM)) {
        ErrorLog("TurnOff no permission");
        return KITS::ERR_NO_PERMISSION;
    }
    std::string appPackageName = ExternalDepsProxy::GetInstance().GetBundleNameByUid(IPCSkeleton::GetCallingUid());
    ExternalDepsProxy::GetInstance().WriteAppBehaviorHiSysEvent(SubErrorCode::RESTART_NFC, appPackageName);

    if (nfcService_.expired()) {
        ErrorLog("nfcService_ expired.");
        return KITS::ERR_NFC_PARAMETERS;
    }
    return nfcService_.lock()->ExecuteTask(KITS::TASK_RESTART);
}

ErrCode NfcControllerImpl::RegisterNfcStatusCallBack(const sptr<INfcControllerCallback>& cb, const std::string& type)
{
    if (cb == nullptr || cb->AsObject() == nullptr) {
        ErrorLog("input callback nullptr.");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (nfcService_.expired()) {
        ErrorLog("nfcService_ expired.");
        return KITS::ERR_NFC_PARAMETERS;
    }

    std::unique_ptr<NfcControllerDeathRecipient> recipient
        = std::make_unique<NfcControllerDeathRecipient>(this, IPCSkeleton::GetCallingTokenID());
    sptr<IRemoteObject::DeathRecipient> dr(recipient.release());
    if (!cb->AsObject()->AddDeathRecipient(dr)) {
        ErrorLog("Failed to add death recipient");
        return KITS::ERR_NFC_PARAMETERS;
    }

    std::lock_guard<std::mutex> guard(mutex_);
    deathRecipient_ = dr;
    callback_ = cb;
    return nfcService_.lock()->SetRegisterCallBack(cb, type, IPCSkeleton::GetCallingTokenID());
}

ErrCode NfcControllerImpl::UnregisterNfcStatusCallBack(const std::string& type)
{
    auto nfcServicePtr = nfcService_.lock();
    if (nfcServicePtr == nullptr) {
        ErrorLog("nfcService_ is nullptr");
        return KITS::ERR_NFC_PARAMETERS;
    }
    return nfcServicePtr->RemoveRegisterCallBack(type, IPCSkeleton::GetCallingTokenID());
}

KITS::ErrorCode NfcControllerImpl::UnRegisterAllCallBack(Security::AccessToken::AccessTokenID callerToken)
{
    auto nfcServicePtr = nfcService_.lock();
    if (nfcServicePtr == nullptr) {
        ErrorLog("nfcService_ is nullptr");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!nfcServicePtr->RemoveAllRegisterCallBack(callerToken)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_NFC_PARAMETERS;
}

ErrCode NfcControllerImpl::GetTagServiceIface(sptr<IRemoteObject>& funcResult)
{
    funcResult == nullptr;
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("GetTagServiceIface no permission");
        return KITS::ERR_NO_PERMISSION;
    }

    if (nfcService_.expired()) {
        ErrorLog("nfcService_ expired.");
        return KITS::ERR_NFC_PARAMETERS;
    }
    funcResult = nfcService_.lock()->GetTagServiceIface();
    return KITS::ERR_NONE;
}

ErrCode NfcControllerImpl::RegNdefMsgCb(const sptr<INdefMsgCallback>& cb)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("RegNdefMsgCb no permission");
        return KITS::ERR_NO_PERMISSION;
    }
    if (cb == nullptr) {
        ErrorLog("input callback nullptr.");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (nfcService_.expired()) {
        ErrorLog("nfcService_ expired");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (nfcService_.lock()->RegNdefMsgCb(cb)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_NFC_PARAMETERS;
}

ErrCode NfcControllerImpl::RegQueryApplicationCb(const sptr<IQueryAppInfoCallback>& cb)
{
#ifdef VENDOR_APPLICATIONS_ENABLED
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("RegQueryApplicationCb no permission");
        return KITS::ERR_NO_PERMISSION;
    }
    ExternalDepsProxy::GetInstance().RegQueryApplicationCb(cb);
#endif
    return KITS::ERR_NONE;
}

ErrCode NfcControllerImpl::RegCardEmulationNotifyCb(const sptr<IOnCardEmulationNotifyCb>& cb)
{
#ifdef VENDOR_APPLICATIONS_ENABLED
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("RegCardEmulationNotifyCb no permission");
        return KITS::ERR_NO_PERMISSION;
    }
    ExternalDepsProxy::GetInstance().RegCardEmulationNotifyCb(cb);
#endif
    return KITS::ERR_NONE;
}

ErrCode NfcControllerImpl::NotifyEventStatus(int32_t eventType, int32_t arg1, const std::string& arg2)
{
#ifdef VENDOR_APPLICATIONS_ENABLED
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("NotifyEventStatus no permission");
        return KITS::ERR_NO_PERMISSION;
    }
    auto nfcServicePtr = nfcService_.lock();
    if (nfcServicePtr == nullptr) {
        ErrorLog("nfcService_ is nullptr");
        return KITS::ERR_NFC_PARAMETERS;
    }

    nfcServicePtr->OnVendorEvent(eventType, arg1, arg2);
#endif
    return KITS::ERR_NONE;
}

ErrCode NfcControllerImpl::GetHceServiceIface(sptr<IRemoteObject>& funcResult)
{
    funcResult == nullptr;
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("GetHceServiceIface no permission");
        return KITS::ERR_NO_PERMISSION;
    }
    if (nfcService_.expired()) {
        ErrorLog("nfcService_ expired");
        return KITS::ERR_NFC_PARAMETERS;
    }
    funcResult = nfcService_.lock()->GetHceServiceIface();
    return KITS::ERR_NONE;
}

void NfcControllerImpl::RemoveNfcDeathRecipient(const wptr<IRemoteObject> &remote)
{
    std::lock_guard<std::mutex> guard(mutex_);
    if (callback_ == nullptr) {
        ErrorLog("OnRemoteDied callback_ is nullptr");
        return;
    }
    auto serviceRemote = callback_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
        callback_ = nullptr;
        ErrorLog("on remote died");
    }
}
}  // namespace NFC
}  // namespace OHOS
