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

#include "hce_service.h"

#include "ce_payment_services_parcelable.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "ihce_session.h"
#include "hce_cmd_callback_stub.h"
#include "nfc_controller.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "hce_session_proxy.h"
#include "nfc_sa_client.h"

namespace OHOS {
namespace NFC {
namespace KITS {
std::mutex g_hceSessionProxyLock;
static sptr<HCE::HceCmdCallbackStub> g_hceCmdCallbackStub =
    sptr<HCE::HceCmdCallbackStub>(new HCE::HceCmdCallbackStub);

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
    if (!NfcSaClient::GetInstance().CheckNfcSystemAbility()) {
        WarnLog("Nfc SA not started yet."); // callback will be registered on NFC SA starts
        return ErrorCode::ERR_NONE;
    }
    int32_t res = ErrorCode::ERR_NONE;
    OHOS::sptr<IHceSession> hceSession = GetHceSessionProxy(res);
    if (res == ErrorCode::ERR_NO_PERMISSION) {
        ErrorLog("HceService::RegHceCmdCallback, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    if (hceSession == nullptr || hceSession->AsObject() == nullptr) {
        ErrorLog("HceService::RegHceCmdCallback, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    if (g_hceCmdCallbackStub == nullptr) {
        ErrorLog("g_hceCmdCallbackStub is nullptr");
        return KITS::ERR_HCE_PARAMETERS;
    }
    g_hceCmdCallbackStub->RegHceCmdCallback(callback);
    hceSession->RegHceCmdCallback(g_hceCmdCallbackStub, type);
    return ErrorCode::ERR_NONE;
}

ErrorCode HceService::UnRegHceCmdCallback(const sptr<IHceCmdCallback> &callback, const std::string &type)
{
    InfoLog("HceService::UnRegHceCmdCallback");
    if (!NfcSaClient::GetInstance().CheckNfcSystemAbility()) {
        InfoLog("Nfc SA not started yet."); // no need to invoke into NFC service when NFC SA not start
        return ErrorCode::ERR_NONE;
    }
    int32_t res = ErrorCode::ERR_NONE;
    OHOS::sptr<IHceSession> hceSession = GetHceSessionProxy(res);
    if (res == ErrorCode::ERR_NO_PERMISSION) {
        ErrorLog("HceService::UnRegHceCmdCallback, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    if (hceSession == nullptr || hceSession->AsObject() == nullptr) {
        ErrorLog("HceService::UnRegHceCmdCallback, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    if (g_hceCmdCallbackStub == nullptr) {
        ErrorLog("g_hceCmdCallbackStub is nullptr");
        return KITS::ERR_HCE_PARAMETERS;
    }
    g_hceCmdCallbackStub->UnRegHceCmdCallback(callback);
    hceSession->UnregHceCmdCallback(g_hceCmdCallbackStub, type);
    return ErrorCode::ERR_NONE;
}

ErrorCode HceService::StopHce(ElementName &element)
{
    InfoLog("HceService::StopHce");
    if (!NfcSaClient::GetInstance().CheckNfcSystemAbility()) {
        InfoLog("Nfc SA not started yet."); // no need to invoke into NFC service when NFC SA not start
        return ErrorCode::ERR_NONE;
    }
    int32_t res = ErrorCode::ERR_NONE;
    OHOS::sptr<IHceSession> hceSession = GetHceSessionProxy(res);
    if (res == ErrorCode::ERR_NO_PERMISSION) {
        ErrorLog("HceService::StopHce, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    if (hceSession == nullptr || hceSession->AsObject() == nullptr) {
        ErrorLog("HceService::StopHce, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    ErrorCode ret = static_cast<ErrorCode>(hceSession->StopHce(element));
    if (ret != ErrorCode::ERR_NO_PERMISSION) {
        ret = ErrorCode::ERR_NONE;
    }
    return ret;
}

ErrorCode HceService::IsDefaultService(ElementName &element, const std::string &type, bool &isDefaultService)
{
    InfoLog("HceService::IsDefaultService");
    int32_t res = ErrorCode::ERR_NONE;
    OHOS::sptr<IHceSession> hceSession = GetHceSessionProxy(res);
    if (res == ErrorCode::ERR_NO_PERMISSION) {
        ErrorLog("HceService::IsDefaultService, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    if (hceSession == nullptr || hceSession->AsObject() == nullptr) {
        ErrorLog("HceService::IsDefaultService, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    return static_cast<ErrorCode>(hceSession->IsDefaultService(element, type, isDefaultService));
}

int HceService::SendRawFrame(std::string hexCmdData, bool raw, std::string &hexRespData)
{
    InfoLog("HceService::SendRawFrame");
    int32_t res = ErrorCode::ERR_NONE;
    OHOS::sptr<IHceSession> hceSession = GetHceSessionProxy(res);
    if (res == ErrorCode::ERR_NO_PERMISSION) {
        ErrorLog("HceService::SendRawFrame, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    if (hceSession == nullptr || hceSession->AsObject() == nullptr) {
        ErrorLog("HceService::SendRawFrame, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    res = static_cast<int>(hceSession->SendRawFrame(hexCmdData, raw, hexRespData));
    if (res != ErrorCode::ERR_NO_PERMISSION) {
        res = ErrorCode::ERR_NONE;
    }
    return res;
}

int HceService::GetPaymentServices(std::vector<AbilityInfo> &abilityInfos)
{
    InfoLog("HceService::GetPaymentServices");
    int32_t res = ErrorCode::ERR_NONE;
    OHOS::sptr<IHceSession> hceSession = GetHceSessionProxy(res);
    if (res == ErrorCode::ERR_NO_PERMISSION) {
        ErrorLog("HceService::GetPaymentServices, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    if (hceSession == nullptr || hceSession->AsObject() == nullptr) {
        ErrorLog("HceService::GetPaymentServices, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }

    KITS::CePaymentServicesParcelable paymentServices;
    ErrCode errCode = hceSession->GetPaymentServices(paymentServices);

    std::vector<AbilityInfo> paymentAbilityInfos = paymentServices.paymentAbilityInfos;
    InfoLog("size %{public}zu", paymentAbilityInfos.size());
    abilityInfos = std::move(paymentAbilityInfos);
    return static_cast<int>(errCode);
}

KITS::ErrorCode HceService::StartHce(const ElementName &element, const std::vector<std::string> &aids)
{
    InfoLog("HceService::StartHce");
    if (!NfcSaClient::GetInstance().CheckNfcSystemAbility()) {
        WarnLog("Nfc SA not started yet.");
        return ErrorCode::ERR_NONE;
    }
    int32_t res = ErrorCode::ERR_NONE;
    OHOS::sptr<IHceSession> hceSession = GetHceSessionProxy(res);
    if (res == ErrorCode::ERR_NO_PERMISSION) {
        ErrorLog("HceService::StartHce, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    if (hceSession == nullptr || hceSession->AsObject() == nullptr) {
        ErrorLog("HceService::StartHce, ERR_HCE_STATE_UNBIND");
        return ErrorCode::ERR_HCE_STATE_UNBIND;
    }
    ErrorCode ret = static_cast<ErrorCode>(hceSession->StartHce(element, aids));
    if (ret != ErrorCode::ERR_NO_PERMISSION) {
        ret = ErrorCode::ERR_NONE;
    }
    return ret;
}

OHOS::sptr<IHceSession> HceService::GetHceSessionProxy(int32_t &res)
{
    return iface_cast<IHceSession>(NfcController::GetInstance().GetHceServiceIface(res));
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS