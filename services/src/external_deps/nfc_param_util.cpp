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
#include "nfc_param_util.h"

#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "parameter.h"

namespace OHOS {
namespace NFC {
void NfcParamUtil::UpdateNfcStateToParam(int newState)
{
    if (newState != KITS::STATE_ON && newState != KITS::STATE_OFF) {
        ErrorLog("illegal state [%{public}d].", newState);
        return;
    }
    InfoLog("UpdateNfcStateToParam, new nfc state[%{public}d]", newState);
    int errCode = SetParameter(NFC_SWITCH_STATE_PARAM_NAME, std::to_string(newState).c_str());
    if (errCode < 0) {
        ErrorLog("fail to set nfc switch param, errCode[%{public}d]", errCode);
    }
}

int NfcParamUtil::GetNfcStateFromParam()
{
    char nfcState[PROPERTY_VALUE_MAX] = {0};
    int errCode = GetParameter(NFC_SWITCH_STATE_PARAM_NAME, "", nfcState, PROPERTY_VALUE_MAX);
    if (errCode != NFC_SWITCH_PARAM_LEN) {
        ErrorLog("failed to get nfc switch state, errCode[%{public}d]", errCode);
        return 0; // return invalid nfc state
    }
    InfoLog("GetNfcStateFromParam, nfc state[%{public}s]", nfcState);
    errno = 0;
    char *endptr = nullptr;
    long int num = std::strtol(nfcState, &endptr, DECIMAL_NOTATION);
    if (errno == ERANGE) {
        ErrorLog("strtol errno = ERANGE");
        return 0; // return invalid nfc state
    }
    return static_cast<int>(num);
}
}  // namespace NFC
}  // namespace OHOS