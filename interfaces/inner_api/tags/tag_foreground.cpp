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
#include "tag_foreground.h"

#include "loghelper.h"
#include "iforeground_callback.h"
#include "foreground_callback_stub.h"
#include "nfc_controller.h"
#include "nfc_sdk_common.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "tag_session_proxy.h"

namespace OHOS {
namespace NFC {
namespace KITS {
TagForeground::TagForeground()
{
    DebugLog("TagForeground: new ability TagForeground");
}

TagForeground::~TagForeground()
{
    DebugLog("destruct TagForeground");
}

OHOS::sptr<TAG::ITagSession> TagForeground::GetTagSessionProxy()
{
    if (tagSessionProxy_ == nullptr) {
        OHOS::sptr<IRemoteObject> iface = NfcController::GetInstance().GetTagServiceIface();
        if (iface != nullptr) {
            tagSessionProxy_ = new TAG::TagSessionProxy(iface);
        }
    }
    return tagSessionProxy_;
}

TagForeground &TagForeground::GetInstance()
{
    DebugLog("TagForeground::GetInstance in.");
    static TagForeground instance;
    return instance;
}

ErrorCode TagForeground::RegForeground(AppExecFwk::ElementName &element,
    std::vector<uint32_t> &discTech, const sptr<KITS::IForegroundCallback> &callback)
{
    DebugLog("TagForeground::RegForeground");
    bool isNfcOpen = false;
    NfcController::GetInstance().IsNfcOpen(isNfcOpen);
    if (!isNfcOpen) {
        ErrorLog("RegForeground: nfc is not open");
        return ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }
    OHOS::sptr<TAG::ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr) {
        ErrorLog("TagForeground::RegForeground, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    return tagSession->RegForegroundDispatch(element, discTech, callback);
}

ErrorCode TagForeground::UnregForeground(AppExecFwk::ElementName &element)
{
    DebugLog("TagForeground::UnregForeground");
    bool isNfcOpen = false;
    NfcController::GetInstance().IsNfcOpen(isNfcOpen);
    if (!isNfcOpen) {
        ErrorLog("UnregForeground: nfc is not open");
        return ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }
    OHOS::sptr<TAG::ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr) {
        ErrorLog("TagForeground::UnregForeground, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    return tagSession->UnregForegroundDispatch(element);
}

ErrorCode TagForeground::RegReaderMode(AppExecFwk::ElementName &element,
                                       std::vector<uint32_t> &discTech,
                                       const sptr<KITS::IReaderModeCallback> &callback)
{
    DebugLog("TagForeground::RegReaderMode");
    bool isNfcOpen = false;
    NfcController::GetInstance().IsNfcOpen(isNfcOpen);
    if (!isNfcOpen) {
        ErrorLog("RegReaderMode: nfc is not open");
        return ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }
    OHOS::sptr<TAG::ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr) {
        ErrorLog("TagForeground::RegReaderMode, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    return tagSession->RegReaderMode(element, discTech, callback);
}

ErrorCode TagForeground::UnregReaderMode(AppExecFwk::ElementName &element)
{
    DebugLog("TagForeground::UnregReaderMode");
    bool isNfcOpen = false;
    NfcController::GetInstance().IsNfcOpen(isNfcOpen);
    if (!isNfcOpen) {
        ErrorLog("UnregReaderMode: nfc is not open");
        return ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }
    OHOS::sptr<TAG::ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr) {
        ErrorLog("TagForeground::UnregReaderMode, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    return tagSession->UnregReaderMode(element);
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS