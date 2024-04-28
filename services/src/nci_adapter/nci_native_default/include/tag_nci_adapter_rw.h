/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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
#ifndef TAG_NCI_ADAPTER_RW_H
#define TAG_NCI_ADAPTER_RW_H
#include <mutex>
#include <vector>
#include "ndef_utils.h"
#include "nfa_api.h"
#include "nfa_rw_api.h"
#include "nfc_config.h"
#include "synchronize_event.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class TagNciAdapterRw final {
public:
    static TagNciAdapterRw& GetInstance();
    TagNciAdapterRw();
    ~TagNciAdapterRw();
#if (NXP_EXTNS == TRUE)
    enum TagState {
        IDLE = 0,
        SLEEP,
        ACTIVE,
        INACTIVE
    };
#else
    enum TagState { IDLE = 0, SLEEP, ACTIVE };
#endif

    // tag connection and read or write.
    // interfaces for nfc host
    tNFA_STATUS Connect(uint32_t idx);
    bool Disconnect();
    bool Reconnect();
    int Transceive(const std::string& request, std::string& response);
    void SetTimeout(const uint32_t timeout, const uint32_t technology);
    uint32_t GetTimeout(uint32_t technology) const;

    // functions for ndef tag only.
    void ReadNdef(std::string& response);
    bool WriteNdef(std::string& ndefMessage);
    bool IsNdefFormatable();
    bool FormatNdef();
    bool SetReadOnly() const;
    bool DetectNdefInfo(std::vector<int>& ndefInfo);
    bool IsNdefFormattable();

    // functions for checking the tag field on or not.
    bool IsTagFieldOn();
    void OnRfDiscLock();
    void OffRfDiscLock();
    void AbortWait();
    // Sync tag connection status from NfccNciAdapter
    bool IsTagDeactivating();
    void SetTagActivated();
    void SetTagDeactivated(bool isSleep);
    TagState GetTagState();
#if (NXP_EXTNS == TRUE)
    bool IsMultiMFCTag();
#endif
    // functions for checking the tag field on or not.
    void ResetTagFieldOnFlag();
    void HandleFieldCheckResult(uint8_t status);
    void HandleNdefCheckResult(uint8_t status, uint32_t currentSize, uint32_t flag, uint32_t maxSize);
    void HandleDeactivatedResult(tNFA_DEACTIVATE_TYPE deactType);
    void HandleTranceiveData(uint8_t status, uint8_t* data, uint32_t dataLen);

private:
    bool Reselect(tNFA_INTF_TYPE rfInterface, bool isSwitchingIface);
    tNFA_STATUS HandleMfcTransceiveData(std::string& response);
    tNFA_STATUS SendRawFrameForHaltPICC();
    bool IsTagActive() const;
    // spacial card
    bool IsT2TNackRsp(const uint8_t* response, uint32_t responseLen);
    // mifare
    bool IsMifareConnected();
    bool DeactiveForReselect();
    bool IsCashbeeCard();
    tNFA_STATUS SelectCard(tNFA_INTF_TYPE rfInterface);
    tNFA_STATUS RetryToWaitSuccess(tNFA_INTF_TYPE rfInterface);
    bool IsMifareUL(tNFA_ACTIVATED activated);
    void SetIsoDepFwt(tNFA_ACTIVATED activated, uint32_t technology);
    void RetryThreeTimes(int retryIn);
    tNFA_RW_PRES_CHK_OPTION presChkOption_;
    std::basic_string<uint8_t> receivedData_ {};
    bool isMfcTransRspErr_ = false;
    // synchronized lock
    std::mutex rfDiscoveryMutex_;
    OHOS::NFC::SynchronizeEvent transceiveEvent_;
    OHOS::NFC::SynchronizeEvent fieldCheckEvent_;
    OHOS::NFC::SynchronizeEvent checkNdefEvent_;
    OHOS::NFC::SynchronizeEvent activatedEvent_;
    OHOS::NFC::SynchronizeEvent deactivatedEvent_;

    bool isWaitingDeactRst_ = false; // deactive wrating state in reselect command can be modeified only in reselect
    bool isCashbee_ = false;
    TagState tagState_ = IDLE;
    // tag connection status data
    bool isInTransceive_ = false;
    bool isTransceiveTimeout_ = false;
    bool isTagFieldOn_ = false;
    // ndef checked status.
    uint32_t lastNdefCheckedStatus_ = NFA_STATUS_FAILED;
    uint32_t lastCheckedNdefMode_ = 0;
    bool isNdefCapable_ = false;
    uint32_t lastCheckedNdefSize_ = 0;
    uint32_t lastCheckedNdefMaxSize_ = 0;
    bool isNdefChecking_ = false;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_NCI_ADAPTER_RW_H
