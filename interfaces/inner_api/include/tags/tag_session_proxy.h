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
#ifndef TAG_SESSION_PROXY_H
#define TAG_SESSION_PROXY_H
#include "iremote_proxy.h"
#include "itag_session.h"
#include "nfc_basic_proxy.h"

namespace OHOS {
namespace NFC {
namespace TAG {
class TagSessionProxy final : public OHOS::IRemoteProxy<ITagSession>, public NfcBasicProxy {
public:
    explicit TagSessionProxy(const OHOS::sptr<OHOS::IRemoteObject>& remote)
        : OHOS::IRemoteProxy<ITagSession>(remote), NfcBasicProxy(remote)
    {
    }
    ~TagSessionProxy() override {}

    int Connect(int tagRfDiscId, int technology) override;
    int Reconnect(int tagRfDiscId) override;
    void Disconnect(int tagRfDiscId) override;
    int GetMaxTransceiveLength(int technology, int &maxSize) override;
    int SetTimeout(int timeout, int technology) override;
    int GetTimeout(int technology, int &timeout) override;
    int SendRawFrame(int tagRfDiscId, std::string hexCmdData, bool raw, std::string &hexRespData) override;

    std::vector<int> GetTechList(int tagRfDiscId) override;
    bool IsTagFieldOn(int tagRfDiscId) override;
    bool IsNdef(int tagRfDiscId) override;
    std::string NdefRead(int tagRfDiscId) override;
    int NdefWrite(int tagRfDiscId, std::string msg) override;
    int NdefMakeReadOnly(int tagRfDiscId) override;
    int FormatNdef(int tagRfDiscId, const std::string& key) override;
    int CanMakeReadOnly(int ndefType, bool &canSetReadOnly) override;
    int IsSupportedApdusExtended(bool &isSupported) override;
private:
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_SESSION_PROXY_H
