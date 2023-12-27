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
#include "ce_payment_services_parcelable.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
const uint32_t MAX_APP_LIST_NUM = 100;
CePaymentServicesParcelable::CePaymentServicesParcelable()
{
}
CePaymentServicesParcelable::~CePaymentServicesParcelable()
{
    paymentAbilityInfos.clear();
}
bool CePaymentServicesParcelable::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint32(paymentAbilityInfos.size())) {
        ErrorLog("write size failed");
        return false;
    }
    for (unsigned int i = 0; i < paymentAbilityInfos.size(); i++) {
        if (!paymentAbilityInfos[i].Marshalling(parcel)) {
            ErrorLog("write ability failed");
            return false;
        }
    }
    return true;
}
CePaymentServicesParcelable *CePaymentServicesParcelable::Unmarshalling(Parcel &parcel)
{
    uint32_t extraLen = 0;
    parcel.ReadUint32(extraLen);
    if (extraLen >= MAX_APP_LIST_NUM) {
        ErrorLog("invalid length");
        return nullptr;
    }
    std::vector<AbilityInfo> abilityInfos;
    for (uint32_t i = 0; i < extraLen; i++) {
        AbilityInfo *ability = AbilityInfo::Unmarshalling(parcel);
        if (ability == nullptr) {
            ErrorLog("Unmarshalling ability failed");
            return nullptr;
        }
        abilityInfos.push_back(*(ability));
        // push back copy the ability so it can be deleted
        delete ability;
    }
    CePaymentServicesParcelable *paymentService = new (std::nothrow) CePaymentServicesParcelable();
    paymentService->paymentAbilityInfos = std::move(abilityInfos);
    return paymentService;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS