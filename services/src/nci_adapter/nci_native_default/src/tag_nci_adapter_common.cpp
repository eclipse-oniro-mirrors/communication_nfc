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
#include "tag_nci_adapter_common.h"
#include <unistd.h>
#include "nfc_brcm_defs.h"
#include "nfc_config.h"
#include "nfc_sdk_common.h"
#include "loghelper.h"
#include "nfa_api.h"
#include "rw_int.h"
#include "securec.h"
#include "extns.h"

namespace OHOS {
namespace NFC {
namespace NCI {
static const uint32_t ISO14443_3A_DEFAULT_TIMEOUT = 618;   // NfcA
static const uint32_t ISO14443_3B_DEFAULT_TIMEOUT = 1000;  // NfcB
static const uint32_t ISO14443_4_DEFAULT_TIMEOUT = 618;    // ISO-DEP
static const uint32_t FELICA_DEFAULT_TIMEOUT = 255;        // Felica
static const uint32_t ISO15693_DEFAULT_TIMEOUT = 1000;     // NfcV
static const uint32_t NDEF_DEFAULT_TIMEOUT = 1000;
static const uint32_t NDEF_FORMATABLE_DEFAULT_TIMEOUT = 1000;
static const uint32_t MIFARE_CLASSIC_DEFAULT_TIMEOUT = 618;  // MifareClassic
static const uint32_t MIFARE_UL_DEFAULT_TIMEOUT = 618;       // MifareUltralight

TagNciAdapterCommon::TagNciAdapterCommon()
    : discNtfIndex_(0),
      isSkipNdefRead_(false),
      isMultiProtoMFC_(false)
{
    ResetTimeout();
    ResetTag();
    if (NfcConfig::hasKey(NAME_LEGACY_MIFARE_READER)) {
        isLegacyMifareReader_ = (NfcConfig::getUnsigned(NAME_LEGACY_MIFARE_READER) != 0);
    } else {
        isLegacyMifareReader_ = true;
    }
    DebugLog("TagNciAdapterCommon::TagNciAdapterCommon: isLegacyMifareReader_ = %{public}d", isLegacyMifareReader_);
    if (NfcConfig::hasKey(NAME_NXP_SUPPORT_NON_STD_CARD)) {
        isMultiTagSupported_ = (NfcConfig::getUnsigned(NAME_NXP_SUPPORT_NON_STD_CARD) != 0);
    } else {
        isMultiTagSupported_ = false;
    }
}

TagNciAdapterCommon::~TagNciAdapterCommon()
{
    tagTechList_.clear();
    tagRfDiscIdList_.clear();
    tagRfProtocols_.clear();
    tagPollBytes_.clear();
    tagActivatedBytes_.clear();
    multiTagDiscId_.clear();
    multiTagDiscProtocol_.clear();
    techListIndex_ = 0;
    multiTagTmpTechIdx_ = 0;
    discRstEvtNum_ = 0;
    selectedTagIdx_ = 0;
    connectedProtocol_ = NCI_PROTOCOL_UNKNOWN;
    isFelicaLite_ = false;
    isMifareUltralight_ = false;
    isMifareDESFire_ = false;
    isMultiTag_ = false;
    discNtfIndex_ = 0;
    isMultiProtoMFC_ = false;
};

TagNciAdapterCommon& TagNciAdapterCommon::GetInstance()
{
    static TagNciAdapterCommon TagNciAdapterCommon;
    return TagNciAdapterCommon;
}

void TagNciAdapterCommon::ClearMultiMFCTagState()
{
    isSkipNdefRead_ = false;
    isMultiProtoMFC_ = false;
    lastTagFoundTime_ = 0;
}

void TagNciAdapterCommon::ResetTag()
{
    DebugLog("TagNciAdapterCommon::ResetTag");
    // tag data
    tagTechList_.clear();
    tagRfDiscIdList_.clear();
    tagRfProtocols_.clear();
    tagPollBytes_.clear();
    tagActivatedBytes_.clear();
    multiTagDiscId_.clear();
    multiTagDiscProtocol_.clear();

    // disc idxes
    techListIndex_ = 0;
    multiTagTmpTechIdx_ = 0;
    discRstEvtNum_ = 0;
    discNtfIndex_ = 0;
    selectedTagIdx_ = 0;

    // connection datas
    connectedProtocol_ = NCI_PROTOCOL_UNKNOWN;

    isFelicaLite_ = false;
    isMifareUltralight_ = false;
    isMifareDESFire_ = false;
    isMultiTag_ = false;

    ResetTimeout();

    //  special data
#if (NXP_EXTNS == TRUE)
    Extns::GetInstance().EXTNS_SetConnectFlag(false);
#endif
}

void TagNciAdapterCommon::ResetTimeout()
{
    technologyTimeoutsTable_[TARGET_TYPE_ISO14443_3A] = ISO14443_3A_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TARGET_TYPE_ISO14443_3B] = ISO14443_3B_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TARGET_TYPE_ISO14443_4] = ISO14443_4_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TARGET_TYPE_FELICA] = FELICA_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TARGET_TYPE_V] = ISO15693_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TARGET_TYPE_NDEF] = NDEF_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TARGET_TYPE_NDEF_FORMATABLE] = NDEF_FORMATABLE_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TARGET_TYPE_MIFARE_CLASSIC] = MIFARE_CLASSIC_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TARGET_TYPE_MIFARE_UL] = MIFARE_UL_DEFAULT_TIMEOUT;
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
