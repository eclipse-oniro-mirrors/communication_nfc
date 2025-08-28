/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "hce_session.h"

#include "accesstoken_kit.h"
#include "external_deps_proxy.h"
#include "hce_cmd_death_recipient.h"
#include "ipc_skeleton.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace HCE {
using OHOS::AppExecFwk::ElementName;

HceSession::HceSession(std::shared_ptr<INfcService> service) : nfcService_(service)
{
    if (service == nullptr) {
        ErrorLog("HceSession create fail, service is nullptr");
        return;
    }
    ceService_ = service->GetCeService();
}

HceSession::~HceSession()
{
}

int32_t HceSession::CallbackEnter(uint32_t code)
{
    InfoLog("HceSession code[%{public}u]", code);
    return ERR_NONE;
}

int32_t HceSession::CallbackExit(uint32_t code, int32_t result)
{
    InfoLog("HceSession code[%{public}u], result[%{public}d]", code, result);
    return ERR_NONE;
}

void HceSession::RemoveHceDeathRecipient(const wptr<IRemoteObject> &remote)
{
    InfoLog("enter.");
    std::lock_guard<std::mutex> guard(mutex_);
    if (hceCmdCallback_ == nullptr) {
        ErrorLog("hce OnRemoteDied callback_ is nullptr");
        return;
    }
    auto serviceRemote = hceCmdCallback_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
        hceCmdCallback_ = nullptr;
        ErrorLog("hce on remote died");
    }
}

ErrCode HceSession::RegHceCmdCallback(const sptr<IHceCmdCallback>& cb, const std::string& type)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("RegHceCmdCallback, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }
    if (cb == nullptr || cb->AsObject() == nullptr) {
        ErrorLog("input callback nullptr.");
        return KITS::ERR_HCE_PARAMETERS;
    }
    if (ceService_.expired()) {
        ErrorLog("ceService_ is nullptr");
        return KITS::ERR_HCE_PARAMETERS;
    }

    std::unique_ptr<HceCmdDeathRecipient> recipient =
        std::make_unique<HceCmdDeathRecipient>(this, IPCSkeleton::GetCallingTokenID());
    sptr<IRemoteObject::DeathRecipient> dr(recipient.release());
    if (!cb->AsObject()->AddDeathRecipient(dr)) {
        ErrorLog("Failed to add death recipient");
        return KITS::ERR_HCE_PARAMETERS;
    }

    std::lock_guard<std::mutex> guard(mutex_);
    deathRecipient_ = dr;
    hceCmdCallback_ = cb;
    if (ceService_.lock()->RegHceCmdCallback(cb, type, IPCSkeleton::GetCallingTokenID())) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_NFC_PARAMETERS;
}

ErrCode HceSession::UnregHceCmdCallback(const sptr<IHceCmdCallback>& cb, const std::string& type)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("UnregHceCmdCallback, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }
    if (cb == nullptr || cb->AsObject() == nullptr) {
        ErrorLog("input callback nullptr.");
        return KITS::ERR_HCE_PARAMETERS;
    }
    if (ceService_.expired()) {
        ErrorLog("ceService_ is nullptr");
        return KITS::ERR_HCE_PARAMETERS;
    }

    std::lock_guard<std::mutex> guard(mutex_);
    if (!cb->AsObject()->RemoveDeathRecipient(deathRecipient_)) {
        ErrorLog("Failed to remove death recipient");
        return KITS::ERR_NONE;
    }
    if (ceService_.lock()->UnRegHceCmdCallback(type, IPCSkeleton::GetCallingTokenID())) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_HCE_PARAMETERS;
}

