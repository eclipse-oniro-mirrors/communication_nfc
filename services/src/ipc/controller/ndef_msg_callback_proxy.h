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

#ifndef OHOS_NDEF_MSG_CALLBACK_PROXY_H
#define OHOS_NDEF_MSG_CALLBACK_PROXY_H

#include "indef_msg_callback.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace NFC {
class NdefMsgCallbackProxy : public IRemoteProxy<INdefMsgCallback> {
public:
    explicit NdefMsgCallbackProxy(const sptr<IRemoteObject> &remote);

    virtual ~NdefMsgCallbackProxy() {}

    bool OnNdefMsgDiscovered(const std::string &tagUid, const std::string &ndef, int ndefMsgType) override;

private:
    static inline BrokerDelegator<NdefMsgCallbackProxy> g_delegator;
};
}  // namespace NFC
}  // namespace OHOS
#endif // OHOS_NDEF_MSG_CALLBACK_PROXY_H