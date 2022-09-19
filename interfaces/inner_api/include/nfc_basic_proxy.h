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
#ifndef NFC_BASIC_PROXY_H
#define NFC_BASIC_PROXY_H

#include "iremote_object.h"
#include "message_parcel.h"
#include "message_option.h"

namespace OHOS {
namespace NFC {
class NfcBasicProxy {
public:
    explicit NfcBasicProxy(const OHOS::sptr<OHOS::IRemoteObject>& obj) : remoteObj_(obj) {}
    virtual ~NfcBasicProxy() {}

    int SendRequestExpectReplyInt(int cmd, OHOS::MessageParcel& data, OHOS::MessageOption& option, int& result);
    int SendRequestExpectReplyBool(int cmd, OHOS::MessageParcel& data, OHOS::MessageOption& option, bool& result);
    int SendRequestExpectReplyNone(int cmd, OHOS::MessageParcel& data, OHOS::MessageOption& option);
private:
    OHOS::sptr<OHOS::IRemoteObject> remoteObj_ {};
};
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_BASIC_PROXY_H
