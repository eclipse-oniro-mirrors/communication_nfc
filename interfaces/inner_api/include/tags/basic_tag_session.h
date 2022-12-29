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
#ifndef BASIC_TAG_SESSION_H
#define BASIC_TAG_SESSION_H

#include "itag_session.h"
#include "taginfo.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class BasicTagSession {
public:
    explicit BasicTagSession(std::weak_ptr<TagInfo> tagInfo, KITS::TagTechnology technology);
    virtual ~BasicTagSession() {}

    int Connect();
    bool IsConnected() const;
    int Close();
    int SetTimeout(int timeout);
    int GetTimeout(int &timeout);
    std::string GetTagUid();
    int SendCommand(std::string& hexCmdData, bool raw, std::string &hexRespData);
    int GetMaxSendCommandLength(int &maxSize) const;
    std::weak_ptr<TagInfo> GetTagInfo() const;

protected:
    OHOS::sptr<TAG::ITagSession> GetTagSessionProxy() const;
    int GetTagRfDiscId() const;
    void SetConnectedTagTech(KITS::TagTechnology tech) const;
    KITS::TagTechnology GetConnectedTagTech() const;

private:
    std::weak_ptr<TagInfo> tagInfo_;
    KITS::TagTechnology tagTechnology_;
    bool isConnected_;
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // BASIC_TAG_SESSION_H
