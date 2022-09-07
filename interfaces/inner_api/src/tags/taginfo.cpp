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
                 std::vector<std::shared_ptr<AppExecFwk::PacMap>> tagTechExtrasDatas,
                 std::string& tagUid,
                 int tagRfDiscId,
                 OHOS::sptr<TAG::ITagSession> tagSession)
    : tagRfDiscId_(tagRfDiscId),
      connectedTagTech_(KITS::TagTechnology::NFC_INVALID_TECH),
      tagUid_(tagUid),
      tagTechList_(std::move(tagTechList)),
      remoteTagSession_(tagSession),
      tagTechExtrasDatas_(tagTechExtrasDatas)
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

std::vector<std::shared_ptr<AppExecFwk::PacMap>> TagInfo::GetTagExtrasDatas() const
{
    return tagTechExtrasDatas_;
}

std::vector<int> TagInfo::GetTagTechList() const
{
    return std::move(tagTechList_);
}

AppExecFwk::PacMap TagInfo::GetTechExtrasData(KITS::TagTechnology tech)
{
    AppExecFwk::PacMap pacmap;
    if (tagTechExtrasDatas_.empty()) {
        return pacmap;
    }

    int extraLength = tagTechExtrasDatas_.size();
    DebugLog("GetTechExtrasData length is %{public}d ", extraLength);

    for (int i = 0; i < tagTechList_.size(); i++) {
        if (static_cast<int>(tech) == tagTechList_[i]) {
            pacmap = *(tagTechExtrasDatas_[i]);
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

    return extrasData.GetIntValue(extrasName);
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
        ErrorLog("TagInfo::Marshalling remoteTagSession_ is null.");
        return false;
    }
    if (tagTechList_.size() > MAX_TAG_TECH_NUM) {
        ErrorLog("TagInfo::Marshalling more than MAX_TAG_TECH_NUM.");
        return false;
    }
    parcel.WriteInt32(tagRfDiscId_);
    parcel.WriteString(tagUid_);
    parcel.WriteInt32(tagTechList_.size());
    parcel.WriteInt32Vector(tagTechList_);
    parcel.WriteObject<IRemoteObject>(remoteTagSession_->AsObject());

    if (tagTechList_.size() > 0 && (!tagTechExtrasDatas_.empty())) {
        for (size_t i = 0; i < tagTechExtrasDatas_.size(); i++) {
            parcel.WriteParcelable(tagTechExtrasDatas_[i].get());
        }
    }
    return true;
}

std::shared_ptr<TagInfo> TagInfo::Unmarshalling(Parcel& parcel)
{
    int tagRfDiscId = parcel.ReadInt32();
    std::string tagUid = parcel.ReadString();
    int size = parcel.ReadInt32();
    if (size > MAX_TAG_TECH_NUM) {
        ErrorLog("TagInfo::Marshalling more than MAX_TAG_TECH_NUM.");
        return nullptr;
    }
    std::vector<int> tagTechList;
    parcel.ReadInt32Vector(&tagTechList);
    sptr<IRemoteObject> tagService = parcel.ReadObject<IRemoteObject>();
    if (tagService == nullptr) {
        ErrorLog("TagInfo::Unmarshalling tagService is null.");
        return nullptr;
    }
    OHOS::sptr<TAG::ITagSession> tagSession = new TAG::TagSessionProxy(tagService);
    std::vector<std::shared_ptr<AppExecFwk::PacMap>> tagTechExtrasDatas_;

    if (tagTechExtrasDatas_.empty()) {
        ErrorLog("TagInfo::Unmarshalling tagTechExtrasDatas is null.");
        return nullptr;
    }
    
    if (tagTechList.size() > 0 ) {
        for (size_t i = 0; i < tagTechExtrasDatas_.size(); i++){
            std::shared_ptr<AppExecFwk::PacMap> tagTechExtrasData(parcel.ReadParcelable<AppExecFwk::PacMap>());
            tagTechExtrasDatas_[i] = tagTechExtrasData;
        }
    } else {
        ErrorLog("TagInfo::Unmarshalling tagTechList is empty.");
        return nullptr;
    }
    std::shared_ptr<TagInfo> tag = std::make_shared<TagInfo>(tagTechList, tagTechExtrasDatas_,
        tagUid, tagRfDiscId, tagSession);
    return tag;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS