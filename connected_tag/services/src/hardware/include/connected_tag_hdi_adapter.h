/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#ifndef OHOS_CONNECTED_TAG_HDI_ADAPTER_H
#define OHOS_CONNECTED_TAG_HDI_ADAPTER_H

#include <string>
namespace OHOS {
namespace ConnectedTag {
class ConnectedTagHdiAdapter {
public:
    ~ConnectedTagHdiAdapter();
    static ConnectedTagHdiAdapter &GetInstance();

    int32_t Init();

    int32_t Uninit();

    std::string ReadNdefTag();

    int32_t WriteNdefTag(std::string data);
private:
    ConnectedTagHdiAdapter();
};
}  // namespace Nfc_Connected_Tag
}  // namespace OHOS
#endif
