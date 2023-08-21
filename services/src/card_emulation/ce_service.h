/*
 * Copyright (C) 2023-2023 Huawei Device Co., Ltd.
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

#ifndef CE_SERVICE_H
#define CE_SERVICE_H

#include "nfc_service.h"

namespace OHOS {
namespace NFC {

class NfcService;
class CommonEventHandler;

class CeService {
public:
    explicit CeService(std::weak_ptr<NfcService> nfcService);
    ~CeService();

    void HandleFieldActivated();
    void HandleFieldDeactivated();
    static void PublishFieldOnOrOffCommonEvent(bool isFieldOn);

private:
    bool IsWalletProcessExist();
    void NotifyWalletFieldEvent(std::string event);

private:
    uint64_t lastFieldOnTime_ = 0;
    uint64_t lastFieldOffTime_ = 0;

    std::weak_ptr<NfcService> nfcService_ {};

    friend class NfcService;
};
} // NFC
} // OHOS
#endif