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
#include "nfc_service_tdd.h"

#include <cstddef>
#include <cstdint>

#include "tag_session.h"
#include "nfc_sdk_common.h"
#include "tag_dispatcher.h"

namespace OHOS {
namespace NFC {

int NfcServiceImpl::GetNfcState()
{
    return 0;
}

bool NfcServiceImpl::IsNfcEnabled()
{
    return false;
}

OHOS::sptr<IRemoteObject> NfcServiceImpl::GetTagServiceIface()
{
    return nullptr;
}

int NfcServiceImpl::GetScreenState()
{
    return 0;
}

int NfcServiceImpl::GetNciVersion()
{
    return 0;
}

std::weak_ptr<TAG::TagDispatcher> NfcServiceImpl::GetTagDispatcher()
{
    std::shared_ptr<TAG::TagDispatcher> tagDispatcher = std::make_shared<TAG::TagDispatcher>(nullptr);
    return tagDispatcher;
}

std::weak_ptr<NfcPollingManager> NfcServiceImpl::GetNfcPollingManager()
{
    std::shared_ptr<NfcPollingManager> nfcPollingManager = nullptr;
    return nfcPollingManager;
}

std::weak_ptr<NfcRoutingManager> NfcServiceImpl::GetNfcRoutingManager()
{
    std::shared_ptr<NfcRoutingManager> nfcRoutingManager = nullptr;
    return nfcRoutingManager;
}
}
}