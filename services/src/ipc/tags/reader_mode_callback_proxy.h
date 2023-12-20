/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#ifndef READER_MODE_CALLBACK_PROXY_H
#define READER_MODE_CALLBACK_PROXY_H

#include "ireader_mode_callback.h"
#include "iremote_proxy.h"
#include "message_parcel.h"
#include "nfc_sdk_common.h"
#include "taginfo_parcelable.h"

namespace OHOS {
namespace NFC {
namespace TAG {
class ReaderModeCallbackProxy : public IRemoteProxy<KITS::IReaderModeCallback> {
public:
    explicit ReaderModeCallbackProxy(const sptr<IRemoteObject> &remote);

    virtual ~ReaderModeCallbackProxy() {}

    void OnTagDiscovered(KITS::TagInfoParcelable* tagInfo) override;

private:
    static inline BrokerDelegator<ReaderModeCallbackProxy> g_delegator;
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif