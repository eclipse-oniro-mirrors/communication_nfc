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
     * @brief Set card emulation listener to receive field on/off event.
     * @param listener The listener to receive field on/off event.
     */
    void SetCeHostListener(std::weak_ptr<INciCeInterface::ICeHostListener> listener);
    void ClearT3tIdentifiersCache();
    uint32_t GetLfT3tMax();
    uint32_t GetLastError();
    void Abort();

    /**
     * @brief Whether nfc is enabled or disabled.
     * @return true/false - nfc is enabled/disabled.
     */
    bool IsNfcActive();

    /**
     * @brief Initialize nfc.
     * @return true/false - initialize is successful or not successful.
     */
    bool Initialize();

    /**
     * @brief Deinitialize nfc.
     * @return true/false - deinitialize is successful or not successful.
     */
    bool Deinitialize();

    /**
     * @brief Enable discovery for nfc.
     * @param techMask Supported rf technology for nfc.
     * @param enableReaderMode True/false to enable/disable reader mode
     * @param enableHostRouting True/false to enable/disable host routing
     * @param restart True/false to restart or not restart
     */
    void EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart);

    /**
     * @brief Disable discovery for nfc.
     */
    void DisableDiscovery();

    /**
     * @brief Send raw data.
     * @param rawData Data needed to send
     * @return True/false to successful/failed to send
     */
    bool SendRawFrame(std::string& rawData);

    /**
     * @brief Send the status of screen.
     * @param screenStateMask The state of screen
     */
    void SetScreenStatus(unsigned char screenStateMask);

    /**
     * @brief Get nci version.
     * @return Nci version
     */
    uint32_t GetNciVersion() const;
    bool RegisterT3tIdentifier(const std::string& t3tIdentifier) const;
    void DeregisterT3tIdentifier(uint32_t handle) const;

    /**
     * @brief Check whether to load firmware.
     * @return True/false to success/fail to load firmware.
     */
    bool CheckFirmware();

    /**
     * @brief Dump debug info for nfc.
     * @param fd File descriptor to store debug info.
     */
    void Dump(uint32_t fd) const;

    /**
     * @brief Reset nfc chip.
     */
    void FactoryReset() const;

    /**
     * @brief Close nfc.
     */
    void Shutdown() const;

    /**
     * @brief whether tag is active.
     * @return True/false tag is active/deactive.
     */
    bool IsTagActive() const;

    /**
     * @brief Start or stop rf discovery.
     * @param isStart True/false start/stop rf discovery.
     */
    void StartRfDiscovery(bool isStart);

    /**
     * @brief Query whether to start rf discovery.
     * @return True/false to start/stop rf discovery.
     */
    bool IsRfEbabled();

    /**
     * @brief Config commit routing table for nfc.
     * @return True/false to be successful/failed to config routing table.
     */
    bool CommitRouting();

    /**
     * @brief Computer routing params.
     * @return True/false to be successful/failed to computer params.
     */
    bool ComputeRoutingParams(int defaultPaymentType);

    /**
     * @brief Whether rf field is on or off.
     * @return True/false to be field on/off.
     */
    bool isRfFieldOn();
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
    void DoNfaDmSetConfig();
    void DoNfaSetPowerSubState();
    // static callback functions regiter to nci stack.
    static void NfcConnectionCallback(uint8_t connEvent, tNFA_CONN_EVT_DATA* eventData);
    static void NfcDeviceManagementCallback(uint8_t dmEvent, tNFA_DM_CBACK_DATA* eventData);
    static void PrivateNciCallback(uint8_t event, uint16_t paramLen, uint8_t *param);

    static const tNFA_TECHNOLOGY_MASK DEFAULT_TECH_MASK =
        (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B | NFA_TECHNOLOGY_MASK_F |
         NFA_TECHNOLOGY_MASK_V);
    static const uint32_t DEFAULT_DISCOVERY_DURATION = 500;
    static const uint32_t DISCOVERY_DURATION = 200;
    static const uint32_t NFA_SCREEN_POLLING_TAG_MASK = 0x10;
    OHOS::NFC::SynchronizeEvent nfcEnableEvent_;
    OHOS::NFC::SynchronizeEvent nfcDisableEvent_;
    OHOS::NFC::SynchronizeEvent nfcStartStopPollingEvent_;
    OHOS::NFC::SynchronizeEvent nfcSetPowerSubStateEvent_;
    OHOS::NFC::SynchronizeEvent nfcSetConfigEvent_;
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
    uint64_t lastRfFieldTime = 0;
    bool isRfFieldOn_ = false;
    std::weak_ptr<INciCeInterface::ICeHostListener> cardEmulationListener_;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // NFCC_NCI_ADAPTER_H
