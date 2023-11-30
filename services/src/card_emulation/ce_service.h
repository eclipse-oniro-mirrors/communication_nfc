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
#include "host_card_emulation_manager.h"
#include "inci_ce_interface.h"
#include "ihce_cmd_callback.h"
#include "app_data_parser.h"
#include "common_event_manager.h"

namespace OHOS {
namespace NFC {
class NfcService;
class NfcEventHandler;
class HostCardEmulationManager;
class CeService {
public:
    struct AidEntry {
        std::string aid;
        int route;
        int aidInfo;
        int power;
    };

    explicit CeService(std::weak_ptr<NfcService> nfcService,
                       std::weak_ptr<NCI::INciCeInterface> nciCeProxy);
    ~CeService();

    void HandleFieldActivated();
    void HandleFieldDeactivated();
    void OnCardEmulationData(const std::vector<uint8_t> &data);
    void OnCardEmulationActivated();
    void OnCardEmulationDeactivated();
    static void PublishFieldOnOrOffCommonEvent(bool isFieldOn);
    bool RegHceCmdCallback(const sptr<KITS::IHceCmdCallback> &callback,
                           const std::string &type);

    bool SendHostApduData(std::string hexCmdData, bool raw,
                          std::string &hexRespData);

    void InitConfigAidRouting();

private:
    uint64_t lastFieldOnTime_ = 0;
    uint64_t lastFieldOffTime_ = 0;

    std::weak_ptr<NfcService> nfcService_{};

    friend class NfcService;
    std::weak_ptr<NCI::INciCeInterface> nciCeProxy_{};
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager_{};
};
} // namespace NFC
} // namespace OHOS
#endif