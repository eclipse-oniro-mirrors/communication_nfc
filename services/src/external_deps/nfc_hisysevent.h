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
#ifndef NFC_HISYSEVENT_H
#define NFC_HISYSEVENT_H
#include <string>

namespace OHOS {
namespace NFC {
enum MainErrorCode {
    NFC_OPEN_FAILED = 101,
    NFC_CLOSE_FAILED = 102,
    FIRMWARE_UPDATE_FAILED = 103,
    PASSIVE_LISTEN_FAILED = 104,
    SET_READER_MODE_EVENT = 105,
    OPEN_NFC_EVENT = 106,
    CLOSE_NFC_EVENT = 107,
    INIT_SA_FAILED = 201, // error code for init sa failed
    NDEF_TEL_EVENT = 301,
    NDEF_SMS_EVENT = 302,
    NDEF_MAIL_EVENT = 303,
    NDEF_TEXT_EVENT = 304,
    NDEF_VCARD_EVENT = 305,
    NDEF_APP_NOT_INSTALL = 306,
    HCE_SWIPE_CARD = 307,
};

enum SubErrorCode {
    DEFAULT_ERR_DEF = 10000,
    NCI_RESP_TIMEOUT = 10001,
    NCI_RESP_ERROR = 10002,
};

const int DEFAULT_COUNT = 1;
const int NOT_COUNT = 0;

typedef struct {
    MainErrorCode mainErrorCode;
    SubErrorCode subErrorCode;
    int defaultRoute;
    int screenState;
    int nfcState;
    int passiveListenState;
    std::string version;
    std::string appPackageName;
} NfcFailedParams;

class NfcHisysEvent {
public:
    static void WriteNfcFailedHiSysEvent(const NfcFailedParams* failedParams);
    static void WriteOpenAndCloseHiSysEvent(int openRequestCnt, int openFailCnt,
                                            int closeRequestCnt, int closeFailCnt);
    static void WriteHceSwipeResultHiSysEvent(const std::string &appPackageName, int hceSwipeCnt);
    static void WriteDefaultPaymentAppChangeHiSysEvent(const std::string &oldAppPackageName,
                                                       const std::string &newAppPackageName);
    static void WriteForegroundAppChangeHiSysEvent(const std::string &appPackageName);
    static void WriteTagFoundHiSysEvent(const std::vector<int> &techList);
    static void WritePassiveListenHiSysEvent(int requestCnt, int failCnt);
    static void WriteFirmwareUpdateHiSysEvent(int requestCnt, int failCnt);
    static void BuildFailedParams(NfcFailedParams &nfcFailedParams,
                                  MainErrorCode mainErrorCode,
                                  SubErrorCode subErrorCode);
};
}  // namespace NFC
}  // namespace OHOS
#endif