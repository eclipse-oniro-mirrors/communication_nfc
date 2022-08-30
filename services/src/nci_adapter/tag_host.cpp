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

#include <memory>
#include <thread>

#include "loghelper.h"
#include "nfa_api.h"
#include "nfc_sdk_common.h"
#include "tag_nci_adapter.h"
#include "taginfo.h"

namespace OHOS {
namespace NFC {
namespace NCI {
OHOS::NFC::SynchronizeEvent TagHost::filedCheckWatchDog_;
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
      techExtras_(std::make_shared<AppExecFwk::PacMap>()),
      tagPollBytes_(std::move(tagPollBytes)),
      tagActivatedBytes_(std::move(tagActivatedBytes)),
      connectedTagDiscId_(-1),
      connectedTechIndex_(-1),
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
            if (connectedTagDiscId_ == -1) {
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
            connectedTechIndex_ = i;
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
    connectedTagDiscId_ = -1;
    connectedTechIndex_ = -1;
    isTagFieldOn_ = false;
    bool result = TagNciAdapter::GetInstance().Disconnect();
    {
        NFC::SynchronizeGuard guard(filedCheckWatchDog_);
        filedCheckWatchDog_.NotifyOne();
    }
    DebugLog("TagHost::Disconnect exit, result = %{public}d", result);
    return result;
}

bool TagHost::Reconnect()
{
    DebugLog("TagHost::Reconnect");
    if (connectedTechIndex_ == -1) {
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

void TagHost::FiledCheckingThread(TagHost::TagDisconnectedCallBack callback, int delayedMs)
{
    DebugLog("FiledCheckingThread::Start Filed Checking");
    while (isFieldChecking_) {
        NFC::SynchronizeGuard guard(filedCheckWatchDog_);
        bool isNotify = filedCheckWatchDog_.Wait(delayedMs);
        if (isNotify || !isTagFieldOn_) {
            break;
        }
        if (isPauseFieldChecking_) {
            continue;
        }
        bool result = TagNciAdapter::GetInstance().IsTagFieldOn();
        if (!result) {
            DebugLog("FiledCheckingThread::Tag lost...");
            break;
        }
    }
    isTagFieldOn_ = false;
    TagNciAdapter::GetInstance().Disconnect();
    if (callback != nullptr && isFieldChecking_ && tagRfDiscIdList_.size() > 0) {
        DebugLog("FiledCheckingThread::Disconnect callback %{public}d", tagRfDiscIdList_[0]);
        callback(tagRfDiscIdList_[0]);
    }
    DebugLog("FiledCheckingThread::End Filed Checking");
}

void TagHost::OnFieldChecking(TagDisconnectedCallBack callback, int delayedMs)
{
    DebugLog("TagHost::OnFieldChecking");
    isTagFieldOn_ = true;
    isFieldChecking_ = true;
    if (delayedMs <= 0) {
        delayedMs = DEFAULT_PRESENCE_CHECK_WATCH_DOG_TIMEOUT;
    }
    std::thread(&TagHost::FiledCheckingThread, this, callback, delayedMs).detach();
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

            case TARGET_TYPE_KOVIO_BARCODE:
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
    if (tech == -1) {
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
        int sak = (act.at(0) & (0xff));
        pacMap.PutLongValue(KITS::TagInfo::SAK, sak);
        DebugLog("DoTargetTypeIso144433a SAK: %{public}d", sak);
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

    if (poll.length() < NCI_APP_DATA_LENGTH + NCI_PROTOCOL_INFO_LENGTH) {
        DebugLog("DoTargetTypeIso144433b poll.len: %{public}d", (int)poll.length());
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

    if (poll.length() < NCI_POLL_LENGTH_MIN) {
        DebugLog("DoTargetTypeV poll.len: %{public}d", (int)poll.length());
        return;
    }

    pacMap.PutLongValue(KITS::TagInfo::RESPONSE_FLAGS, poll.at(0));
    DebugLog("DoTargetTypeV::RESPONSE_FLAGS: %{public}d", poll.at(0));
    pacMap.PutLongValue(KITS::TagInfo::DSF_ID, poll.at(1));
    DebugLog("DoTargetTypeV::DSF_ID: %{public}d", poll.at(1));
}

void TagHost::DoTargetTypeF(AppExecFwk::PacMap &pacMap, int index)
{
    std::string poll = tagPollBytes_[index];
    if (poll.empty()) {
        DebugLog("DoTargetTypeF poll empty");
        return;
    }

    if (poll.length() < SENSF_RES_LENGTH) {
        DebugLog("DoTargetTypeF no ppm, poll.len: %{public}d", (int)poll.length());
        return;
    }
    pacMap.PutStringValue(KITS::TagInfo::NFCF_PMM, poll.substr(0, SENSF_RES_LENGTH)); // 8 bytes for ppm

    if (poll.length() < F_POLL_LENGTH) {
        DebugLog("DoTargetTypeF no sc, poll.len: %{public}d", (int)poll.length());
        return;
    }
    pacMap.PutStringValue(KITS::TagInfo::NFCF_SC, poll.substr(SENSF_RES_LENGTH, 2)); // 2 bytes for sc
}

AppExecFwk::PacMap TagHost::ParseTechExtras(int index)
{
    AppExecFwk::PacMap pacMap;
    int targetType = tagTechList_[index];
    DebugLog("ParseTechExtras::targetType: %{public}d", targetType);
    switch (targetType) {
        case TARGET_TYPE_MIFARE_CLASSIC:
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
            pacMap.PutLongValue(KITS::TagInfo::MIFARE_ULTRALIGHT_C_TYPE, isUlC);
            DebugLog("ParseTechExtras::TARGET_TYPE_MIFARE_UL MIFARE_ULTRALIGHT_C_TYPE: %{public}d", isUlC);
            break;
        }

        case TARGET_TYPE_FELICA: {
            DoTargetTypeF(pacMap, index);
            break;
        }
        default:
            DebugLog("ParseTechExtras::unhandle for : %{public}d", targetType);
            break;
    }
    return pacMap;
}

std::weak_ptr<AppExecFwk::PacMap> TagHost::GetTechExtrasData()
{
    DebugLog("TagHost::GetTechExtrasData, tech len.%{public}d", (int)tagTechList_.size());
    for (std::size_t i = 0; i < tagTechList_.size(); i++) {
        if (tagTechList_[i] == TARGET_TYPE_NDEF || tagTechList_[i] == TARGET_TYPE_NDEF_FORMATABLE) {
            continue;
        }
        AppExecFwk::PacMap extra = ParseTechExtras(i);
        if (!(extra.IsEmpty())) {
            techExtras_->PutPacMap(KITS::TagInfo::TECH_EXTRA_DATA_PREFIX + std::to_string(i), extra);
        }
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
    int targetTypeNdef = TARGET_TYPE_NDEF;
    int targetTypeNdefFormatable = TARGET_TYPE_NDEF_FORMATABLE;
    uint32_t index = tagTechList_.size();
    for (uint32_t i = 0; i < index; i++) {
        TagNciAdapter::GetInstance().Reconnect(tagRfDiscIdList_[i], tagActivatedProtocols_[i], tagTechList_[i], false);
        std::vector<int> ndefInfo;
        if (TagNciAdapter::GetInstance().IsNdefMsgContained(ndefInfo)) {
            DebugLog("Add ndef tag info, index: %{public}d", index);
            tagTechList_.push_back(targetTypeNdef);
            tagRfDiscIdList_.push_back(tagRfDiscIdList_[i]);
            tagActivatedProtocols_.push_back(tagActivatedProtocols_[i]);

            AppExecFwk::PacMap pacMap;
            std::string ndefMsg = "";
            TagNciAdapter::GetInstance().ReadNdef(ndefMsg);
            pacMap.PutStringValue(KITS::TagInfo::NDEF_MSG, ndefMsg);
            pacMap.PutLongValue(KITS::TagInfo::NDEF_FORUM_TYPE, GetNdefType(tagActivatedProtocols_[i]));
            DebugLog("ParseTechExtras::TARGET_TYPE_NDEF NDEF_FORUM_TYPE: %{public}d",
                GetNdefType(tagActivatedProtocols_[i]));
            pacMap.PutLongValue("NDEF_TAG_LENGTH", ndefInfo[0]);
            pacMap.PutLongValue(KITS::TagInfo::NDEF_TAG_MODE, ndefInfo[1]);
            DebugLog("ParseTechExtras::TARGET_TYPE_NDEF NDEF_TAG_MODE: %{public}d", ndefInfo[1]);
            techExtras_->PutPacMap(KITS::TagInfo::TECH_EXTRA_DATA_PREFIX + std::to_string(index), pacMap);

            foundFormat = false;
            break;
        }
        if (!foundFormat && TagNciAdapter::GetInstance().IsNdefFormattable()) {
            formatHandle = tagRfDiscIdList_[i];
            formatLibNfcType = tagActivatedProtocols_[i];
            foundFormat = true;
        }
    }
    if (foundFormat) {
        DebugLog("Add ndef formatable tag info, index: %{public}d", index);
        tagTechList_.push_back(targetTypeNdefFormatable);
        tagRfDiscIdList_.push_back(formatHandle);
        tagActivatedProtocols_.push_back(formatLibNfcType);
    }
}

int TagHost::GetNdefType(int protocol) const
{
    int ndefType = NDEF_UNKNOWN_TYPE;
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
    if (connectedTechIndex_ != -1) {
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
    const unsigned char MIFARE_READ = 0x30;
    const unsigned char PAGE_ADDR = 0x02;
    std::string command = {MIFARE_READ, PAGE_ADDR};
    std::string response;
    TagNciAdapter::GetInstance().Transceive(command, response);
    if (!(response.empty()) && response.length() == NCI_MIFARE_ULTRALIGHT_C_RESPONSE_LENGTH) {
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
