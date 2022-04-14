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
#ifndef NAPI_CONNECTED_TAG_IMPL_H_
#define NAPI_CONNECTED_TAG_IMPL_H_

#include "tag_session_proxy.h"
#include "error_code.h"
#include "iconnected_tag.h"
#include "iconnected_tag_callback.h"

namespace OHOS {
namespace ConnectedTag {
class ConnectedTagImpl : public IConnectedTag {
public:
    explicit ConnectedTagImpl();
    virtual ~ConnectedTagImpl();

    static ConnectedTagImpl& GetInstance();

    ErrCode Init() override;

    ErrCode Uninit() override;

    ErrCode ReadNdefTag(std::string &response) override;

    ErrCode WriteNdefTag(std::string data) override;

    ErrCode RegListener(const sptr<IConnectedTagCallBack> &callback) override;

    ErrCode UnregListener(const sptr<IConnectedTagCallBack> &callback) override;
private:
    sptr<ITagSession> tagSessionProxy_;
};
}  // namespace ConnectedTag
}  // namespace OHOS
#endif /* NAPI_CONNECTED_TAG_IMPL_H_ */
