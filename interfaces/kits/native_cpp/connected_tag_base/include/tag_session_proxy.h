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
#ifndef OHOS_NFC_TAG_SESSION_PROXY_H
#define OHOS_NFC_TAG_SESSION_PROXY_H
#include "i_tag_session.h"
#include <stdint.h>
#include "iremote_proxy.h"

namespace OHOS {
namespace ConnectedTag {
class TagSessionProxy : public IRemoteProxy<ITagSession>, public IRemoteObject::DeathRecipient {
public:
    explicit TagSessionProxy(const sptr<IRemoteObject> &impl);
    ~TagSessionProxy();

    ErrCode Init() override;

    ErrCode Uninit() override;

    ErrCode ReadNdefTag(std::string &response) override;

    ErrCode WriteNdefTag(std::string data) override;

    ErrCode RegListener(const sptr<IConnectedTagCallBack> &callback) override;

    ErrCode UnregListener(const sptr<IConnectedTagCallBack> &callback) override;

    /**
    * @Description Handle remote object died event.
    * @param remoteObject remote object.
    */
    void OnRemoteDied(const wptr<IRemoteObject> &remoteObject) override;

private:
    bool mRemoteDied;
};
}  // namespace ConnectedTag
}  // namespace OHOS
#endif
