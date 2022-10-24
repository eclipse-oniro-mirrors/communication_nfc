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

#include "nfc_controller_death_recipient.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
NfcControllerDeathRecipient::NfcControllerDeathRecipient(
    sptr<NfcControllerStub> nfcConctrolService, Security::AccessToken::AccessTokenID callerToken)
{
    InfoLog("NfcControllerDeathRecipient, ##callerToken=%{public}d\n", callerToken);
    nfcConctrolService_ = nfcConctrolService;
    callerToken_ = callerToken;
}

void NfcControllerDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    ErrorLog("NfcControllerDeathRecipient::OnRemoteDied.");
    if (nfcConctrolService_ == nullptr) {
        ErrorLog("NfcControllerDeathRecipient nfcConctrolService_ is nullptr!");
        return;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    InfoLog("OnRemoteDied, ##callerToken=%{public}d\n", callerToken_);
    KITS::ErrorCode ret = nfcConctrolService_->UnRegisterAllCallBack(callerToken_);
    InfoLog("OnRemoteDied, UnRegisterAllCallBack##ret=%{public}d\n", ret);
    nfcConctrolService_->RemoveNfcDeathRecipient(remote);
}
} // namespace NFC
} // namespace OHOS