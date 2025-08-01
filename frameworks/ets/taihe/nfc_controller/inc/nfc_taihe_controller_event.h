/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef NFC_TAIHE_CONTROLLER_EVENT
#define NFC_TAIHE_CONTROLLER_EVENT

#include "infc_controller_callback.h"
#include "nfc_sdk_common.h"
#include "nfc_sa_client.h"
#include "system_ability_status_change_stub.h"

#include "ohos.nfc.controller.nfcController.proj.hpp"
#include "ohos.nfc.controller.nfcController.impl.hpp"
#include "taihe/runtime.hpp"

namespace OHOS {
namespace NFC {
namespace KITS {
class NfcStateListenerEvent : public INfcControllerCallback {
public:
    NfcStateListenerEvent() {}
    ~NfcStateListenerEvent() {}

public:
    void OnNfcStateChanged(int nfcState) override;
    OHOS::sptr<OHOS::IRemoteObject> AsObject() override;
};

class NfcTaiheSAStatusChange : public SystemAbilityStatusChangeStub {
public:
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void Init(int32_t systemAbilityId);
};

class NfcStateEventRegister {
public:
    static NfcStateEventRegister& GetInstance();

    void Register(taihe::callback_view<void(ohos::nfc::controller::nfcController::NfcState)> callback);
    void Unregister();

private:
    NfcStateEventRegister()
    {
        saStatusListener_ = std::make_shared<NfcTaiheSAStatusChange>();
        saStatusListener_->Init(NFC_MANAGER_SYS_ABILITY_ID);
    }

    ~NfcStateEventRegister()
    {
        saStatusListener_ = nullptr;
    }

    std::shared_ptr<NfcTaiheSAStatusChange> saStatusListener_;
};

}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif // #define NFC_TAIHE_CONTROLLER_EVENT
