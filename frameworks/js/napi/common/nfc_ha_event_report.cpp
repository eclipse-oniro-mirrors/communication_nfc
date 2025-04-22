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
#include "nfc_ha_event_report.h"
#include "nfc_sdk_common.h"
#include "loghelper.h"
#include <random>
namespace OHOS {
namespace NFC {
namespace KITS {
const int64_t REPORT_CONFIG_TIMEOUT = 90; // report once every 90s
const int64_t REPORT_CONFIG_ROW = 30; // or report once every 30 data entries
const int64_t MAX_RAMDOM_VALUE = 999999;
const int64_t PROCESSOR_ID_NOT_CREATE = -1;
static int64_t g_processorID = PROCESSOR_ID_NOT_CREATE;

NfcHaEventReport::NfcHaEventReport(const std::string &sdk, const std::string &api)
{
    apiName_ = api;
    sdkName_ = sdk;
    std::random_device randSeed;
    std::mt19937 gen(randSeed());
    std::uniform_int_distribution<> dis(0, MAX_RAMDOM_VALUE);
    transId_ = std::string("transId_") + std::to_string(dis(gen));

    beginTime_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
        .count();
    if (g_processorID == PROCESSOR_ID_NOT_CREATE) {
        g_processorID = AddProcessor();
    }
}

NfcHaEventReport::~NfcHaEventReport()
{
}

void NfcHaEventReport::ReportSdkEvent(const int result, const int errCode)
{
    int64_t endTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
        .count();
    OHOS::HiviewDFX::HiAppEvent::Event event("api_diagnostic", "api_exec_end", OHOS::HiviewDFX::HiAppEvent::BEHAVIOR);
    event.AddParam("trans_id", this->transId_);
    event.AddParam("api_name", this->apiName_);
    event.AddParam("sdk_name", this->sdkName_);
    event.AddParam("begin_time", this->beginTime_);
    event.AddParam("end_time", endTime);
    event.AddParam("result", result);
    event.AddParam("error_code", errCode);
    int ret = Write(event);
    InfoLog("transId:%{public}s, apiName:%{public}s, sdkName:%{public}s, "
        "startTime:%{public}ld, endTime:%{public}ld, result:%{public}d, errCode:%{public}d, ret:%{public}d",
        this->transId_.c_str(), this->apiName_.c_str(), this->sdkName_.c_str(),
        this->beginTime_, endTime, result, errCode, ret);
}

int64_t NfcHaEventReport::AddProcessor()
{
    OHOS::HiviewDFX::HiAppEvent::ReportConfig config;
    config.name = "ha_app_event";
    std::string appId = "";
    if (!NfcSdkCommon::GetConfigFromJson(KEY_REPORT_APPID, appId)) {
        ErrorLog("GetConfigFromJson error appId:%{public}s", appId.c_str());
        return 0;
    }
    config.appId = appId;
    config.routeInfo = "AUTO";
    config.triggerCond.timeout = REPORT_CONFIG_TIMEOUT;
    config.triggerCond.row = REPORT_CONFIG_ROW;
    config.eventConfigs.clear();
    {
        OHOS::HiviewDFX::HiAppEvent::EventConfig event;
        event.domain = "api_diagnostic";
        event.name = "api_exec_end";
        event.isRealTime = false;
        config.eventConfigs.push_back(event);
    }
    {
        OHOS::HiviewDFX::HiAppEvent::EventConfig event2;
        event2.domain = "api_diagnostic";
        event2.name = "api_called_stat";
        event2.isRealTime = true;
        config.eventConfigs.push_back(event2);
    }
    {
        OHOS::HiviewDFX::HiAppEvent::EventConfig event3;
        event3.domain = "api_diagnostic";
        event3.name = "api_called_stat_cnt";
        event3.isRealTime = true;
        config.eventConfigs.push_back(event3);
    }
    return OHOS::HiviewDFX::HiAppEvent::AppEventProcessorMgr::AddProcessor(config);
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
