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

#include "taihe/array.hpp"
#include "taihe/runtime.hpp"

class NfcTaiheUtil {
public:
    static std::string TaiheArrayToHexString(const ::taihe::array_view<int32_t> &data);
    static std::vector<std::string> TaiheStringArrayToStringVec(const ::taihe::array_view<::taihe::string> &data);
    static std::vector<int> TaiheIntArrayToIntVec(const ::taihe::array<int32_t> &data);

    static ::taihe::array<int32_t> HexStringToTaiheArray(const std::string &src);
};
#endif // #define NFC_TAIHE_UTIL_H