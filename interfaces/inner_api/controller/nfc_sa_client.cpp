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

#include "nfc_sa_client.h"

#include "iremote_broker.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "infc_controller_callback.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
static constexpr int32_t NFC_LOADSA_TIMEOUT_MS = 1000; // ms

NfcSaClient &NfcSaClient::GetInstance()
{
    DebugLog("NfcSaClient::%{public}s enter", __func__);
    static NfcSaClient nfcSaClient;
    return nfcSaClient;
}

sptr<IRemoteObject> NfcSaClient::LoadNfcSa(int32_t systemAbilityId)
{
    DebugLog("NfcSaClient::%{public}s enter, systemAbilityId [%{public}d] loading", __func__, systemAbilityId);
    InitLoadState();
    sptr<ISystemAbilityManager> samgr =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        ErrorLog("NfcSaClient::%{public}s get system ability manager failed!", __func__);
        return nullptr;
    }
    auto object = samgr->CheckSystemAbility(systemAbilityId);
    if (object != nullptr) {
        InfoLog("NfcSaClient::%{public}s CheckSystemAbility systemAbilityId [%{public}d] SUCCESS",
            __func__, systemAbilityId);
        remoteObject_ = object;
        return remoteObject_;
    }

    auto nfcSaLoadCallback = sptr<NfcSaLoadCallback>(new NfcSaLoadCallback());
    int32_t ret = samgr->LoadSystemAbility(systemAbilityId, nfcSaLoadCallback);
    if (ret != ERR_NONE) {
        ErrorLog("NfcSaClient::%{public}s LoadSystemAbility systemAbilityId [%{public}d] FAILED, ret %{public}d",
            __func__, systemAbilityId, ret);
        return nullptr;
    }
    if (WaitLoadStateChange(systemAbilityId)) {
        InfoLog("NfcSaClient::%{public}s LoadSystemAbility systemAbilityId [%{public}d] SUCCESS",
            __func__, systemAbilityId);
        return remoteObject_;
    }
    ErrorLog("NfcSaClient::%{public}s LoadSystemAbility systemAbilityId [%{public}d] FAILED",
        __func__, systemAbilityId);
    return nullptr;
}

void NfcSaClient::InitLoadState()
{
    std::unique_lock<std::mutex> lock(locatorMutex_);
    loadState_ = false;
}

bool NfcSaClient::WaitLoadStateChange(int32_t systemAbilityId)
{
    std::unique_lock<std::mutex> lock(locatorMutex_);
    auto wait = locatorCond_.wait_for(lock, std::chrono::milliseconds(NFC_LOADSA_TIMEOUT_MS), [this] {
        return loadState_ == true;
    });
    if (!wait) {
        ErrorLog("NfcSaClient::%{public}s locator sa  [%{public}d] time out.", __func__, systemAbilityId);
        return false;
    }
    return true;
}

void NfcSaClient::LoadSystemAbilitySuccess(const sptr<IRemoteObject> &remoteObject)
{
    std::unique_lock<std::mutex> lock(locatorMutex_);
    loadState_ = true;
    remoteObject_ = remoteObject;
    locatorCond_.notify_one();
}

void NfcSaClient::LoadSystemAbilityFail()
{
    std::unique_lock<std::mutex> lock(locatorMutex_);
    loadState_ = false;
    locatorCond_.notify_one();
}

void NfcSaLoadCallback::OnLoadSystemAbilitySuccess(
    int32_t systemAbilityId, const sptr<IRemoteObject> &remoteObject)
{
    DebugLog("NfcSaClient Load SA success, systemAbilityId = [%{public}d]", systemAbilityId);
    NfcSaClient::GetInstance().LoadSystemAbilitySuccess(remoteObject);
}

void NfcSaLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    DebugLog("NfcSaClient Load SA failed, systemAbilityId = [%{public}d]", systemAbilityId);
    NfcSaClient::GetInstance().LoadSystemAbilityFail();
}
}  // namespace NFC
}  // namespace OHOS