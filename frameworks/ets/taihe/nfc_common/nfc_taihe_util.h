/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef NFC_TAIHE_UTIL_H
#define NFC_TAIHE_UTIL_H

#include <string>
#include <vector>

#include "taihe/runtime.hpp"

namespace OHOS {
namespace NFC {
namespace KITS {
class NfcTaiheUtil {
public:
    static std::string TaiheArrayToHexString(taihe::array_view<uint8_t> data);
    static std::vector<std::string> TaiheStringArrayToStringVec(taihe::array_view<::taihe::string> data);

private:
    const uint16_t MAX_ARRAY_LEN = 512;
    const uint16_t MAX_AID_LIST_NUM = 100;
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif // #define NFC_TAIHE_UTIL_H