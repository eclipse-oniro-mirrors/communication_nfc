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
#ifndef READER_MODE_DEATH_RECIPIENT_H
#define READER_MODE_DEATH_RECIPIENT_H
#include <unistd.h>
#include <ipc_object_stub.h>
#include "tag_session_stub.h"
#include "access_token.h"

namespace OHOS {
namespace NFC {
class ReaderModeDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit ReaderModeDeathRecipient(sptr<TAG::TagSessionStub> tagSession,
        Security::AccessToken::AccessTokenID callerToken);
    ~ReaderModeDeathRecipient() = default;
    void OnRemoteDied(const wptr<IRemoteObject> &remote) override;

private:
    sptr<TAG::TagSessionStub> tagSession_ = nullptr;
    Security::AccessToken::AccessTokenID callerToken_;
    std::mutex mutex_;
};
}  // namespace NFC
}  // namespace OHOS
#endif  // FOREGROUND_DEATH_RECIPIENT_H
