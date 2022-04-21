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

#include "infc_controller_service.h"
#include "iremote_stub.h"
#include "message_parcel.h"

namespace OHOS {
namespace NFC {
class NfcControllerStub : public OHOS::IRemoteStub<OHOS::NFC::INfcControllerService> {
public:
    int OnRemoteRequest(uint32_t code,                /* [in] */
                        OHOS::MessageParcel& data,    /* [in] */
                        OHOS::MessageParcel& reply,   /* [out] */
                        OHOS::MessageOption& option); /* [in] */

    NfcControllerStub() {}
    virtual ~NfcControllerStub() {}

private:
    int HandleGetState(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleTurnOn(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleTurnOff(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);

private:
};
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_CONTROLLER_STUB_H
