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
#ifndef OHOS_I_CE_PAYMENT_SERVICES_PARCELABLE_H
#define OHOS_I_CE_PAYMENT_SERVICES_PARCELABLE_H

#include "parcel.h"
#include "ability_info.h"
namespace OHOS {
namespace NFC {
namespace KITS {
using AppExecFwk::AbilityInfo;
struct CePaymentServicesParcelable : public Parcelable {
    CePaymentServicesParcelable();
    ~CePaymentServicesParcelable();
    bool Marshalling(Parcel &parcel) const override;
    static CePaymentServicesParcelable *Unmarshalling(Parcel &parcel);
    std::vector<AbilityInfo> paymentAbilityInfos;
};
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif