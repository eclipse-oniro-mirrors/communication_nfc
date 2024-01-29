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

#include "external_deps_proxy.h"
#include "foreground_death_recipient.h"
#include "ipc_skeleton.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include "nfc_permission_checker.h"
#include "reader_mode_death_recipient.h"

namespace OHOS {
namespace NFC {
namespace TAG {
using OHOS::AppExecFwk::ElementName;
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
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_CONNECT):
            return HandleConnect(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_RECONNECT):
            return HandleReconnect(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_DISCONNECT):
            return HandleDisconnect(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_SET_TIMEOUT):
            return HandleSetTimeout(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_GET_TIMEOUT):
            return HandleGetTimeout(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_RESET_TIMEOUT):
            return HandleResetTimeout(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_GET_TECHLIST):
            return HandleGetTechList(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_IS_PRESENT):
            return HandleIsTagFieldOn(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_IS_NDEF):
            return HandleIsNdef(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_SEND_RAW_FRAME):
            return HandleSendRawFrame(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_NDEF_READ):
            return HandleNdefRead(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_NDEF_WRITE):
            return HandleNdefWrite(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_NDEF_MAKE_READ_ONLY):
            return HandleNdefMakeReadOnly(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_FORMAT_NDEF):
            return HandleFormatNdef(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_CAN_MAKE_READ_ONLY):
            return HandleCanMakeReadOnly(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_GET_MAX_TRANSCEIVE_LENGTH):
            return HandleGetMaxTransceiveLength(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_IS_SUPPORTED_APDUS_EXTENDED):
            return HandleIsSupportedApdusExtended(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_REG_FOREGROUND):
            return HandleRegForegroundDispatch(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_UNREG_FOREGROUND):
            return HandleUnregForegroundDispatch(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_REG_READER_MODE):
            return HandleRegReaderMode(data, reply);
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_UNREG_READER_MODE):
            return HandleUnregReaderMode(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
}

int TagSessionStub::HandleConnect(MessageParcel& data, MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
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
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
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
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleDisconnect, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    Disconnect(tagRfDiscId);
    return ERR_NONE;
}

int TagSessionStub::HandleSetTimeout(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleSetTimeout, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    int tagRfDiscId = data.ReadInt32();
    int tech = data.ReadInt32();
    int timeout = data.ReadInt32();
    int statusCode = SetTimeout(tagRfDiscId, timeout, tech);
    reply.WriteInt32(statusCode);
    return statusCode;
}

int TagSessionStub::HandleGetTimeout(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleGetTimeout, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    int timeout = 0;
    int tagRfDiscId = data.ReadInt32();
    int tech = data.ReadInt32();
    int statusCode = GetTimeout(tagRfDiscId, tech, timeout);
    reply.WriteInt32(timeout);
    return statusCode;
}

int TagSessionStub::HandleResetTimeout(OHOS::MessageParcel& data, OHOS::MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleResetTimeout, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    int tagRfDiscId = data.ReadInt32();
    ResetTimeout(tagRfDiscId);
    return ERR_NONE;
}

int TagSessionStub::HandleGetTechList(MessageParcel& data, MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleGetTechList, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    std::vector<int> techList = GetTechList(tagRfDiscId);
    reply.WriteInt32Vector(techList);
    return ERR_NONE;
}

int TagSessionStub::HandleIsTagFieldOn(MessageParcel& data, MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleIsTagFieldOn, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    int tagRfDiscId = data.ReadInt32();
    reply.WriteBool(IsNdef(tagRfDiscId));
    return ERR_NONE;
}

int TagSessionStub::HandleIsNdef(MessageParcel& data, MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleIsNdef, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    int tagRfDiscId = data.ReadInt32();
    reply.WriteBool(IsNdef(tagRfDiscId));
    return ERR_NONE;
}

int TagSessionStub::HandleSendRawFrame(MessageParcel& data, MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleSendRawFrame, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    std::string hexCmdData = data.ReadString();
    bool raw = data.ReadBool();
    std::string hexRespData;
    int statusCode = SendRawFrame(tagRfDiscId, hexCmdData, raw, hexRespData);
    reply.WriteString(hexRespData);
    DebugLog("TagSessionStub::HandleSendRawFrame:statusCode=0x%{public}X", statusCode);
    reply.WriteInt32(statusCode);
    return statusCode;
}

int TagSessionStub::HandleNdefRead(MessageParcel& data, MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
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
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
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
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleNdefMakeReadOnly, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }

