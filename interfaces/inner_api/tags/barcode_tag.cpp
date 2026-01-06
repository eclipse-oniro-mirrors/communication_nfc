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
#include "barcode_tag.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
BarcodeTag::BarcodeTag(std::weak_ptr<TagInfo> tag)
    : BasicTagSession(tag, KITS::TagTechnology::NFC_BARCODE)
{
}

std::shared_ptr<BarcodeTag> BarcodeTag::GetTag(std::weak_ptr<TagInfo> tag)
{
    auto tagPtr = tag.lock();
    if (tagPtr == nullptr) {
        ErrorLog("tag is nullptr");
        return nullptr;
    }
    if (tag.expired() || !tagPtr->IsTechSupported(KITS::TagTechnology::NFC_BARCODE)) {
        ErrorLog("BarcodeTag::GetTag error, no mathced technology.");
        return nullptr;
    }
    return std::make_shared<BarcodeTag>(tag);
}

std::string BarcodeTag::GetBarcode()
{
    return GetTagUid();
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS