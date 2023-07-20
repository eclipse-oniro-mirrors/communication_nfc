/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "nfc_service_fuzz.h"

#include <cstddef>
#include <cstdint>

#include "tag_session.h"
#include "nfc_sdk_common.h"
#include "tag_dispatcher.h"

namespace OHOS {
namespace NFC {

int NfcServiceFuzz::GetNfcState()
{
    return 0;
}

bool NfcServiceFuzz::IsNfcEnabled()
{
    return false;
}

OHOS::sptr<IRemoteObject> NfcServiceFuzz::GetTagServiceIface()
{
    return nullptr;
}

int NfcServiceFuzz::GetScreenState()
{
    return 0;
}

int NfcServiceFuzz::GetNciVersion()
{
    return 0;
}

std::weak_ptr<TAG::TagDispatcher> NfcServiceFuzz::GetTagDispatcher()
{
    std::shared_ptr<TAG::TagDispatcher> tagDispatcher = std::make_shared<TAG::TagDispatcher>(nullptr);
    return tagDispatcher;
}

bool NfcServiceFuzz::EnableForegroundDispatch(AppExecFwk::ElementName element, std::vector<uint32_t> &discTech,
    const sptr<KITS::IForegroundCallback> &callback)
{
    return true;
}

bool NfcServiceFuzz::DisableForegroundDispatch(AppExecFwk::ElementName element)
{
    return true;
}

bool NfcServiceFuzz::DisableForegroundByDeathRcpt()
{
    return true;
}

bool NfcServiceFuzz::IsForegroundEnabled()
{
    return true;
}

void NfcServiceFuzz::SendTagToForeground(KITS::TagInfoParcelable tagInfo)
{
}
}
}