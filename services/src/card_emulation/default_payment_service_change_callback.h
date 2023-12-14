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
#ifndef NFC_DEFAULT_PAYMENT_APP_CHANGE_H
#define NFC_DEFAULT_PAYMENT_APP_CHANGE_H

#include "data_ability_observer_stub.h"
#include "idefault_payment_service_change_callback.h"
#include "element_name.h"
#include "setting_data_share_impl.h"

namespace OHOS {
namespace NFC {
using OHOS::AppExecFwk::ElementName;
class DefaultPaymentServiceChangeCallback : public AAFwk::DataAbilityObserverStub
{
public:
    explicit DefaultPaymentServiceChangeCallback(sptr<IDefaultPaymentServiceChangeCallback> callback)
        : callback_(callback)
    {
    }
    virtual ~DefaultPaymentServiceChangeCallback()
    {
    }
    void OnChange() override
    {
        if (callback_ == nullptr) {
            return;
        }
        callback_->OnDefaultPaymentServiceChange();
    }

private:
    sptr<IDefaultPaymentServiceChangeCallback> callback_;
};

} // namespace NFC
} // namespace OHOS
#endif