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
#include "nfc_polling_params.h"
#include <string>

namespace OHOS {
namespace NFC {
int NfcPollingParams::NFC_POLL_DEFAULT = -1;

NfcPollingParams::NfcPollingParams() : techMask_(0),
    enableLowPowerPolling_(true),
    enableReaderMode_(false),
    enableHostRouting_(false)
{
}

bool NfcPollingParams::operator==(const std::shared_ptr<NfcPollingParams> params) const
{
    return techMask_ == params->techMask_ &&
        (enableLowPowerPolling_ == params->enableLowPowerPolling_) &&
        (enableReaderMode_ == params->enableReaderMode_) &&
        (enableHostRouting_ == params->enableHostRouting_);
}

std::shared_ptr<NfcPollingParams> NfcPollingParams::GetNfcOffParameters()
{
    return std::make_shared<NfcPollingParams>();
}

int NfcPollingParams::GetTechMask() const
{
    return techMask_;
}

bool NfcPollingParams::ShouldEnablePolling() const
{
    return (techMask_ != 0) || enableHostRouting_;
}

bool NfcPollingParams::ShouldEnableLowPowerPolling() const
{
    return enableLowPowerPolling_;
}

bool NfcPollingParams::ShouldEnableReaderMode() const
{
    return enableReaderMode_;
}

bool NfcPollingParams::ShouldEnableHostRouting() const
{
    return enableHostRouting_;
}

void NfcPollingParams::SetTechMask(int techMask)
{
    techMask_ = techMask;
}

std::string NfcPollingParams::ToString()
{
    std::string str;
    return str.append("techMask = ")
        .append(std::to_string(techMask_))
        .append(", ")
        .append("enableLowPowerPolling = ")
        .append(std::to_string(enableLowPowerPolling_))
        .append(", ")
        .append("enableReaderMode = ")
        .append(std::to_string(enableReaderMode_))
        .append(", ")
        .append("enableHostRouting = ")
        .append(std::to_string(enableHostRouting_))
        .append(".");
}
}  // namespace NFC
}  // namespace OHOS
