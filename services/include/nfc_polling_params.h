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
#ifndef NFC_POLLING_PARAMS_H
#define NFC_POLLING_PARAMS_H

#include <memory>

namespace OHOS {
namespace NFC {
class NfcPollingParams {
public:
    explicit NfcPollingParams();
    ~NfcPollingParams() {}
    bool operator==(const std::shared_ptr<NfcPollingParams> params) const;
    static std::shared_ptr<NfcPollingParams> GetNfcOffParameters();
    std::string ToString();

public:
    void SetTechMask(int techMask);
    int GetTechMask() const;
    bool ShouldEnablePolling() const;
    bool ShouldEnableLowPowerPolling() const;
    bool ShouldEnableReaderMode() const;
    bool ShouldEnableHostRouting() const;

    static int NFC_POLL_DEFAULT;

private:
    int techMask_;
    bool enableLowPowerPolling_;
    bool enableReaderMode_;
    bool enableHostRouting_;
};
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_POLLING_PARAMS_H
