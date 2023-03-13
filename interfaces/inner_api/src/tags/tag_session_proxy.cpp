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

#include "loghelper.h"
#include "message_option.h"
#include "message_parcel.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace TAG {
int TagSessionProxy::Connect(int tagRfDiscId, int technology)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(tagRfDiscId);
    data.WriteInt32(static_cast<int32_t>(technology));
    MessageOption option(MessageOption::TF_SYNC);
    int result;
    SendRequestExpectReplyInt(KITS::COMMAND_CONNECT, data, option, result);
    return result;
}

int TagSessionProxy::Reconnect(int tagRfDiscId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(tagRfDiscId);
    MessageOption option(MessageOption::TF_SYNC);
    int result;
    SendRequestExpectReplyInt(KITS::COMMAND_RECONNECT, data, option, result);
    return result;
}

void TagSessionProxy::Disconnect(int tagRfDiscId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return;
    }
    data.WriteInt32(tagRfDiscId);
    MessageOption option(MessageOption::TF_ASYNC);
    SendRequestExpectReplyNone(KITS::COMMAND_DISCONNECT, data, option);
}

int TagSessionProxy::SetTimeout(int timeout, int technology)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(technology);
    data.WriteInt32(timeout);
    MessageOption option(MessageOption::TF_SYNC);
    return SendRequestExpectReplyNone(KITS::COMMAND_SET_TIMEOUT, data, option);
}

int TagSessionProxy::GetTimeout(int technology, int &timeout)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(technology);
    MessageOption option(MessageOption::TF_SYNC);
    return SendRequestExpectReplyInt(KITS::COMMAND_GET_TIMEOUT, data, option, timeout);
}

int TagSessionProxy::GetMaxTransceiveLength(int technology, int &maxSize)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(technology);
    MessageOption option(MessageOption::TF_SYNC);
    return SendRequestExpectReplyInt(KITS::COMMAND_GET_MAX_TRANSCEIVE_LENGTH, data, option, maxSize);
}

int TagSessionProxy::SendRawFrame(int tagRfDiscId, std::string hexCmdData, bool raw, std::string &hexRespData)
{
    MessageParcel data, reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(tagRfDiscId);
    data.WriteString(hexCmdData);
    data.WriteBool(raw);
    int statusCode = Remote()->SendRequest(KITS::COMMAND_SEND_RAW_FRAME, data, reply, option);
    if (statusCode == ERR_NONE) {
        hexRespData = reply.ReadString();
    }
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
    int res = Remote()->SendRequest(KITS::COMMAND_GET_TECHLIST, data, reply, option);
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
    SendRequestExpectReplyBool(KITS::COMMAND_IS_PRESENT, data, option, result);
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
    SendRequestExpectReplyBool(KITS::COMMAND_IS_NDEF, data, option, result);
    return result;
}

std::string TagSessionProxy::NdefRead(int tagRfDiscId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return "";
    }
    data.WriteInt32(tagRfDiscId);
    int res = Remote()->SendRequest(KITS::COMMAND_NDEF_READ, data, reply, option);
    if (res != ERR_NONE) {
        ErrorLog("It is failed To Ndef Read with Res(%{public}d).", res);
        return std::string();
    }
    return reply.ReadString();
}

int TagSessionProxy::NdefWrite(int tagRfDiscId, std::string msg)
{
    int result = 0;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(tagRfDiscId);
    data.WriteString(msg);
    MessageOption option(MessageOption::TF_SYNC);
    SendRequestExpectReplyInt(KITS::COMMAND_NDEF_WRITE, data, option, result);
    return result;
}

int TagSessionProxy::NdefMakeReadOnly(int tagRfDiscId)
{
    int result = 0;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(tagRfDiscId);
    MessageOption option(MessageOption::TF_SYNC);
    SendRequestExpectReplyInt(KITS::COMMAND_NDEF_MAKE_READ_ONLY, data, option, result);
    return result;
}

int TagSessionProxy::FormatNdef(int tagRfDiscId, const std::string& key)
{
    int result = 0;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    data.WriteInt32(tagRfDiscId);
    data.WriteString(key);
    MessageOption option(MessageOption::TF_SYNC);
    SendRequestExpectReplyInt(KITS::COMMAND_FORMAT_NDEF, data, option, result);
    return result;
}

int TagSessionProxy::CanMakeReadOnly(int ndefType, bool &canSetReadOnly)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return false;
    }
    data.WriteInt32(ndefType);
    MessageOption option(MessageOption::TF_SYNC);
    int statusCode = SendRequestExpectReplyBool(KITS::COMMAND_CAN_MAKE_READ_ONLY, data, option, canSetReadOnly);
    return statusCode;
}

int TagSessionProxy::IsSupportedApdusExtended(bool &isSupported)
{
    MessageParcel data;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    return SendRequestExpectReplyBool(KITS::COMMAND_IS_SUPPORTED_APDUS_EXTENDED, data, option, isSupported);
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
