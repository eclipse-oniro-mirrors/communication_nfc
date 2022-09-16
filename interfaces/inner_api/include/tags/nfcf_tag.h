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
#ifndef NFCF_TAG_H
#define NFCF_TAG_H

#include "basic_tag_session.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class NfcFTag final : public BasicTagSession {
public:
    explicit NfcFTag(std::weak_ptr<TagInfo> tag);
    ~NfcFTag() {}

    /**
     * @Description Get an object of NfcFTag for the given tag.
     * @param tag compatible with all types of tag
     * @return std::shared_ptr<NfcFTag>
     */
    static std::shared_ptr<NfcFTag> GetTag(std::weak_ptr<TagInfo> tag);
    /**
     * @Description Obtains the system code from this {@code NfcFTag} instance.
     * @param void
     * @return The system code
     */
    std::string getSystemCode() const;
    /**
     * @Description Obtains the PMm (consisting of the IC code and manufacturer parameters).
     * @param void
     * @return THe PMm
     */
    std::string getPmm() const;
private:
    std::string systemCode_;
    std::string pmm_;
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // NFCF_TAG_H
