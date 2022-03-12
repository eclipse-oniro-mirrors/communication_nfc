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
#include "connected_tag_service_impl.h"
#include "error_code.h"
#include "connected_tag_hdi_adapter.h"
#include "ipc_cmd.h"
#include "log.h"
#include "permission_tools.h"

namespace OHOS {
namespace ConnectedTag {
std::mutex NfcConnectedTagServiceImpl::g_instanceLock;
sptr<NfcConnectedTagServiceImpl> NfcConnectedTagServiceImpl::g_instance;
const bool REGISTER_RESULT = SystemAbility::MakeAndRegisterAbility(NfcConnectedTagServiceImpl::GetInstance().GetRefPtr());

sptr<NfcConnectedTagServiceImpl> NfcConnectedTagServiceImpl::GetInstance()
{
    if (g_instance == nullptr) {
        std::lock_guard<std::mutex> autoLock(g_instanceLock);
        if (g_instance == nullptr) {
            auto service = new (std::nothrow) NfcConnectedTagServiceImpl;
            g_instance = service;
        }
    }
    return g_instance;
}

NfcConnectedTagServiceImpl::NfcConnectedTagServiceImpl()
    : SystemAbility(NFC_CONNECTED_TAG_ABILITY_ID, true), mPublishFlag(false), mState(ServiceRunningState::STATE_NOT_START)
{
}

NfcConnectedTagServiceImpl::~NfcConnectedTagServiceImpl()
{
}

void NfcConnectedTagServiceImpl::OnStart()
{
    HILOGI("NfcConnectedTagServiceImpl::OnStart() in");
    if (mState == ServiceRunningState::STATE_RUNNING) {
        HILOGI("Service has already started.");
        return;
    }
    if (!ServiceInit()) {
        HILOGE("Failed to init service");
        OnStop();
        return;
    }
    mState = ServiceRunningState::STATE_RUNNING;
    HILOGI("Start service!");
}

void NfcConnectedTagServiceImpl::OnStop()
{
    HILOGI("NfcConnectedTagServiceImpl::OnStop() in");
    mState = ServiceRunningState::STATE_NOT_START;
    mPublishFlag = false;
    HILOGI("Stop service!");
}

bool NfcConnectedTagServiceImpl::ServiceInit()
{
    HILOGI("NfcConnectedTagServiceImpl::ServiceInit() in!");
    if (!mPublishFlag) {
        bool ret = Publish(NfcConnectedTagServiceImpl::GetInstance());
        if (!ret) {
            HILOGE("Failed to publish service!");
            return false;
        }
        mPublishFlag = true;
    }
    return true;
}

ErrCode NfcConnectedTagServiceImpl::Init()
{
    HILOGE("NfcConnectedTagServiceImpl:Init() in!");
    if (!PermissionTools::IsGranted(OHOS::ConnectedTag::TAG_PERMISSION)) {
        HILOGE("NfcConnectedTagServiceImpl:Init() IsGranted failed!");
        return NFC_OPT_FAILED;
    }
    if (NfcHdiAdapter::GetInstance().Init() == 0) {
        return NFC_OPT_SUCCESS;
    }
    return NFC_OPT_FAILED;
}

ErrCode NfcConnectedTagServiceImpl::Uninit()
{
    if (!PermissionTools::IsGranted(OHOS::ConnectedTag::TAG_PERMISSION)) {
        HILOGE("NfcConnectedTagServiceImpl:Uninit() IsGranted failed!");
        return NFC_OPT_FAILED;
    }
    if (NfcHdiAdapter::GetInstance().Uninit() == 0) {
        return NFC_OPT_SUCCESS;
    }
    return NFC_OPT_FAILED;
}

ErrCode NfcConnectedTagServiceImpl::ReadNdefTag(std::string &response)
{
    if (!PermissionTools::IsGranted(OHOS::ConnectedTag::TAG_PERMISSION)) {
        HILOGE("NfcConnectedTagServiceImpl:ReadNdefTag() IsGranted failed!");
        return NFC_OPT_FAILED;
    }
    response = NfcHdiAdapter::GetInstance().ReadNdefTag();
    return NFC_OPT_SUCCESS;
}

ErrCode NfcConnectedTagServiceImpl::WriteNdefTag(std::string data)
{
    if (!PermissionTools::IsGranted(OHOS::ConnectedTag::TAG_PERMISSION)) {
        HILOGE("NfcConnectedTagServiceImpl:WriteNdefTag() IsGranted failed!");
        return NFC_OPT_FAILED;
    }
    if (NfcHdiAdapter::GetInstance().WriteNdefTag(data) == 0) {
        return NFC_OPT_SUCCESS;
    }
    return NFC_OPT_FAILED;
}

ErrCode NfcConnectedTagServiceImpl::RegListener(const sptr<IConnectedTagCallBack> &callback)
{
    if (!PermissionTools::IsGranted(OHOS::ConnectedTag::TAG_PERMISSION)) {
        HILOGE("NfcConnectedTagServiceImpl:RegListener() IsGranted failed!");
        return NFC_OPT_FAILED;
    }
    return NFC_OPT_SUCCESS;
}
ErrCode NfcConnectedTagServiceImpl::UnregListener(const sptr<IConnectedTagCallBack> &callback)
{
    if (!PermissionTools::IsGranted(OHOS::ConnectedTag::TAG_PERMISSION)) {
        HILOGE("NfcConnectedTagServiceImpl:UnregListener() IsGranted failed!");
        return NFC_OPT_FAILED;
    }
    return NFC_OPT_SUCCESS;
}
}  // namespace ConnectedTag
}  // namespace OHOS