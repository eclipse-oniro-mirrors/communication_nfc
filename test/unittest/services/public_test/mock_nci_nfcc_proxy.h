/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef MOCK_NCI_NFCC_PROXY_H
#define MOCK_NCI_NFCC_PROXY_H

#include "inci_nfcc_interface.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
class MockNciNfccProxy final : public NCI::INciNfccInterface {
public:
    bool Initialize() override
    {
        return true;
    };

    bool Deinitialize() override
    {
        return true;
    };

    void EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart) override
    {
        return;
    };

    void DisableDiscovery() override
    {
        return;
    };

    bool SetScreenStatus(uint8_t screenStateMask) override
    {
        return true;
    };

    int GetNciVersion() override
    {
        return 0;
    };

    void Abort() override
    {
        InfoLog("MockNciNfccProxy abort");
        return;
    };

    void FactoryReset() override
    {
        return;
    };

    void Shutdown() override
    {
        return;
    };

    void NotifyMessageToVendor(int key, const std::string& value) override
    {
        return;
    };

    void UpdateWantExtInfoByVendor(AAFwk::Want& want, const std::string& uri) override
    {
        return;
    };
};
}
}
#endif // MOCK_NCI_NFCC_PROXY_H