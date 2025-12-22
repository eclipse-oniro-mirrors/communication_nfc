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
#include "reader_mode_callback_stub.h"
#include "system_ability_definition.h"
#include "tag_session_proxy.h"
#include "nfc_sa_client.h"

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

OHOS::sptr<ITagSession> TagForeground::GetTagSessionProxy()
{
    return iface_cast<ITagSession>(NfcController::GetInstance().GetTagServiceIface());
}

TagForeground &TagForeground::GetInstance()
{
    DebugLog("TagForeground::GetInstance in.");
    static TagForeground instance;
    return instance;
}

int TagForeground::RegForeground(AppExecFwk::ElementName &element,
    std::vector<uint32_t> &discTech, const sptr<KITS::IForegroundCallback> &callback)
{
    DebugLog("TagForeground::RegForeground");
    bool isNfcOpen = false;
    NfcController::GetInstance().IsNfcOpen(isNfcOpen);
    if (!isNfcOpen) {
        ErrorLog("RegForeground: nfc is not open");
        return ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }

    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr || tagSession->AsObject() == nullptr) {
        ErrorLog("TagForeground::RegForeground, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }

    TAG::ForegroundCallbackStub::GetInstance()->RegForegroundDispatch(callback);
    return static_cast<int>(
        tagSession->RegForegroundDispatch(element, discTech, TAG::ForegroundCallbackStub::GetInstance()));
}

int TagForeground::UnregForeground(AppExecFwk::ElementName &element)
{
    DebugLog("TagForeground::UnregForeground");
    if (!NfcSaClient::GetInstance().CheckNfcSystemAbility()) {
        WarnLog("Nfc SA not started yet.");
        return ErrorCode::ERR_NONE;
    }

    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr || tagSession->AsObject() == nullptr) {
        ErrorLog("TagForeground::UnregForeground, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    return static_cast<int>(tagSession->UnregForegroundDispatch(element));
}

int TagForeground::RegReaderMode(AppExecFwk::ElementName &element,
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
    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr || tagSession->AsObject() == nullptr) {
        ErrorLog("TagForeground::RegReaderMode, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }

    TAG::ReaderModeCallbackStub::GetInstance()->RegReaderMode(callback);
    return static_cast<int>(tagSession->RegReaderMode(element, discTech, TAG::ReaderModeCallbackStub::GetInstance()));
}

int TagForeground::RegReaderModeWithIntvl(AppExecFwk::ElementName &element,
    std::vector<uint32_t> &discTech,
    const sptr<KITS::IReaderModeCallback> &callback,
    int interval)
{
    DebugLog("TagForeground::RegReaderModeWithIntvl");
    bool isNfcOpen = false;
    NfcController::GetInstance().IsNfcOpen(isNfcOpen);
    if (!isNfcOpen) {
        ErrorLog("nfc is not open");
        return ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }
    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr || tagSession->AsObject() == nullptr) {
        ErrorLog("ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }

    TAG::ReaderModeCallbackStub::GetInstance()->RegReaderMode(callback);
    return static_cast<int>(tagSession->RegReaderModeWithIntvl(
        element, discTech, TAG::ReaderModeCallbackStub::GetInstance(), interval));
}

int TagForeground::UnregReaderMode(AppExecFwk::ElementName &element)
{
    DebugLog("TagForeground::UnregReaderMode");
    if (!NfcSaClient::GetInstance().CheckNfcSystemAbility()) {
        WarnLog("Nfc SA not started yet.");
        return ErrorCode::ERR_NONE;
    }
    OHOS::sptr<ITagSession> tagSession = GetTagSessionProxy();
    if (tagSession == nullptr || tagSession->AsObject() == nullptr) {
        ErrorLog("TagForeground::UnregReaderMode, ERR_NO_PERMISSION");
        return ErrorCode::ERR_NO_PERMISSION;
    }
    return static_cast<int>(tagSession->UnregReaderMode(element));
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS