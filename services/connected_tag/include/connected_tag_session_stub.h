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
#ifndef OHOS_TAG_SESSION_STUB_H
#define OHOS_TAG_SESSION_STUB_H

#include <map>
#include <stdint.h>
#include <string>
#include "i_tag_session.h"
#include "iremote_stub.h"


namespace OHOS {
namespace ConnectedTag {
class TagSessionStub : public IRemoteStub<ITagSession> {
public:
    TagSessionStub();
    virtual ~TagSessionStub();

    using handleFunc = void (TagSessionStub::*)(uint32_t code, MessageParcel &data, MessageParcel &reply);
    using HandleFuncMap = std::map<int, handleFunc>;

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
private:
    void InitHandleMap();
    void OnInit(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnUninit(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnReadNdefTag(uint32_t code, MessageParcel &data, MessageParcel &reply);
    void OnWriteNdefTag(uint32_t code, MessageParcel &data, MessageParcel &reply);
private:
    HandleFuncMap handleFuncMap;
};
}  // namespace Nfc_Connected_Tag
}  // namespace OHOS
#endif
