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
#ifndef BARCODE_TAG_H
#define BARCODE_TAG_H

#include "basic_tag_session.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class BarcodeTag final : public BasicTagSession {
public:
    explicit BarcodeTag(std::weak_ptr<TagInfo> tag);
    ~BarcodeTag() override {}

    /**
     * @Description Get an object of BarcodeTag for the given tag.
     * @param tag compatible with all types of tag
     * @return std::shared_ptr<BarcodeTag>
     */
    static std::shared_ptr<BarcodeTag> GetTag(std::weak_ptr<TagInfo> tag);

    /**
     * @Description get barcode tag data
     * @return barcode tag data
     */
    std::string GetBarcode();
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // BARCODE_TAG_H
