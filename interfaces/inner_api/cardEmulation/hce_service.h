/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef HCE_SERVICE_H
#define HCE_SERVICE_H

#include "nfc_sdk_common.h"
#include "ihce_cmd_callback.h"
#include "ihce_session.h"
#include "ability_info.h"

namespace OHOS {
namespace NFC {
namespace KITS {
using OHOS::AppExecFwk::ElementName;
using AppExecFwk::AbilityInfo;
class HceService final {
public:
    explicit HceService();
    ~HceService();

    static HceService &GetInstance();

    ErrorCode RegHceCmdCallback(const sptr<KITS::IHceCmdCallback> &callback, const std::string &type);
    ErrorCode StopHce(ElementName &element);
    ErrorCode IsDefaultService(ElementName &element, const std::string &type, bool &isDefaultService);
    int SendRawFrame(std::string hexCmdData, bool raw, std::string &hexRespData);
    int GetPaymentServices(std::vector<AbilityInfo> &abilityInfos);
    KITS::ErrorCode StartHce(const ElementName &element, const std::vector<std::string> &aids);

protected:
    OHOS::sptr<HCE::IHceSession> GetHceSessionProxy();

private:
    OHOS::sptr<HCE::IHceSession> hceSessionProxy_;
};
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif