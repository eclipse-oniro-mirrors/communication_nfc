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
#include "nci_nfcc_impl_vendor.h"
#include "nci_native_adapter.h"

namespace OHOS {
namespace NFC {
namespace NCI {
NciNfccImplVendor::NciNfccImplVendor()
{
    vendorNfccImpl_ = NciNativeAdapter::GetInstance().GetNciNfccInterface();
}

NciNfccImplVendor::~NciNfccImplVendor()
{
}

/**
 * @brief Initialize when turn on NFC
 * @return True if success, otherwise false.
 */
bool NciNfccImplVendor::Initialize()
{
    if (vendorNfccImpl_) {
        return vendorNfccImpl_->Initialize();
    }
    return true;
}

/**
 * @brief Deinitialize when turn off NFC
 * @return True if success, otherwise false.
 */
bool NciNfccImplVendor::Deinitialize()
{
    if (vendorNfccImpl_) {
        return vendorNfccImpl_->Deinitialize();
    }
    return true;
}

/**
 * @brief Start polling and listening
 * @param techMask bitmask of the technologies
 * @param enableReaderMode if enable tag polling
 * @param enableHostRouting if enable host routing
 * @param restart true if need restart, otherwise false.
 */
void NciNfccImplVendor::EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart)
{
    if (vendorNfccImpl_) {
        return vendorNfccImpl_->EnableDiscovery(techMask, enableReaderMode, enableHostRouting, restart);
    }
}

/**
 * @brief Stop polling and listening
 */
void NciNfccImplVendor::DisableDiscovery()
{
    if (vendorNfccImpl_) {
        return vendorNfccImpl_->DisableDiscovery();
    }
}

/**
 * @brief Set the screen statue to nfc controller.
 * @param screenStateMask the bitmask of the screen state
 * @return True if success, otherwise false.
 */
bool NciNfccImplVendor::SetScreenStatus(uint8_t screenStateMask)
{
    if (vendorNfccImpl_) {
        return vendorNfccImpl_->SetScreenStatus(screenStateMask);
    }
    return true;
}

/**
 * @brief Get Nci version supprted by nfc controller.
 * @return 0x20 if it's NCI2.0, otherwise 0x10 if it's NCI1.0.
 */
int NciNfccImplVendor::GetNciVersion()
{
    if (vendorNfccImpl_) {
        return vendorNfccImpl_->GetNciVersion();
    }
    return 0x10;
}

/**
 * @brief Abort the nfc controller if NCI timeout.
 */
void NciNfccImplVendor::Abort()
{
    if (vendorNfccImpl_) {
        return vendorNfccImpl_->Abort();
    }
}

/**
 * @brief Do factory reset for nfc controller.
 */
void NciNfccImplVendor::FactoryReset()
{
    if (vendorNfccImpl_) {
        return vendorNfccImpl_->FactoryReset();
    }
}

/**
 * @brief Shutdown the device. Enable the nfc functionality if support power off case.
 */
void NciNfccImplVendor::Shutdown()
{
    if (vendorNfccImpl_) {
        return vendorNfccImpl_->Shutdown();
    }
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
