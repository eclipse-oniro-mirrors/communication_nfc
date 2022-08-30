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
#ifndef I_NFC_NCI_H
#define I_NFC_NCI_H

#include "ndef_utils.h"
#include "nfa_api.h"
#include "nfa_ce_api.h"
#include "nfa_ee_api.h"
#include "nfa_hci_api.h"
#include "nfa_rw_api.h"
#include "nfc_hal_api.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class INfcNci {
public:
    virtual ~INfcNci() {}
    virtual void NfaInit(tHAL_NFC_ENTRY* halEntryTbl) = 0;
    virtual tNFA_STATUS NfaEnable(tNFA_DM_CBACK* dmCback, tNFA_CONN_CBACK* connCback) = 0;
    virtual tNFA_STATUS NfaDisable(bool graceful) = 0;
    virtual tNFA_STATUS NfaEnablePolling(tNFA_TECHNOLOGY_MASK pollMask) = 0;
    virtual tNFA_STATUS NfaDisablePolling(void) = 0;
    virtual tNFA_STATUS NfaEnableListening(void) = 0;
    virtual tNFA_STATUS NfaDisableListening(void) = 0;
    virtual tNFA_STATUS NfaStartRfDiscovery(void) = 0;
    virtual tNFA_STATUS NfaStopRfDiscovery(void) = 0;
    virtual tNFA_STATUS NfaSetRfDiscoveryDuration(uint16_t discoveryPeriodMs) = 0;
    virtual tNFA_STATUS NfaSelect(uint8_t rfDiscId, tNFA_NFC_PROTOCOL protocol, tNFA_INTF_TYPE rfInterface) = 0;
    virtual tNFA_STATUS NfaDeactivate(bool sleepMode) = 0;
    virtual tNFA_STATUS NfaSendRawFrame(uint8_t* rawData, uint16_t dataLen, uint16_t presenceCheckStartDelay) = 0;
    virtual tNFA_STATUS NfaRegisterNDefTypeHandler(bool handleWholeMessage,
                                                   tNFA_TNF tnf,
                                                   uint8_t* typeName,
                                                   uint8_t typeNameLen,
                                                   tNFA_NDEF_CBACK* ndefCback) = 0;
    virtual tNFA_STATUS NfaRwDetectNdef(void) = 0;
    virtual tNFA_STATUS NfaRwReadNdef(void) = 0;
    virtual tNFA_STATUS NfaRwWriteNdef(uint8_t* data, uint32_t len) = 0;
    virtual tNFA_STATUS NfaRwPresenceCheck(tNFA_RW_PRES_CHK_OPTION option) = 0;
    virtual tNFA_STATUS NfaRwFormatTag(void) = 0;
    virtual tNFA_STATUS NfaRwSetTagReadOnly(bool hardLock) = 0;
    virtual void NdefMsgInit(uint8_t* msg, uint32_t maxSize, uint32_t* curSize) = 0;
    virtual tNDEF_STATUS NdefMsgAddRec(uint8_t* msg,
                                       uint32_t maxSize,
                                       uint32_t* curSize,
                                       uint8_t tnf,
                                       uint8_t* type,
                                       uint8_t typeLen,
                                       uint8_t* id,
                                       uint8_t idLen,
                                       uint8_t* payload,
                                       uint32_t payloadLen) = 0;
    virtual uint8_t NfcGetNciVersion() = 0;
    virtual void NfcAdaptationInitialize() = 0;
    virtual void NfcAdaptationFinalize() = 0;
    virtual tHAL_NFC_ENTRY* NfcAdaptationGetHalEntryFuncs() = 0;
    virtual void NfcAdaptationDownloadFirmware() = 0;
    virtual void NfcAdaptationDump(int fd) = 0;
    virtual void NfcAdaptationFactoryReset() = 0;
    virtual void NfcAdaptationDeviceShutdown() = 0;
    virtual tNFA_STATUS NfcHciRegister(char* appName, tNFA_HCI_CBACK* cback, bool sendConnEvts) = 0;
    virtual tNFA_STATUS NfcEeGetInfo(uint8_t* numNfcee, tNFA_EE_INFO* info) = 0;
    virtual tNFA_STATUS NfcEeRegister(tNFA_EE_CBACK* cback) = 0;
    virtual tNFA_STATUS NfcEeDeregister(tNFA_EE_CBACK* cback) = 0;
    virtual tNFA_STATUS NfcEeSetDefaultTechRouting(tNFA_HANDLE eeHandle,
                                                   tNFA_TECHNOLOGY_MASK technologiesSwitchOn,
                                                   tNFA_TECHNOLOGY_MASK technologiesSwitchOff,
                                                   tNFA_TECHNOLOGY_MASK technologiesBatteryOff,
                                                   tNFA_TECHNOLOGY_MASK technologiesScreenLock,
                                                   tNFA_TECHNOLOGY_MASK technologiesScreenOff,
                                                   tNFA_TECHNOLOGY_MASK technologiesScreenOffLock) = 0;
    virtual tNFA_STATUS NfcEeClearDefaultTechRouting(tNFA_HANDLE eeHandle, tNFA_TECHNOLOGY_MASK clearTechnology) = 0;
    virtual tNFA_STATUS NfcEeSetDefaultProtoRouting(tNFA_HANDLE eeHandle,
                                                    tNFA_PROTOCOL_MASK protocolsSwitchOn,
                                                    tNFA_PROTOCOL_MASK protocolsSwitchOff,
                                                    tNFA_PROTOCOL_MASK protocolsBatteryOff,
                                                    tNFA_PROTOCOL_MASK technologiesScreenLock,
                                                    tNFA_PROTOCOL_MASK technologiesScreenOff,
                                                    tNFA_PROTOCOL_MASK technologiesScreenOffLock) = 0;
    virtual tNFA_STATUS NfcEeClearDefaultProtoRouting(tNFA_HANDLE eeHandle, tNFA_PROTOCOL_MASK clearProtocol) = 0;
    virtual tNFA_STATUS NfcEeAddAidRouting(
        tNFA_HANDLE eeHandle, uint8_t aidLen, uint8_t* aid, tNFA_EE_PWR_STATE powerState, uint8_t aidInfo) = 0;
    virtual tNFA_STATUS NfcEeRemoveAidRouting(uint8_t aidLen, uint8_t* aid) = 0;
    virtual tNFA_STATUS NfcEeUpdateNow(void) = 0;
    virtual tNFA_STATUS NfcEeModeSet(tNFA_HANDLE eeHandle, tNFA_EE_MD mode) = 0;
    virtual uint16_t NfcGetAidTableSize() = 0;
    virtual tNFA_STATUS NfcCeSetIsoDepListenTech(tNFA_TECHNOLOGY_MASK techMask) = 0;
    virtual tNFA_STATUS NfcCeRegisterAidOnDH(uint8_t aid[NFC_MAX_AID_LEN],
                                             uint8_t aidLen,
                                             tNFA_CONN_CBACK* connCback) = 0;
    virtual tNFA_STATUS NfcSetPowerSubStateForScreenState(uint8_t screenState) = 0;
    virtual tNFA_STATUS NfcSetConfig(tNFA_PMID paramId, uint8_t length, uint8_t* data) = 0;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif /* I_NFC_NCI_H */
