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

#ifndef OHOS_NFC_CONNECTED_TAG_SERVICE_IMPL_H
#define OHOS_NFC_CONNECTED_TAG_SERVICE_IMPL_H

#include "system_ability.h"
#include "connected_tag_session_stub.h"
#include "iremote_object.h"
#include "error_code.h"

namespace OHOS {
namespace ConnectedTag {
enum ServiceRunningState {
    STATE_NOT_START,
    STATE_RUNNING
};
class NfcConnectedTagServiceImpl : public SystemAbility, public TagSessionStub {
DECLARE_SYSTEM_ABILITY(NfcConnectedTagServiceImpl);
public:
    NfcConnectedTagServiceImpl();
    virtual ~NfcConnectedTagServiceImpl();

    static sptr<NfcConnectedTagServiceImpl> GetInstance();

    void OnStart() override;
    void OnStop() override;

    ErrCode Init() override;

    ErrCode Uninit() override;

    ErrCode ReadNdefTag(std::string &response) override;

    ErrCode WriteNdefTag(std::string data) override;

    ErrCode RegListener(const sptr<IConnectedTagCallBack> &callback) override;

    ErrCode UnregListener(const sptr<IConnectedTagCallBack> &callback) override;
private:
    bool ServiceInit();

private:
    static sptr<NfcConnectedTagServiceImpl> g_instance;
    static std::mutex g_instanceLock;
    bool mPublishFlag;
    ServiceRunningState mState;
};
}  // namespace OHOS_NFC_CONNECTED_TAG_SERVICE_IMPL_H
}  // namespace OHOS
#endif