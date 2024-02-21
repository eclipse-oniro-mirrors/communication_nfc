/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "start_hce_info_parcelable.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"
namespace OHOS {
namespace NFC {
namespace KITS {
StartHceInfoParcelable::StartHceInfoParcelable(const std::vector<std::string> &aids, const ElementName &element)
{
    aids_ = std::move(aids);
    aidsCount_ = aids.size();
    element_.SetBundleName(element.GetBundleName());
    element_.SetAbilityName(element.GetAbilityName());
    element_.SetDeviceID(element.GetDeviceID());
    element_.SetModuleName(element.GetModuleName());
}
StartHceInfoParcelable::StartHceInfoParcelable(Parcel &parcel)
{
    parcel.ReadUint32(aidsCount_);
    if (aidsCount_ > MAX_AID_LIST_NUM_PER_APP) {
        ErrorLog("invalid length");
        return;
    }

    for (uint32_t i = 0; i < aidsCount_; i++) {
        std::string aid;
        parcel.ReadString(aid);
        aids_.push_back(aid);
    }

    element_.ReadFromParcel(parcel);
}
StartHceInfoParcelable::StartHceInfoParcelable()
{
}
StartHceInfoParcelable::~StartHceInfoParcelable()
{
    aids_.clear();
    element_.SetBundleName("");
    element_.SetAbilityName("");
    element_.SetDeviceID("");
    element_.SetModuleName("");
}
bool StartHceInfoParcelable::Marshalling(Parcel &parcel) const
{
    if (aids_.size() > MAX_AID_LIST_NUM_PER_APP) {
        ErrorLog("invalid length");
        return false;
    }
    if (!parcel.WriteUint32(aids_.size())) {
        ErrorLog("write size failed");
        return false;
    }
    for (uint32_t i = 0; i < aids_.size(); i++) {
        if (!parcel.WriteString(aids_[i])) {
            ErrorLog("write aid failed");
            return false;
        }
    }
    if (!element_.Marshalling(parcel)) {
        ErrorLog("write element failed");
        return false;
    }
    return true;
}
void StartHceInfoParcelable::SetAids(const std::vector<std::string> &aids)
{
    aids_ = std::move(aids);
    aidsCount_ = aids.size();
}
void StartHceInfoParcelable::SetElement(const ElementName &element)
{
    element_.SetBundleName(element.GetBundleName());
    element_.SetAbilityName(element.GetAbilityName());
    element_.SetDeviceID(element.GetDeviceID());
    element_.SetModuleName(element.GetModuleName());
}
std::vector<std::string> StartHceInfoParcelable::GetAids()
{
    return aids_;
}
ElementName StartHceInfoParcelable::GetElement()
{
    return element_;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS