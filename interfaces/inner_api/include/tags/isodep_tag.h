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
#ifndef ISODEP_TAG_H
#define ISODEP_TAG_H

#include "basic_tag_session.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class IsoDepTag final : public BasicTagSession {
public:
    explicit IsoDepTag(std::weak_ptr<TagInfo> tag);
    ~IsoDepTag() {}

    /**
     * @Description Get an object of IsoDep for the given tag. It corresponding T1T(14443A-3) and T4T(14443A-4,
     * 14443B-4(Type-B)) defined by NFC Forum.
     * @param tag compatible with all types of tag
     * @return std::shared_ptr<IsoDepTag>
     */
    static std::shared_ptr<IsoDepTag> GetTag(std::weak_ptr<TagInfo> tag);
    /**
     * @Description Get Historical bytes of the tag.
     * @param void
     * @return Historical bytes
     */
    std::string GetHistoricalBytes() const;
    /**
     * @Description Get HiLayerResponse bytes of the tag.
     * @param void
     * @return HiLayerResponse bytes
     */
    std::string GetHiLayerResponse() const;

    /**
     * @Description is supported for 3 bytes length APUDs to send, the max length is 65535 bytes.
     * @param isSupported the output for checking supportting extended apdu or not.
     * @return the error code of calling function.
     */
    int IsExtendedApduSupported(bool &isSupported) const;

private:
    std::string historicalBytes_ {};
    std::string hiLayerResponse_ {};
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // ISODEP_TAG_H
