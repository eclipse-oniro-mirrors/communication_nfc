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
#ifndef TAG_NCI_ADAPTER_COMMON_H
#define TAG_NCI_ADAPTER_COMMON_H
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
const uint32_t MAX_NUM_TECHNOLOGY = 12;
class TagNciAdapterCommon final {
public:
    static TagNciAdapterCommon& GetInstance();
    TagNciAdapterCommon();
    ~TagNciAdapterCommon();
#if (NXP_EXTNS == TRUE)
    void ClearMultiMFCTagState();
#endif
    void ResetTag();
    void ResetTimeout();

    static const auto TARGET_TYPE_UNKNOWN = 0;
    static const auto TARGET_TYPE_ISO14443_3A = 1;
    static const auto TARGET_TYPE_ISO14443_3B = 2;
    static const auto TARGET_TYPE_ISO14443_4 = 3;
    static const auto TARGET_TYPE_FELICA = 4;
    static const auto TARGET_TYPE_V = 5;
    static const auto TARGET_TYPE_NDEF = 6;
    static const auto TARGET_TYPE_NDEF_FORMATABLE = 7;
    static const auto TARGET_TYPE_MIFARE_CLASSIC = 8;
    static const auto TARGET_TYPE_MIFARE_UL = 9;
    bool isLegacyMifareReader_ = false;
    bool isMultiTagSupported_ = false;
    uint32_t discNtfIndex_ = 0;
    bool isSkipNdefRead_ = false;
    bool isMultiProtoMFC_ = false;
    long lastTagFoundTime_ = 0;
    uint32_t connectedType_ = TARGET_TYPE_UNKNOWN;
    uint32_t connectedRfIface_ = NFA_INTERFACE_ISO_DEP;
    // timeout and time diffs
    int technologyTimeoutsTable_[MAX_NUM_TECHNOLOGY] = {0}; // index equals to the technology value
    // tag technology data for tag host and nfcservice.
    std::vector<int> tagTechList_ {};
    std::vector<uint32_t> tagRfDiscIdList_ {};          // disc id
    std::vector<uint32_t> tagRfProtocols_ {};           // protocol
    std::vector<std::string> tagPollBytes_ {};
    std::vector<std::string> tagActivatedBytes_ {};     // current tech list index
    // multiple protocol tag values
    std::vector<int> multiTagDiscId_ {};
    std::vector<int> multiTagDiscProtocol_ {};
    std::string readNdefData_ = {};
    // special vals for special tags
    uint8_t nfcID0_[4] {}; // Type B
    uint32_t techListIndex_ = 0;             // current tech list index
    uint32_t multiTagTmpTechIdx_ = 0; // to store the last techlist index for the last tag
    uint32_t discRstEvtNum_ = 0; // number of tag, increased with the times of NFA_DISC_RESULT_EVT
                                    // and decreased while selecting next tag
    uint32_t selectedTagIdx_ = 0;          // to store the last selected tag index
    bool isMultiTag_ = false;
    // multiple protocol tag special status
    bool isIsoDepDhReqFailed_ = false;
    // tag connection status data
    uint32_t connectedProtocol_ = 0;
    bool isReconnecting_ = false;
    bool isSwitchingRfIface_ = false;
    uint32_t connectedTechIdx_ = 0;
    bool isReconnected_ = false;
    // spec tag type
    bool isFelicaLite_ = false;
    bool isMifareUltralight_ = false;
    bool isMifareDESFire_ = false;
    bool isNdefReadTimeOut_ = false;
    bool isNdefReading_ = false;
    bool isNdefWriteSuccess_ = false;
    bool isNdefFormatSuccess_ = false;
    uint32_t t1tMaxMessageSize_ = 0;

    OHOS::NFC::SynchronizeEvent reconnectEvent_;
    OHOS::NFC::SynchronizeEvent readNdefEvent_;
    OHOS::NFC::SynchronizeEvent writeNdefEvent_;
    OHOS::NFC::SynchronizeEvent formatNdefEvent_;
    OHOS::NFC::SynchronizeEvent selectEvent_;
    OHOS::NFC::SynchronizeEvent setReadOnlyEvent_;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_NCI_ADAPTER_COMMON_H
