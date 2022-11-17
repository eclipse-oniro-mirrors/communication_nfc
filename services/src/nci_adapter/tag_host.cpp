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
#include "tag_host.h"
#include <thread>
#include "loghelper.h"
#include "nfa_api.h"
#include "nfc_sdk_common.h"
#include "taginfo.h"
#include "tag_nci_adapter.h"

namespace OHOS {
namespace NFC {
namespace NCI {
static const int INVALID_VALUE = -1;
OHOS::NFC::SynchronizeEvent TagHost::fieldCheckWatchDog_;
TagHost::TagHost(const std::vector<int>& tagTechList,
                 const std::vector<int>& tagRfDiscIdList,
                 const std::vector<int>& tagActivatedProtocols,
                 const std::string& tagUid,
                 const std::vector<std::string>& tagPollBytes,
                 const std::vector<std::string>& tagActivatedBytes)
    : tagTechList_(std::move(tagTechList)),
      tagRfDiscIdList_(std::move(tagRfDiscIdList)),
      tagActivatedProtocols_(std::move(tagActivatedProtocols)),
      tagUid_(tagUid),
      tagPollBytes_(std::move(tagPollBytes)),
      tagActivatedBytes_(std::move(tagActivatedBytes)),
      connectedTagDiscId_(INVALID_VALUE),
      connectedTechIndex_(INVALID_VALUE),
      isTagFieldOn_(true),
      isFieldChecking_(false),
      isPauseFieldChecking_(false),
      addNdefTech_(false)
{
}

TagHost::~TagHost()
{
    tagTechList_.clear();
    technologyList_.clear();
    tagRfDiscIdList_.clear();
    tagActivatedProtocols_.clear();
    tagPollBytes_.clear();
    tagActivatedBytes_.clear();
}

bool TagHost::Connect(int technology)
{
    DebugLog("TagHost::Connect");
    PauseFieldChecking();
    std::lock_guard<std::mutex> lock(mutex_);
    bool result = false;
    tNFA_STATUS status;
    for (std::size_t i = 0; i < technologyList_.size(); i++) {
        if (technology != technologyList_[i]) {
            continue;
        }
        // try connect the tag
        if (connectedTagDiscId_ != tagRfDiscIdList_[i]) {
            if (connectedTagDiscId_ == INVALID_VALUE) {
                // first connect
                status = TagNciAdapter::GetInstance().Connect(tagRfDiscIdList_[i],
                    tagActivatedProtocols_[i], tagTechList_[i]);
            } else {
                bool reResult = TagNciAdapter::GetInstance().Reconnect(tagRfDiscIdList_[i],
                    tagActivatedProtocols_[i], tagTechList_[i], false);
                status = reResult ? NFA_STATUS_OK : NFA_STATUS_FAILED;
            }
        } else {
            if (technology == static_cast<int>(KITS::TagTechnology::NFC_NDEF_TECH)) {
                // special for ndef
                i = 0;
            }
            bool reResult = TagNciAdapter::GetInstance().Reconnect(tagRfDiscIdList_[i],
                tagActivatedProtocols_[i], tagTechList_[i], false);
            status = reResult ? NFA_STATUS_OK : NFA_STATUS_FAILED;
        }
        if (status == NFA_STATUS_OK) {
            connectedTagDiscId_ = tagRfDiscIdList_[i];
            connectedTechIndex_ = static_cast<int>(i);
            isTagFieldOn_ = true;
            result = true;
        }
        break;
    }
    ResumeFieldChecking();
    DebugLog("TagHost::Connect exit, result = %{public}d", result);
    return result;
}

bool TagHost::Disconnect()
{
    DebugLog("TagHost::Disconnect");
    std::lock_guard<std::mutex> lock(mutex_);
    connectedTagDiscId_ = INVALID_VALUE;
    connectedTechIndex_ = INVALID_VALUE;
    isTagFieldOn_ = false;
    bool result = TagNciAdapter::GetInstance().Disconnect();
    {
        NFC::SynchronizeGuard guard(fieldCheckWatchDog_);
        fieldCheckWatchDog_.NotifyOne();
    }
    DebugLog("TagHost::Disconnect exit, result = %{public}d", result);
    return result;
}

bool TagHost::Reconnect()
{
    DebugLog("TagHost::Reconnect");
    if (connectedTechIndex_ == INVALID_VALUE) {
        return true;
    }
    PauseFieldChecking();
    std::lock_guard<std::mutex> lock(mutex_);
    bool result = TagNciAdapter::GetInstance().Reconnect(tagRfDiscIdList_[connectedTechIndex_],
        tagActivatedProtocols_[connectedTechIndex_], tagTechList_[connectedTechIndex_], false);
    ResumeFieldChecking();
    DebugLog("TagHost::Reconnect exit, result = %{public}d", result);
    return result;
}

int TagHost::Transceive(std::string& request, std::string& response)
{
    DebugLog("TagHost::Transceive");
    PauseFieldChecking();
    std::lock_guard<std::mutex> lock(mutex_);
    int status = TagNciAdapter::GetInstance().Transceive(request, response);
    ResumeFieldChecking();
    DebugLog("TagHost::Transceive exit, result = %{public}d", status);
    return status;
}

bool TagHost::FieldOnCheckingThread()
{
    DebugLog("TagHost::FieldOnCheckingThread");
    PauseFieldChecking();
    std::lock_guard<std::mutex> lock(mutex_);
    isTagFieldOn_ = TagNciAdapter::GetInstance().IsTagFieldOn();
    ResumeFieldChecking();
    return isTagFieldOn_;
}

bool TagHost::IsTagFieldOn()
{
    DebugLog("TagHost::IsTagFieldOn, result = %{public}d", isTagFieldOn_);
    return isTagFieldOn_;
}

void TagHost::PauseFieldChecking()
{
    isPauseFieldChecking_ = true;
}

void TagHost::ResumeFieldChecking()
{
    isPauseFieldChecking_ = false;
}

void TagHost::FieldCheckingThread(TagHost::TagDisconnectedCallBack callback, int delayedMs)
{
    DebugLog("FieldCheckingThread::Start Field Checking");
    while (isFieldChecking_) {
        NFC::SynchronizeGuard guard(fieldCheckWatchDog_);
        bool isNotify = fieldCheckWatchDog_.Wait(delayedMs);
        if (isNotify || !isTagFieldOn_) {
            break;
        }
        if (isPauseFieldChecking_) {
            continue;
        }
        bool result = TagNciAdapter::GetInstance().IsTagFieldOn();
        if (!result) {
            DebugLog("FieldCheckingThread::Tag lost...");
            break;
        }
    }
    isTagFieldOn_ = false;
    TagNciAdapter::GetInstance().ResetTag();
    TagNciAdapter::GetInstance().Disconnect();
    if (callback != nullptr && isFieldChecking_ && tagRfDiscIdList_.size() > 0) {
        DebugLog("FieldCheckingThread::Disconnect callback %{public}d", tagRfDiscIdList_[0]);
        callback(tagRfDiscIdList_[0]);
    }
    DebugLog("FieldCheckingThread::End Field Checking");
}

void TagHost::OnFieldChecking(TagDisconnectedCallBack callback, int delayedMs)
{
    DebugLog("TagHost::OnFieldChecking");
    isTagFieldOn_ = true;
    isFieldChecking_ = true;
    if (delayedMs <= 0) {
        delayedMs = DEFAULT_PRESENCE_CHECK_WATCH_DOG_TIMEOUT;
    }
    std::thread(&TagHost::FieldCheckingThread, this, callback, delayedMs).detach();
}

void TagHost::OffFieldChecking()
{
    DebugLog("TagHost::OffFieldChecking");
    isFieldChecking_ = false;
}

std::vector<int> TagHost::GetTechList()
{
    for (std::vector<int>::iterator it = tagTechList_.begin(); it != tagTechList_.end(); ++it) {
        KITS::TagTechnology technology = KITS::TagTechnology::NFC_INVALID_TECH;
        switch (*it) {
            case TARGET_TYPE_ISO14443_3A:
                technology = KITS::TagTechnology::NFC_A_TECH;
                break;

            case TARGET_TYPE_ISO14443_3B:
                technology = KITS::TagTechnology::NFC_B_TECH;
                break;

            case TARGET_TYPE_ISO14443_4:
                technology = KITS::TagTechnology::NFC_ISODEP_TECH;
                break;

            case TARGET_TYPE_FELICA:
                technology = KITS::TagTechnology::NFC_F_TECH;
                break;

            case TARGET_TYPE_V:
                technology = KITS::TagTechnology::NFC_V_TECH;
                break;

            case TARGET_TYPE_NDEF:
                technology = KITS::TagTechnology::NFC_NDEF_TECH;
                break;

            case TARGET_TYPE_NDEF_FORMATABLE:
                technology = KITS::TagTechnology::NFC_NDEF_FORMATABLE_TECH;
                break;

            case TARGET_TYPE_MIFARE_CLASSIC:
                technology = KITS::TagTechnology::NFC_MIFARE_CLASSIC_TECH;
                break;

            case TARGET_TYPE_MIFARE_UL:
                technology = KITS::TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH;
                break;

            case TARGET_TYPE_UNKNOWN:
            default:
                technology = KITS::TagTechnology::NFC_INVALID_TECH;
                break;
        }
        technologyList_.push_back(static_cast<int>(technology));
    }
    return technologyList_;
}

void TagHost::RemoveTech(int tech)
{
    DebugLog("TagHost::RemoveTech");
    if (tech == INVALID_VALUE) {
        DebugLog("Remove all");
    }
}

std::string TagHost::GetTagUid()
{
    return tagUid_;
}

void TagHost::DoTargetTypeIso144433a(AppExecFwk::PacMap &pacMap, int index)
{
    std::string act = tagActivatedBytes_[index];
    std::string poll = tagPollBytes_[index];
    if (!(act.empty())) {
        int sak = (KITS::NfcSdkCommon::GetByteFromHexStr(act, 0) & 0xff);
        pacMap.PutIntValue(KITS::TagInfo::SAK, sak);
        DebugLog("DoTargetTypeIso144433a SAK: 0x%{public}X", sak);
    }
    pacMap.PutStringValue(KITS::TagInfo::ATQA, poll);
    DebugLog("DoTargetTypeIso144433a ATQA: %{public}s", poll.c_str());
}

void TagHost::DoTargetTypeIso144433b(AppExecFwk::PacMap &pacMap, int index)
{
    std::string poll = tagPollBytes_[index];
    if (poll.empty()) {
        DebugLog("DoTargetTypeIso144433b poll empty");
        return;
    }

    if (KITS::NfcSdkCommon::GetHexStrBytesLen(poll) < NCI_APP_DATA_LENGTH + NCI_PROTOCOL_INFO_LENGTH) {
        DebugLog("DoTargetTypeIso144433b poll.len: %{public}d", KITS::NfcSdkCommon::GetHexStrBytesLen(poll));
        return;
    }

    std::string appData = poll.substr(0, NCI_APP_DATA_LENGTH);
    pacMap.PutStringValue(KITS::TagInfo::APP_DATA, appData);
    DebugLog("ParseTechExtras::TARGET_TYPE_ISO14443_3B APP_DATA: %{public}s", appData.c_str());

    std::string protoInfo = poll.substr(NCI_APP_DATA_LENGTH, NCI_PROTOCOL_INFO_LENGTH);
    pacMap.PutStringValue(KITS::TagInfo::PROTOCOL_INFO, protoInfo);
    DebugLog("ParseTechExtras::TARGET_TYPE_ISO14443_3B PROTOCOL_INFO: %{public}s", protoInfo.c_str());
}

void TagHost::DoTargetTypeIso144434(AppExecFwk::PacMap &pacMap, int index)
{
    bool hasNfcA = false;
    std::string act = tagActivatedBytes_[index];
    for (std::size_t i = 0; i < tagTechList_.size(); i++) {
        if (tagTechList_[i] == TARGET_TYPE_ISO14443_3A) {
            hasNfcA = true;
            break;
        }
    }
    if (hasNfcA) {
        pacMap.PutStringValue(KITS::TagInfo::HISTORICAL_BYTES, act);
        DebugLog("DoTargetTypeIso144434::HISTORICAL_BYTES: %{public}s", act.c_str());
    } else {
        pacMap.PutStringValue(KITS::TagInfo::HILAYER_RESPONSE, act);
        DebugLog("DoTargetTypeIso144434::HILAYER_RESPONSE: %{public}s", act.c_str());
    }
}

void TagHost::DoTargetTypeV(AppExecFwk::PacMap &pacMap, int index)
{
    std::string poll = tagPollBytes_[index];
    if (poll.empty()) {
        DebugLog("DoTargetTypeV poll empty");
        return;
    }

    if (KITS::NfcSdkCommon::GetHexStrBytesLen(poll) < NCI_POLL_LENGTH_MIN) {
        DebugLog("DoTargetTypeV poll.len: %{public}d", KITS::NfcSdkCommon::GetHexStrBytesLen(poll));
        return;
    }

    // 1st byte is response flag, 2nd byte is dsf id.
    pacMap.PutIntValue(KITS::TagInfo::RESPONSE_FLAGS, KITS::NfcSdkCommon::GetByteFromHexStr(poll, 0));
    DebugLog("DoTargetTypeV::RESPONSE_FLAGS: %{public}d", KITS::NfcSdkCommon::GetByteFromHexStr(poll, 0));
    pacMap.PutIntValue(KITS::TagInfo::DSF_ID, KITS::NfcSdkCommon::GetByteFromHexStr(poll, 1));
    DebugLog("DoTargetTypeV::DSF_ID: %{public}d", KITS::NfcSdkCommon::GetByteFromHexStr(poll, 1));
}

void TagHost::DoTargetTypeF(AppExecFwk::PacMap &pacMap, int index)
{
    std::string poll = tagPollBytes_[index];
    if (poll.empty()) {
        DebugLog("DoTargetTypeF poll empty");
        return;
    }

    if (KITS::NfcSdkCommon::GetHexStrBytesLen(poll) < SENSF_RES_LENGTH) {
        DebugLog("DoTargetTypeF no ppm, poll.len: %{public}d", KITS::NfcSdkCommon::GetHexStrBytesLen(poll));
        return;
    }
    pacMap.PutStringValue(KITS::TagInfo::NFCF_PMM, poll.substr(0, SENSF_RES_LENGTH)); // 8 bytes for ppm

    if (KITS::NfcSdkCommon::GetHexStrBytesLen(poll) < F_POLL_LENGTH) {
        DebugLog("DoTargetTypeF no sc, poll.len: %{public}d", KITS::NfcSdkCommon::GetHexStrBytesLen(poll));
        return;
    }
    pacMap.PutStringValue(KITS::TagInfo::NFCF_SC, poll.substr(SENSF_RES_LENGTH, 2)); // 2 bytes for sc
}

void TagHost::DoTargetTypeNdef(AppExecFwk::PacMap &pacMap)
{
    DebugLog("DoTargetTypeNdef");
    pacMap = ndefExtras_;
    ndefExtras_.Clear();
}

AppExecFwk::PacMap TagHost::ParseTechExtras(int index)
{
    AppExecFwk::PacMap pacMap;
    int targetType = tagTechList_[index];
    DebugLog("ParseTechExtras::targetType: %{public}d", targetType);
    switch (targetType) {
        case TARGET_TYPE_MIFARE_CLASSIC:
            break;

        case TARGET_TYPE_ISO14443_3A: {
            DoTargetTypeIso144433a(pacMap, index);
            break;
        }

        case TARGET_TYPE_ISO14443_3B: {
            DoTargetTypeIso144433b(pacMap, index);
            break;
        }

        case TARGET_TYPE_ISO14443_4: {
            DoTargetTypeIso144434(pacMap, index);
            break;
        }

        case TARGET_TYPE_V: {
            DoTargetTypeV(pacMap, index);
            break;
        }

        case TARGET_TYPE_MIFARE_UL: {
            bool isUlC = IsUltralightC();
            pacMap.PutBooleanValue(KITS::TagInfo::MIFARE_ULTRALIGHT_C_TYPE, isUlC);
            DebugLog("ParseTechExtras::TARGET_TYPE_MIFARE_UL MIFARE_ULTRALIGHT_C_TYPE: %{public}d", isUlC);
            break;
        }

        case TARGET_TYPE_FELICA: {
            DoTargetTypeF(pacMap, index);
            break;
        }

        case TARGET_TYPE_NDEF: {
            DoTargetTypeNdef(pacMap);
            break;
        }

        case TARGET_TYPE_NDEF_FORMATABLE:
            break;

        case TARGET_TYPE_UNKNOWN:
            break;

        default:
            DebugLog("ParseTechExtras::unhandle for : %{public}d", targetType);
            break;
    }
    return pacMap;
}

std::vector<AppExecFwk::PacMap> TagHost::GetTechExtrasData()
{
    DebugLog("TagHost::GetTechExtrasData, tech len.%{public}zu", tagTechList_.size());
    techExtras_.clear();
    for (std::size_t i = 0; i < tagTechList_.size(); i++) {
        AppExecFwk::PacMap extra = ParseTechExtras(i);
        techExtras_.push_back(extra);
    }
    return techExtras_;
}

int TagHost::GetTagRfDiscId()
{
    if (tagTechList_.size() > 0) {
        return tagRfDiscIdList_[0];
    }
    return 0;
}

bool TagHost::SetNdefReadOnly()
{
    DebugLog("TagHost::SetNdefReadOnly");
    PauseFieldChecking();
    std::lock_guard<std::mutex> lock(mutex_);
    bool result = TagNciAdapter::GetInstance().SetReadOnly();
    ResumeFieldChecking();
    return result;
}

std::string TagHost::ReadNdef()
{
    DebugLog("TagHost::ReadNdef");
    PauseFieldChecking();
    std::string response = "";
    this->AddNdefTech();
    std::lock_guard<std::mutex> lock(mutex_);
    TagNciAdapter::GetInstance().ReadNdef(response);
    ResumeFieldChecking();
    return response;
}

void TagHost::AddNdefTech()
{
    if (addNdefTech_) {
        return;
    }
    addNdefTech_ = true;
    DebugLog("TagHost::AddNdefTech");
    std::lock_guard<std::mutex> lock(mutex_);
    bool foundFormat = false;
    int formatHandle = 0;
    int formatLibNfcType = 0;
    uint32_t index = tagTechList_.size();
    for (uint32_t i = 0; i < index; i++) {
        TagNciAdapter::GetInstance().Reconnect(tagRfDiscIdList_[i], tagActivatedProtocols_[i], tagTechList_[i], false);

        if (!foundFormat) {
            if (TagNciAdapter::GetInstance().IsNdefFormattable()) {
                formatHandle = tagRfDiscIdList_[i];
                formatLibNfcType = tagActivatedProtocols_[i];
                foundFormat = true;
            }
            Reconnect();
        }
        std::vector<int> ndefInfo;
        if (TagNciAdapter::GetInstance().IsNdefMsgContained(ndefInfo)) {
            if (ndefInfo.size() < NDEF_INFO_SIZE) {
                WarnLog("TagHost::AddNdefTech, invalid size = %{public}zu", ndefInfo.size());
                return;
            }
            DebugLog("Add ndef tag info, index: %{public}d", index);
            // parse extras data for ndef tech.
            AppExecFwk::PacMap pacMap;
            std::string ndefMsg = "";
            TagNciAdapter::GetInstance().ReadNdef(ndefMsg);
            pacMap.PutStringValue(KITS::TagInfo::NDEF_MSG, ndefMsg);
            pacMap.PutIntValue(KITS::TagInfo::NDEF_FORUM_TYPE, GetNdefType(tagActivatedProtocols_[i]));
            DebugLog("ParseTechExtras::TARGET_TYPE_NDEF NDEF_FORUM_TYPE: %{public}d",
                GetNdefType(tagActivatedProtocols_[i]));
            pacMap.PutIntValue(KITS::TagInfo::NDEF_TAG_LENGTH, ndefInfo[NDEF_SIZE_INDEX]);
            pacMap.PutIntValue(KITS::TagInfo::NDEF_TAG_MODE, ndefInfo[NDEF_MODE_INDEX]);
            DebugLog("ParseTechExtras::TARGET_TYPE_NDEF NDEF_TAG_MODE: %{public}d", ndefInfo[1]);

            AddNdefTechToTagInfo(TARGET_TYPE_NDEF, tagRfDiscIdList_[i], tagActivatedProtocols_[i], pacMap);
            foundFormat = false;
            break;
        }
    }
    if (foundFormat) {
        DebugLog("Add ndef formatable tag info, index: %{public}d", index);
        AppExecFwk::PacMap pacMap;
        AddNdefTechToTagInfo(TARGET_TYPE_NDEF_FORMATABLE, formatHandle, formatLibNfcType, pacMap);
    }
}

void TagHost::AddNdefTechToTagInfo(int tech, int discId, int actProto, AppExecFwk::PacMap pacMap)
{
    InfoLog("AddNdefTechToTagInfo: tech = %{public}d", tech);
    tagTechList_.push_back(tech);
    tagRfDiscIdList_.push_back(discId);
    tagActivatedProtocols_.push_back(actProto);
    ndefExtras_ = pacMap; // techExtras_ will be handled in ParseTechExtras()
}

int TagHost::GetNdefType(int protocol) const
{
    int ndefType;
    if (NFA_PROTOCOL_T1T == protocol) {
        ndefType = NDEF_TYPE1_TAG;
    } else if (NFA_PROTOCOL_T2T == protocol) {
        ndefType = NDEF_TYPE2_TAG;
    } else if (NFA_PROTOCOL_T3T == protocol) {
        ndefType = NDEF_TYPE3_TAG;
    } else if (NFA_PROTOCOL_ISO_DEP == protocol) {
        ndefType = NDEF_TYPE4_TAG;
    } else if (NFC_PROTOCOL_MIFARE == protocol) {
        ndefType = NDEF_MIFARE_CLASSIC_TAG;
    } else {
        /* NFA_PROTOCOL_T5T, NFA_PROTOCOL_INVALID and others */
        ndefType = NDEF_UNKNOWN_TYPE;
    }
    return ndefType;
}

bool TagHost::WriteNdef(std::string& data)
{
    DebugLog("TagHost::WriteNdef");
    PauseFieldChecking();
    std::lock_guard<std::mutex> lock(mutex_);
    bool result = TagNciAdapter::GetInstance().WriteNdef(data);
    ResumeFieldChecking();
    DebugLog("TagHost::WriteNdef exit, result = %{public}d", result);
    return result;
}

bool TagHost::FormatNdef(const std::string& key)
{
    DebugLog("TagHost::FormatNdef");
    if (key.empty()) {
        DebugLog("key is null");
        return false;
    }
    PauseFieldChecking();
    std::lock_guard<std::mutex> lock(mutex_);
    bool result = TagNciAdapter::GetInstance().FormatNdef();
    ResumeFieldChecking();
    DebugLog("TagHost::FormatNdef exit, result = %{public}d", result);
    return result;
}

bool TagHost::IsNdefFormatable()
{
    DebugLog("TagHost::IsNdefFormatable");
    bool result = TagNciAdapter::GetInstance().IsNdefFormatable();
    DebugLog("TagHost::IsNdefFormatable exit, result = %{public}d", result);
    return result;
}

bool TagHost::IsNdefMsgContained(std::vector<int>& ndefInfo)
{
    DebugLog("TagHost::IsNdefMsgContained");
    PauseFieldChecking();
    std::lock_guard<std::mutex> lock(mutex_);
    bool result = TagNciAdapter::GetInstance().IsNdefMsgContained(ndefInfo);
    ResumeFieldChecking();
    if (result) {
        DebugLog("NDEF supported by the tag");
    } else {
        DebugLog("NDEF unsupported by the tag");
    }
    return result;
}

int TagHost::GetConnectedTech()
{
    DebugLog("TagHost::GetConnectedTech");
    if (connectedTechIndex_ != INVALID_VALUE) {
        return tagTechList_[connectedTechIndex_];
    }
    return 0;
}

bool TagHost::IsUltralightC()
{
    PauseFieldChecking();
    std::lock_guard<std::mutex> lock(mutex_);
    bool result = false;

    // read the date content of speci addressed pages, see MIFARE Ultralight C
    std::string command = "3002"; // 0x30 for mifare read, 0x02 for page address
    std::string response;
    TagNciAdapter::GetInstance().Transceive(command, response);
    if (KITS::NfcSdkCommon::GetHexStrBytesLen(response) == NCI_MIFARE_ULTRALIGHT_C_RESPONSE_LENGTH) {
        if (response[DATA_BYTE2] == NCI_MIFARE_ULTRALIGHT_C_BLANK_CARD &&
            response[DATA_BYTE3] == NCI_MIFARE_ULTRALIGHT_C_BLANK_CARD &&
            response[DATA_BYTE4] == NCI_MIFARE_ULTRALIGHT_C_BLANK_CARD &&
            response[DATA_BYTE5] == NCI_MIFARE_ULTRALIGHT_C_BLANK_CARD &&
            response[DATA_BYTE6] == NCI_MIFARE_ULTRALIGHT_C_BLANK_CARD &&
            response[DATA_BYTE7] == NCI_MIFARE_ULTRALIGHT_C_BLANK_CARD &&
            response[DATA_BYTE8] == NCI_MIFARE_ULTRALIGHT_C_VERSION_INFO_FIRST &&
            response[DATA_BYTE9] == NCI_MIFARE_ULTRALIGHT_C_VERSION_INFO_SECOND) {
            result = true;
        } else if (response[DATA_BYTE4] == NCI_MIFARE_ULTRALIGHT_C_NDEF_CC &&
                   ((response[DATA_BYTE5] & 0xff) < NCI_MIFARE_ULTRALIGHT_C_NDEF_MAJOR_VERSION) &&
                   ((response[DATA_BYTE6] & 0xff) > NCI_MIFARE_ULTRALIGHT_C_NDEF_TAG_SIZE)) {
            result = true;
        }
    }
    ResumeFieldChecking();
    return result;
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
