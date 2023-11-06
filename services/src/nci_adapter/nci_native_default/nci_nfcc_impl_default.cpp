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
#include "native_impl_default/nfcc_nci_adapter.h"

namespace OHOS {
namespace NFC {
namespace NCI {
NciNfccImplDefault::NciNfccImplDefault()
{
}

NciNfccImplDefault::~NciNfccImplDefault()
{
}

/**
 * @brief Initialize when turn on NFC
 * @return True if success, otherwise false.
 */
bool NciNfccImplDefault::Initialize()
{
    return NfccNciAdapter::GetInstance().Initialize();
}

/**
 * @brief Deinitialize when turn off NFC
 * @return True if success, otherwise false.
 */
bool NciNfccImplDefault::Deinitialize()
{
    return NfccNciAdapter::GetInstance().Deinitialize();
}

/**
 * @brief Start polling and listening
 * @param techMask bitmask of the technologies
 * @param enableReaderMode if enable tag polling
 * @param enableHostRouting if enable host routing
 * @param restart true if need restart, otherwise false.
 */
void NciNfccImplDefault::EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart)
{
    NfccNciAdapter::GetInstance().EnableDiscovery(techMask, enableReaderMode, enableHostRouting, restart);
}

/**
 * @brief Stop polling and listening
 */
void NciNfccImplDefault::DisableDiscovery()
{
    NfccNciAdapter::GetInstance().DisableDiscovery();
}

/**
 * @brief Set the screen statue to nfc controller.
 * @param screenStateMask the bitmask of the screen state
 * @return True if success, otherwise false.
 */
bool NciNfccImplDefault::SetScreenStatus(uint8_t screenStateMask)
{
    NfccNciAdapter::GetInstance().SetScreenStatus(screenStateMask);
    return false;
}

/**
 * @brief Get Nci version supprted by nfc controller.
 * @return 0x20 if it's NCI2.0, otherwise 0x10 if it's NCI1.0.
 */
int NciNfccImplDefault::GetNciVersion()
{
    return NfccNciAdapter::GetInstance().GetNciVersion();
}

/**
 * @brief Abort the nfc controller if NCI timeout.
 */
void NciNfccImplDefault::Abort()
{
    NfccNciAdapter::GetInstance().Abort();
}

/**
 * @brief Do factory reset for nfc controller.
 */
void NciNfccImplDefault::FactoryReset()
{
    NfccNciAdapter::GetInstance().FactoryReset();
}

/**
 * @brief Shutdown the device. Enable the nfc functionality if support power off case.
 */
void NciNfccImplDefault::Shutdown()
{
    NfccNciAdapter::GetInstance().Shutdown();
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
