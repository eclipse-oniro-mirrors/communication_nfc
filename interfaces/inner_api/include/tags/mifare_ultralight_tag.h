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
#ifndef MIFARE_ULTRALIGHT_TAG_H
#define MIFARE_ULTRALIGHT_TAG_H

#include "basic_tag_session.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class MifareUltralightTag final : public BasicTagSession {
public:
    static const int NXP_MANUFACTURER_ID = 0x04;
    static const int MU_MAX_PAGE_COUNT = 256;
    static const int MU_PAGE_SIZE = 4;

    static const char MIFARE_ULTRALIGHT_READ = 0x30;
    static const char MIFARE_ULTRALIGHT_WRITE = 0xA2;

    enum EmType { TYPE_UNKNOWN = 0, TYPE_ULTRALIGHT = 1, TYPE_ULTRALIGHT_C = 2 };

public:
    explicit MifareUltralightTag(std::weak_ptr<TagInfo> tag);
    ~MifareUltralightTag();

    /**
     * @Description Get an object of MifareUltralightTag for the given tag.
     * @param tag compatible with all types of tag
     * @return std::shared_ptr<MifareUltralightTag>
     */
    static std::shared_ptr<MifareUltralightTag> GetTag(std::weak_ptr<TagInfo> tag);
    /**
     * @Description Read 4 pages(16 bytes).
     * @param pageIndex index of page to read
     * @param hexRespData the hex response data for reading.
     * @return the error code of calling function.
     */
    int ReadMultiplePages(uint32_t pageIndex, std::string &hexRespData);
    /**
     * @Description Write a page
     * @param pageIndex index of page to write
     * @param data page data to write
     * @return Errorcode of write. if return 0, means successful.
     */
    int WriteSinglePage(uint32_t pageIndex, const std::string& data);
    /**
     * @Description Get the type of the MifareUltralight tag in bytes.
     * @param void
     * @return type of MifareUltralight tag.
     */
    EmType GetType() const;

private:
    EmType type_ {EmType::TYPE_UNKNOWN};
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // MIFARE_ULTRALIGHT_TAG_H
