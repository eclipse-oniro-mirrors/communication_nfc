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
#ifndef NFC_STATE_CHANGE_CALLBACK_H
#define NFC_STATE_CHANGE_CALLBACK_H

#include "data_ability_observer_stub.h"
#include "nfc_data_share_impl.h"
#include "infc_controller_callback.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class NfcStateChangeCallback : public AAFwk::DataAbilityObserverStub {
public:
    explicit NfcStateChangeCallback(sptr<INfcControllerCallback> callback) : callback_(callback)
    {}
    virtual ~NfcStateChangeCallback() {}
    void OnChange() override
    {
        if (callback_ == nullptr) {
            return;
        }
        int state = NfcState::STATE_OFF;
        Uri nfcEnableUri(NFC_DATA_URI);
        DelayedSingleton<NfcDataShareImpl>::GetInstance()->
            GetValue(nfcEnableUri, DATA_SHARE_KEY_STATE, state);
        callback_->OnNfcStateChanged(state);
    }

private:
    sptr<INfcControllerCallback> callback_;
};
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif // NFC_STATE_CHANGE_CALLBACK_H