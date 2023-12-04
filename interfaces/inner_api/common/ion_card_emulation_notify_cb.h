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
#ifndef OHOS_I_ON_CARD_EMULATION_NOTIFY_CB_H
#define OHOS_I_ON_CARD_EMULATION_NOTIFY_CB_H

#include <iremote_broker.h>

namespace OHOS {
namespace NFC {
using OnCardEmulationNotifyCb = void (*)(uint32_t, std::string);
class IOnCardEmulationNotifyCb : public IRemoteBroker {
public:
    virtual bool OnCardEmulationNotify(uint32_t eventType, std::string apduData) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.IOnCardEmulationNotifyCb");
};
}  // namespace NFC
}  // namespace OHOS
#endif  // OHOS_I_ON_CARD_EMULATION_NOTIFY_CB_H
