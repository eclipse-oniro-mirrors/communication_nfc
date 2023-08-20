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
#ifndef NFC_NCI_ADAPTOR_H
#define NFC_NCI_ADAPTOR_H

#include "infc_nci.h"

namespace OHOS {
namespace NFC {
namespace NCI {
class NfcNciAdaptor : public INfcNci {
public:
    static NfcNciAdaptor& GetInstance();
    explicit NfcNciAdaptor();
    ~NfcNciAdaptor() override;
    static tNFA_PROPRIETARY_CFG* pNfaProprietaryCfg;
    bool IsNciFuncSymbolFound();
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
    tNFA_STATUS NfcHciRegister(std::string appName, tNFA_HCI_CBACK* cback, bool sendConnEvts) override;
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
    bool NfcConfigHasKey(const std::string& key) override;
    unsigned NfcConfigGetUnsigned(const std::string& key) override;
    unsigned NfcConfigGetUnsignedWithDefaultValue(const std::string& key, unsigned defaultValue) override;
    void NfcConfigGetBytes(const std::string& key, std::vector<uint8_t>& value) override;
    tNFA_STATUS NfaCeConfigureUiccListenTech(tNFA_HANDLE eeHandle, tNFA_TECHNOLOGY_MASK techMask) override;
    tNFA_STATUS NfaEeAddSystemCodeRouting(uint16_t systemCode,
                                          tNFA_HANDLE eeHandle,
                                          tNFA_EE_PWR_STATE powerState) override;

