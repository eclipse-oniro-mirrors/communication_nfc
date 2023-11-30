/*
 * Copyright (C) 2022 - 2023 Huawei Device Co., Ltd.
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
#ifndef I_HCE_SESSION_H
#define I_HCE_SESSION_H

#include "element_name.h"
#include "ihce_cmd_callback.h"
#include "iremote_broker.h"
#include "nfc_sdk_common.h"
#include "parcel.h"

namespace OHOS {
namespace NFC {
namespace HCE {
class IHceSession : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.cardemulation.IHceSession");

    virtual ~IHceSession() {}

    virtual KITS::ErrorCode RegHceCmdCallback(
        const sptr<KITS::IHceCmdCallback> &callback,
        const std::string &type) = 0;

    virtual int SendRawFrame(std::string hexCmdData, bool raw,
                             std::string &hexRespData) = 0;

private:
};
} // namespace HCE
} // namespace NFC
} // namespace OHOS
#endif
