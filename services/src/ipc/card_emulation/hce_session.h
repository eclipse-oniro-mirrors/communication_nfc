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
#ifndef HCE_SESSION_H
#define HCE_SESSION_H

#include "access_token.h"
#include "element_name.h"
#include "infc_service.h"
#include "ihce_session.h"
#include "hce_session_stub.h"
#include "host_card_emulation_manager.h"
#include "ability_info.h"

namespace OHOS {
namespace NFC {
namespace HCE {
using AppExecFwk::AbilityInfo;
using OHOS::AppExecFwk::ElementName;
class HceSession final : public HceSessionStub {
public:
    // Constructor/Destructor
    explicit HceSession(std::shared_ptr<NFC::INfcService> service);
    ~HceSession() override;
    
    HceSession(const HceSession &) = delete;
    HceSession &operator=(const HceSession &) = delete;

    int32_t CallbackEnter(uint32_t code) override;
    int32_t CallbackExit(uint32_t code, int32_t result) override;

    ErrCode RegHceCmdCallback(const sptr<IHceCmdCallback>& cb, const std::string& type) override;

    ErrCode UnregHceCmdCallback(const sptr<IHceCmdCallback>& cb, const std::string& type) override;

    KITS::ErrorCode UnRegAllCallback(Security::AccessToken::AccessTokenID callerToken);
    KITS::ErrorCode HandleWhenRemoteDie(Security::AccessToken::AccessTokenID callerToken);

    ErrCode SendRawFrame(const std::string& hexCmdData, bool raw, std::string& hexRespData) override;

    ErrCode GetPaymentServices(CePaymentServicesParcelable& parcelable) override;

    ErrCode IsDefaultService(const ElementName& element, const std::string& type, bool& isDefaultService) override;

    ErrCode StartHce(const ElementName& element, const std::vector<std::string>& aids) override;

    ErrCode StopHce(const ElementName& element) override;

    void RemoveHceDeathRecipient(const wptr<IRemoteObject>& remote);

private:
#ifdef NFC_SIM_FEATURE
    void AppendSimBundle(std::vector<AbilityInfo> &paymentAbilityInfos);
#endif

    std::weak_ptr<NFC::INfcService> nfcService_{};
    std::weak_ptr<CeService> ceService_{};
    std::mutex mutex_{};
    sptr<KITS::IHceCmdCallback> hceCmdCallback_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
};
} // namespace HCE
} // namespace NFC
} // namespace OHOS
#endif
