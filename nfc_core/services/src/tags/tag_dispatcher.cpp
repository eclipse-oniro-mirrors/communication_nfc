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

#include "itag_host.h"
#include "loghelper.h"

namespace OHOS {
using TagHostMapIter = std::map<int, std::shared_ptr<NFC::NCI::ITagHost>>::iterator;
namespace NFC {
namespace TAG {
/**
 * @brief Find the TagHost by the rfDiscId
 * @param key the rfDiscId
 * @return the TagHost
 */
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
/**
 * @brief Find and remove the TagHost by rfDiscId.
 */
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
/**
 * @brief Register the TagHost Object
 * @param tag the TagHost
 */
void TagDispatcher::RegisterTagHost(std::shared_ptr<NCI::ITagHost> tag)
{
    std::lock_guard<std::mutex> lock(mutex_);
    tagHostMap_.insert(make_pair(tag->GetTagRfDiscId(), tag));
}
/**
 * @brief Unregister the TagHost Object
 * @param handle the TagHost rfDiscId
 */
void TagDispatcher::UnregisterTagHost(int rfDiscId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    tagHostMap_.erase(rfDiscId);
}

TagDispatcher::TagDispatcher(std::weak_ptr<NFC::INfcService> nfcService)
    : nfcService_(nfcService)
{
}

TagDispatcher::~TagDispatcher()
{
    std::lock_guard<std::mutex> guard(mutex_);
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
