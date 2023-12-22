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
#ifndef NFC_ABILITY_CONNECTION_CALLBACK_H
#define NFC_ABILITY_CONNECTION_CALLBACK_H

#include "ability_connect_callback_interface.h"
#include "ability_connect_callback_stub.h"
#include "host_card_emulation_manager.h"

namespace OHOS {
namespace NFC {
class HostCardEmulationManager;
class NfcAbilityConnectionCallback : public AAFwk::AbilityConnectionStub {
public:
    NfcAbilityConnectionCallback();
    virtual ~NfcAbilityConnectionCallback();

    void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
                              const sptr<IRemoteObject> &remoteObject,
                              int resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element,
                                 int resultCode) override;
    bool ServiceConnected();
    void SetHceManager(std::weak_ptr<HostCardEmulationManager> hceManager);
    AppExecFwk::ElementName GetConnectedElement();

private:
    bool serviceConnected_;
    std::weak_ptr<HostCardEmulationManager> hceManager_{};
    AppExecFwk::ElementName connectedElement_;
};
} // namespace NFC
} // namespace OHOS
#endif
