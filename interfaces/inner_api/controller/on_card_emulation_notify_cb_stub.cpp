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

#include "on_card_emulation_notify_cb_stub.h"

#include "nfc_service_ipc_interface_code.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
OnCardEmulationNotifyCbStub::OnCardEmulationNotifyCbStub() : callback_(nullptr), isRemoteDied_(false)
{}

OnCardEmulationNotifyCbStub::~OnCardEmulationNotifyCbStub()
{}

OnCardEmulationNotifyCbStub& OnCardEmulationNotifyCbStub::GetInstance()
{
    static OnCardEmulationNotifyCbStub instance;
    return instance;
}

bool OnCardEmulationNotifyCbStub::OnCardEmulationNotify(uint32_t eventType, std::string apduData)
{
    if (callback_) {
        InfoLog("OnCardEmulationNotify:call callback_");
        callback_(eventType, apduData);
        return true;
    }
    return false;
}

int OnCardEmulationNotifyCbStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    InfoLog("OnRemoteRequest: code = %{public}d", code);
    if (isRemoteDied_) {
        ErrorLog("remote service is died.");
        return KITS::ERR_NFC_STATE_UNBIND;
    }
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        ErrorLog("OnRemoteRequest: token verification error.");
        return KITS::ERR_NFC_PARAMETERS;
    }
    int exception = data.ReadInt32();
    if (exception) {
        ErrorLog("OnRemoteRequest:got exception: (%{public}d).", exception);
        return exception;
    }

    int ret = KITS::ERR_NFC_STATE_UNBIND;
    switch (code) {
        case static_cast<uint32_t>(NfcServiceIpcInterfaceCode::COMMAND_ON_CARD_EMULATION_NOTIFY): {
            ret = RemoteCardEmulationNotify(data, reply);
            break;
        }

        default: {
            ret = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
        }
    }
    return ret;
}

KITS::ErrorCode OnCardEmulationNotifyCbStub::RegisterCallback(const OnCardEmulationNotifyCb callback)
{
    if (callback_ != nullptr) {
        InfoLog("RegisterCallback::callback_ has registered!");
        return KITS::ERR_NFC_PARAMETERS;
    }
    std::unique_lock<std::shared_mutex> guard(mutex_);
    if (callback == nullptr) {
        InfoLog("RegisterCallback::callback is nullptr!");
        callback_ = callback;
        return KITS::ERR_NFC_PARAMETERS;
    }
    callback_ = callback;
    return KITS::ERR_NONE;
}

int OnCardEmulationNotifyCbStub::RemoteCardEmulationNotify(MessageParcel &data, MessageParcel &reply)
{
    std::unique_lock<std::shared_mutex> guard(mutex_);
    uint32_t eventType = data.ReadInt32();
    std::string apduData = data.ReadString();
    OnCardEmulationNotify(eventType, apduData);
    return KITS::ERR_NONE;
}
}  // namespace NFC
}  // namespace OHOS