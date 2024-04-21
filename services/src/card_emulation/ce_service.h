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
#include "element_name.h"
#include "idefault_payment_service_change_callback.h"
#include "default_payment_service_change_callback.h"
#include "app_state_observer.h"
#include "infc_app_state_observer.h"

namespace OHOS {
namespace NFC {
class NfcService;
class NfcEventHandler;
class HostCardEmulationManager;
using OHOS::AppExecFwk::ElementName;
class CeService : public IDefaultPaymentServiceChangeCallback,
                  public std::enable_shared_from_this<CeService>,
                  public INfcAppStateObserver {
public:
    struct AidEntry {
        std::string aid;
        int route;
        int aidInfo;
        int power;
        bool operator==(const AidEntry &other) const
        {
            return aid == other.aid && route == other.route && aidInfo == other.aidInfo && power == other.power;
        }
    };

    explicit CeService(std::weak_ptr<NfcService> nfcService, std::weak_ptr<NCI::INciCeInterface> nciCeProxy);
    ~CeService();

    void HandleFieldActivated();
    void HandleFieldDeactivated();
    void OnCardEmulationData(const std::vector<uint8_t> &data);
    void OnCardEmulationActivated();
    void OnCardEmulationDeactivated();
    static void PublishFieldOnOrOffCommonEvent(bool isFieldOn);
    bool RegHceCmdCallback(const sptr<KITS::IHceCmdCallback> &callback, const std::string &type,
                           Security::AccessToken::AccessTokenID callerToken);

    bool UnRegHceCmdCallback(const std::string &type, Security::AccessToken::AccessTokenID callerToken);

    bool UnRegAllCallback(Security::AccessToken::AccessTokenID callerToken);
    bool IsDefaultService(ElementName &element, const std::string &type);

    bool SendHostApduData(const std::string &hexCmdData, bool raw, std::string &hexRespData,
                          Security::AccessToken::AccessTokenID callerToken);

    bool InitConfigAidRouting();
    void OnDefaultPaymentServiceChange() override;
    OHOS::sptr<OHOS::IRemoteObject> AsObject() override;
    void Initialize();
    void Deinitialize();
    bool StartHce(const ElementName &element, const std::vector<std::string> &aids);
    bool StopHce(const ElementName &element, Security::AccessToken::AccessTokenID callerToken);
    bool HandleWhenRemoteDie(Security::AccessToken::AccessTokenID callerToken);
    void OnAppAddOrChangeOrRemove(std::shared_ptr<EventFwk::CommonEventData> data);
   
    void ConfigRoutingAndCommit();
    void SearchElementByAid(const std::string &aid, ElementName &aidElement);
    KITS::DefaultPaymentType GetDefaultPaymentType();

    void HandleAppStateChanged(const std::string &bundleName, const std::string &abilityName,
                               int abilityState) override;

private:
    void BuildAidEntries(std::map<std::string, AidEntry> &aidEntries);
    void ClearAidEntriesCache();
    bool IsDynamicAid(const std::string &aid);
    bool IsPaymentAid(const std::string &aid, const AppDataParser::HceAppAidInfo &hceApp);
    void SetHceInfo(const ElementName &element, const std::vector<std::string> &aids);
    void ClearHceInfo();
    bool AppEventCheckValid(std::shared_ptr<EventFwk::CommonEventData> data);
    void UpdateDefaultPaymentBundleInstalledStatus(bool installed);

    void HandleOtherAidConflicted(const std::vector<AppDataParser::HceAppAidInfo> &hceApps);
    bool UpdateDefaultPaymentType();
    void UpdateDefaultPaymentElement(const ElementName &element);

    uint64_t lastFieldOnTime_ = 0;
    uint64_t lastFieldOffTime_ = 0;

    std::weak_ptr<NfcService> nfcService_{};

    friend class NfcService;
    std::weak_ptr<NCI::INciCeInterface> nciCeProxy_{};
    std::shared_ptr<HostCardEmulationManager> hostCardEmulationManager_{};
    ElementName defaultPaymentElement_;
    bool defaultPaymentBundleInstalled_ {};
    KITS::DefaultPaymentType defaultPaymentType_;
    sptr<DefaultPaymentServiceChangeCallback> dataRdbObserver_;

    ElementName foregroundElement_ {};
    std::vector<std::string> dynamicAids_ {};

    std::mutex configRoutingMutex_ {};
    std::map<std::string, AidEntry> aidToAidEntry_{};
    std::shared_ptr<AppStateObserver> appStateObserver_;
};
} // namespace NFC
} // namespace OHOS
#endif