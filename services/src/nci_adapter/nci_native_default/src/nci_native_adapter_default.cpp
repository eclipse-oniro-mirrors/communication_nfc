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

#include "nci_native_adapter_default.h"
#include "nci_ce_impl_default.h"
#include "nci_nfcc_impl_default.h"
#include "nci_tag_impl_default.h"

namespace OHOS {
namespace NFC {
namespace NCI {
DECLARE_NATIVE_INTERFACE(NciNativeAdapterDefault);

std::shared_ptr<INciCeInterface> NciNativeAdapterDefault::GetNciCeInterface()
{
    return std::make_shared<NciCeImplDefault>();
}

std::shared_ptr<INciNfccInterface> NciNativeAdapterDefault::GetNciNfccInterface()
{
    return std::make_shared<NciNfccImplDefault>();
}

std::shared_ptr<INciTagInterface> NciNativeAdapterDefault::GetNciTagInterface()
{
    return std::make_shared<NciTagImplDefault>();
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS