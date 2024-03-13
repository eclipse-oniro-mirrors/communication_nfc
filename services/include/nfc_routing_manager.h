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
#ifndef NFC_ROUTING_MANAGER_H
#define NFC_ROUTING_MANAGER_H
#include "nfc_event_handler.h"
#include "inci_ce_interface.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
class NfcService;
class NfcRoutingManager {
public:
    NfcRoutingManager(std::shared_ptr<NfcEventHandler> eventHandler,
                      std::weak_ptr<NCI::INciCeInterface> nciCeProxy,
                      std::weak_ptr<NfcService> nfcService);
    ~NfcRoutingManager();

    // commit routing
    void HandleCommitRouting();
    void HandleComputeRoutingParams(int defaultPaymentType);
    void CommitRouting();
    void ComputeRoutingParams(KITS::DefaultPaymentType defaultPaymentType);

private:
    std::shared_ptr<NfcEventHandler> eventHandler_ {};
    std::weak_ptr<NCI::INciCeInterface> nciCeProxy_ {};
    std::weak_ptr<NfcService> nfcService_ {};

    // lock
    std::mutex mutex_ {};
};
} // namespace NFC
} // namespace OHOS
#endif // NFC_ROUTING_MANAGER_H