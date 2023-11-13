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
#include "nci_ce_impl_default.h"
#include "native_impl_default/nfcc_nci_adapter.h"
#include "native_impl_default/routing_manager.h"

namespace OHOS {
namespace NFC {
namespace NCI {
NciCeImplDefault::NciCeImplDefault()
{
}

NciCeImplDefault::~NciCeImplDefault()
{
}

/**
 * @brief Set the listener to receive the card emulation notifications.
 * @param listener The listener to receive the card emulation notifications.
 */
void NciCeImplDefault::SetCeHostListener(std::weak_ptr<ICeHostListener> listener)
{
    return NfccNciAdapter::GetInstance().SetCeHostListener(listener);
}

/**
 * @brief compute the routing parameters based on the default payment app and all installed app.
 * @return True if success, otherwise false.
 */
bool NciCeImplDefault::ComputeRoutingParams()
{
    return RoutingManager::GetInstance().ComputeRoutingParams();
}

/**
 * @brief Commit the routing parameters to nfc controller.
 * @return True if success, otherwise false.
 */
bool NciCeImplDefault::CommitRouting()
{
    return RoutingManager::GetInstance().CommitRouting();
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS