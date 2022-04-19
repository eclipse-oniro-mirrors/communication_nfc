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
#include "nci_adaptations.h"

#include "NfcAdaptation.h"

namespace OHOS {
namespace NFC {
namespace NCI {
NciAdaptations::NciAdaptations() {}

NciAdaptations::~NciAdaptations() {}

void NciAdaptations::NfaInit(tHAL_NFC_ENTRY* halEntryTbl)
{
    NFA_Init(halEntryTbl);
}

tNFA_STATUS NciAdaptations::NfaEnable(tNFA_DM_CBACK* dmCback, tNFA_CONN_CBACK* connCback)
{
    return NFA_Enable(dmCback, connCback);
}

tNFA_STATUS NciAdaptations::NfaDisable(bool graceful)
{
    return NFA_Disable(graceful);
}

tNFA_STATUS NciAdaptations::NfaEnablePolling(tNFA_TECHNOLOGY_MASK pollMask)
{
    return NFA_EnablePolling(pollMask);
}

tNFA_STATUS NciAdaptations::NfaDisablePolling(void)
{
    return NFA_DisablePolling();
}

tNFA_STATUS NciAdaptations::NfaEnableListening(void)
{
    return NFA_EnableListening();
}

tNFA_STATUS NciAdaptations::NfaDisableListening(void)
{
    return NFA_DisableListening();
}

tNFA_STATUS NciAdaptations::NfaStartRfDiscovery(void)
{
    return NFA_StartRfDiscovery();
}

tNFA_STATUS NciAdaptations::NfaStopRfDiscovery(void)
{
    return NFA_StopRfDiscovery();
}

tNFA_STATUS NciAdaptations::NfaSetRfDiscoveryDuration(uint16_t discoveryPeriodMs)
{
    return NFA_SetRfDiscoveryDuration(discoveryPeriodMs);
}

tNFA_STATUS NciAdaptations::NfaSelect(uint8_t rfDiscId, tNFA_NFC_PROTOCOL protocol, tNFA_INTF_TYPE rfInterface)
{
    return NFA_Select(rfDiscId, protocol, rfInterface);
}

tNFA_STATUS NciAdaptations::NfaDeactivate(bool sleepMode)
{
    return NFA_Deactivate(sleepMode);
}

tNFA_STATUS NciAdaptations::NfaSendRawFrame(uint8_t* rawData, uint16_t dataLen, uint16_t presenceCheckStartDelay)
{
    return NFA_SendRawFrame(rawData, dataLen, presenceCheckStartDelay);
}

tNFA_STATUS NciAdaptations::NfaRegisterNDefTypeHandler(
    bool handleWholeMessage, tNFA_TNF tnf, uint8_t* typeName, uint8_t typeNameLen, tNFA_NDEF_CBACK* ndefCback)
{
    return NFA_RegisterNDefTypeHandler(handleWholeMessage, tnf, typeName, typeNameLen, ndefCback);
}

tNFA_STATUS NciAdaptations::NfaRwDetectNdef(void)
{
    return NFA_RwDetectNDef();
}

tNFA_STATUS NciAdaptations::NfaRwReadNdef(void)
{
    return NFA_RwReadNDef();
}

tNFA_STATUS NciAdaptations::NfaRwWriteNdef(uint8_t* data, uint32_t len)
{
    return NFA_RwWriteNDef(data, len);
}

tNFA_STATUS NciAdaptations::NfaRwPresenceCheck(tNFA_RW_PRES_CHK_OPTION option)
{
    return NFA_RwPresenceCheck(option);
}

tNFA_STATUS NciAdaptations::NfaRwFormatTag(void)
{
    return NFA_RwFormatTag();
}

tNFA_STATUS NciAdaptations::NfaRwSetTagReadOnly(bool hardLock)
{
    return NFA_RwSetTagReadOnly(hardLock);
}

void NciAdaptations::NdefMsgInit(uint8_t* msg, uint32_t maxSize, uint32_t* curSize)
{
    NDEF_MsgInit(msg, maxSize, curSize);
}

tNDEF_STATUS NciAdaptations::NdefMsgAddRec(uint8_t* msg,
                                           uint32_t maxSize,
                                           uint32_t* curSize,
                                           uint8_t tnf,
                                           uint8_t* type,
                                           uint8_t typeLen,
                                           uint8_t* id,
                                           uint8_t idLen,
                                           uint8_t* payload,
                                           uint32_t payloadLen)
{
    return NDEF_MsgAddRec(msg, maxSize, curSize, tnf, type, typeLen, id, idLen, payload, payloadLen);
}

uint8_t NciAdaptations::NfcGetNciVersion()
{
    return NFC_GetNCIVersion();
}

void NciAdaptations::NfcAdaptationInitialize()
{
    NfcAdaptation& nfcAdaptInstance = NfcAdaptation::GetInstance();
    nfcAdaptInstance.Initialize();
}

void NciAdaptations::NfcAdaptationFinalize()
{
    NfcAdaptation& nfcAdaptInstance = NfcAdaptation::GetInstance();
    nfcAdaptInstance.Finalize();
}

tHAL_NFC_ENTRY* NciAdaptations::NfcAdaptationGetHalEntryFuncs()
{
    NfcAdaptation& nfcAdaptInstance = NfcAdaptation::GetInstance();
    return nfcAdaptInstance.GetHalEntryFuncs();
}

void NciAdaptations::NfcAdaptationDownloadFirmware()
{
    NfcAdaptation& nfcAdaptInstance = NfcAdaptation::GetInstance();
#if (NXP_EXTNS == TRUE)
    nfcAdaptInstance.DownloadFirmware(nullptr, true);
#else
    nfcAdaptInstance.DownloadFirmware();
#endif
}

void NciAdaptations::NfcAdaptationDump(int fd)
{
    NfcAdaptation& nfcAdaptInstance = NfcAdaptation::GetInstance();
    nfcAdaptInstance.Dump(fd);
}

void NciAdaptations::NfcAdaptationFactoryReset()
{
    NfcAdaptation& nfcAdaptInstance = NfcAdaptation::GetInstance();
    nfcAdaptInstance.FactoryReset();
}

void NciAdaptations::NfcAdaptationDeviceShutdown()
{
    NfcAdaptation& nfcAdaptInstance = NfcAdaptation::GetInstance();
    nfcAdaptInstance.DeviceShutdown();
}

tNFA_STATUS NciAdaptations::NfcHciRegister(char* appName, tNFA_HCI_CBACK* cback, bool sendConnEvts)
{
    return NFA_HciRegister(appName, cback, sendConnEvts);
}

tNFA_STATUS NciAdaptations::NfcEeGetInfo(uint8_t* numNfcee, tNFA_EE_INFO* info)
{
    return NFA_EeGetInfo(numNfcee, info);
}

tNFA_STATUS NciAdaptations::NfcEeRegister(tNFA_EE_CBACK* cback)
{
    return NFA_EeRegister(cback);
}

tNFA_STATUS NciAdaptations::NfcEeDeregister(tNFA_EE_CBACK* cback)
{
    return NFA_EeDeregister(cback);
}

tNFA_STATUS NciAdaptations::NfcEeSetDefaultTechRouting(tNFA_HANDLE eeHandle,
                                                       tNFA_TECHNOLOGY_MASK technologiesSwitchOn,
                                                       tNFA_TECHNOLOGY_MASK technologiesSwitchOff,
                                                       tNFA_TECHNOLOGY_MASK technologiesBatteryOff,
                                                       tNFA_TECHNOLOGY_MASK technologiesScreenLock,
                                                       tNFA_TECHNOLOGY_MASK technologiesScreenOff,
                                                       tNFA_TECHNOLOGY_MASK technologiesScreenOffLock)
{
    return NFA_EeSetDefaultTechRouting(eeHandle,
                                       technologiesSwitchOn,
                                       technologiesSwitchOff,
                                       technologiesBatteryOff,
                                       technologiesScreenLock,
                                       technologiesScreenOff,
                                       technologiesScreenOffLock);
}

tNFA_STATUS NciAdaptations::NfcEeClearDefaultTechRouting(tNFA_HANDLE eeHandle, tNFA_TECHNOLOGY_MASK clearTechnology)
{
    return NFA_EeClearDefaultTechRouting(eeHandle, clearTechnology);
}

tNFA_STATUS NciAdaptations::NfcEeSetDefaultProtoRouting(tNFA_HANDLE eeHandle,
                                                        tNFA_PROTOCOL_MASK protocolsSwitchOn,
                                                        tNFA_PROTOCOL_MASK protocolsSwitchOff,
                                                        tNFA_PROTOCOL_MASK protocolsBatteryOff,
                                                        tNFA_PROTOCOL_MASK technologiesScreenLock,
                                                        tNFA_PROTOCOL_MASK technologiesScreenOff,
                                                        tNFA_PROTOCOL_MASK technologiesScreenOffLock)
{
    return NFA_EeSetDefaultProtoRouting(eeHandle,
                                        protocolsSwitchOn,
                                        protocolsSwitchOff,
                                        protocolsBatteryOff,
                                        technologiesScreenLock,
                                        technologiesScreenOff,
                                        technologiesScreenOffLock);
}

tNFA_STATUS NciAdaptations::NfcEeClearDefaultProtoRouting(tNFA_HANDLE eeHandle, tNFA_PROTOCOL_MASK clearProtocol)
{
    return NFA_EeClearDefaultProtoRouting(eeHandle, clearProtocol);
}

tNFA_STATUS NciAdaptations::NfcEeAddAidRouting(
    tNFA_HANDLE eeHandle, uint8_t aidLen, uint8_t* aid, tNFA_EE_PWR_STATE powerState, uint8_t aidInfo)
{
    return NFA_EeAddAidRouting(eeHandle, aidLen, aid, powerState, aidInfo);
}

tNFA_STATUS NciAdaptations::NfcEeRemoveAidRouting(uint8_t aidLen, uint8_t* aid)
{
    return NFA_EeRemoveAidRouting(aidLen, aid);
}

tNFA_STATUS NciAdaptations::NfcEeUpdateNow(void)
{
    return NFA_EeUpdateNow();
}

uint16_t NciAdaptations::NfcGetAidTableSize()
{
    return NFA_GetAidTableSize();
}

tNFA_STATUS NciAdaptations::NfcEeModeSet(tNFA_HANDLE eeHandle, tNFA_EE_MD mode)
{
    return NFA_EeModeSet(eeHandle, mode);
}

tNFA_STATUS NciAdaptations::NfcCeSetIsoDepListenTech(tNFA_TECHNOLOGY_MASK techMask)
{
    return NFA_CeSetIsoDepListenTech(techMask);
}

tNFA_STATUS NciAdaptations::NfcCeRegisterAidOnDH(uint8_t aid[NFC_MAX_AID_LEN], uint8_t aidLen,
                                                 tNFA_CONN_CBACK* connCback)
{
    return NFA_CeRegisterAidOnDH(aid, aidLen, connCback);
}

tNFA_STATUS NciAdaptations::NfcSetPowerSubStateForScreenState(uint8_t screenState)
{
    return NFA_SetPowerSubStateForScreenState(screenState);
}

tNFA_STATUS NciAdaptations::NfcSetConfig(tNFA_PMID paramId, uint8_t length, uint8_t* data)
{
    return NFA_SetConfig(paramId, length, data);
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
