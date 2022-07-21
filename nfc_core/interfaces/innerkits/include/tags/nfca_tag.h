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
#ifndef NFC_A_TAG_H
#define NFC_A_TAG_H

#include "basic_tag_session.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class NfcATag final : public BasicTagSession {
public:
    explicit NfcATag(std::weak_ptr<TagInfo> tag);
    ~NfcATag() {}

    /**
     * @Description Get an object of IsoDep for the given tag. It corresponding T1T(14443A-3) and T4T(14443A-4,
     * 14443B-4(Type-B)) defined by NFC Forum.
     * @param tag compatible with all types of tag
     * @return std::shared_ptr<NfcATag>
     */
    static std::shared_ptr<NfcATag> GetTag(std::weak_ptr<TagInfo> tag);
    /**
     * @Description Get SAK bytes of the tag.
     * @param void
     * @return SAK bytes
     */
    int GetSak() const;
    /**
     * @Description Get ATQA bytes of the tag.
     * @param void
     * @return ATQA bytes
     */
    std::string GetAtqa() const;

private:
    int sak_ {};
    std::string atqa_ {};
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_A_TAG_H
