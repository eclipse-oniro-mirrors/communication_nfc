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

#include "loghelper.h"
#include "hce_session.h"
#include "external_deps_proxy.h"

namespace OHOS {
namespace NFC {
namespace HCE {
using OHOS::AppExecFwk::ElementName;
const std::string DUMP_LINE = "---------------------------";
const std::string DUMP_END = "\n";

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

KITS::ErrorCode HceSession::RegHceCmdCallbackByToken(const sptr<KITS::IHceCmdCallback> &callback,
                                                     const std::string &type,
                                                     Security::AccessToken::AccessTokenID callerToken)
{
    if (ceService_.expired()) {
        ErrorLog("RegHceCmdCallback:ceService_ is nullptr");
        return NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }
    if (ceService_.lock()->RegHceCmdCallback(callback, type, callerToken)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_NFC_PARAMETERS;
}

KITS::ErrorCode HceSession::UnRegHceCmdCallback(const std::string &type,
                                                Security::AccessToken::AccessTokenID callerToken)
{
    if (ceService_.expired()) {
        ErrorLog("UnRegHceCmdCallback ceService_ is nullptr");
        return NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }
    if (ceService_.lock()->UnRegHceCmdCallback(type, callerToken)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_HCE_PARAMETERS;
}

KITS::ErrorCode HceSession::UnRegAllCallback(Security::AccessToken::AccessTokenID callerToken)
{
    if (ceService_.expired()) {
        ErrorLog("UnRegAllCallback ceService_ is nullptr");
        return NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS;
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
        return NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }
    if (ceService_.lock()->HandleWhenRemoteDie(callerToken)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_HCE_PARAMETERS;
}

int HceSession::SendRawFrameByToken(std::string hexCmdData, bool raw, std::string &hexRespData,
                                    Security::AccessToken::AccessTokenID callerToken)
{
    if (ceService_.expired()) {
        ErrorLog("SendRawFrame ceService_ is nullptr");
        return NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }
    bool success = ceService_.lock()->SendHostApduData(hexCmdData, raw, hexRespData, callerToken);
    if (success) {
        return NFC::KITS::ErrorCode::ERR_NONE;
    } else {
        return NFC::KITS::ErrorCode::ERR_HCE_STATE_IO_FAILED;
    }
}

KITS::ErrorCode HceSession::IsDefaultService(ElementName &element, const std::string &type, bool &isDefaultService)
{
    if (ceService_.expired()) {
        ErrorLog("IsDefaultService ceService_ is nullptr");
        return NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }
    isDefaultService = ceService_.lock()->IsDefaultService(element, type);
    return KITS::ERR_NONE;
}

KITS::ErrorCode HceSession::StartHce(const ElementName &element, const std::vector<std::string> &aids)
{
    if (ceService_.expired()) {
        ErrorLog("StartHce ceService_ is nullptr");
        return NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }
    if (ceService_.lock()->StartHce(element, aids)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_HCE_PARAMETERS;
}

KITS::ErrorCode HceSession::StopHce(const ElementName &element, Security::AccessToken::AccessTokenID callerToken)
{
    if (ceService_.expired()) {
        ErrorLog("StopHce ceService_ is nullptr");
        return NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }
    if (ceService_.lock()->StopHce(element, callerToken)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_HCE_PARAMETERS;
}

int32_t HceSession::Dump(int32_t fd, const std::vector<std::u16string> &args)
{
    std::string info = GetDumpInfo();
    int ret = dprintf(fd, "%s\n", info.c_str());
    if (ret < 0) {
        ErrorLog("hceSession Dump ret = %{public}d", ret);
        return NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }
    return NFC::KITS::ErrorCode::ERR_NONE;
}

std::string HceSession::GetDumpInfo()
{
    std::string info;
    return info.append(DUMP_LINE)
        .append("Hce DUMP ")
        .append(DUMP_LINE)
        .append(DUMP_END)
        .append("NFC_STATE          : ")
        .append(std::to_string(nfcService_.lock()->GetNfcState()))
        .append(DUMP_END)
        .append("SCREEN_STATE       : ")
        .append(std::to_string(nfcService_.lock()->GetScreenState()))
        .append(DUMP_END)
        .append("NCI_VERSION        : ")
        .append(std::to_string(nfcService_.lock()->GetNciVersion()))
        .append(DUMP_END);
}
int HceSession::GetPaymentServices(std::vector<AbilityInfo> &abilityInfos)
{
    ExternalDepsProxy::GetInstance().GetPaymentAbilityInfos(abilityInfos);
#ifdef NFC_SIM_FEATURE
    AppendSimBundle(abilityInfos);
#endif
    return NFC::KITS::ErrorCode::ERR_NONE;
}
#ifdef NFC_SIM_FEATURE
void HceSession::AppendSimBundle(std::vector<AbilityInfo> &paymentAbilityInfos)
{
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
#endif
} // namespace HCE
} // namespace NFC
} // namespace OHOS
