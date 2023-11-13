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
#include "nci_ce_proxy.h"

namespace OHOS {
namespace NFC {
namespace NCI {
NciCeProxy::NciCeProxy(std::shared_ptr<INciCeInterface> ceInterface)
{
    nciCeInterface_ = ceInterface;
}

NciCeProxy::~NciCeProxy()
{
}

/**
 * @brief Set the listener to receive the card emulation notifications.
 * @param listener The listener to receive the card emulation notifications.
 */
void NciCeProxy::SetCeHostListener(std::weak_ptr<ICeHostListener> listener)
{
    if (nciCeInterface_ != nullptr && !listener.expired()) {
        return nciCeInterface_->SetCeHostListener(listener);
    }
}

/**
 * @brief compute the routing parameters based on the default payment app and all installed app.
 * @return True if success, otherwise false.
 */
bool NciCeProxy::ComputeRoutingParams()
{
    if (nciCeInterface_ != nullptr) {
        return nciCeInterface_->ComputeRoutingParams();
    }
    return false;
}

/**
 * @brief Commit the routing parameters to nfc controller.
 * @return True if success, otherwise false.
 */
bool NciCeProxy::CommitRouting()
{
    if (nciCeInterface_ != nullptr) {
        return nciCeInterface_->CommitRouting();
    }
    return false;
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS