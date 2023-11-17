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

#include "nci_nfcc_impl_default.h"
#include "nfcc_nci_adapter.h"

namespace OHOS {
namespace NFC {
namespace NCI {
bool NciNfccImplDefault::Initialize()
{
    return NfccNciAdapter::GetInstance().Initialize();
}

bool NciNfccImplDefault::Deinitialize()
{
    return NfccNciAdapter::GetInstance().Deinitialize();
}

void NciNfccImplDefault::EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart)
{
    NfccNciAdapter::GetInstance().EnableDiscovery(techMask, enableReaderMode, enableHostRouting, restart);
}

void NciNfccImplDefault::DisableDiscovery()
{
    NfccNciAdapter::GetInstance().DisableDiscovery();
}

bool NciNfccImplDefault::SetScreenStatus(uint8_t screenStateMask)
{
    NfccNciAdapter::GetInstance().SetScreenStatus(screenStateMask);
    return true;
}

int NciNfccImplDefault::GetNciVersion()
{
    return NfccNciAdapter::GetInstance().GetNciVersion();
}

void NciNfccImplDefault::Abort()
{
    NfccNciAdapter::GetInstance().Abort();
}

void NciNfccImplDefault::FactoryReset()
{
    NfccNciAdapter::GetInstance().FactoryReset();
}

void NciNfccImplDefault::Shutdown()
{
    NfccNciAdapter::GetInstance().Shutdown();
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS