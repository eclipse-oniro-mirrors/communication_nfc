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

#ifndef NCI_NFCC_IMPL_DEFAULT_H
#define NCI_NFCC_IMPL_DEFAULT_H

#include "inci_nfcc_interface.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class NciNfccImplDefault : public INciNfccInterface {
public:
    ~NciNfccImplDefault() override = default;
    bool Initialize() override;
    bool Deinitialize() override;
    void EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart) override;
    void DisableDiscovery() override;
    bool SetScreenStatus(uint8_t screenStateMask) override;
    int GetNciVersion() override;
    void Abort() override;
    void FactoryReset() override;
    void Shutdown() override;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS

#endif