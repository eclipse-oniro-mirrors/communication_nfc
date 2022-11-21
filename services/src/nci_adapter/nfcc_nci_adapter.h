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
#ifndef NCI_MANAGER_H
#define NCI_MANAGER_H

#include <memory>
#include <mutex>

#include "infc_nci.h"
#include "synchronize_event.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class NfccNciAdapter final {
public:
    static NfccNciAdapter& GetInstance();
    static int GetIsoDepMaxTransceiveLength();
    static void ClearT3tIdentifiersCache();
    static int GetLfT3tMax();
    static int GetLastError();
    static void Abort();
    static bool IsNfcActive();
    bool Initialize();
    bool Deinitialize();
    void EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart);
    void DisableDiscovery();
    bool SendRawFrame(std::string& rawData);
    void SetScreenStatus(unsigned char screenStateMask) const;
    int GetNciVersion() const;
    bool RegisterT3tIdentifier(const std::string& t3tIdentifier) const;
    void DeregisterT3tIdentifier(int handle) const;
    bool CheckFirmware();
    void Dump(int fd) const;
    void FactoryReset() const;
    void Shutdown() const;
    bool IsTagActive() const;
    void SetNciAdaptation(std::shared_ptr<INfcNci> nciAdaptation);
    void StartRfDiscovery(bool isStart) const;
    bool IsRfEbabled();

private:
    static const tNFA_TECHNOLOGY_MASK DEFAULT_TECH_MASK =
        (NFA_TECHNOLOGY_MASK_A | NFA_TECHNOLOGY_MASK_B | NFA_TECHNOLOGY_MASK_F |
         NFA_TECHNOLOGY_MASK_V | NFA_TECHNOLOGY_MASK_A_ACTIVE);
    static const int DEFAULT_DISCOVERY_DURATION = 500;
    static const int DISCOVERY_DURATION = 200;
    static const int NFA_SCREEN_POLLING_TAG_MASK = 0x10;
    NfccNciAdapter();
    ~NfccNciAdapter();
    tNFA_STATUS StartPolling(tNFA_TECHNOLOGY_MASK techMask) const;
    tNFA_STATUS StopPolling() const;
    static void DoNfaActivatedEvt(tNFA_CONN_EVT_DATA* eventData);
    static void DoNfaDeactivatedEvt(tNFA_CONN_EVT_DATA* eventData);
    static void DoNfaDiscResultEvt(tNFA_CONN_EVT_DATA* eventData);
    static void DoNfaPresenceEvt(tNFA_CONN_EVT_DATA* eventData);
    static void NfcConnectionCallback(uint8_t connEvent, tNFA_CONN_EVT_DATA* eventData);
    static void DoNfaDmEnableEvt(tNFA_DM_CBACK_DATA* eventData);
    static void DoNfaDmDisableEvt(tNFA_DM_CBACK_DATA* eventData);
    static void DoNfaDmRfFieldEvt(tNFA_DM_CBACK_DATA* eventData);
    static void DoNfaDmNfccTimeoutEvt(tNFA_DM_CBACK_DATA* eventData);
    static void NfcDeviceManagementCallback(uint8_t dmEvent, tNFA_DM_CBACK_DATA* eventData);
    static uint8_t GetDiscovryParam(unsigned char screenState, unsigned char screenStateMask);

    std::mutex mutex_ {};
    static OHOS::NFC::SynchronizeEvent nfcEnableEvent_;
    static OHOS::NFC::SynchronizeEvent nfcDisableEvent_;
    static bool isNfcEnabled_;
    static bool rfEnabled_;
    static bool discoveryEnabled_;  // is polling or listening
    static bool pollingEnabled_;    // is polling for tag
    static bool isDisabling_;
    static bool readerModeEnabled_;
    static unsigned long discoveryDuration_;
    static bool isTagActive_;
    static unsigned char curScreenState_;
    static std::shared_ptr<INfcNci> nciAdaptation_;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // NCI_MANAGER_H
