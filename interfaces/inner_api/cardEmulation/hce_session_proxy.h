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
#ifndef HCE_SESSION_PROXY_H
#define HCE_SESSION_PROXY_H

#include "element_name.h"
#include "iremote_proxy.h"
#include "ihce_session.h"
#include "nfc_basic_proxy.h"
#include "ihce_cmd_callback.h"
#include "ability_info.h"


namespace OHOS {
namespace NFC {
namespace HCE {
using OHOS::AppExecFwk::ElementName;
using AppExecFwk::AbilityInfo;
class HceSessionProxy final : public OHOS::IRemoteProxy<IHceSession>, public NfcBasicProxy {
public:
    explicit HceSessionProxy(const OHOS::sptr<OHOS::IRemoteObject> &remote)
        : OHOS::IRemoteProxy<IHceSession>(remote), NfcBasicProxy(remote)
    {
    }
    ~HceSessionProxy() override {}

    KITS::ErrorCode RegHceCmdCallback(const sptr<KITS::IHceCmdCallback> &callback, const std::string &type) override;

    int SendRawFrame(std::string hexCmdData, bool raw, std::string &hexRespData) override;
    int GetPaymentServices(std::vector<AbilityInfo> &abilityInfos) override;
    KITS::ErrorCode StopHce(ElementName &element) override;
    KITS::ErrorCode IsDefaultService(ElementName &element, const std::string &type, bool &isDefaultService) override;
    KITS::ErrorCode StartHce(const ElementName &element, const std::vector<std::string> &aids) override;
};
} // namespace HCE
} // namespace NFC
} // namespace OHOS
#endif
