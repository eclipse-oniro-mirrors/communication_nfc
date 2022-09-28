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
#ifndef I_NFC_CONTROLLER_SERVICE_H
#define I_NFC_CONTROLLER_SERVICE_H

#include "nfc_sdk_common.h"
#include "infc_controller_callback.h"
#include "iremote_broker.h"

namespace OHOS {
namespace NFC {
class INfcControllerService : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.INfcControllerService");

    virtual ~INfcControllerService() {}
    /**
     * @brief  Get the NFC state
     * @return The NFC State
     */
    virtual int GetState() = 0;
    /**
     * @brief Turn On NFC
     * @return true - turn on; the other
     */
    virtual bool TurnOn() = 0;
    /**
     * @brief Turn Off NFC
     * @return true - turn off; the other
     */
    virtual bool TurnOff() = 0;
     /**
     * @brief NFC enable
     * @return true - NFC enabled; the other
     */
    virtual bool IsNfcOpen() = 0;

    virtual KITS::NfcErrorCode RegisterCallBack(const sptr<INfcControllerCallback> &callback,
        const std::string& type) = 0;
    virtual KITS::NfcErrorCode UnRegisterCallBack(const std::string& type) = 0;
    virtual OHOS::sptr<IRemoteObject> GetTagServiceIface() = 0;
private:
};
}  // namespace NFC
}  // namespace OHOS
#endif  // I_NFC_CONTROLLER_SERVICE_H