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
#ifndef NFC_SERVICE_H
#define NFC_SERVICE_H
#include <future>
#include <mutex>
#include "access_token.h"
#include "ce_service.h"
#include "common_event_manager.h"
#include "infc_controller_callback.h"
#include "infc_service.h"
#include "infcc_host.h"
#include "itag_host.h"
#include "nfc_controller_impl.h"
#include "nfc_polling_params.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
class NfcEventHandler;
class NfcControllerImpl;
class CeService;
class NfcStateRegistryRecord {
public:
    std::string type_ = "";
    Security::AccessToken::AccessTokenID callerToken_ = 0;
    sptr<INfcControllerCallback> nfcStateChangeCallback_ = nullptr;
};

class ForegroundRegistryData {
public:
    bool isEnable_ = false;
    uint16_t techMask_ = 0xFFFF;
    AppExecFwk::ElementName element_;
    Security::AccessToken::AccessTokenID callerToken_ = 0;
    sptr<KITS::IForegroundCallback> callback_ = nullptr;
};

class NfcService final : public NCI::INfccHost::INfccHostListener,
    public INfcService,
    public std::enable_shared_from_this<NfcService> {
public:
    NfcService(std::unique_ptr<NCI::INfccHost> nfccHost = nullptr);
    ~NfcService() override;
    NfcService& operator=(const NfcService&) = delete;
    NfcService(const NfcService&) = delete;
    bool Initialize();
    std::weak_ptr<NfcService> GetInstance() const;
    void OnTagDiscovered(std::shared_ptr<NCI::ITagHost> tagHost) override;
    void FieldActivated() override;
    void FieldDeactivated() override;
    OHOS::sptr<IRemoteObject> GetTagServiceIface() override;
    bool EnableForegroundDispatch(AppExecFwk::ElementName element, std::vector<uint32_t> &discTech,
        const sptr<KITS::IForegroundCallback> &callback) override;
    bool DisableForegroundDispatch(AppExecFwk::ElementName element) override;
    bool DisableForegroundByDeathRcpt() override;
    bool IsForegroundEnabled() override;
    void SendTagToForeground(KITS::TagInfoParcelable tagInfo) override;

protected:
    // screen changed
    void HandleScreenChanged(int screenState);
    // package updated
    void HandlePackageUpdated(std::shared_ptr<EventFwk::CommonEventData> data);
    // commit routing
    void HandleCommitRouting();
    void HandleComputeRoutingParams();

private:
    std::weak_ptr<TAG::TagDispatcher> GetTagDispatcher() override;

    bool IsNfcEnabled() override;
    int GetNfcState() override;
    int GetScreenState() override;
    int GetNciVersion() override;
    std::weak_ptr<NCI::INfccHost> GetNfccHost() override
    {
        return nfccHost_;
    }

    bool IsNfcTaskReady(std::future<int>& future) const;
    void ExecuteTask(KITS::NfcTask param);
    void UpdateNfcState(int newState);
    // TurnOn/TurnOff Nfc
    void NfcTaskThread(KITS::NfcTask params, std::promise<int> promise);
    bool DoTurnOn();
    bool DoTurnOff();
    void DoInitialize();
    static void UnloadNfcSa();

    // register callback based on different access token ID.
    int SetRegisterCallBack(const sptr<INfcControllerCallback> &callback,
        const std::string& type, Security::AccessToken::AccessTokenID callerToken);
    int RemoveRegisterCallBack(const std::string& type, Security::AccessToken::AccessTokenID callerToken);
    int RemoveAllRegisterCallBack(Security::AccessToken::AccessTokenID callerToken);
    // polling
    void StartPollingLoop(bool force);
    std::shared_ptr<NfcPollingParams> GetPollingParameters(int screenState);
    uint16_t GetTechMaskFromTechList(std::vector<uint32_t> &discTech);
    // commit routing
    void CommitRouting();
    void ComputeRoutingParams();

private:
    // ms wait for initialization, included firmware download.
    static constexpr const int WAIT_MS_INIT = 90 * 1000;
    // ms wait for setting the routing table.
    static constexpr const int WAIT_MS_SET_ROUTE = 10 * 1000;
    int nciVersion_ = 0;

    // service
    std::weak_ptr<NfcService> nfcService_ {};
    // NCI
    std::shared_ptr<NCI::INfccHost> nfccHost_ {};

    OHOS::sptr<NfcControllerImpl> nfcControllerImpl_;
    OHOS::sptr<IRemoteObject> tagSessionIface_{};
    std::shared_ptr<NfcEventHandler> eventHandler_ {};
    std::shared_ptr<CeService> ceService_ {};
    std::shared_ptr<TAG::TagDispatcher> tagDispatcher_ {};
    // save current state.
    int nfcState_;
    int screenState_ {};
    // polling
    std::shared_ptr<NfcPollingParams> currPollingParams_;
    ForegroundRegistryData foregroundData_;

    std::vector<NfcStateRegistryRecord> stateRecords_;
    // lock
    std::mutex mutex_ {};
    std::future<int> future_ {};
    std::unique_ptr<std::thread> task_ {};
    std::unique_ptr<std::thread> rootTask_ {};
    static uint32_t unloadStaSaTimerId;

    friend class NfcWatchDog;
    friend class NfcControllerImpl;
    friend class TAG::TagDispatcher;
    friend class NfcSaManager;
    friend class NfcEventHandler;
    friend class CeService;
};
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_SERVICE_H