    int tagRfDiscId = data.ReadInt32();
    reply.WriteInt32(NdefMakeReadOnly(tagRfDiscId));
    return ERR_NONE;
}

int TagSessionStub::HandleFormatNdef(MessageParcel& data, MessageParcel& reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
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
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
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
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
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
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleIsSupportedApdusExtended, ERR_NO_PERMISSION");
        return KITS::ErrorCode::ERR_NO_PERMISSION;
    }
    bool isSupported = false;
    int statusCode = IsSupportedApdusExtended(isSupported);
    reply.WriteBool(isSupported);
    return statusCode;
}

void TagSessionStub::RemoveForegroundDeathRcpt(const wptr<IRemoteObject> &remote)
{
    std::lock_guard<std::mutex> guard(mutex_);
    if (foregroundCallback_ == nullptr) {
        ErrorLog("OnRemoteDied callback_ is nullptr");
        return;
    }
    auto serviceRemote = foregroundCallback_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        serviceRemote->RemoveDeathRecipient(foregroundDeathRecipient_);
        foregroundCallback_ = nullptr;
        ErrorLog("on remote died");
    }
}

int TagSessionStub::HandleRegForegroundDispatch(MessageParcel &data, MessageParcel &reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleRegForegroundDispatch, ERR_NO_PERMISSION");
        int ret = KITS::ErrorCode::ERR_NO_PERMISSION;
        reply.WriteInt32(ret);
        return ret;
    }
    ElementName* element = ElementName::Unmarshalling(data);
    if (element == nullptr) {
        ErrorLog("HandleRegForegroundDispatch, unmarshalled element is null");
        return KITS::ERR_NFC_PARAMETERS;
    }
    std::vector<uint32_t> discTech;
    data.ReadUInt32Vector(&discTech);
    KITS::ErrorCode ret = KITS::ERR_NFC_PARAMETERS;
    do {
        sptr<IRemoteObject> remote = data.ReadRemoteObject();
        if (remote == nullptr) {
            DebugLog("Failed to readRemoteObject!");
            break;
        }
        std::unique_ptr<ForegroundDeathRecipient> recipient
            = std::make_unique<ForegroundDeathRecipient>(this, IPCSkeleton::GetCallingTokenID());
        sptr<IRemoteObject::DeathRecipient> dr(recipient.release());
        if ((remote->IsProxyObject()) && (!remote->AddDeathRecipient(dr))) {
            ErrorLog("Failed to add death recipient");
            break;
        }
        {
            std::lock_guard<std::mutex> guard(mutex_);
            foregroundDeathRecipient_ = dr;
            foregroundCallback_ = iface_cast<KITS::IForegroundCallback>(remote);
            if (foregroundCallback_ == nullptr) {
                foregroundCallback_ = new (std::nothrow) ForegroundCallbackProxy(remote);
                DebugLog("create new `ForegroundCallbackProxy`!");
            }
            ret = RegForegroundDispatch(*(element), discTech, foregroundCallback_);
        }
    } while (0);
    reply.WriteInt32(ret);

    // element is newed by Unmarshalling, should be deleted
    delete element;
    element = nullptr;
    return ERR_NONE;
}

int TagSessionStub::HandleUnregForegroundDispatch(MessageParcel &data, MessageParcel &reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleUnregForegroundDispatch, ERR_NO_PERMISSION");
        int ret = KITS::ErrorCode::ERR_NO_PERMISSION;
        reply.WriteInt32(ret);
        return ret;
    }
    InfoLog("HandleUnregForegroundDispatch");
    ElementName* element = ElementName::Unmarshalling(data);
    if (element == nullptr) {
        ErrorLog("HandleUnregForegroundDispatch, unmarshalled element is null");
        return KITS::ERR_NFC_PARAMETERS;
    }
    int exception = data.ReadInt32();
    if (exception) {
        // element is newed by Unmarshalling, should be deleted
        delete element;
        element = nullptr;
        return KITS::ERR_NFC_PARAMETERS;
    }
    KITS::ErrorCode ret = UnregForegroundDispatch(*(element));
    DebugLog("HandleUnregForegroundDispatch end##ret=%{public}d\n", ret);
    reply.WriteInt32(ret);

    // element is newed by Unmarshalling, should be deleted
    delete element;
    element = nullptr;
    return ERR_NONE;
}