KITS::ErrorCode HceSession::UnRegAllCallback(Security::AccessToken::AccessTokenID callerToken)
{
    if (ceService_.expired()) {
        ErrorLog("UnRegAllCallback ceService_ is nullptr");
        return KITS::ERR_HCE_PARAMETERS;
    }
    if (ceService_.lock()->UnRegAllCallback(callerToken)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_HCE_PARAMETERS;
}

KITS::ErrorCode HceSession::HandleWhenRemoteDie(Security::AccessToken::AccessTokenID callerToken)
{
    if (ceService_.expired()) {
        ErrorLog("HandleWhenRemoteDie ceService_ is nullptr");
        return KITS::ERR_HCE_PARAMETERS;
    }
    if (ceService_.lock()->HandleWhenRemoteDie(callerToken)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_HCE_PARAMETERS;
}

ErrCode HceSession::SendRawFrame(const std::string& hexCmdData, bool raw, std::string& hexRespData)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("SendRawFrame, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    if (hexCmdData.size() > KITS::MAX_APDU_DATA_HEX_STR) {
        ErrorLog("raw frame too long");
        return KITS::ERR_HCE_PARAMETERS;
    }

    if (ceService_.expired()) {
        ErrorLog("SendRawFrame ceService_ is nullptr");
        return KITS::ERR_HCE_PARAMETERS;
    }

    if (ceService_.lock()->SendHostApduData(hexCmdData, raw, hexRespData, IPCSkeleton::GetCallingTokenID())) {
        return KITS::ERR_NONE;
    } else {
        return KITS::ERR_HCE_STATE_IO_FAILED;
    }
}

ErrCode HceSession::IsDefaultService(const ElementName& element, const std::string& type, bool& isDefaultService)
{
    if (ceService_.expired()) {
        ErrorLog("IsDefaultService ceService_ is nullptr");
        return KITS::ERR_HCE_PARAMETERS;
    }
    isDefaultService = ceService_.lock()->IsDefaultService(element, type);
    return KITS::ERR_NONE;
}

ErrCode HceSession::StartHce(const ElementName& element, const std::vector<std::string>& aids)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("StartHce, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    Security::AccessToken::HapTokenInfo hapTokenInfo;
    Security::AccessToken::AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    int result = Security::AccessToken::AccessTokenKit::GetHapTokenInfo(callerToken, hapTokenInfo);
    InfoLog("get hap token info, result = %{public}d", result);
    if (result) {
        return KITS::ERR_HCE_PARAMETERS;
    }
    if (hapTokenInfo.bundleName.empty()) {
        ErrorLog("StartHce: not got bundle name");
        return KITS::ERR_HCE_PARAMETERS;
    }
    if (hapTokenInfo.bundleName != element.GetBundleName()) {
        ErrorLog("StartHce: wrong bundle name");
        return KITS::ERR_HCE_PARAMETERS;
    }

    if (ceService_.expired()) {
        ErrorLog("StartHce ceService_ is nullptr");
        return KITS::ERR_HCE_PARAMETERS;
    }
    if (ceService_.lock()->StartHce(element, aids)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_HCE_PARAMETERS;
}

ErrCode HceSession::StopHce(const ElementName& element)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("StopHce, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    Security::AccessToken::HapTokenInfo hapTokenInfo;
    Security::AccessToken::AccessTokenID callerToken = IPCSkeleton::GetCallingTokenID();
    int result = Security::AccessToken::AccessTokenKit::GetHapTokenInfo(callerToken, hapTokenInfo);
    InfoLog("get hap token info, result = %{public}d", result);
    if (result) {
        return KITS::ERR_HCE_PARAMETERS;
    }
    if (hapTokenInfo.bundleName.empty()) {
        ErrorLog("StopHce: not got bundle name");
        return KITS::ERR_HCE_PARAMETERS;
    }
    if (hapTokenInfo.bundleName != element.GetBundleName()) {
        ErrorLog("StopHce: wrong bundle name");
        return KITS::ERR_HCE_PARAMETERS;
    }

    if (ceService_.expired()) {
        ErrorLog("StopHce ceService_ is nullptr");
        return KITS::ERR_HCE_PARAMETERS;
    }
    if (ceService_.lock()->StopHce(element, IPCSkeleton::GetCallingTokenID())) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_HCE_PARAMETERS;
}

ErrCode HceSession::GetPaymentServices(CePaymentServicesParcelable& parcelable)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::CARD_EMU_PERM)) {
        ErrorLog("GetPaymentServices, ERR_NO_PERMISSION");
        return KITS::ERR_NO_PERMISSION;
    }

    if (!ExternalDepsProxy::GetInstance().IsSystemApp(IPCSkeleton::GetCallingUid())) {
        ErrorLog("HandleGetPaymentServices, ERR_NOT_SYSTEM_APP");
        return KITS::ERR_NOT_SYSTEM_APP;
    }

    ExternalDepsProxy::GetInstance().GetPaymentAbilityInfos(parcelable.paymentAbilityInfos);
#ifdef NFC_SIM_FEATURE
    AppendSimBundle(parcelable.paymentAbilityInfos);
#endif
    return KITS::ERR_NONE;
}

#ifdef NFC_SIM_FEATURE
void HceSession::AppendSimBundle(std::vector<AbilityInfo> &paymentAbilityInfos)
{
    if (nfcService_.expired()) {
        ErrorLog("nfcService_ nullptr");
        return;
    }
    std::string simBundleName = nfcService_.lock()->GetSimVendorBundleName();
    AppExecFwk::BundleInfo bundleInfo;
    bool result = ExternalDepsProxy::GetInstance().GetBundleInfo(bundleInfo, simBundleName);
    if (!result) {
        ErrorLog("get sim bundle info failed.");
        return;
    }
    AbilityInfo simAbility;
    simAbility.bundleName = simBundleName;
    simAbility.labelId = bundleInfo.applicationInfo.labelId;
    simAbility.iconId = bundleInfo.applicationInfo.iconId;
    paymentAbilityInfos.push_back(simAbility);
}
#endif // NFC_SIM_FEATURE
} // namespace HCE
} // namespace NFC
} // namespace OHOS
