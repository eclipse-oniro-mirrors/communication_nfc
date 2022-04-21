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
std::mutex ConnectedTagServiceImpl::g_instanceLock;
sptr<ConnectedTagServiceImpl> ConnectedTagServiceImpl::g_instance;
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(ConnectedTagServiceImpl::GetInstance().GetRefPtr());

sptr<ConnectedTagServiceImpl> ConnectedTagServiceImpl::GetInstance()
{
    if (g_instance == nullptr) {
        std::lock_guard<std::mutex> autoLock(g_instanceLock);
        if (g_instance == nullptr) {
            auto service = new (std::nothrow) ConnectedTagServiceImpl;
            g_instance = service;
        }
    }
    return g_instance;
}

ConnectedTagServiceImpl::ConnectedTagServiceImpl()
    : SystemAbility(NFC_CONNECTED_TAG_ABILITY_ID, true), mPublishFlag(false),
    mState(ServiceRunningState::STATE_NOT_START)
{
}

ConnectedTagServiceImpl::~ConnectedTagServiceImpl()
{
}

void ConnectedTagServiceImpl::OnStart()
{
    HILOGI("ConnectedTagServiceImpl::OnStart() in");
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

void ConnectedTagServiceImpl::OnStop()
{
    HILOGI("ConnectedTagServiceImpl::OnStop() in");
    mState = ServiceRunningState::STATE_NOT_START;
    mPublishFlag = false;
    HILOGI("Stop service!");
}

bool ConnectedTagServiceImpl::ServiceInit()
{
    HILOGI("ConnectedTagServiceImpl::ServiceInit() in!");
    if (!mPublishFlag) {
        bool ret = Publish(ConnectedTagServiceImpl::GetInstance());
        if (!ret) {
            HILOGE("Failed to publish service!");
            return false;
        }
        mPublishFlag = true;
    }
    return true;
}

ErrCode ConnectedTagServiceImpl::Init()
{
    HILOGE("ConnectedTagServiceImpl:Init() in!");
    if (!PermissionTools::IsGranted(OHOS::ConnectedTag::TAG_PERMISSION)) {
        HILOGE("ConnectedTagServiceImpl:Init() IsGranted failed!");
        return NFC_OPT_FAILED;
    }
    if (ConnectedTagHdiAdapter::GetInstance().Init() == 0) {
        return NFC_OPT_SUCCESS;
    }
    return NFC_OPT_FAILED;
}

ErrCode ConnectedTagServiceImpl::Uninit()
{
    if (!PermissionTools::IsGranted(OHOS::ConnectedTag::TAG_PERMISSION)) {
        HILOGE("ConnectedTagServiceImpl:Uninit() IsGranted failed!");
        return NFC_OPT_FAILED;
    }
    if (ConnectedTagHdiAdapter::GetInstance().Uninit() == 0) {
        return NFC_OPT_SUCCESS;
    }
    return NFC_OPT_FAILED;
}

ErrCode ConnectedTagServiceImpl::ReadNdefTag(std::string &response)
{
    if (!PermissionTools::IsGranted(OHOS::ConnectedTag::TAG_PERMISSION)) {
        HILOGE("ConnectedTagServiceImpl:ReadNdefTag() IsGranted failed!");
        return NFC_OPT_FAILED;
    }
    response = ConnectedTagHdiAdapter::GetInstance().ReadNdefTag();
    return NFC_OPT_SUCCESS;
}

ErrCode ConnectedTagServiceImpl::WriteNdefTag(std::string data)
{
    if (!PermissionTools::IsGranted(OHOS::ConnectedTag::TAG_PERMISSION)) {
        HILOGE("ConnectedTagServiceImpl:WriteNdefTag() IsGranted failed!");
        return NFC_OPT_FAILED;
    }
    if (ConnectedTagHdiAdapter::GetInstance().WriteNdefTag(data) == 0) {
        return NFC_OPT_SUCCESS;
    }
    return NFC_OPT_FAILED;
}

ErrCode ConnectedTagServiceImpl::RegListener(const sptr<IConnectedTagCallBack> &callback)
{
    if (!PermissionTools::IsGranted(OHOS::ConnectedTag::TAG_PERMISSION)) {
        HILOGE("ConnectedTagServiceImpl:RegListener() IsGranted failed!");
        return NFC_OPT_FAILED;
    }
    return NFC_OPT_SUCCESS;
}
ErrCode ConnectedTagServiceImpl::UnregListener(const sptr<IConnectedTagCallBack> &callback)
{
    if (!PermissionTools::IsGranted(OHOS::ConnectedTag::TAG_PERMISSION)) {
        HILOGE("ConnectedTagServiceImpl:UnregListener() IsGranted failed!");
        return NFC_OPT_FAILED;
    }
    return NFC_OPT_SUCCESS;
}
}  // namespace ConnectedTag
}  // namespace OHOS