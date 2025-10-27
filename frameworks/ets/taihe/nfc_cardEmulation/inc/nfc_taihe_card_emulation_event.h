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

#ifndef NFC_TAIHE_CARD_EMULATION_EVENT
#define NFC_TAIHE_CARD_EMULATION_EVENT

#include <string>

#include "ihce_cmd_callback.h"
#include "nfc_sdk_common.h"
#include "nfc_sa_client.h"
#include "system_ability_status_change_stub.h"

#include "ohos.nfc.cardEmulation.nfcCardEmulation.proj.hpp"
#include "ohos.nfc.cardEmulation.nfcCardEmulation.impl.hpp"
#include "taihe/runtime.hpp"

namespace OHOS {
namespace NFC {
namespace KITS {
class HceCmdListenerEvent : public IHceCmdCallback {
public:
    HceCmdListenerEvent() {}
    virtual ~HceCmdListenerEvent() {}

public:
    void OnCeApduData(const std::vector<uint8_t>& data) override;
    OHOS::sptr<OHOS::IRemoteObject> AsObject() override;
};

class NfcTaiheHceSAStatusChange : public SystemAbilityStatusChangeStub {
public:
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void Init(int32_t systemAbilityId);
};

class NfcHceEventRegister {
public:
    static NfcHceEventRegister& GetInstance();

    void Register(
        std::string type, ::taihecallback_view<void>(uintptr_t err, ::taihe::array_view<uint8_t> data)> callback);
    void Unregister(taihe::string_view type);

private:
    NfcHceEventRegister() {}
    ~NfcHceEventRegister() {}

    std::shared_ptr<NfcTaiheHceSAStatusChange> saStatusListener_;
};

}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif // #define NFC_TAIHE_CARD_EMULATION_EVENT
