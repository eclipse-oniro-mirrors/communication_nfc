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
#ifndef TAG_INFO_PARCELABLE_H
#define TAG_INFO_PARCELABLE_H
#include "pac_map.h"
#include "parcel.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class TagInfoParcelable : public Parcelable {
public:
    TagInfoParcelable(std::vector<int> tagTechList,
        std::vector<AppExecFwk::PacMap> tagTechExtrasData,
        std::string &tagUid,
        int tagRfDiscId,
        OHOS::sptr<IRemoteObject> tagServiceIface);
    ~TagInfoParcelable();

    bool Marshalling(Parcel &parcel) const override;
    static TagInfoParcelable *Unmarshalling(Parcel &parcel);
    std::string ToString();
    std::string GetUid();
    std::vector<int> GetTechList();
    int GetDiscId();
    std::vector<AppExecFwk::PacMap> GetTechExtrasDataList();

private:
    int tagRfDiscId_;
    std::string tagUid_;
    std::vector<int> tagTechList_;
    std::vector<AppExecFwk::PacMap> tagTechExtrasData_;
    OHOS::sptr<IRemoteObject> tagServiceIface_;
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_INFO_PARCELABLE_H
