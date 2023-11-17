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
#ifndef NFC_POLLING_MANAGER_H
#define NFC_POLLING_MANAGER_H
#include "access_token.h"
#include "common_event_manager.h"
#include "iforeground_callback.h"
#include "nfc_polling_params.h"
#include "taginfo_parcelable.h"
#include "inci_nfcc_interface.h"
#include "inci_tag_interface.h"

namespace OHOS {
namespace NFC {
// ms wait for setting the routing table.
static const int WAIT_MS_SET_ROUTE = 10 * 1000;
class NfcService;
class NfcPollingManager {
public:
    NfcPollingManager(std::weak_ptr<NfcService> nfcService,
                      std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy,
                      std::weak_ptr<NCI::INciTagInterface> nciTagProxy);
    ~NfcPollingManager();
    class ForegroundRegistryData {
    public:
        bool isEnabled_ = false;
        uint16_t techMask_ = 0xFFFF;
        AppExecFwk::ElementName element_;
        Security::AccessToken::AccessTokenID callerToken_ = 0;
        sptr<KITS::IForegroundCallback> callback_ = nullptr;
    };

    void ResetCurrPollingParams();
    std::shared_ptr<NfcPollingParams> GetCurrentParameters();
    std::shared_ptr<NfcPollingParams> GetPollingParameters(int screenState);

    // polling
    void StartPollingLoop(bool force);
    // screen changed
    void HandleScreenChanged(int screenState);
    // package updated
    void HandlePackageUpdated(std::shared_ptr<EventFwk::CommonEventData> data);

    bool EnableForegroundDispatch(AppExecFwk::ElementName element, std::vector<uint32_t> &discTech,
                                  const sptr<KITS::IForegroundCallback> &callback);
    bool DisableForegroundDispatch(AppExecFwk::ElementName element);
    bool DisableForegroundByDeathRcpt();
    bool IsForegroundEnabled();
    void SendTagToForeground(KITS::TagInfoParcelable tagInfo);
    std::shared_ptr<NfcPollingManager::ForegroundRegistryData> GetForegroundData();

private:
    int screenState_ = 0;
    std::shared_ptr<NfcPollingManager::ForegroundRegistryData> foregroundData_ {};
    std::shared_ptr<NfcPollingParams> currPollingParams_ {};
    std::weak_ptr<NfcService> nfcService_ {};
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy_ {};
    std::weak_ptr<NCI::INciTagInterface> nciTagProxy_ {};

    // lock
    std::mutex mutex_ {};
};
} // namespace NFC
} // namespace OHOS
#endif // NFC_POLLING_MANAGER_H