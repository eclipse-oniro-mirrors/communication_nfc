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
#include "taginfo.h"

#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "parcel.h"
#include "refbase.h"
#include "tag_session_proxy.h"

namespace OHOS {
namespace NFC {
namespace KITS {
TagInfo::TagInfo(std::vector<int> tagTechList,
                 std::weak_ptr<AppExecFwk::PacMap> tagTechExtrasData,
                 std::string& tagUid,
                 int tagRfDiscId,
                 OHOS::sptr<TAG::ITagSession> tagSession)
    : tagRfDiscId_(tagRfDiscId),
      connectedTagTech_(KITS::TagTechnology::NFC_INVALID_TECH),
      tagUid_(tagUid),
      tagTechList_(std::move(tagTechList)),
      remoteTagSession_(tagSession),
      tagTechExtrasData_(tagTechExtrasData.lock())
{
}

TagInfo::~TagInfo()
{
    tagUid_.clear();
    tagTechList_.clear();
    connectedTagTech_ = KITS::TagTechnology::NFC_INVALID_TECH;
    tagRfDiscId_ = 0;
}

bool TagInfo::IsTechSupported(KITS::TagTechnology tech)
{
    for (auto n : tagTechList_) {
        if (n == static_cast<int>(tech)) {
            return true;
        }
    }
    return false;
}

OHOS::sptr<TAG::ITagSession> TagInfo::GetRemoteTagSession() const
{
    return remoteTagSession_;
}

std::weak_ptr<AppExecFwk::PacMap> TagInfo::GetTagExtrasData() const
{
    return tagTechExtrasData_;
}

std::vector<int> TagInfo::GetTagTechList() const
{
    return std::move(tagTechList_);
}

AppExecFwk::PacMap TagInfo::GetTechExtrasData(KITS::TagTechnology tech)
{
    AppExecFwk::PacMap pacmap;
    if (!tagTechExtrasData_) {
        return pacmap;
    }

    for (int i = 0; i < int(tagTechList_.size()); i++) {
        if (static_cast<int>(tech) == tagTechList_[i]) {
            pacmap = tagTechExtrasData_->GetPacMap(TECH_EXTRA_DATA_PREFIX + std::to_string(i));
        }
    }
    return pacmap;
}

std::string TagInfo::GetStringExtrasData(AppExecFwk::PacMap& extrasData, const std::string& extrasName)
{
    std::string value = "";
    if (extrasData.IsEmpty() || extrasName.empty()) {
        return value;
    }

    return extrasData.GetStringValue(extrasName);
}

int TagInfo::GetIntExtrasData(AppExecFwk::PacMap& extrasData, const std::string& extrasName)
{
    if (extrasData.IsEmpty() || extrasName.empty()) {
        return NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }

    return extrasData.GetLongValue(extrasName);
}

void TagInfo::SetConnectedTagTech(KITS::TagTechnology connectedTagTech)
{
    connectedTagTech_ = connectedTagTech;
}

KITS::TagTechnology TagInfo::GetConnectedTagTech() const
{
    return connectedTagTech_;
}

std::string TagInfo::GetTagUid() const
{
    return tagUid_;
}

int TagInfo::GetTagRfDiscId() const
{
    return tagRfDiscId_;
}

bool TagInfo::Marshalling(Parcel& parcel) const
{
    if (remoteTagSession_ == nullptr) {
        WarnLog("TagInfo::Marshalling remoteTagSession_ is null.");
        return false;
    }
    if (tagTechList_.size() > MAX_TAG_TECH_NUM) {
        WarnLog("TagInfo::Marshalling more than MAX_TAG_TECH_NUM.");
        return false;
    }
    parcel.WriteInt32(tagRfDiscId_);
    parcel.WriteString(tagUid_);
    parcel.WriteInt32(tagTechList_.size());
    parcel.WriteInt32Vector(tagTechList_);
    parcel.WriteObject<IRemoteObject>(remoteTagSession_->AsObject());
    if (tagTechList_.size() > 0 && tagTechExtrasData_ != nullptr) {
        parcel.WriteParcelable(tagTechExtrasData_.get());
    }
    return true;
}

std::shared_ptr<TagInfo> TagInfo::Unmarshalling(Parcel& parcel)
{
    int tagRfDiscId = parcel.ReadInt32();
    std::string tagUid = parcel.ReadString();
    int size = parcel.ReadInt32();
    if (size > MAX_TAG_TECH_NUM) {
        WarnLog("TagInfo::Marshalling more than MAX_TAG_TECH_NUM.");
        return nullptr;
    }
    std::vector<int> tagTechList;
    parcel.ReadInt32Vector(&tagTechList);
    sptr<IRemoteObject> tagService = parcel.ReadObject<IRemoteObject>();
    if (tagService == nullptr) {
        WarnLog("TagInfo::Unmarshalling tagService is null.");
        return nullptr;
    }
    OHOS::sptr<TAG::ITagSession> tagSession = new TAG::TagSessionProxy(tagService);
    std::shared_ptr<AppExecFwk::PacMap> tagTechExtrasData(parcel.ReadParcelable<AppExecFwk::PacMap>());
    if (tagTechList.size() > 0 && tagTechExtrasData == nullptr) {
        WarnLog("TagInfo::Unmarshalling tagTechExtrasData is null.");
        return nullptr;
    }
    std::shared_ptr<TagInfo> tag = std::make_shared<TagInfo>(tagTechList, tagTechExtrasData,
        tagUid, tagRfDiscId, tagSession);
    return tag;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS