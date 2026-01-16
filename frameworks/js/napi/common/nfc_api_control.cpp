/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "nfc_api_control.h"
#include <cstring>
#include <mutex>
#include "param_wrapper.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
static constexpr const char* NFC_NOT_SUPPORT_KEY = "const.nfc.not_support";
static constexpr const char* PARAM_TRUE = "true";
static constexpr const char* PARAM_FALSE = "false";

static std::mutex g_nfcApiControlMutex;

bool IsNfcNotSupported()
{
    std::lock_guard<std::mutex> lock(g_nfcApiControlMutex);
    static std::string nfcNotSupported = "";
    if (!nfcNotSupported.empty()) {
        bool result = nfcNotSupported == PARAM_TRUE;
        if (result) {
            WarnLog("nfc not supported");
        }
        return result;
    }
    int32_t res = OHOS::system::GetStringParameter(NFC_NOT_SUPPORT_KEY, nfcNotSupported, PARAM_FALSE);
    InfoLog("res=%{public}d, nfcNotSupported=%{public}s", res, nfcNotSupported.c_str());
    if (res != 0) {
        nfcNotSupported = "";
    }
    return nfcNotSupported == PARAM_TRUE;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
