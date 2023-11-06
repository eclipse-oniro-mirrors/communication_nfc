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
#include "nci_nfcc_proxy.h"

namespace OHOS {
namespace NFC {
namespace NCI {
NciNfccProxy::NciNfccProxy(std::shared_ptr<INciNfccInterface> nfccInterface)
{
    nfccInterface_ = nfccInterface;
}

NciNfccProxy::~NciNfccProxy()
{
}

/**
 * @brief Initialize when turn on NFC
 * @return True if success, otherwise false.
 */
bool NciNfccProxy::Initialize()
{
    if (nfccInterface_!= nullptr) {
        return nfccInterface_->Initialize();
    }
    return false;
}

/**
 * @brief Deinitialize when turn off NFC
 * @return True if success, otherwise false.
 */
bool NciNfccProxy::Deinitialize()
{
    if (nfccInterface_!= nullptr) {
        return nfccInterface_->Deinitialize();
    }
    return false;
}

/**
 * @brief Start polling and listening
 * @param techMask bitmask of the technologies
 * @param enableReaderMode if enable tag polling
 * @param enableHostRouting if enable host routing
 * @param restart true if need restart, otherwise false.
 */
void NciNfccProxy::EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart)
{
    if (nfccInterface_!= nullptr) {
        return nfccInterface_->EnableDiscovery(techMask, enableReaderMode, enableHostRouting, restart);
    }
}

/**
 * @brief Stop polling and listening
 */
void NciNfccProxy::DisableDiscovery()
{
    if (nfccInterface_!= nullptr) {
        return nfccInterface_->DisableDiscovery();
    }
}

/**
 * @brief Set the screen statue to nfc controller.
 * @param screenStateMask the bitmask of the screen state
 * @return True if success, otherwise false.
 */
bool NciNfccProxy::SetScreenStatus(uint8_t screenStateMask)
{
    if (nfccInterface_!= nullptr) {
        return nfccInterface_->SetScreenStatus(screenStateMask);
    }
    return false;
}

/**
 * @brief Get Nci version supprted by nfc controller.
 * @return 0x20 if it's NCI2.0, otherwise 0x10 if it's NCI1.0.
 */
int NciNfccProxy::GetNciVersion()
{
    if (nfccInterface_!= nullptr) {
        return nfccInterface_->GetNciVersion();
    }
    return 0x10;
}

/**
 * @brief Abort the nfc controller if NCI timeout.
 */
void NciNfccProxy::Abort()
{
    if (nfccInterface_!= nullptr) {
        return nfccInterface_->Abort();
    }
}

/**
 * @brief Do factory reset for nfc controller.
 */
void NciNfccProxy::FactoryReset()
{
    if (nfccInterface_!= nullptr) {
        return nfccInterface_->FactoryReset();
    }
}

/**
 * @brief Shutdown the device. Enable the nfc functionality if support power off case.
 */
void NciNfccProxy::Shutdown()
{
    if (nfccInterface_!= nullptr) {
        return nfccInterface_->Shutdown();
    }
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
