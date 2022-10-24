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
#include "nfcf_tag.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
NfcFTag::NfcFTag(std::weak_ptr<TagInfo> tag) : BasicTagSession(tag, KITS::TagTechnology::NFC_F_TECH)
{
    if (tag.expired()) {
        ErrorLog("NfcFTag::NfcFTag tag invalid ");
        return;
    }
    AppExecFwk::PacMap extraData = tag.lock()->GetTechExtrasByTech(KITS::TagTechnology::NFC_F_TECH);
    if (extraData.IsEmpty()) {
        ErrorLog("NfcFTag::NfcFTag extra data invalid");
        return;
    }
    pmm_ = tag.lock()->GetStringExtrasData(extraData, TagInfo::NFCF_PMM);
    systemCode_ = tag.lock()->GetStringExtrasData(extraData, TagInfo::NFCF_SC);
}

std::shared_ptr<NfcFTag> NfcFTag::GetTag(std::weak_ptr<TagInfo> tag)
{
    if (tag.expired() || !tag.lock()->IsTechSupported(KITS::TagTechnology::NFC_F_TECH)) {
        ErrorLog("NfcFTag::GetTag error, no mathced technology.");
        return nullptr;
    }
    return std::make_shared<NfcFTag>(tag);
}

std::string NfcFTag::getSystemCode() const
{
    return systemCode_;
}

std::string NfcFTag::getPmm() const
{
    return pmm_;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
