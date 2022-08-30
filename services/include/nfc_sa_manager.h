/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef NFC_SA_MANAGER_H
#define NFC_SA_MANAGER_H

#include "common_event_handler.h"
#include "iservice_registry.h"
#include "nfc_controller_impl.h"
#include "nfc_service.h"
#include "singleton.h"
#include "system_ability.h"

namespace OHOS {
namespace NFC {
enum class ServiceRunningState {
    STATE_NOT_START,
    STATE_RUNNING
};

class NfcSaManager : public SystemAbility {
    DECLARE_DELAYED_SINGLETON(NfcSaManager)
    DECLARE_SYSTEM_ABILITY(NfcSaManager); // necessary
public:
    DISALLOW_COPY_AND_MOVE(NfcSaManager);

    /* Nfc open or close operations */
    void OnStart() override;
    void OnStop() override;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;

    /* query service state */
    ServiceRunningState QueryServiceState() const
    {
        return state_;
    }

private:
    bool Init();
    bool registerToService_ = false;
    ServiceRunningState state_ = ServiceRunningState::STATE_NOT_START;

    std::shared_ptr<NfcService> nfcService_;
    sptr<NfcControllerImpl> nfcControllerImpl_;
};
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_SA_MANAGER_H
