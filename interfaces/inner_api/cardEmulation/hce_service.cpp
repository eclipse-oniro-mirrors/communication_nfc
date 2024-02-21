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
#include "nfc_sdk_common.h"
#include "hce_service.h"
#include "ihce_session.h"
#include "hce_cmd_callback_stub.h"
#include "nfc_controller.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "hce_session_proxy.h"

namespace OHOS {
namespace NFC {
namespace KITS {

HceService::HceService()
{
    DebugLog("[HceService] new HceService");
}

HceService::~HceService()
{
    DebugLog("destruct HceService");
}

HceService &HceService::GetInstance()
{
    static HceService instance;
    return instance;
}

ErrorCode HceService::RegHceCmdCallback(const sptr<IHceCmdCallback> &callback, const std::string &type)
{
    DebugLog("HceService::RegHceCmdCallback");
    OHOS::sptr<HCE::IHceSession> hceSession = GetHceSessionProxy();
    if (hceSession == nullptr) {
        ErrorLog("HceService::RegHceCmdCallback, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    return hceSession->RegHceCmdCallback(callback, type);
}
ErrorCode HceService::StopHce(ElementName &element)
{
    DebugLog("HceService::StopHce");
    OHOS::sptr<HCE::IHceSession> hceSession = GetHceSessionProxy();
    if (hceSession == nullptr) {
        ErrorLog("HceService::StopHce, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    return hceSession->StopHce(element);
}
ErrorCode HceService::IsDefaultService(ElementName &element, const std::string &type, bool &isDefaultService)
{
    DebugLog("HceService::IsDefaultService");
    OHOS::sptr<HCE::IHceSession> hceSession = GetHceSessionProxy();
    if (hceSession == nullptr) {
        ErrorLog("HceService::IsDefaultService, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    return hceSession->IsDefaultService(element, type, isDefaultService);
}
int HceService::SendRawFrame(std::string hexCmdData, bool raw, std::string &hexRespData)
{
    DebugLog("HceService::SendRawFrame");
    OHOS::sptr<HCE::IHceSession> hceSession = GetHceSessionProxy();
    if (hceSession == nullptr) {
        ErrorLog("HceService::SendRawFrame, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    return hceSession->SendRawFrame(hexCmdData, raw, hexRespData);
}
int HceService::GetPaymentServices(std::vector<AbilityInfo> &abilityInfos)
{
    DebugLog("HceService::GetPaymentServices");
    OHOS::sptr<HCE::IHceSession> hceSession = GetHceSessionProxy();
    if (hceSession == nullptr) {
        ErrorLog("HceService::GetPaymentServices, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    return hceSession->GetPaymentServices(abilityInfos);
}

KITS::ErrorCode HceService::StartHce(const ElementName &element, const std::vector<std::string> &aids)
{
    DebugLog("HceService::StartHce");
    OHOS::sptr<HCE::IHceSession> hceSession = GetHceSessionProxy();
    if (hceSession == nullptr) {
        ErrorLog("HceService::StartHce, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    return hceSession->StartHce(element, aids);
}
OHOS::sptr<HCE::IHceSession> HceService::GetHceSessionProxy()
{
    if (hceSessionProxy_ == nullptr) {
        OHOS::sptr<IRemoteObject> iface = NfcController::GetInstance().GetHceServiceIface();
        if (iface != nullptr) {
            hceSessionProxy_ = new HCE::HceSessionProxy(iface);
        }
    }
    return hceSessionProxy_;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS