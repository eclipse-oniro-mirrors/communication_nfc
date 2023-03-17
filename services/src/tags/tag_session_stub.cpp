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
    DebugLog("TagSessionStub OnRemoteRequest occur, code is %d", code);
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        ErrorLog("TagSessionStub OnRemoteRequest GetDescriptor failed");
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }

    switch (code) {
        case KITS::COMMAND_CONNECT:
            return HandleConnect(data, reply);
        case KITS::COMMAND_RECONNECT:
            return HandleReconnect(data, reply);
        case KITS::COMMAND_DISCONNECT:
            return HandleDisconnect(data, reply);
        case KITS::COMMAND_SET_TIMEOUT:
            return HandleSetTimeout(data, reply);
        case KITS::COMMAND_GET_TIMEOUT:
            return HandleGetTimeout(data, reply);
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
            return HandleNdefMakeReadOnly(data, reply);
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
        ErrorLog("HandleConnect, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    int tech = data.ReadInt32();
    int statusCode = Connect(tagRfDiscId, tech);
    reply.WriteInt32(statusCode);
    return statusCode;
}
int TagSessionStub::HandleReconnect(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleReconnect, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    int statusCode = Reconnect(tagRfDiscId);
    reply.WriteInt32(statusCode);
    return statusCode;
}
int TagSessionStub::HandleDisconnect(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleDisconnect, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    Disconnect(tagRfDiscId);
    return ERR_NONE;
}
int TagSessionStub::HandleSetTimeout(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleSetTimeout, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    int tech = data.ReadInt32();
    int timeout = data.ReadInt32();
    int statusCode = SetTimeout(timeout, tech);
    reply.WriteInt32(statusCode);
    return statusCode;
}
int TagSessionStub::HandleGetTimeout(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleGetTimeout, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    int timeout = 0;
    int tech = data.ReadInt32();
    int statusCode = GetTimeout(tech, timeout);
    reply.WriteInt32(timeout);
    return statusCode;
}
int TagSessionStub::HandleGetTechList(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleGetTechList, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    std::vector<int32_t> techList = GetTechList(tagRfDiscId);
    reply.WriteInt32Vector(techList);
    return ERR_NONE;
}
int TagSessionStub::HandleIsTagFieldOn(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleIsTagFieldOn, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    int tagRfDiscId = data.ReadInt32();
    reply.WriteBool(IsNdef(tagRfDiscId));
    return ERR_NONE;
}
int TagSessionStub::HandleIsNdef(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleIsNdef, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    int tagRfDiscId = data.ReadInt32();
    reply.WriteBool(IsNdef(tagRfDiscId));
    return ERR_NONE;
}
int TagSessionStub::HandleSendRawFrame(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleSendRawFrame, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    std::string hexCmdData = data.ReadString();
    bool raw = data.ReadBool();
    std::string hexRespData;
    int statusCode = SendRawFrame(tagRfDiscId, hexCmdData, raw, hexRespData);
    reply.WriteString(hexRespData);
    return statusCode;
}
int TagSessionStub::HandleNdefRead(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleNdefRead, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    std::string readData = NdefRead(tagRfDiscId);
    reply.WriteString(readData);
    return ERR_NONE;
}
int TagSessionStub::HandleNdefWrite(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleNdefWrite, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    std::string msg = data.ReadString();
    int status = NdefWrite(tagRfDiscId, msg);
    reply.WriteInt32(status);
    return ERR_NONE;
}
int TagSessionStub::HandleNdefMakeReadOnly(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleNdefMakeReadOnly, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    reply.WriteInt32(NdefMakeReadOnly(tagRfDiscId));
    return ERR_NONE;
}
int TagSessionStub::HandleFormatNdef(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleFormatNdef, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    std::string key = data.ReadString();
    reply.WriteInt32(FormatNdef(tagRfDiscId, key));
    return ERR_NONE;
}
int TagSessionStub::HandleCanMakeReadOnly(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleCanMakeReadOnly, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    int ndefType = data.ReadInt32();
    bool canSetReadOnly = false;
    int statusCode = CanMakeReadOnly(ndefType, canSetReadOnly);
    reply.WriteBool(canSetReadOnly);
    return statusCode;
}
int TagSessionStub::HandleGetMaxTransceiveLength(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleGetMaxTransceiveLength, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    int maxSize = 0;
    int tech = data.ReadInt32();
    int statusCode = GetMaxTransceiveLength(tech, maxSize);
    reply.WriteInt32(maxSize);
    return statusCode;
}
int TagSessionStub::HandleIsSupportedApdusExtended(MessageParcel& data, MessageParcel& reply)
{
    if (!PermissionTools::IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleIsSupportedApdusExtended, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    bool isSupported = false;
    int statusCode = IsSupportedApdusExtended(isSupported);
    reply.WriteBool(isSupported);
    return statusCode;
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
