/*
 * Copyright (C) 2022 - 2023 Huawei Device Co., Ltd.
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
#include "taginfo_parcelable.h"
#include "loghelper.h"
#include "parcel.h"
#include "refbase.h"

namespace OHOS {
namespace NFC {
namespace KITS {
const uint32_t MAX_TECH_LIST_NUM = 12;

TagInfoParcelable::TagInfoParcelable(std::vector<int> tagTechList,
                                     std::vector<AppExecFwk::PacMap> tagTechExtrasData,
                                     std::string& tagUid,
                                     int tagRfDiscId,
                                     OHOS::sptr<IRemoteObject> tagServiceIface)
{
    tagRfDiscId_ = tagRfDiscId;
    tagUid_ = tagUid;
    tagTechList_ = std::move(tagTechList);
    tagServiceIface_ = tagServiceIface;
    tagTechExtrasData_ = std::move(tagTechExtrasData);
}

TagInfoParcelable::~TagInfoParcelable()
{
    tagUid_.clear();
    tagTechList_.clear();
    tagTechExtrasData_.clear();
    tagRfDiscId_ = 0;
    tagServiceIface_ = nullptr;
}

bool TagInfoParcelable::Marshalling(Parcel &parcel) const
{
    const std::vector<int> tagTechList = std::move(tagTechList_);
    parcel.WriteInt32Vector(tagTechList);
    parcel.WriteInt32(tagTechExtrasData_.size());
    for (unsigned int i = 0; i < tagTechExtrasData_.size(); i++) {
        tagTechExtrasData_[i].Marshalling(parcel);
    }
    parcel.WriteString(tagUid_);
    parcel.WriteInt32(tagRfDiscId_);
    return true;
}

TagInfoParcelable *TagInfoParcelable::Unmarshalling(Parcel &parcel)
{
    std::vector<int> tagTechList;
    parcel.ReadInt32Vector(&tagTechList);

    int32_t extraLen = 0;
    parcel.ReadInt32(extraLen);
    if (extraLen >= MAX_TECH_LIST_NUM) {
        return nullptr;
    }
    std::vector<AppExecFwk::PacMap> tagTechExtrasData;
    for (int i = 0; i < extraLen; i++) {
        AppExecFwk::PacMap* pacMap = AppExecFwk::PacMap::Unmarshalling(parcel);
        tagTechExtrasData.push_back(*(pacMap));
    }
    std::string tagUid;
    parcel.ReadString(tagUid);

    int tagRfDiscId;
    parcel.ReadInt32(tagRfDiscId);
    TagInfoParcelable *taginfo = new (std::nothrow) TagInfoParcelable(tagTechList, tagTechExtrasData,
        tagUid, tagRfDiscId, nullptr);
    return taginfo;
}

std::string TagInfoParcelable::GetUid()
{
    return tagUid_;
}

std::vector<int> TagInfoParcelable::GetTechList()
{
    return tagTechList_;
}

int TagInfoParcelable::GetDiscId()
{
    return tagRfDiscId_;
}

std::vector<AppExecFwk::PacMap> TagInfoParcelable::GetTechExtrasDataList()
{
    return tagTechExtrasData_;
}

std::string TagInfoParcelable::ToString()
{
    std::string res = "tagTechList: [";
    if (tagTechList_.size() <= 0) {
        res += "]";
        return res;
    }
    for (uint32_t i = 0; i < tagTechList_.size() - 1; i++) {
        res += std::to_string(tagTechList_[i]);
        res += ", ";
    }
    res += std::to_string(tagTechList_[tagTechList_.size() - 1]);
    res += "]";
    return res;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS