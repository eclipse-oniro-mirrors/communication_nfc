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
#include "tag_session_proxy.h"

#include "element_name.h"
#include "foreground_callback_stub.h"
#include "loghelper.h"
#include "message_option.h"
#include "message_parcel.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include "reader_mode_callback_stub.h"

namespace OHOS {
namespace NFC {
namespace TAG {
using OHOS::AppExecFwk::ElementName;

int TagSessionProxy::Connect(int tagRfDiscId, int technology)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(tagRfDiscId);
    data.WriteInt32(static_cast<int32_t>(technology));
    MessageOption option(MessageOption::TF_SYNC);
    int statusCode = SendRequestExpectReplyNoneAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_CONNECT), data, reply, option);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::Connect, statusCode = 0x%{public}X", statusCode);
    return statusCode;
}

int TagSessionProxy::Reconnect(int tagRfDiscId)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(tagRfDiscId);
    MessageOption option(MessageOption::TF_SYNC);
    int statusCode = SendRequestExpectReplyNoneAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_RECONNECT), data, reply, option);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::Reconnect, statusCode = 0x%{public}X", statusCode);
    return statusCode;
}

void TagSessionProxy::Disconnect(int tagRfDiscId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return;
    }
    data.WriteInt32(tagRfDiscId);
    MessageOption option(MessageOption::TF_ASYNC);
    SendRequestExpectReplyNone(static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_DISCONNECT),
        data, option);
}

int TagSessionProxy::SetTimeout(int tagRfDiscId, int timeout, int technology)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(tagRfDiscId);
    data.WriteInt32(technology);
    data.WriteInt32(timeout);
    MessageOption option(MessageOption::TF_SYNC);
    int statusCode = SendRequestExpectReplyNoneAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_SET_TIMEOUT), data, reply, option);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::SetTimeout, statusCode = 0x%{public}X", statusCode);
    return statusCode;
}

int TagSessionProxy::GetTimeout(int tagRfDiscId, int technology, int &timeout)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(tagRfDiscId);
    data.WriteInt32(technology);
    MessageOption option(MessageOption::TF_SYNC);
    int statusCode = SendRequestExpectReplyIntAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_GET_TIMEOUT), data, reply, option, timeout);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::GetTimeout, statusCode = 0x%{public}X", statusCode);
    return statusCode;
}

void TagSessionProxy::ResetTimeout(int tagRfDiscId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return;
    }
    data.WriteInt32(tagRfDiscId);
    MessageOption option(MessageOption::TF_SYNC);
    SendRequestExpectReplyNone(static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_RESET_TIMEOUT),
        data, option);
}

int TagSessionProxy::GetMaxTransceiveLength(int technology, int &maxSize)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(technology);
    MessageOption option(MessageOption::TF_SYNC);
    int statusCode = SendRequestExpectReplyIntAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_GET_MAX_TRANSCEIVE_LENGTH),
        data, reply, option, maxSize);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::GetMaxTransceiveLength, statusCode = 0x%{public}X", statusCode);
    return statusCode;
}

int TagSessionProxy::SendRawFrame(const int tagRfDiscId, std::string hexCmdData, bool raw, std::string &hexRespData)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(tagRfDiscId);
    data.WriteString(hexCmdData);
    data.WriteBool(raw);
    int statusCode = SendRequestExpectReplyStringAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_SEND_RAW_FRAME), data, reply, option, hexRespData);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::SendRawFrame, statusCode=0x%{public}X", statusCode);
    return statusCode;
}

std::vector<int> TagSessionProxy::GetTechList(int tagRfDiscId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return std::vector<int>();
    }
    data.WriteInt32(tagRfDiscId);
    int res = Remote()->SendRequest(static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_GET_TECHLIST),
        data, reply, option);
    if (res != ERR_NONE) {
        ErrorLog("It is failed To Get Tech List with Res(%{public}d).", res);
        return std::vector<int>();
    }
    std::vector<int32_t> result {};
    reply.ReadInt32Vector(&result);
    return result;
}

bool TagSessionProxy::IsTagFieldOn(int tagRfDiscId)
{
    bool result = false;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return false;
    }
    data.WriteInt32(tagRfDiscId);
    MessageOption option(MessageOption::TF_SYNC);
    SendRequestExpectReplyBool(static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_IS_PRESENT),
        data, option, result);
    return result;
}

bool TagSessionProxy::IsNdef(int tagRfDiscId)
{
    bool result = false;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return false;
    }
    data.WriteInt32(tagRfDiscId);
    MessageOption option(MessageOption::TF_SYNC);
    SendRequestExpectReplyBool(static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_IS_NDEF),
        data, option, result);
    return result;
}

int TagSessionProxy::NdefRead(int tagRfDiscId, std::string &ndefMessage)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(tagRfDiscId);
    MessageOption option(MessageOption::TF_SYNC);
    int statusCode = SendRequestExpectReplyStringAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_NDEF_READ), data, reply, option, ndefMessage);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::NdefRead statusCode = %{public}X", statusCode);
    return statusCode;
}

