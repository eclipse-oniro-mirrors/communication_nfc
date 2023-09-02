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

#include "nfc_hisysevent.h"
#include "hisysevent.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
template<typename... Types>
static void WriteEvent(const std::string& eventType, HiviewDFX::HiSysEvent::EventType type, Types... args)
{
    int ret = HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::NFC, eventType, type, args...);
    if (ret != 0) {
        ErrorLog("Write event fail: %{public}s", eventType.c_str());
    }
}

void WriteNfcFailedHiSysEvent(const NfcFailedParams* failedParams)
{
    WriteEvent("OPERATION_FAILED", HiviewDFX::HiSysEvent::EventType::FAULT,
               "MAIN_ERROR_CODE", static_cast<int>(failedParams->mainErrorCode),
               "SUB_ERROR_CODE", static_cast<int>(failedParams->subErrorCode),
               "CURRENT_DEFAULT_ROUTE", failedParams->defaultRoute,
               "CURRENT_SCREEN_STATE", failedParams->screenState,
               "CURRENT_NFC_STATE", failedParams->nfcState,
               "CURRENT_PASSIVE_LISTEN_STATE", failedParams->passiveListenState,
               "CURRENT_FIRMWARE_VERSION", failedParams->version,
               "APP_PACKAGE_NAME", failedParams->appPackageName);
}

void WriteOpenAndCloseHiSysEvent(int openRequestCnt, int openFailCnt,
                                 int closeRequestCnt, int closeFailCnt)
{
    WriteEvent("OPEN_AND_CLOSE", HiviewDFX::HiSysEvent::EventType::STATISTIC,
               "OPEN_REQUEST_CNT", openRequestCnt,
               "OPEN_FAILED_CNT", openFailCnt,
               "CLOSE_REQUEST_CNT", closeRequestCnt,
               "CLOSE_FAILED_CNT", closeFailCnt);
}

void WriteTagFoundHiSysEvent(int tagFoundCnt, int typeACnt,
                             int typeBCnt, int typeFCnt, int typeVCnt)
{
    WriteEvent("TAG_FOUND", HiviewDFX::HiSysEvent::EventType::STATISTIC,
               "TOTAL_TAG_FOUND_CNT", tagFoundCnt,
               "TYPE_A_TAG_FOUND", typeACnt,
               "TYPE_B_TAG_FOUND", typeBCnt,
               "TYPE_F_TAG_FOUND", typeFCnt,
               "TYPE_V_TAG_FOUND", typeVCnt);
}

void WritePassiveListenHiSysEvent(int requestCnt, int failCnt)
{
    WriteEvent("PASSIVE_LISTEN", HiviewDFX::HiSysEvent::EventType::STATISTIC,
               "REQUEST_PASSIVE_LISTEN_CNT", requestCnt,
               "FAILED_PASSIVE_LISTEN_CNT", failCnt);
}

void WriteFirmwareUpdateHiSysEvent(int requestCnt, int failCnt)
{
    WriteEvent("FIRMWARE_UPDATE", HiviewDFX::HiSysEvent::EventType::STATISTIC,
               "REQUEST_FIRMWARE_UPDATE_CNT", requestCnt,
               "FAILED_FIRMWARE_UPDATE_CNT", failCnt);
}

NfcFailedParams* BuildFailedParams(MainErrorCode mainErrorCode, SubErrorCode subErrorCode)
{
    NfcFailedParams nfcFailedParams;
    nfcFailedParams.mainErrorCode = mainErrorCode;
    nfcFailedParams.subErrorCode = subErrorCode;
    nfcFailedParams.defaultRoute = 0;
    nfcFailedParams.screenState = 0;
    nfcFailedParams.nfcState = 0;
    nfcFailedParams.passiveListenState = 0;
    nfcFailedParams.version = "VERSION";
    nfcFailedParams.appPackageName = "APPNAME";
    return &nfcFailedParams;
}
}  // namespace NFC
}  // namespace OHOS