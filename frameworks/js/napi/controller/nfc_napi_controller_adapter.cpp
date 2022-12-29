/*
* Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "nfc_napi_controller_adapter.h"
#include <vector>
#include "loghelper.h"
#include "nfc_controller.h"
#include "nfc_sdk_common.h"
#include "nfc_napi_ctrl_utils.h"

namespace OHOS {
namespace NFC {
namespace KITS {
napi_value OpenNfc(napi_env env, napi_callback_info info)
{
    NfcController nfcCtrl = OHOS::NFC::KITS::NfcController::GetInstance();
    int status = nfcCtrl.TurnOn();
    napi_value result;
    napi_get_boolean(env, (status == KITS::ERR_NONE), &result);
    return result;
}

napi_value EnableNfc(napi_env env, napi_callback_info info)
{
    NfcController nfcCtrl = OHOS::NFC::KITS::NfcController::GetInstance();
    int status = nfcCtrl.TurnOn();
    CheckNfcStatusCodeAndThrow(env, status, "enableNfc");
    return CreateUndefined(env);
}

napi_value CloseNfc(napi_env env, napi_callback_info info)
{
    NfcController nfcCtrl = OHOS::NFC::KITS::NfcController::GetInstance();
    int status = nfcCtrl.TurnOff();
    napi_value result;
    napi_get_boolean(env, (status == KITS::ERR_NONE), &result);
    return result;
}

napi_value DisableNfc(napi_env env, napi_callback_info info)
{
    NfcController nfcCtrl = OHOS::NFC::KITS::NfcController::GetInstance();
    int status = nfcCtrl.TurnOff();
    CheckNfcStatusCodeAndThrow(env, status, "disableNfc");
    return CreateUndefined(env);
}

napi_value GetNfcState(napi_env env, napi_callback_info info)
{
    NfcController nfcCtrl = OHOS::NFC::KITS::NfcController::GetInstance();
    napi_value result;
    napi_create_int32(env, nfcCtrl.GetNfcState(), &result);
    return result;
}

napi_value IsNfcAvailable(napi_env env, napi_callback_info info)
{
    NfcController nfcCtrl = OHOS::NFC::KITS::NfcController::GetInstance();
    napi_value result;
    napi_get_boolean(env, nfcCtrl.IsNfcAvailable(), &result);
    return result;
}

napi_value IsNfcOpen(napi_env env, napi_callback_info info)
{
    NfcController nfcCtrl = OHOS::NFC::KITS::NfcController::GetInstance();
    bool isOpen = false;
    int statusCode = nfcCtrl.IsNfcOpen(isOpen);
    if (statusCode != KITS::ERR_NONE) {
        ErrorLog("IsNfcOpen, statusCode = %{public}d", statusCode);
    }
    napi_value result;
    napi_get_boolean(env, isOpen, &result);
    return result;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
