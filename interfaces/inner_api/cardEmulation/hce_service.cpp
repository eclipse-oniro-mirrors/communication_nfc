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

std::mutex g_hceSessionProxyLock;
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
    InfoLog("HceService::RegHceCmdCallback");
    int32_t res = ErrorCode::ERR_NONE;
    OHOS::sptr<HCE::IHceSession> hceSession = GetHceSessionProxy(res);
    if (res == ErrorCode::ERR_NO_PERMISSION) {
        ErrorLog("HceService::RegHceCmdCallback, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    if (hceSession == nullptr) {
        ErrorLog("HceService::RegHceCmdCallback, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    return hceSession->RegHceCmdCallback(callback, type);
}

ErrorCode HceService::UnRegHceCmdCallback(const sptr<IHceCmdCallback> &callback, const std::string &type)
{
    InfoLog("HceService::UnRegHceCmdCallback");
    int32_t res = ErrorCode::ERR_NONE;
    OHOS::sptr<HCE::IHceSession> hceSession = GetHceSessionProxy(res);
    if (res == ErrorCode::ERR_NO_PERMISSION) {
        ErrorLog("HceService::UnRegHceCmdCallback, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    if (hceSession == nullptr) {
        ErrorLog("HceService::UnRegHceCmdCallback, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    return hceSession->UnregHceCmdCallback(callback, type);
}

ErrorCode HceService::StopHce(ElementName &element)
{
    InfoLog("HceService::StopHce");
    int32_t res = ErrorCode::ERR_NONE;
    OHOS::sptr<HCE::IHceSession> hceSession = GetHceSessionProxy(res);
    if (res == ErrorCode::ERR_NO_PERMISSION) {
        ErrorLog("HceService::StopHce, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    if (hceSession == nullptr) {
        ErrorLog("HceService::StopHce, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    return hceSession->StopHce(element);
}
ErrorCode HceService::IsDefaultService(ElementName &element, const std::string &type, bool &isDefaultService)
{
    InfoLog("HceService::IsDefaultService");
    int32_t res = ErrorCode::ERR_NONE;
    OHOS::sptr<HCE::IHceSession> hceSession = GetHceSessionProxy(res);
    if (res == ErrorCode::ERR_NO_PERMISSION) {
        ErrorLog("HceService::IsDefaultService, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    if (hceSession == nullptr) {
        ErrorLog("HceService::IsDefaultService, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    return hceSession->IsDefaultService(element, type, isDefaultService);
}
int HceService::SendRawFrame(std::string hexCmdData, bool raw, std::string &hexRespData)
{
    InfoLog("HceService::SendRawFrame");
    int32_t res = ErrorCode::ERR_NONE;
    OHOS::sptr<HCE::IHceSession> hceSession = GetHceSessionProxy(res);
    if (res == ErrorCode::ERR_NO_PERMISSION) {
        ErrorLog("HceService::SendRawFrame, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    if (hceSession == nullptr) {
        ErrorLog("HceService::SendRawFrame, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    return hceSession->SendRawFrame(hexCmdData, raw, hexRespData);
}
int HceService::GetPaymentServices(std::vector<AbilityInfo> &abilityInfos)
{
    InfoLog("HceService::GetPaymentServices");
    int32_t res = ErrorCode::ERR_NONE;
    OHOS::sptr<HCE::IHceSession> hceSession = GetHceSessionProxy(res);
    if (res == ErrorCode::ERR_NO_PERMISSION) {
        ErrorLog("HceService::GetPaymentServices, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    if (hceSession == nullptr) {
        ErrorLog("HceService::GetPaymentServices, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    return hceSession->GetPaymentServices(abilityInfos);
}

KITS::ErrorCode HceService::StartHce(const ElementName &element, const std::vector<std::string> &aids)
{
    InfoLog("HceService::StartHce");
    int32_t res = ErrorCode::ERR_NONE;
    OHOS::sptr<HCE::IHceSession> hceSession = GetHceSessionProxy(res);
    if (res == ErrorCode::ERR_NO_PERMISSION) {
        ErrorLog("HceService::StartHce, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    if (hceSession == nullptr) {
        ErrorLog("HceService::StartHce, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    return hceSession->StartHce(element, aids);
}
OHOS::sptr<HCE::IHceSession> HceService::GetHceSessionProxy(int32_t &res)
{
    std::lock_guard<std::mutex> lock(g_hceSessionProxyLock);
    if (hceSessionProxy_ == nullptr) {
        OHOS::sptr<IRemoteObject> iface = NfcController::GetInstance().GetHceServiceIface(res);
        if (iface != nullptr) {
            hceSessionProxy_ = new HCE::HceSessionProxy(iface);
        }
    }
    return hceSessionProxy_;
}
void HceService::ClearHceSessionProxy()
{
    WarnLog("ClearHceSessionProxy");
    std::lock_guard<std::mutex> lock(g_hceSessionProxyLock);
    hceSessionProxy_ = nullptr;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS