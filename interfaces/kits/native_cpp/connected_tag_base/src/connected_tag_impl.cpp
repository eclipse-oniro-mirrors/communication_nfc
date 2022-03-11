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
#include "connected_tag_impl.h"
#include "iservice_registry.h"
#include "ipc_cmd.h"
#include "log.h"

namespace OHOS {
namespace ConnectedTag {

ConnectedTagImpl::ConnectedTagImpl()
{
    HILOGI("ConnectedTagImpl() in");
    sptr<ISystemAbilityManager> sa_mgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sa_mgr == nullptr) {
        HILOGE("failed to get SystemAbilityManager");
        return;
    }

    sptr<IRemoteObject> object = sa_mgr->GetSystemAbility(NFC_CONNECTED_TAG_ABILITY_ID);
    if (object == nullptr) {
        HILOGE("failed to get connected tag SERVICE");
        return;
    }

    tagSessionProxy_ = iface_cast<ITagSession>(object);
    if (tagSessionProxy_ == nullptr) {
        tagSessionProxy_ = new (std::nothrow) TagSessionProxy(object);
    }

    if (tagSessionProxy_ == nullptr) {
        HILOGE("TagSessionProxy init failed!");
    }
}
ConnectedTagImpl::~ConnectedTagImpl()
{
}
ConnectedTagImpl& ConnectedTagImpl::GetInstance()
{
    static ConnectedTagImpl tagImplSingleton;
    return tagImplSingleton;
}
ErrCode ConnectedTagImpl::Init()
{
    return tagSessionProxy_->Init();
}
ErrCode ConnectedTagImpl::Uninit()
{
    return tagSessionProxy_->Uninit();
}
ErrCode ConnectedTagImpl::ReadNdefTag(std::string &response)
{
    return tagSessionProxy_->ReadNdefTag(response);
}
ErrCode ConnectedTagImpl::WriteNdefTag(std::string data)
{
    return tagSessionProxy_->WriteNdefTag(data);
}
ErrCode ConnectedTagImpl::RegListener(const sptr<IConnectedTagCallBack> &callback)
{
    return tagSessionProxy_->RegListener(callback);
}
ErrCode ConnectedTagImpl::UnregListener(const sptr<IConnectedTagCallBack> &callback)
{
    return tagSessionProxy_->UnregListener(callback);
}
}  // namespace ConnectedTag
}  // namespace OHOS