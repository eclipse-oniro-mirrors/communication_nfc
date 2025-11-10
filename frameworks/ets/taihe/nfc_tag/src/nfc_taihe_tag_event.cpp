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

#include "nfc_taihe_tag_event.h"

#include <mutex>
#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "tag_foreground.h"

namespace OHOS {
namespace NFC {
namespace KITS {
static std::mutex g_callbackMutex {};
static std::shared_ptr<::taihe::callback_view<void(uintptr_t err, ::ohos::nfc::tag::tag::TagInfo const& tagInfo)>>
    g_tagFgCallback = nullptr;
sptr<NfcFgListenerEvent> g_fgListenerEvent = sptr<NfcFgListenerEvent>(new NfcFgListenerEvent());
static std::shared_ptr<::taihe::callback_view<void(uintptr_t err, ::ohos::nfc::tag::tag::TagInfo const& tagInfo)>>
    g_tagRmCallback = nullptr;
sptr<NfcRmListenerEvent> g_rmListenerEvent = sptr<NfcRmListenerEvent>(new NfcRmListenerEvent());

void NfcFgListenerEvent::OnTagDiscovered(KITS::TagInfoParcelable* tagInfo)
{
    InfoLog("OnNotify rcvd tagInfo: %{public}s", tagInfo->ToString().c_str());
}

OHOS::sptr<OHOS::IRemoteObject> NfcFgListenerEvent::AsObject()
{
    return nullptr;
}

TagFgEventRegister& TagFgEventRegister::GetInstance()
{
    static TagFgEventRegister instance;
    return instance;
}

void TagFgEventRegister::Register(AppExecFwk::ElementName &element, std::vector<uint32_t> &discTech,
    ::taihe::callback_view<void(uintptr_t err, ::ohos::nfc::tag::tag::TagInfo const& tagInfo)> callback)
{
    std::lock_guard<std::mutex> lock(g_callbackMutex);
    int ret = KITS::TagForeground::GetInstance().RegForeground(element, discTech, g_fgListenerEvent);
    if (ret != KITS::ERR_NONE) {
        ErrorLog("Register failed!");
        return;
    }
    g_tagFgCallback = std::make_shared<
        ::taihe::callback_view<void(uintptr_t err, ::ohos::nfc::tag::tag::TagInfo const& tagInfo)>>(callback);
}

void TagFgEventRegister::Unregister(AppExecFwk::ElementName &element)
{
    int ret = KITS::TagForeground::GetInstance().UnregForeground(element);
    if (ret != KITS::ERR_NONE) {
        ErrorLog("unregister failed!");
        return;
    }
    std::lock_guard<std::mutex> lock(g_callbackMutex);
    g_tagFgCallback = nullptr;
}

void NfcRmListenerEvent::OnTagDiscovered(KITS::TagInfoParcelable* tagInfo)
{
    InfoLog("OnNotify rcvd tagInfo: %{public}s", tagInfo->ToString().c_str());
}

OHOS::sptr<OHOS::IRemoteObject> NfcRmListenerEvent::AsObject()
{
    return nullptr;
}

TagRmEventRegister& TagRmEventRegister::GetInstance()
{
    static TagRmEventRegister instance;
    return instance;
}

void TagRmEventRegister::Register(AppExecFwk::ElementName &element, std::vector<uint32_t> &discTech,
    ::taihe::callback_view<void(uintptr_t err, ::ohos::nfc::tag::tag::TagInfo const& tagInfo)> callback)
{
    std::lock_guard<std::mutex> lock(g_callbackMutex);
    int ret = KITS::TagForeground::GetInstance().RegReaderMode(element, discTech, g_rmListenerEvent);
    if (ret != KITS::ERR_NONE) {
        ErrorLog("Register failed!");
        return;
    }
    g_tagRmCallback = std::make_shared<
        ::taihe::callback_view<void(uintptr_t err, ::ohos::nfc::tag::tag::TagInfo const& tagInfo)>>(callback);
}

void TagRmEventRegister::Unregister(AppExecFwk::ElementName &element)
{
    int ret = KITS::TagForeground::GetInstance().UnregReaderMode(element);
    if (ret != KITS::ERR_NONE) {
        ErrorLog("unregister failed!");
        return;
    }
    std::lock_guard<std::mutex> lock(g_callbackMutex);
    g_tagRmCallback = nullptr;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS