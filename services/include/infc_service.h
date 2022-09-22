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
#ifndef I_NFC_SERVICE_H
#define I_NFC_SERVICE_H

#include "infcc_host.h"
#include "tag_dispatcher.h"

namespace OHOS {
namespace NFC {
enum class NfcCommonEvent {
    // tags found
    MSG_TAG_FOUND = 0,
    MSG_TAG_DEBOUNCE,

    // screen changed
    MSG_SCREEN_CHANGED,

    // package updated
    MSG_PACKAGE_UPDATED,

    // card emulation
    MSG_ROUTE_AID,
};

enum class ScreenState {
    SCREEN_STATE_UNKNOWN = 0x00,
    SCREEN_STATE_OFF_UNLOCKED = 0x01,
    SCREEN_STATE_OFF_LOCKED = 0x02,
    SCREEN_STATE_ON_LOCKED = 0x04,
    SCREEN_STATE_ON_UNLOCKED = 0x08,
    // Polling mask
    SCREEN_POLLING_TAG_MASK = 0x10,
    SCREEN_POLLING_READER_MASK = 0x40
};

class INfcService {
public:
    virtual ~INfcService() {}

    virtual bool IsNfcEnabled() = 0;
    virtual int GetNfcState() = 0;
    virtual int GetScreenState() = 0;
    virtual int GetNciVersion() = 0;
    virtual std::weak_ptr<NFC::NCI::INfccHost> GetNfccHost() = 0;
    virtual std::weak_ptr<TAG::TagDispatcher> GetTagDispatcher() = 0;
    virtual OHOS::sptr<IRemoteObject> GetTagServiceIface() = 0;
    virtual void ExecuteStartPollingLoop() = 0;

private:
};
}  // namespace NFC
}  // namespace OHOS
#endif  // I_NFC_SERVICE_H
