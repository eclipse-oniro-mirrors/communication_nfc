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
CePaymentServicesParcelable::CePaymentServicesParcelable(std::vector<AbilityInfo> abilityInfos)
{
    paymentAbilityInfos = std::move(abilityInfos);
}
CePaymentServicesParcelable::CePaymentServicesParcelable()
{
}
CePaymentServicesParcelable::~CePaymentServicesParcelable()
{
    paymentAbilityInfos.clear();
}
bool CePaymentServicesParcelable::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt32(paymentAbilityInfos.size())) {
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
    int32_t extraLen = 0;
    parcel.ReadInt32(extraLen);
    if (extraLen >= MAX_APP_LIST_NUM) {
        return nullptr;
    }
    std::vector<AbilityInfo> abilityInfos;
    for (int i = 0; i < extraLen; i++) {
        AbilityInfo *ability = AbilityInfo::Unmarshalling(parcel);
        abilityInfos.push_back(*(ability));
    }
    CePaymentServicesParcelable *paymentService = new (std::nothrow) CePaymentServicesParcelable(abilityInfos);
    return paymentService;
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS