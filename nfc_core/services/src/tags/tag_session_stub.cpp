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
#include "tag_session_stub.h"

#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "permission_tools.h"

namespace OHOS {
namespace NFC {
namespace TAG {
int TagSessionStub::OnRemoteRequest(uint32_t code,         /* [in] */
                                    MessageParcel& data,   /* [in] */
                                    MessageParcel& reply,  /* [out] */
                                    MessageOption& option) /* [in] */
{
    DebugLog("OnRemoteRequest occur, code is %{public}d", code);
    DebugLog("TagSessionStub OnRemoteRequest occur, code is %d", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        ErrorLog("TagSessionStub OnRemoteRequest GetDescriptor failed");
        return KITS::NfcErrorCode::NFC_SDK_ERROR_INVALID_PARAM;
    }

    switch (code) {
        case KITS::COMMAND_CONNECT:
            return HandleConnect(data, reply);
        case KITS::COMMAND_RECONNECT:
            return HandleReconnect(data, reply);
        case KITS::COMMAND_DISCONNECT:
            return HandleDisconnect(data, reply);
        case KITS::COMMAND_GET_TECHLIST:
            return HandleGetTechList(data, reply);
        case KITS::COMMAND_IS_PRESENT:
            return HandleIsTagFieldOn(data, reply);
        case KITS::COMMAND_IS_NDEF:
            return HandleIsNdef(data, reply);
        case KITS::COMMAND_SEND_RAW_FRAME:
            return HandleSendRawFrame(data, reply);
        case KITS::COMMAND_NDEF_READ:
            return HandleNdefRead(data, reply);
        case KITS::COMMAND_NDEF_WRITE:
            return HandleNdefWrite(data, reply);
        case KITS::COMMAND_NDEF_MAKE_READ_ONLY:
            return HandleCanMakeReadOnly(data, reply);
        case KITS::COMMAND_FORMAT_NDEF:
            return HandleFormatNdef(data, reply);
        case KITS::COMMAND_CAN_MAKE_READ_ONLY:
            return HandleCanMakeReadOnly(data, reply);
        case KITS::COMMAND_GET_MAX_TRANSCEIVE_LENGTH:
            return HandleGetMaxTransceiveLength(data, reply);
        case KITS::COMMAND_IS_SUPPORTED_APDUS_EXTENDED:
            return HandleIsSupportedApdusExtended(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}
int TagSessionStub::HandleConnect(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    int tech = data.ReadInt32();
    int res = Connect(tagRfDiscId, tech);

    reply.WriteInt32(res);
    return ERR_NONE;
}
int TagSessionStub::HandleReconnect(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    int ec = Reconnect(tagRfDiscId);

    reply.WriteInt32(ec);
    return ERR_NONE;
}
int TagSessionStub::HandleDisconnect(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    Disconnect(tagRfDiscId);
    return ERR_NONE;
}
int TagSessionStub::HandleGetTechList(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    std::vector<int32_t> techList = GetTechList(tagRfDiscId);

    reply.WriteInt32Vector(techList);
    return ERR_NONE;
}
int TagSessionStub::HandleIsTagFieldOn(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }
    int tagRfDiscId = data.ReadInt32();
    bool res = IsNdef(tagRfDiscId);

    reply.WriteBool(res);
    return ERR_NONE;
}
int TagSessionStub::HandleIsNdef(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }
    int tagRfDiscId = data.ReadInt32();
    bool res = IsNdef(tagRfDiscId);

    reply.WriteBool(res);
    return ERR_NONE;
}
int TagSessionStub::HandleSendRawFrame(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    std::string commandData = data.ReadString();
    bool raw = data.ReadBool();
    std::unique_ptr<ResResult> res = SendRawFrame(tagRfDiscId, commandData, raw);
    reply.WriteParcelable(res.get());
    reply.WriteInt32(ERR_NONE);
    return ERR_NONE;
}
int TagSessionStub::HandleNdefRead(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    std::string readData = NdefRead(tagRfDiscId);
    reply.WriteString(readData);
    return ERR_NONE;
}
int TagSessionStub::HandleNdefWrite(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    std::string msg = data.ReadString();
    int ec = NdefWrite(tagRfDiscId, msg);

    reply.WriteInt32(ec);
    return ERR_NONE;
}
int TagSessionStub::HandleNdefMakeReadOnly(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    reply.WriteInt32(NdefMakeReadOnly(tagRfDiscId));
    return ERR_NONE;
}
int TagSessionStub::HandleFormatNdef(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    std::string key = data.ReadString();
    reply.WriteBool(FormatNdef(tagRfDiscId, key));
    return ERR_NONE;
}
int TagSessionStub::HandleCanMakeReadOnly(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }
    int tech = data.ReadInt32();
    reply.WriteBool(CanMakeReadOnly(tech));
    return ERR_NONE;
}
int TagSessionStub::HandleGetMaxTransceiveLength(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }
    int tech = data.ReadInt32();
    reply.WriteInt32(GetMaxTransceiveLength(tech));
    return ERR_NONE;
}
int TagSessionStub::HandleIsSupportedApdusExtended(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_PERMISSION;
    }
    reply.WriteBool(IsSupportedApdusExtended());
    return ERR_NONE;
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
