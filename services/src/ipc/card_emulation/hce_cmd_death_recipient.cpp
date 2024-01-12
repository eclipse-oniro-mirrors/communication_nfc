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

#include "hce_cmd_death_recipient.h"

#include "loghelper.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
HceCmdDeathRecipient::HceCmdDeathRecipient(
    sptr<HCE::HceSessionStub> hceSession,
    Security::AccessToken::AccessTokenID callerToken)
    : hceSession_(hceSession), callerToken_(callerToken) {}
void HceCmdDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    if (hceSession_ == nullptr) {
        ErrorLog("HceCmdDeathRecipient hceSession_ is nullptr!");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    KITS::ErrorCode ret = hceSession_->UnRegAllCallback(callerToken_);
    InfoLog("OnRemoteDied, UnRegAllCallback ret=%{public}d", ret);
    hceSession_->RemoveHceDeathRecipient(remote);
}
} // namespace NFC
// namespace NFC
} // namespace OHOS