int TagSessionProxy::NdefWrite(int tagRfDiscId, std::string msg)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(tagRfDiscId);
    data.WriteString(msg);
    MessageOption option(MessageOption::TF_SYNC);
    int statusCode = SendRequestExpectReplyNoneAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_NDEF_WRITE), data, reply, option);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::NdefWrite statusCode = %{public}X", statusCode);
    return statusCode;
}

int TagSessionProxy::NdefMakeReadOnly(int tagRfDiscId)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(tagRfDiscId);
    MessageOption option(MessageOption::TF_SYNC);
    int statusCode = SendRequestExpectReplyNoneAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_NDEF_MAKE_READ_ONLY), data, reply, option);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::NdefMakeReadOnly statusCode = %{public}X", statusCode);
    return statusCode;
}

int TagSessionProxy::FormatNdef(int tagRfDiscId, const std::string& key)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(tagRfDiscId);
    data.WriteString(key);
    MessageOption option(MessageOption::TF_SYNC);
    int statusCode = SendRequestExpectReplyNoneAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_FORMAT_NDEF), data, reply, option);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::FormatNdef statusCode = %{public}X", statusCode);
    return statusCode;
}

int TagSessionProxy::CanMakeReadOnly(int ndefType, bool &canSetReadOnly)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return false;
    }
    data.WriteInt32(ndefType);
    MessageOption option(MessageOption::TF_SYNC);
    int statusCode = SendRequestExpectReplyBoolAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_CAN_MAKE_READ_ONLY),
        data, reply, option, canSetReadOnly);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::CanMakeReadOnly statusCode = %{public}X", statusCode);
    return statusCode;
}

int TagSessionProxy::IsSupportedApdusExtended(bool &isSupported)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    MessageOption option(MessageOption::TF_SYNC);
    int statusCode = SendRequestExpectReplyBoolAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_IS_SUPPORTED_APDUS_EXTENDED),
        data, reply, option, isSupported);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::IsSupportedApdusExtended statusCode = %{public}X", statusCode);
    return statusCode;
}

int TagSessionProxy::RegForegroundDispatch(ElementName &element, std::vector<uint32_t> &discTech,
    const sptr<KITS::IForegroundCallback> &callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    ForegroundCallbackStub::GetInstance()->RegForegroundDispatch(callback);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("RegForegroundDispatch: Write interface token error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!element.Marshalling(data)) {
        ErrorLog("RegForegroundDispatch: Write element error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!data.WriteUInt32Vector(discTech)) {
        ErrorLog("RegForegroundDispatch: Write discTech error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!data.WriteRemoteObject(ForegroundCallbackStub::GetInstance()->AsObject())) {
        ErrorLog("RegForegroundDispatch: WriteRemoteObject failed!");
        return KITS::ERR_NFC_PARAMETERS;
    }
    int statusCode = SendRequestExpectReplyNoneAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_REG_FOREGROUND), data, reply, option);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::RegForegroundDispatch statusCode = %{public}X", statusCode);
    return statusCode;
}

int TagSessionProxy::UnregForegroundDispatch(ElementName &element)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("Write interface token error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!element.Marshalling(data)) {
        ErrorLog("Write element error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    data.WriteInt32(0);
    int statusCode = SendRequestExpectReplyNoneAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_UNREG_FOREGROUND), data, reply, option);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::UnregForegroundDispatch statusCode = %{public}X", statusCode);
    return statusCode;
}

int TagSessionProxy::RegReaderMode(ElementName &element, std::vector<uint32_t> &discTech,
    const sptr<KITS::IReaderModeCallback> &callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    ReaderModeCallbackStub::GetInstance()->RegReaderMode(callback);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("RegReaderMode: Write interface token error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!element.Marshalling(data)) {
        ErrorLog("RegReaderMode: Write element error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!data.WriteUInt32Vector(discTech)) {
        ErrorLog("RegReaderMode: Write discTech error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!data.WriteRemoteObject(ReaderModeCallbackStub::GetInstance()->AsObject())) {
        ErrorLog("RegReaderMode: WriteRemoteObject failed!");
        return KITS::ERR_NFC_PARAMETERS;
    }
    int statusCode = SendRequestExpectReplyNoneAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_REG_READER_MODE), data, reply, option);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::RegReaderMode statusCode = %{public}X", statusCode);
    return statusCode;
}

int TagSessionProxy::UnregReaderMode(ElementName &element)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        ErrorLog("UnregReaderMode:Write interface token error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    if (!element.Marshalling(data)) {
        ErrorLog("UnregReaderMode:Write element error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    data.WriteInt32(0);
    int statusCode = SendRequestExpectReplyNoneAndStatusCode(
        static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_UNREG_READER_MODE), data, reply, option);
    if (statusCode == ERR_NONE) {
        statusCode = reply.ReadInt32();
    }
    InfoLog("TagSessionProxy::UnregReaderMode statusCode = %{public}X", statusCode);
    return statusCode;
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
