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
#include "nfcc_nci_adapter.h"
#include "routing_manager.h"

namespace OHOS {
namespace NFC {
namespace NCI {
void NciCeImplDefault::SetCeHostListener(
    std::weak_ptr<ICeHostListener> listener)
{
    NfccNciAdapter::GetInstance().SetCeHostListener(listener);
}

bool NciCeImplDefault::ComputeRoutingParams(int defaultPaymentType)
{
    return NfccNciAdapter::GetInstance().ComputeRoutingParams(defaultPaymentType);
}

bool NciCeImplDefault::CommitRouting()
{
    bool restart = NfccNciAdapter::GetInstance().IsRfEbabled();
    if (restart) {
        NfccNciAdapter::GetInstance().StartRfDiscovery(false);
    }
    bool commitResult = NfccNciAdapter::GetInstance().CommitRouting();
    if (restart) {
        NfccNciAdapter::GetInstance().StartRfDiscovery(true);
    }
    return commitResult;
}
bool NciCeImplDefault::SendRawFrame(std::string &hexCmdData)
{
    return NfccNciAdapter::GetInstance().SendRawFrame(hexCmdData);
}
bool NciCeImplDefault::AddAidRouting(const std::string &aidStr, int route,
                                     int aidInfo, int power)
{
    return RoutingManager::GetInstance().AddAidRouting(aidStr, route, aidInfo,
                                                       power);
}
bool NciCeImplDefault::ClearAidTable()
{
    return RoutingManager::GetInstance().ClearAidTable();
}
std::string NciCeImplDefault::GetSimVendorBundleName()
{
    // please change it to the sim bundle name of your vendor
    return "";
}
} // namespace NCI
} // namespace NFC
} // namespace OHOS