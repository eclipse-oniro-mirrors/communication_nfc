/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#ifndef NFC_SDK_REPORT
#define NFC_SDK_REPORT

#include "app_event.h"
#include "app_event_processor_mgr.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class NfcHaEventReport {
public:
    NfcHaEventReport(const std::string &sdk, const std::string &api);
    ~NfcHaEventReport();
    void ReportSdkEvent(const int result, const int errCode);

private:
    int64_t AddProcessor();

    int64_t beginTime_ = 0;
    std::string transId_ = "";
    std::string apiName_ = "";
    std::string sdkName_ = "";
};
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif // NFC_SDK_REPORT