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

#ifndef FOREGROUND_CALLBACK_PROXY_H
#define FOREGROUND_CALLBACK_PROXY_H

#include "message_parcel.h"
#include "ihce_cmd_callback.h"
#include "iremote_proxy.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace HCE {
class HceCmdCallbackProxy : public IRemoteProxy<KITS::IHceCmdCallback> {
public:
    explicit HceCmdCallbackProxy(const sptr<IRemoteObject> &remote);

    virtual ~HceCmdCallbackProxy() {}

    void OnCeApduData(const std::vector<uint8_t> &apduData) override;

private:
    static inline BrokerDelegator<HceCmdCallbackProxy> g_delegator;
};
} // namespace HCE
} // namespace NFC
} // namespace OHOS
#endif