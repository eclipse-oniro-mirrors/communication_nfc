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
#ifndef NFC_ICONNECTED_TAG_CALLBACK_H
#define NFC_ICONNECTED_TAG_CALLBACK_H
#include <iremote_stub.h>

namespace OHOS {
namespace ConnectedTag {
class IConnectedTagCallBack : public IRemoteBroker {
public:
    virtual void OnNotify(int nfcRfState) = 0;
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.connected_tag.IConnectedTagCallBack");
};
}  // namespace ConnectedTag
}  // namespace OHOS
#endif // NFC_ICONNECTED_TAG_CALLBACK_H
