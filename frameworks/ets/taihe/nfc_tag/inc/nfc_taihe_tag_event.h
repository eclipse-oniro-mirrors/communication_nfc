/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef NFC_TAIHE_TAG_EVENT
#define NFC_TAIHE_TAG_EVENT

#include "ohos.nfc.tag.tag.proj.hpp"
#include "ohos.nfc.tag.tag.impl.hpp"
#include "taihe/runtime.hpp"

#include "element_name.h"
#include "iforeground_callback.h"
#include "ireader_mode_callback.h"
#include "taginfo_parcelable.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class NfcFgListenerEvent : public IForegroundCallback {
public:
    NfcFgListenerEvent() {}
    virtual ~NfcFgListenerEvent() {}
public:
    void OnTagDiscovered(KITS::TagInfoParcelable* tagInfo) override;
    OHOS::sptr<OHOS::IRemoteObject> AsObject() override;
};

class TagFgEventRegister {
public:
    static TagFgEventRegister& GetInstance();
    void Register(AppExecFwk::ElementName &element, std::vector<uint32_t> &discTech,
        ::taihe::callback_view<void(uintptr_t err, ::ohos::nfc::tag::tag::TagInfo const& tagInfo)> callback);
    void Unregister(AppExecFwk::ElementName &element);

private:
    TagFgEventRegister() {}
    ~TagFgEventRegister() {}
};

class NfcRmListenerEvent : public IReaderModeCallback {
public:
    NfcRmListenerEvent() {}
    virtual ~NfcRmListenerEvent() {}
public:
    void OnTagDiscovered(KITS::TagInfoParcelable* tagInfo) override;
    OHOS::sptr<OHOS::IRemoteObject> AsObject() override;
};

class TagRmEventRegister {
public:
    static TagRmEventRegister& GetInstance();
    void Register(AppExecFwk::ElementName &element, std::vector<uint32_t> &discTech,
        ::taihe::callback_view<void(uintptr_t err, ::ohos::nfc::tag::tag::TagInfo const& tagInfo)> callback);
    void Unregister(AppExecFwk::ElementName &element);

private:
    TagRmEventRegister() {}
    ~TagRmEventRegister() {}
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif // #define NFC_TAIHE_TAG_EVENT