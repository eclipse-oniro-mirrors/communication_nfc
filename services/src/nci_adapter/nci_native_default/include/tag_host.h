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
#ifndef TAG_HOST_H
#define TAG_HOST_H
#include <mutex>
#include <vector>
#include "pac_map.h"
#include "synchronize_event.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class TagHost final {
public:
    static const uint32_t DATA_BYTE2 = 2;
    static const uint32_t DATA_BYTE3 = 3;
    static const uint32_t DATA_BYTE4 = 4;
    static const uint32_t DATA_BYTE5 = 5;
    static const uint32_t DATA_BYTE6 = 6;
    static const uint32_t DATA_BYTE7 = 7;
    static const uint32_t DATA_BYTE8 = 8;
    static const uint32_t DATA_BYTE9 = 9;
    // NFC_B
    static const uint32_t NCI_APP_DATA_LENGTH = 4;
    static const uint32_t NCI_PROTOCOL_INFO_LENGTH = 3;
    // MifareUltralight
    static const uint32_t NCI_MIFARE_ULTRALIGHT_C_RESPONSE_LENGTH = 16;
    static const uint32_t NCI_MIFARE_ULTRALIGHT_C_BLANK_CARD = 0;
    static const uint32_t NCI_MIFARE_ULTRALIGHT_C_VERSION_INFO_FIRST = 0x02;
    static const uint32_t NCI_MIFARE_ULTRALIGHT_C_VERSION_INFO_SECOND = 0x00;
    static const uint32_t NCI_MIFARE_ULTRALIGHT_C_NDEF_CC = 0xE1;
    static const uint32_t NCI_MIFARE_ULTRALIGHT_C_NDEF_MAJOR_VERSION = 0x20;
    static const uint32_t NCI_MIFARE_ULTRALIGHT_C_NDEF_TAG_SIZE = 0x06;
    // Iso15693
    static const uint32_t NCI_POLL_LENGTH_MIN = 2;
    static const uint32_t DEFAULT_PRESENCE_CHECK_WATCH_DOG_TIMEOUT = 125;
    // T1T~T4T for NDEF tag
    static const auto NDEF_UNKNOWN_TYPE = 0;
    static const auto NDEF_TYPE1_TAG = 1;
    static const auto NDEF_TYPE2_TAG = 2;
    static const auto NDEF_TYPE3_TAG = 3;
    static const auto NDEF_TYPE4_TAG = 4;
    static const auto NDEF_MIFARE_CLASSIC_TAG = 101;

    // NfcF, Felica
    static const uint32_t SENSF_RES_LENGTH = 8;
    static const uint32_t F_POLL_LENGTH = 10;

public:
    TagHost(const std::vector<int>& tagTechList,
            const std::vector<uint32_t>& tagRfDiscIdList,
            const std::vector<uint32_t>& tagActivatedProtocols,
            const std::string& uid,
            const std::vector<std::string>& tagPollBytes,
            const std::vector<std::string>& tagActivatedBytes,
            const uint32_t connectedTechIndex);
    ~TagHost();
    bool Connect(int technology);
    bool Disconnect();
    bool Reconnect();
    int Transceive(const std::string& request, std::string& response);

    // get the tag related technologies or uid info.
    std::vector<int> GetTechList();
    uint32_t GetConnectedTech();
    void RemoveTech(int tech);
    std::vector<AppExecFwk::PacMap> GetTechExtrasData();
    std::string GetTagUid();
    uint32_t GetTagRfDiscId();

    // functions for ndef tag only.
    std::string ReadNdef();
    bool WriteNdef(std::string& data);
    bool IsNdefFormatable();
    bool FormatNdef(const std::string& key);
    bool SetNdefReadOnly();
    bool DetectNdefInfo(std::vector<int>& ndefInfo);
    std::string FindNdefTech();

    // functions for checking the tag field on or not.
    bool FieldOnCheckingThread();
    bool IsTagFieldOn();
    void StartFieldOnChecking(uint32_t delayedMs);
    void StopFieldChecking();

    void SetTimeout(uint32_t timeout, int technology);
    uint32_t GetTimeout(uint32_t technology);
    void ResetTimeout();

private:
    AppExecFwk::PacMap ParseTechExtras(uint32_t index);
    void FieldCheckingThread(uint32_t delayedMs);
    void PauseFieldChecking();
    void ResumeFieldChecking();
    void StopFieldCheckingInner();
    void AddNdefTechToTagInfo(uint32_t tech, uint32_t discId, uint32_t actProto, AppExecFwk::PacMap pacMap);
    uint32_t GetNdefType(uint32_t protocol) const;
    bool IsUltralightC();

    void DoTargetTypeIso144433a(AppExecFwk::PacMap &pacMap, uint32_t index);
    void DoTargetTypeIso144433b(AppExecFwk::PacMap &pacMap, uint32_t index);
    void DoTargetTypeIso144434(AppExecFwk::PacMap &pacMap, uint32_t index);
    void DoTargetTypeV(AppExecFwk::PacMap &pacMap, uint32_t index);
    void DoTargetTypeF(AppExecFwk::PacMap &pacMap, uint32_t index);
    void DoTargetTypeNdef(AppExecFwk::PacMap &pacMap);

    static OHOS::NFC::SynchronizeEvent fieldCheckWatchDog_;
    std::mutex mutex_ {};

    // tag datas for tag dispatcher
    std::vector<int> tagTechList_;
    std::vector<AppExecFwk::PacMap> tagTechExtras_;
    std::vector<uint32_t> tagRfDiscIdList_;
    std::vector<uint32_t> tagRfProtocols_;
    std::string tagUid_;
    std::vector<std::string> tagPollBytes_;
    std::vector<std::string> tagActivatedBytes_;

    // tag connection datas
    uint32_t connectedTagDiscId_; // multiproto card can have different values
    uint32_t connectedTechIndex_; // index to find value in arrays of tag data
    volatile bool isTagFieldOn_;
    volatile bool isFieldChecking_;
    volatile bool isPauseFieldChecking_;
    volatile bool isSkipNextFieldChecking_;
    bool addNdefTech_;
    std::vector<int> technologyList_ {};
    /* NDEF */
    static const uint32_t NDEF_INFO_SIZE = 2; // includes size + mode;
    static const uint32_t NDEF_SIZE_INDEX = 0;
    static const uint32_t NDEF_MODE_INDEX = 1;
    AppExecFwk::PacMap ndefExtras_;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_HOST_H
