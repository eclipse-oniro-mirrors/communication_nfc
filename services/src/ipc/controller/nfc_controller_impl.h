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
#ifndef NFC_CONTROLLER_IMPL_H
#define NFC_CONTROLLER_IMPL_H

#include "access_token.h"
#include "infc_controller_callback.h"
#include "nfc_controller_stub.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
class NfcService;
class NfcControllerImpl final : public NfcControllerStub {
public:
    explicit NfcControllerImpl(std::weak_ptr<NfcService> nfcService);
    ~NfcControllerImpl() override;

    int32_t CallbackEnter(uint32_t code) override;
    int32_t CallbackExit(uint32_t code, int32_t result) override;

    ErrCode GetState(int32_t& funcResult) override;
    ErrCode TurnOn() override;
    ErrCode TurnOff() override;

    ErrCode RegisterNfcStatusCallBack(const sptr<INfcControllerCallback>& cb, const std::string& type) override;

    ErrCode UnregisterNfcStatusCallBack(const std::string& type) override;

    KITS::ErrorCode UnRegisterAllCallBack(Security::AccessToken::AccessTokenID callerToken);
    ErrCode GetTagServiceIface(sptr<IRemoteObject>& funcResult) override;
    ErrCode GetHceServiceIface(sptr<IRemoteObject>& funcResult) override;
    ErrCode RegNdefMsgCb(const sptr<INdefMsgCallback>& cb) override;

    ErrCode RegQueryApplicationCb(const sptr<IQueryAppInfoCallback>& cb) override;
    ErrCode RegCardEmulationNotifyCb(const sptr<IOnCardEmulationNotifyCb>& cb) override;
    ErrCode NotifyEventStatus(int32_t eventType, int32_t arg1, const std::string& arg2) override;

    void RemoveNfcDeathRecipient(const wptr<IRemoteObject> &remote);

private:
    std::weak_ptr<NfcService> nfcService_ = {};
    std::mutex mutex_ {};
    sptr<INfcControllerCallback> callback_ = nullptr;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
};
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_CONTROLLER_IMPL_H
