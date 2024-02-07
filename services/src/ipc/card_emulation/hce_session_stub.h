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
#ifndef HCE_SESSION_STUB_H
#define HCE_SESSION_STUB_H

#include "access_token.h"
#include "ihce_cmd_callback.h"
#include "iremote_stub.h"
#include "ihce_session.h"
#include "message_parcel.h"
#include "start_hce_info_parcelable.h"

namespace OHOS {
namespace NFC {
namespace HCE {
class HceSessionStub : public OHOS::IRemoteStub<OHOS::NFC::HCE::IHceSession> {
public:
    int OnRemoteRequest(uint32_t code,                         /* [in] */
                        OHOS::MessageParcel& data,             /* [in] */
                        OHOS::MessageParcel& reply,            /* [out] */
                        OHOS::MessageOption& option) override; /* [in] */
    HceSessionStub() {}
    virtual ~HceSessionStub() {}

    virtual KITS::ErrorCode RegHceCmdCallbackByToken(const sptr<KITS::IHceCmdCallback>& callback,
                                                     const std::string& type,
                                                     Security::AccessToken::AccessTokenID callerToken) = 0;

    virtual KITS::ErrorCode UnRegHceCmdCallback(const std::string& type,
                                                Security::AccessToken::AccessTokenID callerToken) = 0;
    virtual KITS::ErrorCode UnRegAllCallback(Security::AccessToken::AccessTokenID callerToken) = 0;
    virtual KITS::ErrorCode HandleWhenRemoteDie(Security::AccessToken::AccessTokenID callerToken) = 0;

    virtual KITS::ErrorCode StopHce(const ElementName& element,
                                    Security::AccessToken::AccessTokenID callerToken) = 0;

    virtual int SendRawFrameByToken(std::string hexCmdData, bool raw, std::string& hexRespData,
                                    Security::AccessToken::AccessTokenID callerToken) = 0;

    void RemoveHceDeathRecipient(const wptr<IRemoteObject>& remote);

private:
    int HandleRegHceCmdCallback(MessageParcel& data, MessageParcel& reply);

    int HandleSendRawFrame(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);

    int HandleGetPaymentServices(MessageParcel& data, MessageParcel& reply);

    int HandleStopHce(MessageParcel& data, MessageParcel& reply);

    int HandleStartHce(MessageParcel& data, MessageParcel& reply);

    KITS::ErrorCode StartHceInner(std::shared_ptr<KITS::StartHceInfoParcelable> startHceInfo);

    int HandleIsDefaultService(MessageParcel& data, MessageParcel& reply);

    KITS::ErrorCode RegHceCmdCallback(const sptr<KITS::IHceCmdCallback>& callback,
                                      const std::string& type) override;

    int SendRawFrame(std::string hexCmdData, bool raw, std::string& hexRespData) override;
    KITS::ErrorCode StopHce(ElementName &element) override;

private:
    std::mutex mutex_{};
    sptr<KITS::IHceCmdCallback> hceCmdCallback_;
    sptr<IRemoteObject::DeathRecipient> deathRecipient_{nullptr};
};
} // namespace HCE
} // namespace NFC
} // namespace OHOS
#endif // HCE_SESSION_STUB_H