void TagSessionStub::RemoveReaderModeDeathRcpt(const wptr<IRemoteObject> &remote)
{
    std::lock_guard<std::mutex> guard(mutex_);
    if (readerModeCallback_ == nullptr) {
        ErrorLog("OnRemoteDied callback_ is nullptr");
        return;
    }
    auto serviceRemote = readerModeCallback_->AsObject();
    if ((serviceRemote != nullptr) && (serviceRemote == remote.promote())) {
        serviceRemote->RemoveDeathRecipient(readerModeDeathRecipient_);
        readerModeCallback_ = nullptr;
        ErrorLog("on remote died");
    }
}

int TagSessionStub::HandleRegReaderMode(MessageParcel &data, MessageParcel &reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleRegReaderMode, ERR_NO_PERMISSION");
        int ret = KITS::ErrorCode::ERR_NO_PERMISSION;
        reply.WriteInt32(ret);
        return ret;
    }
    ElementName* element = ElementName::Unmarshalling(data);
    if (element == nullptr) {
        ErrorLog("HandleRegReaderMode, unmarshalled element is null");
        return KITS::ERR_NFC_PARAMETERS;
    }
    std::vector<uint32_t> discTech;
    data.ReadUInt32Vector(&discTech);
    KITS::ErrorCode ret = KITS::ERR_NFC_PARAMETERS;
    do {
        sptr<IRemoteObject> remote = data.ReadRemoteObject();
        if (remote == nullptr) {
            DebugLog("Failed to readRemoteObject!");
            break;
        }
        std::unique_ptr<ReaderModeDeathRecipient> recipient
            = std::make_unique<ReaderModeDeathRecipient>(this, IPCSkeleton::GetCallingTokenID());
        sptr<IRemoteObject::DeathRecipient> dr(recipient.release());
        if ((remote->IsProxyObject()) && (!remote->AddDeathRecipient(dr))) {
            ErrorLog("Failed to add death recipient");
            break;
        }
        {
            std::lock_guard<std::mutex> guard(mutex_);
            readerModeDeathRecipient_ = dr;
            readerModeCallback_ = iface_cast<KITS::IReaderModeCallback>(remote);
            if (readerModeCallback_ == nullptr) {
                readerModeCallback_ = new (std::nothrow) ReaderModeCallbackProxy(remote);
                DebugLog("create new `ReaderModeCallbackProxy`!");
            }
            ret = RegReaderMode(*(element), discTech, readerModeCallback_);
        }
    } while (0);
    reply.WriteInt32(ret);

    // element is newed by Unmarshalling, should be deleted
    delete element;
    element = nullptr;
    return ERR_NONE;
}

int TagSessionStub::HandleUnregReaderMode(MessageParcel &data, MessageParcel &reply)
{
    if (!ExternalDepsProxy::GetInstance().IsGranted(OHOS::NFC::TAG_PERM)) {
        ErrorLog("HandleUnregReaderMode, ERR_NO_PERMISSION");
        int ret = KITS::ErrorCode::ERR_NO_PERMISSION;
        reply.WriteInt32(ret);
        return ret;
    }
    InfoLog("HandleUnregReaderMode");
    ElementName* element = ElementName::Unmarshalling(data);
    if (element == nullptr) {
        ErrorLog("HandleUnregReaderMode, unmarshalled element is null");
        return KITS::ERR_NFC_PARAMETERS;
    }
    int exception = data.ReadInt32();
    if (exception) {
        // element is newed by Unmarshalling, should be deleted
        delete element;
        element = nullptr;
        return KITS::ERR_NFC_PARAMETERS;
    }
    KITS::ErrorCode ret = UnregReaderMode(*(element));
    DebugLog("HandleUnregReaderMode end##ret=%{public}d\n", ret);
    reply.WriteInt32(ret);

    // element is newed by Unmarshalling, should be deleted
    delete element;
    element = nullptr;
    return ERR_NONE;
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
