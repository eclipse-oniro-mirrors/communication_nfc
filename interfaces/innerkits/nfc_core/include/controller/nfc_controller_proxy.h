/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#ifndef NFC_CONTROLLER_PROXY_H
#define NFC_CONTROLLER_PROXY_H

#include "iremote_proxy.h"
#include "infc_controller_service.h"
#include "nfc_basic_proxy.h"

namespace OHOS {
namespace NFC {
class NfcControllerProxy final : public OHOS::IRemoteProxy<INfcControllerService>, public NfcBasicProxy {
public:
    explicit NfcControllerProxy(const OHOS::sptr<OHOS::IRemoteObject>& remote)
        : OHOS::IRemoteProxy<INfcControllerService>(remote), NfcBasicProxy(remote)
    {
    }
    ~NfcControllerProxy() override;

    bool TurnOn() override;
    bool TurnOff(bool saveState) override;
    int GetState() override;

private:
    static constexpr int COMMAND_ID = 100;
    static constexpr int COMMAND_GET_STATE = COMMAND_ID + 1;
    static constexpr int COMMAND_TURN_ON = COMMAND_ID + 2;
    static constexpr int COMMAND_TURN_OFF = COMMAND_ID + 3;
};
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_CONTROLLER_PROXY_H
