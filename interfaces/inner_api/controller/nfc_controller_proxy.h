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


#include "nfc_basic_proxy.h"
#include "nfc_sdk_common.h"
#include "indef_msg_callback.h"
#include "infc_controller_callback.h"
#include "infc_controller_service.h"
#include "iremote_proxy.h"
#include "iremote_object.h"

namespace OHOS {
namespace NFC {
class NfcControllerProxy final : public OHOS::IRemoteProxy<INfcControllerService>, public NfcBasicProxy {
public:
    explicit NfcControllerProxy(const OHOS::sptr<OHOS::IRemoteObject>& remote)
        : OHOS::IRemoteProxy<INfcControllerService>(remote), NfcBasicProxy(remote)
    {
    }
    ~NfcControllerProxy() override;

    int TurnOn() override;
    int TurnOff() override;
    int GetState() override;
    int IsNfcOpen(bool &isOpen) override;
    KITS::ErrorCode RegisterCallBack(const sptr<INfcControllerCallback> &callback,
        const std::string& type) override;
    KITS::ErrorCode UnRegisterCallBack(const std::string& type) override;
    OHOS::sptr<IRemoteObject> GetTagServiceIface() override;
    OHOS::sptr<IRemoteObject> GetHceServiceIface() override;
    KITS::ErrorCode RegNdefMsgCb(const sptr<INdefMsgCallback> &callback) override;
#ifdef VENDOR_APPLICATIONS_ENABLED
    KITS::ErrorCode RegQueryApplicationCb(sptr<IQueryAppInfoCallback> callback) override;
    KITS::ErrorCode RegCardEmulationNotifyCb(sptr<IOnCardEmulationNotifyCb> callback) override;
    KITS::ErrorCode NotifyEventStatus(int eventType, int arg1, std::string arg2) override;
#endif

private:
};
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_CONTROLLER_PROXY_H
