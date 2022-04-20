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
#include "nfc_controller_stub.h"

#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "permission_tools.h"

namespace OHOS {
namespace NFC {
int NfcControllerStub::OnRemoteRequest(uint32_t code,         /* [in] */
                                       MessageParcel& data,   /* [in] */
                                       MessageParcel& reply,  /* [out] */
                                       MessageOption& option) /* [in] */
{
    DebugLog("OnRemoteRequest occur, code is %d", code);
    switch (code) {
        case KITS::COMMAND_GET_STATE:
            return HandleGetState(data, reply);
        case KITS::COMMAND_TURN_ON:
            return HandleTurnOn(data, reply);
        case KITS::COMMAND_TURN_OFF:
            return HandleTurnOff(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int NfcControllerStub::HandleGetState(MessageParcel& data, MessageParcel& reply)
{
    int state = GetState();

    reply.WriteInt32(state);
    return ERR_NONE;
}

int NfcControllerStub::HandleTurnOn(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::SYS_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    bool result = TurnOn();
    reply.WriteInt32(result);
    return ERR_NONE;
}

int NfcControllerStub::HandleTurnOff(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::SYS_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    bool saveState = data.ReadBool();

    saveState = TurnOff(saveState);
    reply.WriteInt32(saveState);
    return ERR_NONE;
}
}  // namespace NFC
}  // namespace OHOS
