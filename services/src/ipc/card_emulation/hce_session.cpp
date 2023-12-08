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

HceSession::~HceSession() {}

KITS::ErrorCode HceSession::RegHceCmdCallback(const sptr<KITS::IHceCmdCallback> &callback, const std::string &type)
{
    if (ceService_.expired()) {
        ErrorLog("RegHceCmdCallback:hostCardEmulationManager_ is nullptr");
        return NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }
    if (ceService_.lock()->RegHceCmdCallback(callback, type)) {
        return KITS::ERR_NONE;
    }
    return KITS::ERR_NFC_PARAMETERS;
}

int HceSession::SendRawFrame(std::string hexCmdData, bool raw, std::string &hexRespData)
{
    if (ceService_.expired()) {
        ErrorLog("RegHceCmdCallback:hostCardEmulationManager_ is nullptr");
        return NFC::KITS::ErrorCode::ERR_HCE_PARAMETERS;
    }
    bool success = ceService_.lock()->SendHostApduData(hexCmdData, raw, hexRespData);
    if (success) {
        return NFC::KITS::ErrorCode::ERR_NONE;
    } else {
        return NFC::KITS::ErrorCode::ERR_HCE_STATE_IO_FAILED;
    }
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
    return NFC::KITS::ErrorCode::ERR_NONE;
}
} // namespace HCE
} // namespace NFC
} // namespace OHOS
