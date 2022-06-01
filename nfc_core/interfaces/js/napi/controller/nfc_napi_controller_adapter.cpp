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

namespace OHOS {
namespace NFC {
namespace KITS {
napi_value OpenNfc(napi_env env, napi_callback_info info)
{
    DebugLog("nfc_napi_controller_adapter::OpenNfc");
    NfcController nfcCtrl = OHOS::NFC::KITS::NfcController::GetInstance();
    int ret = nfcCtrl.TurnOn();
    InfoLog("nfc_napi_controller_adapter::OpenNfc ret = %{pubilic}d", ret);
    napi_value result;
    napi_get_boolean(env, ret == NfcErrorCode::NFC_SUCCESS, &result);
    return result;
}

napi_value CloseNfc(napi_env env, napi_callback_info info)
{
    DebugLog("nfc_napi_controller_adapter::CloseNfc");
    NfcController nfcCtrl = OHOS::NFC::KITS::NfcController::GetInstance();
    int ret = nfcCtrl.TurnOff();
    InfoLog("nfc_napi_controller_adapter::CloseNfc ret = %{pubilic}d", ret);
    napi_value result;
    napi_get_boolean(env, ret == NfcErrorCode::NFC_SUCCESS, &result);
    return result;
}

napi_value GetNfcState(napi_env env, napi_callback_info info)
{
    DebugLog("nfc_napi_controller_adapter::GetNfcState");
    NfcController nfcCtrl = OHOS::NFC::KITS::NfcController::GetInstance();
    int ret = nfcCtrl.GetNfcState();
    napi_value result;
    napi_create_int32(env, ret, &result);
    return result;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
