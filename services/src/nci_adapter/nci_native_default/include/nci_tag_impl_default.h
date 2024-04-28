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

#ifndef NCI_TAG_IMPL_DEFAULT_H
#define NCI_TAG_IMPL_DEFAULT_H

#include "inci_tag_interface.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class NciTagImplDefault : public INciTagInterface {
public:
    ~NciTagImplDefault() override = default;
    void SetTagListener(std::weak_ptr<ITagListener> listener) override;
    std::vector<int> GetTechList(uint32_t tagDiscId) override;
    uint32_t GetConnectedTech(uint32_t tagDiscId) override;
    std::vector<AppExecFwk::PacMap> GetTechExtrasData(uint32_t tagDiscId) override;
    std::string GetTagUid(uint32_t tagDiscId) override;
    bool Connect(uint32_t tagDiscId, uint32_t technology) override;
    bool Disconnect(uint32_t tagDiscId) override;
    bool Reconnect(uint32_t tagDiscId) override;
    int Transceive(uint32_t tagDiscId, const std::string &command, std::string &response) override;
    std::string ReadNdef(uint32_t tagDiscId) override;
    std::string FindNdefTech(uint32_t tagDiscId) override;
    bool WriteNdef(uint32_t tagDiscId, std::string &command) override;
    bool FormatNdef(uint32_t tagDiscId, const std::string &key) override;
    bool CanMakeReadOnly(uint32_t ndefType) override;
    bool SetNdefReadOnly(uint32_t tagDiscId) override;
    bool DetectNdefInfo(uint32_t tagDiscId, std::vector<int> &ndefInfo) override;
    bool IsTagFieldOn(uint32_t tagDiscId) override;
    void StartFieldOnChecking(uint32_t tagDiscId, uint32_t delayedMs) override;
    void StopFieldChecking() override;
    void SetTimeout(uint32_t tagDiscId, uint32_t timeout, uint32_t technology) override;
    void GetTimeout(uint32_t tagDiscId, uint32_t &timeout, uint32_t technology) override;
    void ResetTimeout(uint32_t tagDiscId) override;
    uint32_t GetIsoDepMaxTransceiveLength() override;
    bool IsExtendedLengthApduSupported() override;
    uint16_t GetTechMaskFromTechList(const std::vector<uint32_t> &discTech) override;
    std::string GetVendorBrowserBundleName() override;

private:
    static constexpr int ISO_DEP_FRAME_MAX_LEN = 261;
    uint32_t tagDiscId_;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS

#endif