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
#ifndef NFCC_NCI_ADAPTER_H
#define NFCC_NCI_ADAPTER_H
#include <mutex>
#include "extns.h"
#include "ndef_utils.h"
#include "nfa_api.h"
#include "nfa_ce_api.h"
#include "nfa_ee_api.h"
#include "nfa_hci_api.h"
#include "nfa_rw_api.h"
#include "nfc_hal_api.h"
#include "NfcAdaptation.h"
#include "nfc_config.h"
#include "synchronize_event.h"
#include "inci_ce_interface.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class NfccNciAdapter final {
public:
    static NfccNciAdapter& GetInstance();
    ~NfccNciAdapter();

    /**
     * @brief Set card emulation listener to receive filed on/off event.
     * @param listener The listener to receive filed on/off event.
     */
    void SetCeHostListener(std::weak_ptr<INciCeInterface::ICeHostListener> listener);

    void ClearT3tIdentifiersCache();
    uint32_t GetLfT3tMax();
    uint32_t GetLastError();
    void Abort();
    bool IsNfcActive();
    bool Initialize();
    bool Deinitialize();
    void EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart);
    void DisableDiscovery();
    bool SendRawFrame(std::string& rawData);
    void SetScreenStatus(unsigned char screenStateMask);
    uint32_t GetNciVersion() const;
    bool RegisterT3tIdentifier(const std::string& t3tIdentifier) const;
    void DeregisterT3tIdentifier(uint32_t handle) const;
    bool CheckFirmware();
    void Dump(uint32_t fd) const;
    void FactoryReset() const;
    void Shutdown() const;
    bool IsTagActive() const;
    void StartRfDiscovery(bool isStart);
    bool IsRfEbabled();
    bool CommitRouting();
    bool ComputeRoutingParams();

    void OnCardEmulationData(const std::vector<uint8_t> &data);
    void OnCardEmulationActivated();
    void OnCardEmulationDeactivated();

    // method for SAK28 issue
    void SendActEvtForSak28Tag(uint8_t connEvent, tNFA_CONN_EVT_DATA* eventData);

private:
    NfccNciAdapter();

    tNFA_STATUS StartPolling(tNFA_TECHNOLOGY_MASK techMask);
    tNFA_STATUS StopPolling();
    void DoNfaPollEnabledDisabledEvt();
    void DoNfaActivatedEvt(tNFA_CONN_EVT_DATA* eventData);
    void DoNfaSelectResultEvt(uint8_t status);
    void DoNfaDeactivatedEvt(tNFA_CONN_EVT_DATA* eventData);
    void DoNfaDiscResultEvt(tNFA_CONN_EVT_DATA* eventData);
    void DoNfaPresenceEvt(tNFA_CONN_EVT_DATA* eventData);
    void DoNfaDmEnableEvt(tNFA_DM_CBACK_DATA* eventData);
    void DoNfaDmDisableEvt(tNFA_DM_CBACK_DATA* eventData);
    void DoNfaDmRfFieldEvt(tNFA_DM_CBACK_DATA* eventData);
    void DoNfaDmNfccTimeoutEvt(tNFA_DM_CBACK_DATA* eventData);
    uint8_t GetDiscovryParam(unsigned char screenState, unsigned char screenStateMask);
    bool IsDiscTypeListen(tNFC_ACTIVATE_DEVT& actNtf);
    void HandleDiscNtf(tNFC_RESULT_DEVT* discNtf);
    tNFA_STATUS NfaRegVSCback(bool isRegster, tNFA_VSC_CBACK* vscCback);

    // static callback functions regiter to nci stack.
    static void NfcConnectionCallback(uint8_t connEvent, tNFA_CONN_EVT_DATA* eventData);
    static void NfcDeviceManagementCallback(uint8_t dmEvent, tNFA_DM_CBACK_DATA* eventData);
    static void PrivateNciCallback(uint8_t event, uint16_t paramLen, uint8_t *param);

private:
    static const tNFA_TECHNOLOGY_MASK DEFAULT_TECH_MASK =
        (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B | NFA_TECHNOLOGY_MASK_F |
         NFA_TECHNOLOGY_MASK_V);
    static const uint32_t DEFAULT_DISCOVERY_DURATION = 500;
    static const uint32_t DISCOVERY_DURATION = 200;
    static const uint32_t NFA_SCREEN_POLLING_TAG_MASK = 0x10;

    OHOS::NFC::SynchronizeEvent nfcEnableEvent_;
    OHOS::NFC::SynchronizeEvent nfcDisableEvent_;
    OHOS::NFC::SynchronizeEvent nfcStartStopPollingEvent_;
    bool isNfcEnabled_ = false;
    bool isRoutingInited_ = false;
    bool rfEnabled_ = false;
    bool discoveryEnabled_ = false;  // is polling or listening
    bool pollingEnabled_ = false;    // is polling for tag
    bool isDisabling_ = false;
    bool readerModeEnabled_ = false;
    unsigned long discoveryDuration_ = 0;
    bool isTagActive_ = false;
    unsigned char curScreenState_ = NFA_SCREEN_STATE_OFF_LOCKED;
    std::weak_ptr<INciCeInterface::ICeHostListener> cardEmulationListener_;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // NFCC_NCI_ADAPTER_H
