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
#ifndef NFC_SA_CLIENT_H
#define NFC_SA_CLIENT_H

#include "iremote_object.h"
#include "nfc_sdk_common.h"
#include "nfc_controller.h"
#include "system_ability_load_callback_stub.h"

namespace OHOS {
namespace NFC {
class NfcSaLoadCallback : public SystemAbilityLoadCallbackStub {
public:
    void OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr<IRemoteObject> &remoteObject) override;
    void OnLoadSystemAbilityFail(int32_t systemAbilityId) override;
};

class NfcSaClient : public SystemAbilityLoadCallbackStub {
private:
    NfcSaClient() = default;
    ~NfcSaClient() = default;

public:
    static NfcSaClient &GetInstance();
    sptr<IRemoteObject> LoadNfcSa(int32_t systemAbilityId);
    void LoadSystemAbilitySuccess(const sptr<IRemoteObject> &remoteObject);
    void LoadSystemAbilityFail();

private:
    void InitLoadState();
    bool WaitLoadStateChange(int32_t systemAbilityId);

    std::condition_variable locatorCond_;
    std::mutex locatorMutex_;
    bool loadState_ = false;
    sptr<IRemoteObject> remoteObject_;
};
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_SA_CLIENT_H