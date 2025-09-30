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

#include "nfc_taihe_util.h"

#include "loghelper.h"
#include "nfc_sdk_common.h"

using namespace taihe;

namespace OHOS {
namespace NFC {
namespace KITS {
std::string NfcTaiheUtil::TaiheArrayToHexString(array_view<uint8_t> data)
{
    if (data.size() > MAX_ARRAY_LEN) {
        ErrorLog("data size exceed.");
        return "";
    }
    std::vector<uint8_t> dataBytes = {};
    for (uint16_t i = 0; i < data.size(); i++) {
        dataBytes.push_back(data[i]);
    }
    return NfcSdkCommon::BytesVecToHexString(&dataBytes[0], dataBytes.size());
}

std::vector<std::string> NfcTaiheUtil::TaiheStringArrayToStringVec(array_view<::taihe::string> data)
{
    std::vector<std::string> ret;
    if (data.size() > MAX_AID_LIST_NUM) {
        ErrorLog("data size exceed.");
        return ret;
    }
    for (uint16_t i = 0; i < data.size(); i++) {
        ret.push_back(data[i].c_str());
    }
    return ret;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS