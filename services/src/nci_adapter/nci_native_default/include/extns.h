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
#ifndef EXTNS_H
#define EXTNS_H
#include "nfa_api.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class Extns final {
public:
    static Extns& GetInstance();

    tNFA_STATUS EXTNS_Init(tNFA_DM_CBACK *dmCallback, tNFA_CONN_CBACK *connCallback);
    tNFA_STATUS EXTNS_MfcInit(tNFA_ACTIVATED& activationData);
    void EXTNS_Close(void);
    tNFA_STATUS EXTNS_MfcDisconnect(void);
    tNFA_STATUS EXTNS_MfcActivated(void);
    tNFA_STATUS EXTNS_MfcTransceive(uint8_t *data, uint32_t len);
    tNFA_STATUS EXTNS_MfcCallBack(uint8_t *buf, uint32_t buflen);
    void EXTNS_SetConnectFlag(bool flagVal);
    bool EXTNS_GetConnectFlag(void);
    void EXTNS_SetDeactivateFlag(bool flagVal);
    bool EXTNS_GetDeactivateFlag(void);
    bool EXTNS_GetCallBackFlag(void);
    tNFA_STATUS EXTNS_CheckMfcResponse(uint8_t** transceiveData, uint32_t *transceiveDataLen);
    tNFA_STATUS EXTNS_MfcPresenceCheck(void);
    tNFA_STATUS EXTNS_GetPresenceCheckStatus(void);

    tNFA_STATUS EXTNS_MfcRegisterNDefTypeHandler(tNFA_NDEF_CBACK* ndefHandlerCallback);
    tNFA_STATUS EXTNS_MfcReadNDef(void);
    tNFA_STATUS EXTNS_MfcCheckNDef(void);
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // EXTNS_H