    typedef void (*NFA_INIT)(tHAL_NFC_ENTRY* halEntryTbl);
    typedef tNFA_STATUS (*NFA_ENABLE)(tNFA_DM_CBACK* dmCback, tNFA_CONN_CBACK* connCback);
    typedef tNFA_STATUS (*NFA_DISABLE)(bool graceful);
    typedef tNFA_STATUS (*NFA_ENABLE_POLLING)(tNFA_TECHNOLOGY_MASK pollMask);
    typedef tNFA_STATUS (*NFA_DISABLE_POLLING)(void);
    typedef tNFA_STATUS (*NFA_ENABLE_LISTENING)(void);
    typedef tNFA_STATUS (*NFA_DISABLE_LISTENING)(void);
    typedef tNFA_STATUS (*NFA_START_RF_DISCOVERY)(void);
    typedef tNFA_STATUS (*NFA_STOP_RF_DISCOVERY)(void);
    typedef tNFA_STATUS (*NFA_SET_RF_DISCOVERY_DURATION)(uint16_t discoveryPeriodMs);
    typedef tNFA_STATUS (*NFA_SELECT)(uint8_t rfDiscId, tNFA_NFC_PROTOCOL protocol, tNFA_INTF_TYPE rfInterface);
    typedef tNFA_STATUS (*NFA_DEACTIVATE)(bool sleepMode);
    typedef tNFA_STATUS (*NFA_SEND_RAW_FRAME)(uint8_t* rawData, uint16_t dataLen, uint16_t presenceCheckStartDelay);
    typedef tNFA_STATUS (*NFA_REGISTER_NDEF_TYPE_HANDLER)(bool handleWholeMessage,
                                                          tNFA_TNF tnf,
                                                          uint8_t* typeName,
                                                          uint8_t typeNameLen,
                                                          tNFA_NDEF_CBACK* ndefCback);
    typedef tNFA_STATUS (*NFA_RW_DETECT_NDEF)(void);
    typedef tNFA_STATUS (*NFA_RW_READ_NDEF)(void);
    typedef tNFA_STATUS (*NFA_RW_WRITE_NDEF)(uint8_t* data, uint32_t len);
    typedef tNFA_STATUS (*NFA_RW_PRESENCE_CHECK)(tNFA_RW_PRES_CHK_OPTION option);
    typedef tNFA_STATUS (*NFA_RW_FORMAT_TAG)(void);
    typedef tNFA_STATUS (*NFA_RW_SET_TAG_READ_ONLY)(bool hardLock);
    typedef void (*NDEF_MSG_INIT)(uint8_t* msg, uint32_t maxSize, uint32_t* curSize);
    typedef tNDEF_STATUS (*NDEF_MSG_ADD_REC)(uint8_t* msg,
                                             uint32_t maxSize,
                                             uint32_t* curSize,
                                             uint8_t tnf,
                                             uint8_t* type,
                                             uint8_t typeLen,
                                             uint8_t* id,
                                             uint8_t idLen,
                                             uint8_t* payload,
                                             uint32_t payloadLen);
    typedef uint8_t (*NFC_GET_NCI_VERSION)();
    typedef void (*NFC_ADAPTATION_FUNCS)();
    typedef tHAL_NFC_ENTRY* (*NFC_ADAPTATION_GET_HAL_ENTRY_FUNCS)();
    typedef void (*NFC_ADAPTATION_DUMP)(int fd);
    typedef tNFA_STATUS (*NFC_HCI_REGISTER)(std::string appName, tNFA_HCI_CBACK* cback, bool sendConnEvts);
    typedef tNFA_STATUS (*NFC_EE_GET_INFO)(uint8_t* numNfcee, tNFA_EE_INFO* info);
    typedef tNFA_STATUS (*NFC_EE_REGISTER)(tNFA_EE_CBACK* cback);
    typedef tNFA_STATUS (*NFC_EE_DEREGISTER)(tNFA_EE_CBACK* cback);
    typedef tNFA_STATUS (*NFC_EE_SET_DEFAULT_TECH_ROUTING)(tNFA_HANDLE eeHandle,
                                                           tNFA_TECHNOLOGY_MASK technologiesSwitchOn,
                                                           tNFA_TECHNOLOGY_MASK technologiesSwitchOff,
                                                           tNFA_TECHNOLOGY_MASK technologiesBatteryOff,
                                                           tNFA_TECHNOLOGY_MASK technologiesScreenLock,
                                                           tNFA_TECHNOLOGY_MASK technologiesScreenOff,
                                                           tNFA_TECHNOLOGY_MASK technologiesScreenOffLock);
    typedef tNFA_STATUS (*NFC_EE_CLEAR_DEFAULT_TECH_ROUTING)(tNFA_HANDLE eeHandle,
                                                             tNFA_TECHNOLOGY_MASK clearTechnology);
    typedef tNFA_STATUS (*NFC_EE_SET_DEFAULT_PROTO_ROUTING)(tNFA_HANDLE eeHandle,
                                                            tNFA_PROTOCOL_MASK protocolsSwitchOn,
                                                            tNFA_PROTOCOL_MASK protocolsSwitchOff,
                                                            tNFA_PROTOCOL_MASK protocolsBatteryOff,
                                                            tNFA_PROTOCOL_MASK technologiesScreenLock,
                                                            tNFA_PROTOCOL_MASK technologiesScreenOff,
                                                            tNFA_PROTOCOL_MASK technologiesScreenOffLock);
    typedef tNFA_STATUS (*NFC_EE_CLEAR_DEFAULT_PROTO_ROUTING)(tNFA_HANDLE eeHandle, tNFA_PROTOCOL_MASK clearProtocol);
    typedef tNFA_STATUS (*NFC_EE_ADD_AID_ROUTING)(
        tNFA_HANDLE eeHandle, uint8_t aidLen, uint8_t* aid, tNFA_EE_PWR_STATE powerState, uint8_t aidInfo);
    typedef tNFA_STATUS (*NFC_EE_REMOVE_AID_ROUTING)(uint8_t aidLen, uint8_t* aid);
    typedef tNFA_STATUS (*NFC_EE_UPDATE_NOW)(void);
    typedef tNFA_STATUS (*NFC_EE_MODE_SET)(tNFA_HANDLE eeHandle, tNFA_EE_MD mode);
    typedef uint16_t (*NFC_GET_AID_TABLE_SIZE)();
    typedef tNFA_STATUS (*NFC_CE_SET_ISO_DEP_LISTEN_TECH)(tNFA_TECHNOLOGY_MASK techMask);
    typedef tNFA_STATUS (*NFC_CE_REGISTER_AID_ON_DH)(uint8_t aid[NFC_MAX_AID_LEN],
                                                     uint8_t aidLen,
                                                     tNFA_CONN_CBACK* connCback);
    typedef tNFA_STATUS (*NFC_SET_POWER_SUB_STATE_FOR_SCREEN_STATE)(uint8_t screenState);
    typedef tNFA_STATUS (*NFC_SET_CONFIG)(tNFA_PMID paramId, uint8_t length, uint8_t* data);
    typedef bool (*NFC_CONFIG_HAS_KEY)(const std::string& key);
    typedef unsigned (*NFC_CONFIG_GET_UNSIGNED)(const std::string& key);
    typedef unsigned (*NFC_CONFIG_GET_UNSIGNED_WITH_DEFAULT_VALUE)(const std::string& key, unsigned defaultValue);
    typedef void (*NFC_CONFIG_GET_BYTES)(const std::string& key, std::vector<uint8_t>& value);
    typedef tNFA_STATUS (*NFA_CE_CONFIGURE_UICC_LISTEN_TECH)(tNFA_HANDLE eeHandle, tNFA_TECHNOLOGY_MASK techMask);
    typedef tNFA_STATUS (*NFA_EE_ADD_SYSTEM_CODE_ROUTING)(uint16_t systemCode,
                                                          tNFA_HANDLE eeHandle,
                                                          tNFA_EE_PWR_STATE powerState);

private:
    void Init();
    bool initialized_ = false;
    bool isNciFuncSymbolFound_ = false;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif  // NFC_NCI_ADAPTOR_H