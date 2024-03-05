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

#ifndef NCI_CE_IMPL_DEFAULT_H
#define NCI_CE_IMPL_DEFAULT_H

#include "inci_ce_interface.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class NciCeImplDefault : public INciCeInterface {
public:
    ~NciCeImplDefault() override = default;
    void SetCeHostListener(std::weak_ptr<ICeHostListener> listener) override;
    bool ComputeRoutingParams(int defaultPaymentType) override;
    bool CommitRouting() override;
    bool SendRawFrame(std::string &hexCmdData) override;
    bool AddAidRouting(const std::string &aidStr, int route, int aidInfo, int power) override;
    bool ClearAidTable() override;
    std::string GetSimVendorBundleName() override;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS

#endif