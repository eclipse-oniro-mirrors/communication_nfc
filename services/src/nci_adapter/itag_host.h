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
#ifndef I_TAG_HOST_H
#define I_TAG_HOST_H
#include <vector>
#include "pac_map.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class ITagHost {
public:
    using TagDisconnectedCallBack = std::function<void(int)>;

    virtual ~ITagHost() {}
    /**
     * @brief Connect the tag
     * @param technology the technology of the tag
     * @return True if ok
     */
    virtual bool Connect(int technology) = 0;
    /**
     * @brief Disconnect the tag
     * @return True if ok
     */
    virtual bool Disconnect() = 0;
    /**
     * @brief Reconnect the tag
     * @return True if ok
     */
    virtual bool Reconnect() = 0;
    /**
     * @brief Send data to tag and receive response
     * @param request the technology of the tag
     * @param response response from the tag
     * @return 0 if ok
     */
    virtual int Transceive(std::string& request, std::string& response) = 0;

    // get the tag related technologies or uid info.
    virtual std::vector<int> GetTechList() = 0;
    virtual int GetConnectedTech() = 0;
    virtual void RemoveTech(int technology) = 0;
    virtual std::vector<AppExecFwk::PacMap> GetTechExtrasData() = 0;
    virtual std::string GetTagUid() = 0;
    virtual int GetTagRfDiscId() = 0;

    // functions for nedf tag only.
    virtual std::string ReadNdef() = 0;
    virtual bool WriteNdef(std::string& data) = 0;
    virtual bool IsNdefFormatable() = 0;
    virtual bool FormatNdef(const std::string& key) = 0;
    virtual bool SetNdefReadOnly() = 0;
    virtual bool IsNdefMsgContained(std::vector<int>& ndefInfo) = 0;

    // functions for checking the tag field on or not.
    virtual bool FieldOnCheckingThread() = 0;
    virtual bool IsTagFieldOn() = 0;
    virtual void OnFieldChecking(TagDisconnectedCallBack callback, int delayedMs) = 0;
    virtual void OffFieldChecking() = 0;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // I_TAG_HOST_H
