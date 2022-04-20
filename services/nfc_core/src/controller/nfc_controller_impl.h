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
#ifndef NFC_CONTROLLER_IMPL_H
#define NFC_CONTROLLER_IMPL_H

#include "nfc_controller_stub.h"
#include "nfc_service.h"

namespace OHOS {
namespace NFC {
class NfcService;
class NfcControllerImpl final : public NfcControllerStub {
public:
    NfcControllerImpl(std::weak_ptr<NfcService> nfcService);
    ~NfcControllerImpl() override;

    int GetState() override;
    bool TurnOn() override;
    bool TurnOff(bool saveState) override;

private:
    std::weak_ptr<NfcService> nfcService_;
    std::mutex mutex_ {};
};
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_CONTROLLER_IMPL_H
