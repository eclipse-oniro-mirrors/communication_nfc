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
#include "nfcb_tag.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
NfcBTag::NfcBTag(std::weak_ptr<TagInfo> tag) : BasicTagSession(tag, KITS::TagTechnology::NFC_B_TECH)
{
    AppExecFwk::PacMap extraData = tag.lock()->GetTechExtrasByTech(KITS::TagTechnology::NFC_B_TECH);
    if (extraData.IsEmpty()) {
        ErrorLog("NfcBTag::NfcBTag extra data invalid");
        return;
    }

    appData_ = tag.lock()->GetStringExtrasData(extraData, TagInfo::APP_DATA);
    protocolInfo_ = tag.lock()->GetStringExtrasData(extraData, TagInfo::PROTOCOL_INFO);
    InfoLog("NfcBTag::NfcBTag appData_(%{public}s) protocolInfo_(%{public}s)",
        appData_.c_str(), protocolInfo_.c_str());
}

std::shared_ptr<NfcBTag> NfcBTag::GetTag(std::weak_ptr<TagInfo> tag)
{
    if (tag.expired() || !tag.lock()->IsTechSupported(KITS::TagTechnology::NFC_B_TECH)) {
        ErrorLog("NfcBTag::GetTag error, no mathced technology.");
        return nullptr;
    }
    return std::make_shared<NfcBTag>(tag);
}

std::string NfcBTag::GetAppData() const
{
    return appData_;
}

std::string NfcBTag::GetProtocolInfo() const
{
    return protocolInfo_;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
