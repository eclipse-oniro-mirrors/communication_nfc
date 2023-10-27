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
#include "tag_dispatcher.h"

#include <functional>

#include "app_data_parser.h"
#include "itag_host.h"
#include "loghelper.h"
#include "ndef_message.h"
#include "nfc_sdk_common.h"
#include "nfc_hisysevent.h"
#include "nfc_service.h"
#include "run_on_demaind_manager.h"
#include "tag_ability_dispatcher.h"

namespace OHOS {
using TagHostMapIter = std::map<int, std::shared_ptr<NFC::NCI::ITagHost>>::iterator;
namespace NFC {
namespace TAG {
using OHOS::NFC::KITS::TagTechnology;
TagDispatcher::TagDispatcher(std::shared_ptr<NFC::INfcService> nfcService)
    : nfcService_(nfcService),
    lastNdefMsg_("")
{
}

TagDispatcher::~TagDispatcher()
{
    std::lock_guard<std::mutex> guard(mutex_);
}

void TagDispatcher::TagDisconnectedCallback(int tagRfDiscId)
{
    UnregisterTagHost(tagRfDiscId);
    InfoLog("Tag disconnected");
}

int TagDispatcher::HandleTagFound(std::shared_ptr<NCI::ITagHost> tag)
{
    if (tag == nullptr || nfcService_ == nullptr || nfcService_->GetNfcPollingManager().expired()) {
        ErrorLog("HandleTagFound, invalid state.");
        return 0;
    }
    static NCI::ITagHost::TagDisconnectedCallBack callback =
        std::bind(&TagDispatcher::TagDisconnectedCallback, this, std::placeholders::_1);
    int fieldOnCheckInterval_ = DEFAULT_FIELD_ON_CHECK_DURATION;
    if (tag->GetConnectedTech() == static_cast<int>(TagTechnology::NFC_ISODEP_TECH)) {
        fieldOnCheckInterval_ = DEFAULT_ISO_DEP_FIELD_ON_CHECK_DURATION;
    }
    DebugLog("fieldOnCheckInterval_ = %{public}d", fieldOnCheckInterval_);

    std::string ndefMsg = tag->FindNdefTech();
    std::shared_ptr<KITS::NdefMessage> ndefMessage = KITS::NdefMessage::GetNdefMessage(ndefMsg);
    if (ndefMessage == nullptr) {
        if (!tag->Reconnect()) {
            tag->Disconnect();
            ErrorLog("HandleTagFound bad connection, tag disconnected");
            return 0;
        }
    }
    lastNdefMsg_ = ndefMsg;
    RegisterTagHost(tag);
    tag->OnFieldChecking(callback, fieldOnCheckInterval_);
    if (nfcService_->GetNfcPollingManager().lock()->IsForegroundEnabled()) {
        nfcService_->GetNfcPollingManager().lock()->SendTagToForeground(GetTagInfoParcelableFromTag(tag));
        return 0;
    }
    DispatchTag(tag);
    return 0;
}

std::shared_ptr<KITS::TagInfo> TagDispatcher::GetTagInfoFromTag(std::shared_ptr<NCI::ITagHost> tag)
{
    std::vector<int> techList = tag->GetTechList();
    std::string tagUid = tag->GetTagUid();
    std::vector<AppExecFwk::PacMap> tagTechExtras = tag->GetTechExtrasData();
    int tagRfDiscId = tag->GetTagRfDiscId();
    DebugLog("GetTagInfoFromTag: techListLen = %{public}zu, extrasLen = %{public}zu, tagUid = %{private}s,"
        " rfID = %{public}d", techList.size(), tagTechExtras.size(), tagUid.c_str(), tagRfDiscId);
    return std::make_shared<KITS::TagInfo>(techList, tagTechExtras, tagUid, tagRfDiscId,
        nfcService_->GetTagServiceIface());
}

KITS::TagInfoParcelable TagDispatcher::GetTagInfoParcelableFromTag(std::shared_ptr<NCI::ITagHost> tag)
{
    std::vector<int> techList = tag->GetTechList();
    std::string tagUid = tag->GetTagUid();
    std::vector<AppExecFwk::PacMap> tagTechExtras = tag->GetTechExtrasData();
    int tagRfDiscId = tag->GetTagRfDiscId();
    DebugLog("GetTagInfoParcelableFromTag: techListLen = %{public}zu, extrasLen = %{public}zu, tagUid = %{private}s,"
        " rfID = %{public}d", techList.size(), tagTechExtras.size(), tagUid.c_str(), tagRfDiscId);
    KITS::TagInfoParcelable *tagInfo = new (std::nothrow) KITS::TagInfoParcelable(techList, tagTechExtras,
        tagUid, tagRfDiscId, nfcService_->GetTagServiceIface());
    return *(tagInfo);
}

void TagDispatcher::DispatchTag(std::shared_ptr<NCI::ITagHost> tag)
{
    if (tag == nullptr) {
        ErrorLog("DispatchTag: tag is null");
        return;
    }
    std::shared_ptr<KITS::TagInfo> tagInfo = GetTagInfoFromTag(tag);
    if (tagInfo == nullptr) {
        ErrorLog("DispatchTag: taginfo is null");
        return;
    }

    // select the matched applications, try start ability
    std::vector<int> techList = tag->GetTechList();
    // Record types of read tags.
    int tagFoundCnt = 0;
    int typeATagFoundCnt = 0;
    int typeBTagFoundCnt = 0;
    int typeFTagFoundCnt = 0;
    int typeVTagFoundCnt = 0;
    for (size_t i = 0; i < techList.size(); i++) {
        std::string discStrTech = KITS::TagInfo::GetStringTech(techList[i]);
        if (discStrTech.compare("NfcA") == 0) {
            tagFoundCnt++;
            typeATagFoundCnt++;
        } else if (discStrTech.compare("NfcB") == 0) {
            tagFoundCnt++;
            typeBTagFoundCnt++;
        } else if (discStrTech.compare("NfcF") == 0) {
            tagFoundCnt++;
            typeFTagFoundCnt++;
        } else if (discStrTech.compare("NfcV") == 0) {
            tagFoundCnt++;
            typeVTagFoundCnt++;
        } else {
            tagFoundCnt++;
        }
    }
    RunOnDemaindManager::GetInstance().WriteTagFoundHiSysEvent(tagFoundCnt, typeATagFoundCnt,
        typeBTagFoundCnt, typeFTagFoundCnt, typeVTagFoundCnt);

    // start application ability for tag found.
    RunOnDemaindManager::GetInstance().DispatchTagAbility(tagInfo, nfcService_->GetTagServiceIface());
}

void TagDispatcher::HandleTagDebounce()
{
    DebugLog("HandleTagDebounce, unimplimentation...");
}

std::weak_ptr<NCI::ITagHost> TagDispatcher::FindTagHost(int rfDiscId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    TagHostMapIter tagHost = tagHostMap_.find(rfDiscId);
    if (tagHost == tagHostMap_.end()) {
        WarnLog("FindTagHost, rfDiscId: %{public}d not found", rfDiscId);
        return std::shared_ptr<NCI::ITagHost>();
    }
    return tagHost->second;
}

std::shared_ptr<NCI::ITagHost> TagDispatcher::FindAndRemoveTagHost(int rfDiscId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    TagHostMapIter tagHost = tagHostMap_.find(rfDiscId);
    std::shared_ptr<NCI::ITagHost> temp = nullptr;
    if (tagHost == tagHostMap_.end()) {
        WarnLog("FindAndRemoveTagHost, rfDiscId: %{public}d not found", rfDiscId);
    } else {
        temp = tagHost->second;
        tagHostMap_.erase(rfDiscId);
        InfoLog("FindAndRemoveTagHost, rfDiscId: %{public}d removed", rfDiscId);
    }
    return temp;
}

void TagDispatcher::RegisterTagHost(std::shared_ptr<NCI::ITagHost> tag)
{
    std::lock_guard<std::mutex> lock(mutex_);
    tagHostMap_.insert(make_pair(tag->GetTagRfDiscId(), tag));
    InfoLog("RegisterTagHost, rfDiscId: %{public}d", tag->GetTagRfDiscId());
}

void TagDispatcher::UnregisterTagHost(int rfDiscId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    tagHostMap_.erase(rfDiscId);
    InfoLog("UnregisterTagHost, rfDiscId: %{public}d", rfDiscId);
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
