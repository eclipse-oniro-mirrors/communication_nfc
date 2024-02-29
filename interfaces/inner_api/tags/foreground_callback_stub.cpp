/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "foreground_callback_stub.h"

#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace TAG {
ForegroundCallbackStub *g_foregroundCallbackStub = nullptr;

ForegroundCallbackStub::ForegroundCallbackStub() : callback_(nullptr), mRemoteDied(false)
{
    InfoLog("ForegroundCallbackStub");
}

ForegroundCallbackStub::~ForegroundCallbackStub()
{
    if (g_foregroundCallbackStub != nullptr) {
        DebugLog("g_foregroundCallbackStub != nullptr");
        g_foregroundCallbackStub = nullptr;
    }
    InfoLog("~ForegroundCallbackStub");
}

ForegroundCallbackStub* ForegroundCallbackStub::GetInstance()
{
    if (g_foregroundCallbackStub == nullptr) {
        DebugLog("new ForegroundCallbackStub");
        g_foregroundCallbackStub = new ForegroundCallbackStub();
    }
    return g_foregroundCallbackStub;
}

void ForegroundCallbackStub::OnTagDiscovered(KITS::TagInfoParcelable* tagInfo)
{
    if (callback_) {
        DebugLog(" callback_");
        callback_->OnTagDiscovered(tagInfo);
    }
}

int ForegroundCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    DebugLog("ForegroundCallbackStub::OnRemoteRequest,code = %{public}d", code);
    if (mRemoteDied) {
        return KITS::ERR_NFC_STATE_UNBIND;
    }
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        ErrorLog("nfc callback stub token verification error");
        return KITS::ERR_NFC_PARAMETERS;
    }
    int exception = data.ReadInt32();
    if (exception) {
        ErrorLog("ForegroundCallbackStub::OnRemoteRequest, got exception: (%{public}d))", exception);
        return exception;
    }
    int ret = KITS::ERR_NFC_STATE_UNBIND;
    switch (code) {
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_TAG_FOUND_FOREGROUND): {
            ret = RemoteTagDiscovered(data, reply);
            break;
        }
        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }
    return ret;
}

KITS::ErrorCode ForegroundCallbackStub::RegForegroundDispatch(const sptr<KITS::IForegroundCallback> &callback)
{
    DebugLog("ForegroundCallbackStub RegForegroundCallback");
    std::unique_lock<std::shared_mutex> guard(callbackMutex);
    if (callback == nullptr) {
        ErrorLog("RegForegroundCallback:callback is nullptr!");
        callback_ = callback;
        return KITS::ERR_NFC_PARAMETERS;
    }
    callback_ = callback;
    return KITS::ERR_NONE;
}

int ForegroundCallbackStub::RemoteTagDiscovered(MessageParcel &data, MessageParcel &reply)
{
    KITS::TagInfoParcelable* tagInfo = KITS::TagInfoParcelable::Unmarshalling(data);
    if (tagInfo == nullptr) {
        reply.WriteInt32(KITS::ERR_NFC_PARAMETERS); /* Reply 0 to indicate that no exception occurs. */
        return KITS::ERR_NFC_PARAMETERS;
    }
    std::unique_lock<std::shared_mutex> guard(callbackMutex);
    OnTagDiscovered(tagInfo);
    reply.WriteInt32(KITS::ERR_NONE); /* Reply 0 to indicate that no exception occurs. */

    // tagInfo is newed by Unmarshalling, should be deleted
    delete tagInfo;
    tagInfo = nullptr;
    return KITS::ERR_NONE;
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS