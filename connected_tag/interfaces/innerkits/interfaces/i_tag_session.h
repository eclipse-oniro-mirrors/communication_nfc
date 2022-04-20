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
#ifndef OHOS_I_TAG_SESSION_H
#define OHOS_I_TAG_SESSION_H
#include "iremote_broker.h"
#include "error_code.h"
#include "iconnected_tag_callback.h"

namespace OHOS {
namespace ConnectedTag {
class ITagSession : public IRemoteBroker {
public:
    virtual ~ITagSession() {}

    virtual ErrCode Init() = 0;
    virtual ErrCode Uninit() = 0;
    virtual ErrCode ReadNdefTag(std::string &response) = 0;
    virtual ErrCode WriteNdefTag(std::string data) = 0;
    virtual ErrCode RegListener(const sptr<IConnectedTagCallBack> &callback) = 0;
    virtual ErrCode UnregListener(const sptr<IConnectedTagCallBack> &callback) = 0;
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.INfcConnectedTagService");
};
}  // namespace ConnectedTag
}  // namespace OHOS
#endif