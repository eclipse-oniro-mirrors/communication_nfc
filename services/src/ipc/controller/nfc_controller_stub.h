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
#ifndef NFC_CONTROLLER_STUB_H
#define NFC_CONTROLLER_STUB_H

#include "indef_msg_callback.h"
#include "infc_controller_callback.h"
#include "infc_controller_service.h"
#ifdef VENDOR_APPLICATIONS_ENABLED
#include "iquery_app_info_callback.h"
#endif
#include "iremote_stub.h"
#include "nfc_controller_callback_proxy.h"
#include "nfc_sdk_common.h"
#include "message_parcel.h"
#include "access_token.h"

namespace OHOS {
namespace NFC {
class NfcControllerStub : public OHOS::IRemoteStub<OHOS::NFC::INfcControllerService> {
public:
    int OnRemoteRequest(uint32_t code,                /* [in] */
                        OHOS::MessageParcel& data,    /* [in] */
                        OHOS::MessageParcel& reply,   /* [out] */
                        OHOS::MessageOption& option) override; /* [in] */

    NfcControllerStub() {}
    virtual ~NfcControllerStub() {}
    virtual KITS::ErrorCode RegisterCallBack(const sptr<INfcControllerCallback> &callback,
        const std::string& type, Security::AccessToken::AccessTokenID callerToken) = 0;
    virtual KITS::ErrorCode UnRegisterCallBack(const std::string& type,
        Security::AccessToken::AccessTokenID callerToken) = 0;
    virtual KITS::ErrorCode UnRegisterAllCallBack(Security::AccessToken::AccessTokenID callerToken) = 0;
    virtual KITS::ErrorCode RegNdefMsgCallback(const sptr<INdefMsgCallback> &callback) = 0;
    void RemoveNfcDeathRecipient(const wptr<IRemoteObject> &remote);

private:
    int HandleGetState(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleTurnOn(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleTurnOff(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleIsNfcOpen(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleRegisterCallBack(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleUnRegisterCallBack(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleGetNfcTagInterface(MessageParcel& data, MessageParcel& reply);
    int HandleRegNdefMsgCb(MessageParcel& data, MessageParcel& reply);
#ifdef VENDOR_APPLICATIONS_ENABLED
    int HandleRegQueryApplicationCb(MessageParcel& data, MessageParcel& reply);
    int HandleRegCardEmulationNotifyCb(MessageParcel& data, MessageParcel& reply);
    int HandleNotifyEventStatus(MessageParcel& data, MessageParcel& reply);
#endif
    int HandleGetNfcHceInterface(MessageParcel &data, MessageParcel &reply);

private:
    KITS::ErrorCode RegisterCallBack(const sptr<INfcControllerCallback> &callback,
        const std::string& type) override;
    KITS::ErrorCode UnRegisterCallBack(const std::string& type) override;
    KITS::ErrorCode RegNdefMsgCb(const sptr<INdefMsgCallback> &callback) override;

private:
    std::mutex mutex_ {};
    sptr<INfcControllerCallback> callback_;
    sptr<INdefMsgCallback> ndefCallback_;
#ifdef VENDOR_APPLICATIONS_ENABLED
    sptr<IQueryAppInfoCallback> queryAppInfoCallback_ {nullptr};
    sptr<IOnCardEmulationNotifyCb> onCardEmulationNotifyCb_ {nullptr};
#endif
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ {nullptr};
};
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_CONTROLLER_STUB_H
