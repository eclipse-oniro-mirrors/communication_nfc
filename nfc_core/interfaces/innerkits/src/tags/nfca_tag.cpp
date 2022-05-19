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
#include "nfca_tag.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
NfcATag::NfcATag(std::weak_ptr<TagInfo> tag) : BasicTagSession(tag, KITS::TagTechnology::NFC_A_TECH)
{
    if (tag.expired()) {
        DebugLog("NfcATag::NfcATag tag invalid ");
        return;
    }
    AppExecFwk::PacMap extraData = tag.lock()->GetTechExtrasData(KITS::TagTechnology::NFC_A_TECH);
    if (!extraData.IsEmpty()) {
        DebugLog("NfcATag::NfcATag extra data invalid");
        return;
    }

    sak_ = tag.lock()->GetIntExtrasData(extraData, TagInfo::SAK);
    atqa_ = tag.lock()->GetStringExtrasData(extraData, TagInfo::ATQA);
    DebugLog("NfcATag::NfcATag sak_(%d) atqa_(%s)", sak_, atqa_.c_str());
}

std::shared_ptr<NfcATag> NfcATag::GetTag(std::weak_ptr<TagInfo> tag)
{
    DebugLog("NfcATag::GetTag in");
    if (tag.expired() || !tag.lock()->IsTechSupported(KITS::TagTechnology::NFC_A_TECH)) {
        DebugLog("NfcATag::GetTag err");
        return nullptr;
    }

    return std::make_shared<NfcATag>(tag);
}

int NfcATag::GetSak() const
{
    return sak_;
}

std::string NfcATag::GetAtqa() const
{
    return atqa_;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
