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
#ifndef TAG_SESSION_STUB_H
#define TAG_SESSION_STUB_H

#include "iremote_stub.h"
#include "itag_session.h"
#include "message_parcel.h"

namespace OHOS {
namespace NFC {
namespace TAG {
class TagSessionStub : public OHOS::IRemoteStub<OHOS::NFC::TAG::ITagSession> {
public:
    int OnRemoteRequest(uint32_t code,                         /* [in] */
                        OHOS::MessageParcel& data,             /* [in] */
                        OHOS::MessageParcel& reply,            /* [out] */
                        OHOS::MessageOption& option) override; /* [in] */
    TagSessionStub() {}
    virtual ~TagSessionStub() {}

private:
    int HandleConnect(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleReconnect(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleDisconnect(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleSetTimeout(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleGetTimeout(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleGetTechList(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleIsTagFieldOn(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleIsNdef(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleSendRawFrame(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleNdefRead(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleNdefWrite(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleNdefMakeReadOnly(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleFormatNdef(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleCanMakeReadOnly(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleGetMaxTransceiveLength(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);
    int HandleIsSupportedApdusExtended(OHOS::MessageParcel& data, OHOS::MessageParcel& reply);

private:
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_SESSION_STUB_H
