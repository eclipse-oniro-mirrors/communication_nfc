/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#ifndef OHOS_I_START_HCE_INFO_PARCELABLE_H
#define OHOS_I_START_HCE_INFO_PARCELABLE_H

#include "parcel.h"
#include "element_name.h"
namespace OHOS {
namespace NFC {
namespace KITS {
using AppExecFwk::ElementName;
class StartHceInfoParcelable : public Parcelable {
public:
    StartHceInfoParcelable(const std::vector<std::string> &aids, const ElementName &element);
    explicit StartHceInfoParcelable(Parcel &parcel);
    StartHceInfoParcelable();
    ~StartHceInfoParcelable();
    bool Marshalling(Parcel &parcel) const override;
    void SetAids(const std::vector<std::string> &aids);
    void SetElement(const ElementName &element);

    std::vector<std::string> GetAids();
    ElementName GetElement();

private:
    std::vector<std::string> aids_;
    uint32_t aidsCount_;
    ElementName element_;
};
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif