/*
* Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef OHOS_NFC_CONTROLLER_CALLBACK_PROXY_H
#define OHOS_NFC_CONTROLLER_CALLBACK_PROXY_H

#include "message_parcel.h"
#include "infc_controller_callback.h"
#include "iremote_proxy.h"
#include "nfc_sdk_common.h"


namespace OHOS {
namespace NFC {
class NfcControllerCallBackProxy : public IRemoteProxy<INfcControllerCallback> {
public:
    explicit NfcControllerCallBackProxy(const sptr<IRemoteObject> &remote);

    virtual ~NfcControllerCallBackProxy() {}

    void OnNfcStateChanged(int nfcRfState) override;
    
private:
    static inline BrokerDelegator<NfcControllerCallBackProxy> g_delegator;
};
}  // namespace NFC
}  // namespace OHOS
#endif