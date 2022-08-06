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
    int result = 0;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_UNKOWN;
    }
    data.WriteInt32(tagRfDiscId);
    data.WriteInt32(static_cast<int32_t>(technology));
    MessageOption option(MessageOption::TF_SYNC);
    ProcessIntRes(KITS::COMMAND_CONNECT, data, option, result);
    return result;
}

int TagSessionProxy::Reconnect(int tagRfDiscId)
{
    int result = 0;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_UNKOWN;
    }
    data.WriteInt32(tagRfDiscId);
    MessageOption option(MessageOption::TF_SYNC);
    ProcessIntRes(KITS::COMMAND_RECONNECT, data, option, result);
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
    ProcessCommand(KITS::COMMAND_DISCONNECT, data, option);
    return;
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
        InfoLog("It is failed To Get Tech List with Res(%{public}d).", res);
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
    ProcessBoolRes(KITS::COMMAND_IS_PRESENT, data, option, result);
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
    ProcessBoolRes(KITS::COMMAND_IS_NDEF, data, option, result);
    return result;
}

std::unique_ptr<ResResult> TagSessionProxy::SendRawFrame(int tagRfDiscId, std::string msg, bool raw)
{
    MessageParcel data, reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return std::unique_ptr<ResResult>();
    }
    data.WriteInt32(tagRfDiscId);
    data.WriteString(msg);
    data.WriteBool(raw);
    int res = Remote()->SendRequest(KITS::COMMAND_SEND_RAW_FRAME, data, reply, option);
    if (res != ERR_NONE) {
        InfoLog("It is failed To Send Raw Frame with Res(%{public}d).", res);
        return std::unique_ptr<ResResult>();
    }
    sptr<ResResult> result = reply.ReadStrongParcelable<ResResult>();
    int res1 = reply.ReadInt32();
    if (res1 != ERR_NONE) {
        InfoLog("It is failed To Send Raw Frame with Res1(%{public}d).", res1);
        return std::unique_ptr<ResResult>();
    }
    std::unique_ptr<ResResult> resResult = std::make_unique<ResResult>();
    resResult->SetResult(result->GetResult());
    resResult->SetResData(result->GetResData());
    DebugLog("TagSessionProxy::SendRawFrame result.%{public}d", result->GetResult());
    return resResult;
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
        InfoLog("It is failed To Ndef Read with Res(%{public}d).", res);
        return std::string();
    }
    return reply.ReadString();
}

int TagSessionProxy::NdefWrite(int tagRfDiscId, std::string msg)
{
    int result = 0;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_UNKOWN;
    }
    data.WriteInt32(tagRfDiscId);
    data.WriteString(msg);
    MessageOption option(MessageOption::TF_SYNC);
    ProcessIntRes(KITS::COMMAND_NDEF_WRITE, data, option, result);
    return result;
}

int TagSessionProxy::NdefMakeReadOnly(int tagRfDiscId)
{
    int result = 0;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_UNKOWN;
    }
    data.WriteInt32(tagRfDiscId);
    MessageOption option(MessageOption::TF_SYNC);
    ProcessIntRes(KITS::COMMAND_NDEF_MAKE_READ_ONLY, data, option, result);
    return result;
}

int TagSessionProxy::FormatNdef(int tagRfDiscId, const std::string& key)
{
    int result = 0;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_UNKOWN;
    }
    data.WriteInt32(tagRfDiscId);
    data.WriteString(key);
    MessageOption option(MessageOption::TF_SYNC);
    ProcessIntRes(KITS::COMMAND_FORMAT_NDEF, data, option, result);
    return result;
}

bool TagSessionProxy::CanMakeReadOnly(int technology)
{
    bool result = false;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return false;
    }
    data.WriteInt32(technology);
    MessageOption option(MessageOption::TF_SYNC);
    ProcessBoolRes(KITS::COMMAND_CAN_MAKE_READ_ONLY, data, option, result);
    return result;
}

int TagSessionProxy::GetMaxTransceiveLength(int technology)
{
    int result = 0;
    MessageParcel data;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return KITS::NfcErrorCode::NFC_SDK_ERROR_UNKOWN;
    }
    data.WriteInt32(technology);
    MessageOption option(MessageOption::TF_SYNC);
    ProcessIntRes(KITS::COMMAND_GET_MAX_TRANSCEIVE_LENGTH, data, option, result);
    return result;
}

bool TagSessionProxy::IsSupportedApdusExtended()
{
    bool result = false;
    MessageParcel data;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return false;
    }
    ProcessBoolRes(KITS::COMMAND_IS_SUPPORTED_APDUS_EXTENDED, data, option, result);
    return result;
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
