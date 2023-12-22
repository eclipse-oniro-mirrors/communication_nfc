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
#include "reader_mode_death_recipient.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
ReaderModeDeathRecipient::ReaderModeDeathRecipient(
    sptr<TAG::TagSessionStub> tagSession, Security::AccessToken::AccessTokenID callerToken)
{
    tagSession_ = tagSession;
    callerToken_ = callerToken;
}

void ReaderModeDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
}
} // namespace NFC
} // namespace OHOS