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
#include "extns.h"

namespace OHOS {
namespace NFC {
namespace NCI {
Extns& Extns::GetInstance()
{
    static Extns extns;
    return extns;
}

tNFA_STATUS Extns::EXTNS_Init(tNFA_DM_CBACK *dmCallback, tNFA_CONN_CBACK *connCallback)
{
    return NFA_STATUS_OK;
}

tNFA_STATUS Extns::EXTNS_MfcInit(tNFA_ACTIVATED& activationData)
{
    return NFA_STATUS_OK;
}

void Extns::EXTNS_Close(void)
{
}

tNFA_STATUS Extns::EXTNS_MfcDisconnect(void)
{
    return NFA_STATUS_OK;
}

tNFA_STATUS Extns::EXTNS_MfcActivated(void)
{
    return NFA_STATUS_OK;
}

tNFA_STATUS Extns::EXTNS_MfcTransceive(uint8_t *data, uint32_t len)
{
    return NFA_STATUS_OK;
}

tNFA_STATUS Extns::EXTNS_MfcCallBack(uint8_t *buf, uint32_t buflen)
{
    return NFA_STATUS_OK;
}

void Extns::EXTNS_SetConnectFlag(bool flagVal)
{
}

bool Extns::EXTNS_GetConnectFlag(void)
{
    return true;
}

void Extns::EXTNS_SetDeactivateFlag(bool flagVal)
{
}

bool Extns::EXTNS_GetDeactivateFlag(void)
{
    return true;
}

bool Extns::EXTNS_GetCallBackFlag(void)
{
    return true;
}

tNFA_STATUS Extns::EXTNS_CheckMfcResponse(uint8_t** transceiveData, uint32_t *transceiveDataLen)
{
    return NFA_STATUS_OK;
}

tNFA_STATUS Extns::EXTNS_MfcPresenceCheck(void)
{
    return NFA_STATUS_OK;
}

tNFA_STATUS Extns::EXTNS_GetPresenceCheckStatus(void)
{
    return NFA_STATUS_OK;
}

tNFA_STATUS Extns::EXTNS_MfcRegisterNDefTypeHandler(tNFA_NDEF_CBACK* ndefHandlerCallback)
{
    return NFA_STATUS_OK;
}

tNFA_STATUS Extns::EXTNS_MfcReadNDef(void)
{
    return NFA_STATUS_OK;
}

tNFA_STATUS Extns::EXTNS_MfcCheckNDef(void)
{
    return NFA_STATUS_OK;
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
