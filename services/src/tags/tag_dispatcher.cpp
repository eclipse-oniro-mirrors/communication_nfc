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
#include "tag_dispatcher.h"
#include "ability_manager_client.h"
#include "app_data_parser.h"
#include "itag_host.h"
#include "loghelper.h"
#include "want.h"

namespace OHOS {
using TagHostMapIter = std::map<int, std::shared_ptr<NFC::NCI::ITagHost>>::iterator;
namespace NFC {
namespace TAG {
using OHOS::NFC::KITS::TagTechnology;
TagDispatcher::TagDispatcher(std::shared_ptr<NFC::INfcService> nfcService)
    : nfcService_(nfcService)
{
}

TagDispatcher::~TagDispatcher()
{
    std::lock_guard<std::mutex> guard(mutex_);
}

int TagDispatcher::HandleTagFound(std::shared_ptr<NCI::ITagHost> tag)
{
    DebugLog("HandleTagFound, unimplimentation...");
    return 0;
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
        WarnLog("rfDiscId not found");
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
        WarnLog("rfDiscId not found");
    } else {
        temp = tagHost->second;
        tagHostMap_.erase(rfDiscId);
    }
    return temp;
}

void TagDispatcher::RegisterTagHost(std::shared_ptr<NCI::ITagHost> tag)
{
    std::lock_guard<std::mutex> lock(mutex_);
    tagHostMap_.insert(make_pair(tag->GetTagRfDiscId(), tag));
}

void TagDispatcher::UnregisterTagHost(int rfDiscId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    tagHostMap_.erase(rfDiscId);
}

void TagDispatcher::DispatchAbility(ElementName &element,
    std::shared_ptr<KITS::TagInfo> tagInfo)
{
    if (element.GetBundleName().empty() || tagInfo == nullptr) {
        ErrorLog("DispatchAbility element or tagInfo is null");
        return;
    }

    InfoLog("DispatchAbility for app %{public}s, ability = %{public}s", element.GetBundleName().c_str(),
        element.GetAbilityName().c_str());
    AAFwk::Want want;
    want.SetElement(element);
    want.SetParam("uid", tagInfo->GetTagUid());
    want.SetParam("technology", tagInfo->GetTagTechList());
    want.SetParam("tagRfDiscId", tagInfo->GetTagRfDiscId());
    want.SetParam("remoteTagService", nfcService_->GetTagServiceIface());

    // put extra data for all included technology
    std::vector<int> techList = tagInfo->GetTagTechList();
    for (size_t i = 0; i < techList.size(); i++) {
        AppExecFwk::PacMap extra = tagInfo->GetTechExtrasByIndex(i);
        if (techList[i] == static_cast<int>(TagTechnology::NFC_A_TECH)) {
            want.SetParam(KITS::TagInfo::SAK, extra.GetLongValue(KITS::TagInfo::SAK, 0));
            want.SetParam(KITS::TagInfo::ATQA, extra.GetStringValue(KITS::TagInfo::ATQA, ""));
        } else if (techList[i] == static_cast<int>(TagTechnology::NFC_ISODEP_TECH)) {
            want.SetParam(KITS::TagInfo::HISTORICAL_BYTES, extra.GetStringValue(KITS::TagInfo::HISTORICAL_BYTES, ""));
        } else {
        }
    }

    AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    InfoLog("DispatchAbility call StartAbility end ");
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
