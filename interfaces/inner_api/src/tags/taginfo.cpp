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
#include "nfc_controller.h"
#include "nfc_sdk_common.h"
#include "parcel.h"
#include "refbase.h"
#include "tag_session_proxy.h"

namespace OHOS {
namespace NFC {
namespace KITS {
TagInfo::TagInfo(std::vector<int> tagTechList,
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
    connectedTagTech_ = KITS::TagTechnology::NFC_INVALID_TECH;
    if (tagServiceIface != nullptr) {
        tagSessionProxy_ = new TAG::TagSessionProxy(tagServiceIface);
    }
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

OHOS::sptr<TAG::ITagSession> TagInfo::GetTagSessionProxy()
{
    if (tagSessionProxy_ == nullptr) {
        OHOS::sptr<IRemoteObject> iface = NfcController::GetInstance().GetTagServiceIface();
        if (iface != nullptr) {
            tagSessionProxy_ = new TAG::TagSessionProxy(iface);
        }
    }
    return tagSessionProxy_;
}

std::vector<int> TagInfo::GetTagTechList() const
{
    return std::move(tagTechList_);
}

std::string TagInfo::GetStringTach(int tech)
{
    switch (tech) {
        case static_cast<int>(TagTechnology::NFC_A_TECH):
            return "NfcA";
        case static_cast<int>(TagTechnology::NFC_B_TECH):
            return "NfcB";
        case static_cast<int>(TagTechnology::NFC_F_TECH):
            return "NfcF";
        case static_cast<int>(TagTechnology::NFC_V_TECH):
            return "NfcV";
        case static_cast<int>(TagTechnology::NFC_ISODEP_TECH):
            return "IsoDep";
        case static_cast<int>(TagTechnology::NFC_MIFARE_CLASSIC_TECH):
            return "MifareClassic";
        case static_cast<int>(TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH):
            return "MifareUL";
        case static_cast<int>(TagTechnology::NFC_NDEF_TECH):
            return "Ndef";
        case static_cast<int>(TagTechnology::NFC_NDEF_FORMATABLE_TECH):
            return "NdefFormatable";
        default:
            break;
    }
    return "";
}

AppExecFwk::PacMap TagInfo::GetTechExtrasByIndex(size_t techIndex)
{
    AppExecFwk::PacMap pacmap;
    if (tagTechList_.size() == 0 || tagTechList_.size() != tagTechExtrasData_.size()) {
        ErrorLog("Taginfo:: tagTechList_lenth != tagTechExtrasData_length.");
        return pacmap;
    }
    if (techIndex < 0 || techIndex >= tagTechExtrasData_.size()) {
        return pacmap;
    }
    return tagTechExtrasData_[techIndex];
}

AppExecFwk::PacMap TagInfo::GetTechExtrasByTech(KITS::TagTechnology tech)
{
    AppExecFwk::PacMap pacmap;
    if (tagTechList_.size() == 0 || tagTechList_.size() != tagTechExtrasData_.size()) {
        return pacmap;
    }

    for (size_t i = 0; i < tagTechList_.size(); i++) {
        if (static_cast<int>(tech) == tagTechList_[i]) {
            pacmap = tagTechExtrasData_[i];
            break;
        }
    }
    return pacmap;
}

std::string TagInfo::GetStringExtrasData(AppExecFwk::PacMap& extrasData, const std::string& extrasName)
{
    if (extrasData.IsEmpty() || extrasName.empty()) {
        return "";
    }
    return extrasData.GetStringValue(extrasName, "");
}

int TagInfo::GetIntExtrasData(AppExecFwk::PacMap& extrasData, const std::string& extrasName)
{
    if (extrasData.IsEmpty() || extrasName.empty()) {
        return ErrorCode::ERR_TAG_PARAMETERS;
    }
    return extrasData.GetIntValue(extrasName, 0);
}

bool TagInfo::GetBoolExtrasData(AppExecFwk::PacMap& extrasData, const std::string& extrasName)
{
    if (extrasData.IsEmpty() || extrasName.empty()) {
        return false;
    }
    return extrasData.GetBooleanValue(extrasName, false);
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

}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS