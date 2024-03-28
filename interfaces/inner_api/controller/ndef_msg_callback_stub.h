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

#ifndef OHOS_NDEF_MSG_CALLBACK_STUB_H
#define OHOS_NDEF_MSG_CALLBACK_STUB_H

#include <shared_mutex>

#include "indef_msg_callback.h"
#include "iremote_object.h"
#include "iremote_stub.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
class NdefMsgCallbackStub : public IRemoteStub<INdefMsgCallback> {
public:
    NdefMsgCallbackStub();
    virtual ~NdefMsgCallbackStub();
    static NdefMsgCallbackStub& GetInstance();
    KITS::ErrorCode RegisterCallback(const sptr<INdefMsgCallback> &callback);

    int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    bool OnNdefMsgDiscovered(const std::string &tagUid, const std::string &ndef, int ndefMsgType) override;
    int RemoteNdefMsgDiscovered(MessageParcel &data, MessageParcel &reply);
    sptr<INdefMsgCallback> callback_;
    std::shared_mutex mutex_;
    bool isRemoteDied_;
};
}  // namespace NFC
}  // namespace OHOS
#endif  // OHOS_NDEF_MSG_CALLBACK_STUB_H