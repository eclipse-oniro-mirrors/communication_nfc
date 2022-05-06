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
#ifndef NFC_B_TAG_H
#define NFC_B_TAG_H

#include "basic_tag_session.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class NfcBTag final : public BasicTagSession {
public:
    explicit NfcBTag(std::weak_ptr<TagInfo> tag);
    ~NfcBTag() {}

    /**
     * @Description Get an object of IsoDep for the given tag. It corresponding T1T(14443A-3) and T4T(14443A-4,
     * 14443B-4(Type-B)) defined by NFC Forum.
     * @param tag compatible with all types of tag
     * @return std::shared_ptr<NfcBTag>
     */
    static std::shared_ptr<NfcBTag> GetTag(std::weak_ptr<TagInfo> tag);
    /**
     * @Description Get AppData bytes of the tag.
     * @param void
     * @return AppData bytes
     */
    std::string GetAppData() const;
    /**
     * @Description Get ProtocolInfo bytes of the tag.
     * @param void
     * @return ProtocolInfo bytes
     */
    std::string GetProtocolInfo() const;

private:
    std::string appData_ {};
    std::string protocolInfo_ {};
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_B_TAG_H
