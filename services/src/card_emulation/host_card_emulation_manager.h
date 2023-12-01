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
#ifndef HOST_CARDEMULATIONMANAGER_H
#define HOST_CARDEMULATIONMANAGER_H

#include <vector>
#include <string>
#include "nfc_service.h"
#include "access_token.h"
#include "common_event_manager.h"
#include "ihce_cmd_callback.h"
#include "element_name.h"
#include "inci_ce_interface.h"
#include "nfc_ability_connection_callback.h"

namespace OHOS {
namespace NFC {
using OHOS::AppExecFwk::ElementName;
class NfcService;
class HostCardEmulationManager {
public:
    enum HceState {
        INITIAL_STATE = 0,
        WAIT_FOR_SELECT,
        WAIT_FOR_SERVICE,
        WAIT_FOR_DEACTIVATE,
        DATA_TRANSFER,
    };
    explicit HostCardEmulationManager(std::weak_ptr<NfcService> nfcService,
                                      std::weak_ptr<NCI::INciCeInterface> nciCeProxy);
    ~HostCardEmulationManager();
    void OnHostCardEmulationDataNfcA(const std::vector<uint8_t>& data);
    void OnCardEmulationActivated();
    void OnCardEmulationDeactivated();
    class HceCmdRegistryData {
    public:
        bool isEnabled_ = false;
        AppExecFwk::ElementName element_;
        Security::AccessToken::AccessTokenID callerToken_ = 0;
        sptr<KITS::IHceCmdCallback> callback_ = nullptr;
    };
    bool RegHceCmdCallback(const sptr<KITS::IHceCmdCallback>& callback, const std::string& type);

    bool SendHostApduData(std::string hexCmdData, bool raw, const std::string& hexRespData);

private:
    void HandleDataOnW4Select(const std::string aid, const std::vector<uint8_t>& data);
    void HandleDataOnDataTransfer(const std::string aid, const std::vector<uint8_t>& data);
    bool ExistService();
    std::string ParseSelectAid(const std::vector<uint8_t>& data);
    void SendDataToService(const std::vector<uint8_t>& data);
    bool DispatchAbilitySingleApp(const std::string aid);

    std::weak_ptr<NfcService> nfcService_{};
    std::weak_ptr<NCI::INciCeInterface> nciCeProxy_{};
    friend class NfcService;
    std::shared_ptr<HostCardEmulationManager::HceCmdRegistryData> hceCmdRegistryData_{};
    HceState hceState_;
    std::vector<uint8_t> queueHceData_{};
    sptr<NfcAbilityConnectionCallback> connect_{};
};
} // namespace NFC
} // namespace OHOS
#endif // HOST_CARDEMULATIONMANAGER_H
