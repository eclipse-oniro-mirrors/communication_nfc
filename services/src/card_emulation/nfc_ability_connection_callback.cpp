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
#include "nfc_ability_connection_callback.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
NfcAbilityConnectionCallback::NfcAbilityConnectionCallback()
{
    serviceConnected_ = false;
}

NfcAbilityConnectionCallback::~NfcAbilityConnectionCallback()
{
    serviceConnected_ = false;
}

void NfcAbilityConnectionCallback::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
                                                        const sptr<IRemoteObject> &remoteObject, int resultCode)
{
    InfoLog("service connected: %{public}s, result code %{public}d", element.GetURI().c_str(), resultCode);
    serviceConnected_ = true;
    connectedElement_.SetBundleName(element.GetBundleName());
    connectedElement_.SetAbilityName(element.GetAbilityName());
    connectedElement_.SetDeviceID(element.GetDeviceID());
    connectedElement_.SetModuleName(element.GetModuleName());
    if (hceManager_.expired()) {
        ErrorLog("hce manager is expired");
        return;
    }
    hceManager_.lock()->HandleQueueData();
}

void NfcAbilityConnectionCallback::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode)
{
    InfoLog("service disconnected done: %{public}s, result code %{public}d", element.GetURI().c_str(), resultCode);
    serviceConnected_ = false;
    connectedElement_.SetBundleName("");
    connectedElement_.SetAbilityName("");
    connectedElement_.SetDeviceID("");
    connectedElement_.SetModuleName("");
}
bool NfcAbilityConnectionCallback::ServiceConnected()
{
    return serviceConnected_;
}
void NfcAbilityConnectionCallback::SetHceManager(std::weak_ptr<HostCardEmulationManager> hceManager)
{
    hceManager_ = hceManager;
}
AppExecFwk::ElementName NfcAbilityConnectionCallback::GetConnectedElement()
{
    return connectedElement_;
}
} // namespace NFC
} // namespace OHOS
