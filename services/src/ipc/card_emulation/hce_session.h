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
#ifndef HCE_SESSION_H
#define HCE_SESSION_H

#include "element_name.h"
#include "infc_service.h"
#include "ihce_session.h"
#include "hce_session_stub.h"
#include "host_card_emulation_manager.h"

namespace OHOS {
namespace NFC {
namespace HCE {
using OHOS::AppExecFwk::ElementName;
class HceSession final : public HceSessionStub {
public:
    // Constructor/Destructor
    explicit HceSession(std::shared_ptr<NFC::INfcService> service);
    ~HceSession() override;
    HceSession(const HceSession &) = delete;
    HceSession &operator=(const HceSession &) = delete;

    KITS::ErrorCode RegHceCmdCallback(
        const sptr<KITS::IHceCmdCallback> &callback,
        const std::string &type) override;

    int SendRawFrame(std::string hexCmdData, bool raw,
                     std::string &hexRespData) override;

    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;

private:
    std::string GetDumpInfo();
    std::weak_ptr<NFC::INfcService> nfcService_{};
    std::weak_ptr<CeService> ceService_{};
};
} // namespace HCE
} // namespace NFC
} // namespace OHOS
#endif
