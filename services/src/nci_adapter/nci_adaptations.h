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
#ifndef NCI_ADAPTATIONS_H
#define NCI_ADAPTATIONS_H

#include "infc_nci.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class NciAdaptations : public INfcNci {
public:
    explicit NciAdaptations();
    ~NciAdaptations() override;
    void NfaInit(tHAL_NFC_ENTRY* halEntryTbl) override;
    tNFA_STATUS NfaEnable(tNFA_DM_CBACK* dmCback, tNFA_CONN_CBACK* connCback) override;
    tNFA_STATUS NfaDisable(bool graceful) override;
    tNFA_STATUS NfaEnablePolling(tNFA_TECHNOLOGY_MASK pollMask) override;
    tNFA_STATUS NfaDisablePolling(void) override;
    tNFA_STATUS NfaEnableListening(void) override;
    tNFA_STATUS NfaDisableListening(void) override;
    tNFA_STATUS NfaStartRfDiscovery(void) override;
    tNFA_STATUS NfaStopRfDiscovery(void) override;
    tNFA_STATUS NfaSetRfDiscoveryDuration(uint16_t discoveryPeriodMs) override;
    tNFA_STATUS NfaSelect(uint8_t rfDiscId, tNFA_NFC_PROTOCOL protocol, tNFA_INTF_TYPE rfInterface) override;
    tNFA_STATUS NfaDeactivate(bool sleepMode) override;
    tNFA_STATUS NfaSendRawFrame(uint8_t* rawData, uint16_t dataLen, uint16_t presenceCheckStartDelay) override;
    tNFA_STATUS NfaRegisterNDefTypeHandler(bool handleWholeMessage,
                                           tNFA_TNF tnf,
                                           uint8_t* typeName,
                                           uint8_t typeNameLen,
                                           tNFA_NDEF_CBACK* ndefCback) override;
    tNFA_STATUS NfaRwDetectNdef(void) override;
    tNFA_STATUS NfaRwReadNdef(void) override;
    tNFA_STATUS NfaRwWriteNdef(uint8_t* data, uint32_t len) override;
    tNFA_STATUS NfaRwPresenceCheck(tNFA_RW_PRES_CHK_OPTION option) override;
    tNFA_STATUS NfaRwFormatTag(void) override;
    tNFA_STATUS NfaRwSetTagReadOnly(bool hardLock) override;
    void NdefMsgInit(uint8_t* msg, uint32_t maxSize, uint32_t* curSize) override;
    tNDEF_STATUS NdefMsgAddRec(uint8_t* msg,
                               uint32_t maxSize,
                               uint32_t* curSize,
                               uint8_t tnf,
                               uint8_t* type,
                               uint8_t typeLen,
                               uint8_t* id,
                               uint8_t idLen,
                               uint8_t* payload,
                               uint32_t payloadLen) override;
    uint8_t NfcGetNciVersion() override;
    void NfcAdaptationInitialize() override;
    void NfcAdaptationFinalize() override;
    tHAL_NFC_ENTRY* NfcAdaptationGetHalEntryFuncs() override;
    void NfcAdaptationDownloadFirmware() override;
    void NfcAdaptationDump(int fd) override;
    void NfcAdaptationFactoryReset() override;
    void NfcAdaptationDeviceShutdown() override;
    tNFA_STATUS NfcHciRegister(char* appName, tNFA_HCI_CBACK* cback, bool sendConnEvts) override;
    tNFA_STATUS NfcEeGetInfo(uint8_t* numNfcee, tNFA_EE_INFO* info) override;
    tNFA_STATUS NfcEeRegister(tNFA_EE_CBACK* cback) override;
    tNFA_STATUS NfcEeDeregister(tNFA_EE_CBACK* cback) override;
    tNFA_STATUS NfcEeSetDefaultTechRouting(tNFA_HANDLE eeHandle,
                                           tNFA_TECHNOLOGY_MASK technologiesSwitchOn,
                                           tNFA_TECHNOLOGY_MASK technologiesSwitchOff,
                                           tNFA_TECHNOLOGY_MASK technologiesBatteryOff,
                                           tNFA_TECHNOLOGY_MASK technologiesScreenLock,
                                           tNFA_TECHNOLOGY_MASK technologiesScreenOff,
                                           tNFA_TECHNOLOGY_MASK technologiesScreenOffLock) override;
    tNFA_STATUS NfcEeClearDefaultTechRouting(tNFA_HANDLE eeHandle,
                                             tNFA_TECHNOLOGY_MASK clearTechnology) override;
    tNFA_STATUS NfcEeSetDefaultProtoRouting(tNFA_HANDLE eeHandle,
                                            tNFA_PROTOCOL_MASK protocolsSwitchOn,
                                            tNFA_PROTOCOL_MASK protocolsSwitchOff,
                                            tNFA_PROTOCOL_MASK protocolsBatteryOff,
                                            tNFA_PROTOCOL_MASK technologiesScreenLock,
                                            tNFA_PROTOCOL_MASK technologiesScreenOff,
                                            tNFA_PROTOCOL_MASK technologiesScreenOffLock) override;
    tNFA_STATUS NfcEeClearDefaultProtoRouting(tNFA_HANDLE eeHandle, tNFA_PROTOCOL_MASK clearProtocol) override;
    tNFA_STATUS NfcEeAddAidRouting(
        tNFA_HANDLE eeHandle, uint8_t aidLen, uint8_t* aid, tNFA_EE_PWR_STATE powerState, uint8_t aidInfo) override;
    tNFA_STATUS NfcEeRemoveAidRouting(uint8_t aidLen, uint8_t* aid) override;
    tNFA_STATUS NfcEeUpdateNow(void) override;
    tNFA_STATUS NfcEeModeSet(tNFA_HANDLE eeHandle, tNFA_EE_MD mode) override;
    uint16_t NfcGetAidTableSize() override;
    tNFA_STATUS NfcCeSetIsoDepListenTech(tNFA_TECHNOLOGY_MASK techMask) override;
    tNFA_STATUS NfcCeRegisterAidOnDH(uint8_t aid[NFC_MAX_AID_LEN],
                                     uint8_t aidLen,
                                     tNFA_CONN_CBACK* connCback) override;
    tNFA_STATUS NfcSetPowerSubStateForScreenState(uint8_t screenState) override;
    tNFA_STATUS NfcSetConfig(tNFA_PMID paramId, uint8_t length, uint8_t* data) override;

private:
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // NCI_ADAPTATIONS_H
