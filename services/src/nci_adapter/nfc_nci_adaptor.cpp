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
#include "nfc_nci_adaptor.h"
#include "vendor_ext_service.h"
#include "loghelper.h"
#include "nfa_api.h"
#include "nfc_hal_api.h"
#include <dlfcn.h>

namespace OHOS {
namespace NFC {
namespace NCI {

static NfcNciAdaptor::NFA_INIT nfaInitFuncHandle;
static NfcNciAdaptor::NFA_ENABLE nfaEnableFuncHandle;
static NfcNciAdaptor::NFA_DISABLE nfaDisableFuncHandle;
static NfcNciAdaptor::NFA_ENABLE_POLLING nfaEnablePollingFuncHandle;
static NfcNciAdaptor::NFA_DISABLE_POLLING nfaDisablePollingFuncHandle;
static NfcNciAdaptor::NFA_ENABLE_LISTENING nfaEnableListeningFuncHandle;
static NfcNciAdaptor::NFA_DISABLE_LISTENING nfaDisableListeningFuncHandle;
static NfcNciAdaptor::NFA_START_RF_DISCOVERY nfaStartRfDiscoveryFuncHandle;
static NfcNciAdaptor::NFA_STOP_RF_DISCOVERY nfaStopRfDiscoveryFuncHandle;
static NfcNciAdaptor::NFA_SET_RF_DISCOVERY_DURATION nfaSetRfDiscoveryDurationFuncHandle;
static NfcNciAdaptor::NFA_SELECT nfaSelectFuncHandle;
static NfcNciAdaptor::NFA_DEACTIVATE nfaDeactivateFuncHandle;
static NfcNciAdaptor::NFA_SEND_RAW_FRAME nfaSendRawFrameFuncHandle;
static NfcNciAdaptor::NFA_REGISTER_NDEF_TYPE_HANDLER nfaRegisterNDefTypeHandlerFuncHandle;
static NfcNciAdaptor::NFA_RW_DETECT_NDEF nfaRwDetectNdefFuncHandle;
static NfcNciAdaptor::NFA_RW_READ_NDEF nfaRwReadNdefFuncHandle;
static NfcNciAdaptor::NFA_RW_WRITE_NDEF nfaRwWriteNdefFuncHandle;
static NfcNciAdaptor::NFA_RW_PRESENCE_CHECK nfaRwPresenceCheckFuncHandle;
static NfcNciAdaptor::NFA_RW_FORMAT_TAG nfaRwFormatTagFuncHandle;
static NfcNciAdaptor::NFA_RW_SET_TAG_READ_ONLY nfaRwSetTagReadOnlyFuncHandle;
static NfcNciAdaptor::NDEF_MSG_INIT ndefMsgInitFuncHandle;
static NfcNciAdaptor::NDEF_MSG_ADD_REC ndefMsgAddRecFuncHandle;
static NfcNciAdaptor::NFC_GET_NCI_VERSION nfcGetNciVersionFuncHandle;
static NfcNciAdaptor::NFC_ADAPTATION_FUNCS nfcAdaptationInitializeFuncHandle;
static NfcNciAdaptor::NFC_ADAPTATION_FUNCS nfcAdaptationFinalizeFuncHandle;
static NfcNciAdaptor::NFC_ADAPTATION_FUNCS nfcAdaptationDownloadFirmwareFuncHandle;
static NfcNciAdaptor::NFC_ADAPTATION_FUNCS nfcAdaptationFactoryResetFuncHandle;
static NfcNciAdaptor::NFC_ADAPTATION_FUNCS nfcAdaptationDeviceShutdownFuncHandle;
static NfcNciAdaptor::NFC_ADAPTATION_GET_HAL_ENTRY_FUNCS nfcAdaptationGetHalEntryFuncHandle;
static NfcNciAdaptor::NFC_ADAPTATION_DUMP nfcAdaptationDumpFuncHandle;
static NfcNciAdaptor::NFC_HCI_REGISTER nfcHciRegisterFuncHandle;
static NfcNciAdaptor::NFC_EE_GET_INFO nfcEeGetInfoFuncHandle;
static NfcNciAdaptor::NFC_EE_REGISTER nfcEeRegisterFuncHandle;
static NfcNciAdaptor::NFC_EE_DEREGISTER nfcEeDeregisterFuncHandle;
static NfcNciAdaptor::NFC_EE_SET_DEFAULT_TECH_ROUTING nfcEeSetDefaultTechRoutingFuncHandle;
static NfcNciAdaptor::NFC_EE_CLEAR_DEFAULT_TECH_ROUTING nfcEeClearDefaultTechRoutingFuncHandle;
static NfcNciAdaptor::NFC_EE_SET_DEFAULT_PROTO_ROUTING nfcEeSetDefaultProtoRoutingFuncHandle;
static NfcNciAdaptor::NFC_EE_CLEAR_DEFAULT_PROTO_ROUTING nfcEeClearDefaultProtoRoutingFuncHandle;
static NfcNciAdaptor::NFC_EE_ADD_AID_ROUTING nfcEeAddAidRoutingFuncHandle;
static NfcNciAdaptor::NFC_EE_REMOVE_AID_ROUTING nfcEeRemoveAidRoutingFuncHandle;
static NfcNciAdaptor::NFC_EE_UPDATE_NOW nfcEeUpdateNowFuncHandle;
static NfcNciAdaptor::NFC_EE_MODE_SET nfcEeModeSetFuncHandle;
static NfcNciAdaptor::NFC_GET_AID_TABLE_SIZE nfcGetAidTableSizeFuncHandle;
static NfcNciAdaptor::NFC_CE_SET_ISO_DEP_LISTEN_TECH nfcCeSetIsoDepListenTechFuncHandle;
static NfcNciAdaptor::NFC_CE_REGISTER_AID_ON_DH nfcCeRegisterAidOnDHFuncHandle;
static NfcNciAdaptor::NFC_SET_POWER_SUB_STATE_FOR_SCREEN_STATE nfcSetPowerSubStateForScreenStateFuncHandle;
static NfcNciAdaptor::NFC_SET_CONFIG nfcSetConfigFuncHandle;
static NfcNciAdaptor::NFC_CONFIG_HAS_KEY nfcConfigHasKeyFuncHandle;
static NfcNciAdaptor::NFC_CONFIG_GET_UNSIGNED nfcConfigGetUnsignedFuncHandle;
static NfcNciAdaptor::NFC_CONFIG_GET_UNSIGNED_WITH_DEFAULT_VALUE nfcConfigGetUnsignedWithDefaultValueFuncHandle;
static NfcNciAdaptor::NFC_CONFIG_GET_BYTES nfcConfigGetBytesFuncHandle;
static NfcNciAdaptor::NFA_CE_CONFIGURE_UICC_LISTEN_TECH nfaCeConfigureUiccListenTechFuncHandle;
static NfcNciAdaptor::NFA_EE_ADD_SYSTEM_CODE_ROUTING nfaEeAddSystemCodeRoutingFuncHandle;

static void* g_pLibHandle = nullptr;
static NfcNciAdaptor nciAdaptor_;

NfcNciAdaptor& NfcNciAdaptor::GetInstance()
{
    return nciAdaptor_;
}

NfcNciAdaptor::NfcNciAdaptor()
{
    if (!initialized_) {
        Init();
        initialized_ = true;
    }
}

NfcNciAdaptor::~NfcNciAdaptor()
{
    g_pLibHandle = nullptr;
    VendorExtService::OnStopExtService();
}

void NfcNciAdaptor::Init()
{
    VendorExtService::OnStartExtService();
    std::string chipType = VendorExtService::GetNfcChipType();
    std::string pVendorLibName = std::string("/system/lib64/libnfc_nci") + std::string("_")
                                                            + chipType + std::string(".z.so");
    g_pLibHandle = dlopen(pVendorLibName.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!g_pLibHandle) {
        WarnLog("%{public}s: cannot open vendor library: %{public}s", __func__, dlerror());
        std::string pDefaultLibName = "/system/lib64/libnfc-nci.z.so";
        g_pLibHandle = dlopen(pDefaultLibName.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if (!g_pLibHandle) {
            ErrorLog("%{public}s: cannot open default library: %{public}s", __func__, dlerror());
            VendorExtService::OnStopExtService();
            return;
        }
    } else {
        InfoLog("%{public}s: successfully open vendor library", __func__);
    }

    // has found the lib, load the NfaInit to check symbols exist or not.
    if (g_pLibHandle) {
        const char* pChFuncName = "NfaInit";
        nfaInitFuncHandle = (NFA_INIT)dlsym(g_pLibHandle, pChFuncName);
        if (nfaInitFuncHandle) {
            isNciFuncSymbolFound_ = true;
        }
    }
}

bool NfcNciAdaptor::IsNciFuncSymbolFound()
{
    return isNciFuncSymbolFound_;
}

tNFA_PROPRIETARY_CFG* NfcNciAdaptor::pNfaProprietaryCfg =
                                    (tNFA_PROPRIETARY_CFG*)dlsym(g_pLibHandle, "pNfaProprietaryCfg");

void NfcNciAdaptor::NfaInit(tHAL_NFC_ENTRY* halEntryTbl)
{
    const char* pChFuncName = "NfaInit";
    nfaInitFuncHandle = (NFA_INIT)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaInitFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return;
    }
    nfaInitFuncHandle(halEntryTbl);
}

tNFA_STATUS NfcNciAdaptor::NfaEnable(tNFA_DM_CBACK* dmCback, tNFA_CONN_CBACK* connCback)
{
    const char* pChFuncName = "NfaEnable";
    nfaEnableFuncHandle = (NFA_ENABLE)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaEnableFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaEnableFuncHandle(dmCback, connCback);
}

tNFA_STATUS NfcNciAdaptor::NfaDisable(bool graceful)
{
    const char* pChFuncName = "NfaDisable";
    nfaDisableFuncHandle = (NFA_DISABLE)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaDisableFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaDisableFuncHandle(graceful);
}

tNFA_STATUS NfcNciAdaptor::NfaEnablePolling(tNFA_TECHNOLOGY_MASK pollMask)
{
    const char* pChFuncName = "NfaEnablePolling";
    nfaEnablePollingFuncHandle = (NFA_ENABLE_POLLING)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaEnablePollingFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaEnablePollingFuncHandle(pollMask);
}

tNFA_STATUS NfcNciAdaptor::NfaDisablePolling(void)
{
    const char* pChFuncName = "NfaDisablePolling";
    nfaDisablePollingFuncHandle = (NFA_DISABLE_POLLING)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaDisablePollingFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaDisablePollingFuncHandle();
}

tNFA_STATUS NfcNciAdaptor::NfaEnableListening(void)
{
    const char* pChFuncName = "NfaEnableListening";
    nfaEnableListeningFuncHandle = (NFA_ENABLE_LISTENING)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaEnableListeningFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaEnableListeningFuncHandle();
}

tNFA_STATUS NfcNciAdaptor::NfaDisableListening(void)
{
    const char* pChFuncName = "NfaDisableListening";
    nfaDisableListeningFuncHandle = (NFA_DISABLE_LISTENING)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaDisableListeningFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaDisableListeningFuncHandle();
}

tNFA_STATUS NfcNciAdaptor::NfaStartRfDiscovery(void)
{
    const char* pChFuncName = "NfaStartRfDiscovery";
    nfaStartRfDiscoveryFuncHandle = (NFA_START_RF_DISCOVERY)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaStartRfDiscoveryFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaStartRfDiscoveryFuncHandle();
}

tNFA_STATUS NfcNciAdaptor::NfaStopRfDiscovery(void)
{
    const char* pChFuncName = "NfaStopRfDiscovery";
    nfaStopRfDiscoveryFuncHandle = (NFA_STOP_RF_DISCOVERY)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaStopRfDiscoveryFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaStopRfDiscoveryFuncHandle();
}

tNFA_STATUS NfcNciAdaptor::NfaSetRfDiscoveryDuration(uint16_t discoveryPeriodMs)
{
    const char* pChFuncName = "NfaSetRfDiscoveryDuration";
    nfaSetRfDiscoveryDurationFuncHandle = (NFA_SET_RF_DISCOVERY_DURATION)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaSetRfDiscoveryDurationFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaSetRfDiscoveryDurationFuncHandle(discoveryPeriodMs);
}

tNFA_STATUS NfcNciAdaptor::NfaSelect(uint8_t rfDiscId, tNFA_NFC_PROTOCOL protocol, tNFA_INTF_TYPE rfInterface)
{
    const char* pChFuncName = "NfaSelect";
    nfaSelectFuncHandle = (NFA_SELECT)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaSelectFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaSelectFuncHandle(rfDiscId, protocol, rfInterface);
}

tNFA_STATUS NfcNciAdaptor::NfaDeactivate(bool sleepMode)
{
    const char* pChFuncName = "NfaDeactivate";
    nfaDeactivateFuncHandle = (NFA_DEACTIVATE)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaDeactivateFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaDeactivateFuncHandle(sleepMode);
}

tNFA_STATUS NfcNciAdaptor::NfaSendRawFrame(uint8_t* rawData, uint16_t dataLen, uint16_t presenceCheckStartDelay)
{
    const char* pChFuncName = "NfaSendRawFrame";
    nfaSendRawFrameFuncHandle = (NFA_SEND_RAW_FRAME)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaSendRawFrameFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaSendRawFrameFuncHandle(rawData, dataLen, presenceCheckStartDelay);
}

tNFA_STATUS NfcNciAdaptor::NfaRegisterNDefTypeHandler(
    bool handleWholeMessage, tNFA_TNF tnf, uint8_t* typeName, uint8_t typeNameLen, tNFA_NDEF_CBACK* ndefCback)
{
    const char* pChFuncName = "NfaRegisterNDefTypeHandler";
    nfaRegisterNDefTypeHandlerFuncHandle = (NFA_REGISTER_NDEF_TYPE_HANDLER)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaRegisterNDefTypeHandlerFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaRegisterNDefTypeHandlerFuncHandle(handleWholeMessage, tnf, typeName, typeNameLen, ndefCback);
}

tNFA_STATUS NfcNciAdaptor::NfaRwDetectNdef(void)
{
    const char* pChFuncName = "NfaRwDetectNdef";
    nfaRwDetectNdefFuncHandle = (NFA_RW_DETECT_NDEF)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaRwDetectNdefFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaRwDetectNdefFuncHandle();
}

tNFA_STATUS NfcNciAdaptor::NfaRwReadNdef(void)
{
    const char* pChFuncName = "NfaRwReadNdef";
    nfaRwReadNdefFuncHandle = (NFA_RW_READ_NDEF)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaRwReadNdefFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaRwReadNdefFuncHandle();
}

tNFA_STATUS NfcNciAdaptor::NfaRwWriteNdef(uint8_t* data, uint32_t len)
{
    const char* pChFuncName = "NfaRwWriteNdef";
    nfaRwWriteNdefFuncHandle = (NFA_RW_WRITE_NDEF)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaRwWriteNdefFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaRwWriteNdefFuncHandle(data, len);
}

tNFA_STATUS NfcNciAdaptor::NfaRwPresenceCheck(tNFA_RW_PRES_CHK_OPTION option)
{
    const char* pChFuncName = "NfaRwPresenceCheck";
    nfaRwPresenceCheckFuncHandle = (NFA_RW_PRESENCE_CHECK)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaRwPresenceCheckFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaRwPresenceCheckFuncHandle(option);
}

tNFA_STATUS NfcNciAdaptor::NfaRwFormatTag(void)
{
    const char* pChFuncName = "NfaRwFormatTag";
    nfaRwFormatTagFuncHandle = (NFA_RW_FORMAT_TAG)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaRwFormatTagFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaRwFormatTagFuncHandle();
}

tNFA_STATUS NfcNciAdaptor::NfaRwSetTagReadOnly(bool hardLock)
{
    const char* pChFuncName = "NfaRwSetTagReadOnly";
    nfaRwSetTagReadOnlyFuncHandle = (NFA_RW_SET_TAG_READ_ONLY)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaRwSetTagReadOnlyFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaRwSetTagReadOnlyFuncHandle(hardLock);
}

void NfcNciAdaptor::NdefMsgInit(uint8_t* msg, uint32_t maxSize, uint32_t* curSize)
{
    const char* pChFuncName = "NdefMsgInit";
    ndefMsgInitFuncHandle = (NDEF_MSG_INIT)dlsym(g_pLibHandle, pChFuncName);
    if (!ndefMsgInitFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return;
    }
    ndefMsgInitFuncHandle(msg, maxSize, curSize);
}

tNDEF_STATUS NfcNciAdaptor::NdefMsgAddRec(uint8_t* msg,
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
    const char* pChFuncName = "NdefMsgAddRec";
    ndefMsgAddRecFuncHandle = (NDEF_MSG_ADD_REC)dlsym(g_pLibHandle, pChFuncName);
    if (!ndefMsgAddRecFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NDEF_REC_NOT_FOUND;
    }
    return ndefMsgAddRecFuncHandle(msg, maxSize, curSize, tnf, type, typeLen, id, idLen, payload, payloadLen);
}

uint8_t NfcNciAdaptor::NfcGetNciVersion()
{
    const char* pChFuncName = "NfcGetNciVersion";
    nfcGetNciVersionFuncHandle = (NFC_GET_NCI_VERSION)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcGetNciVersionFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NCI_VERSION_UNKNOWN;
    }
    return nfcGetNciVersionFuncHandle();
}

void NfcNciAdaptor::NfcAdaptationInitialize()
{
    const char* pChFuncName = "NfcAdaptationInitialize";
    nfcAdaptationInitializeFuncHandle = (NFC_ADAPTATION_FUNCS)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcAdaptationInitializeFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return;
    }
    nfcAdaptationInitializeFuncHandle();
}

void NfcNciAdaptor::NfcAdaptationFinalize()
{
    const char* pChFuncName = "NfcAdaptationFinalize";
    nfcAdaptationFinalizeFuncHandle = (NFC_ADAPTATION_FUNCS)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcAdaptationFinalizeFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return;
    }
    nfcAdaptationFinalizeFuncHandle();
}

tHAL_NFC_ENTRY* NfcNciAdaptor::NfcAdaptationGetHalEntryFuncs()
{
    const char* pChFuncName = "NfcAdaptationGetHalEntryFuncs";
    nfcAdaptationGetHalEntryFuncHandle = (NFC_ADAPTATION_GET_HAL_ENTRY_FUNCS)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcAdaptationGetHalEntryFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return nullptr;
    }
    return nfcAdaptationGetHalEntryFuncHandle();
}

void NfcNciAdaptor::NfcAdaptationDownloadFirmware()
{
    const char* pChFuncName = "NfcAdaptationDownloadFirmware";
    nfcAdaptationDownloadFirmwareFuncHandle = (NFC_ADAPTATION_FUNCS)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcAdaptationDownloadFirmwareFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return;
    }
    nfcAdaptationDownloadFirmwareFuncHandle();
}

void NfcNciAdaptor::NfcAdaptationDump(int fd)
{
    const char* pChFuncName = "NfcAdaptationDump";
    nfcAdaptationDumpFuncHandle = (NFC_ADAPTATION_DUMP)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcAdaptationDumpFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return;
    }
    nfcAdaptationDumpFuncHandle(fd);
}

void NfcNciAdaptor::NfcAdaptationFactoryReset()
{
    const char* pChFuncName = "NfcAdaptationFactoryReset";
    nfcAdaptationFactoryResetFuncHandle = (NFC_ADAPTATION_FUNCS)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcAdaptationFactoryResetFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return;
    }
    nfcAdaptationFactoryResetFuncHandle();
}

void NfcNciAdaptor::NfcAdaptationDeviceShutdown()
{
    const char* pChFuncName = "NfcAdaptationDeviceShutdown";
    nfcAdaptationDeviceShutdownFuncHandle = (NFC_ADAPTATION_FUNCS)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcAdaptationDeviceShutdownFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return;
    }
    nfcAdaptationDeviceShutdownFuncHandle();
}

tNFA_STATUS NfcNciAdaptor::NfcHciRegister(std::string appName, tNFA_HCI_CBACK* cback, bool sendConnEvts)
{
    const char* pChFuncName = "NfaHciRegister";
    nfcHciRegisterFuncHandle = (NFC_HCI_REGISTER)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcHciRegisterFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcHciRegisterFuncHandle(appName, cback, sendConnEvts);
}

tNFA_STATUS NfcNciAdaptor::NfcEeGetInfo(uint8_t* numNfcee, tNFA_EE_INFO* info)
{
    const char* pChFuncName = "NfaEeGetInfo";
    nfcEeGetInfoFuncHandle = (NFC_EE_GET_INFO)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcEeGetInfoFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcEeGetInfoFuncHandle(numNfcee, info);
}

tNFA_STATUS NfcNciAdaptor::NfcEeRegister(tNFA_EE_CBACK* cback)
{
    const char* pChFuncName = "NfaEeRegister";
    nfcEeRegisterFuncHandle = (NFC_EE_REGISTER)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcEeRegisterFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcEeRegisterFuncHandle(cback);
}

tNFA_STATUS NfcNciAdaptor::NfcEeDeregister(tNFA_EE_CBACK* cback)
{
    const char* pChFuncName = "NfaEeDeregister";
    nfcEeDeregisterFuncHandle = (NFC_EE_DEREGISTER)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcEeDeregisterFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcEeDeregisterFuncHandle(cback);
}

tNFA_STATUS NfcNciAdaptor::NfcEeSetDefaultTechRouting(tNFA_HANDLE eeHandle,
                                                      tNFA_TECHNOLOGY_MASK technologiesSwitchOn,
                                                      tNFA_TECHNOLOGY_MASK technologiesSwitchOff,
                                                      tNFA_TECHNOLOGY_MASK technologiesBatteryOff,
                                                      tNFA_TECHNOLOGY_MASK technologiesScreenLock,
                                                      tNFA_TECHNOLOGY_MASK technologiesScreenOff,
                                                      tNFA_TECHNOLOGY_MASK technologiesScreenOffLock)
{
    const char* pChFuncName = "NfaEeSetDefaultTechRouting";
    nfcEeSetDefaultTechRoutingFuncHandle = (NFC_EE_SET_DEFAULT_TECH_ROUTING)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcEeSetDefaultTechRoutingFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcEeSetDefaultTechRoutingFuncHandle(eeHandle,
                                                technologiesSwitchOn,
                                                technologiesSwitchOff,
                                                technologiesBatteryOff,
                                                technologiesScreenLock,
                                                technologiesScreenOff,
                                                technologiesScreenOffLock);
}

tNFA_STATUS NfcNciAdaptor::NfcEeClearDefaultTechRouting(tNFA_HANDLE eeHandle, tNFA_TECHNOLOGY_MASK clearTechnology)
{
    const char* pChFuncName = "NfaEeClearDefaultTechRouting";
    nfcEeClearDefaultTechRoutingFuncHandle = (NFC_EE_CLEAR_DEFAULT_TECH_ROUTING)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcEeClearDefaultTechRoutingFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcEeClearDefaultTechRoutingFuncHandle(eeHandle, clearTechnology);
}

tNFA_STATUS NfcNciAdaptor::NfcEeSetDefaultProtoRouting(tNFA_HANDLE eeHandle,
                                                       tNFA_PROTOCOL_MASK protocolsSwitchOn,
                                                       tNFA_PROTOCOL_MASK protocolsSwitchOff,
                                                       tNFA_PROTOCOL_MASK protocolsBatteryOff,
                                                       tNFA_PROTOCOL_MASK technologiesScreenLock,
                                                       tNFA_PROTOCOL_MASK technologiesScreenOff,
                                                       tNFA_PROTOCOL_MASK technologiesScreenOffLock)
{
    const char* pChFuncName = "NfaEeSetDefaultProtoRouting";
    nfcEeSetDefaultProtoRoutingFuncHandle = (NFC_EE_SET_DEFAULT_PROTO_ROUTING)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcEeSetDefaultProtoRoutingFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcEeSetDefaultProtoRoutingFuncHandle(eeHandle,
                                                 protocolsSwitchOn,
                                                 protocolsSwitchOff,
                                                 protocolsBatteryOff,
                                                 technologiesScreenLock,
                                                 technologiesScreenOff,
                                                 technologiesScreenOffLock);
}

tNFA_STATUS NfcNciAdaptor::NfcEeClearDefaultProtoRouting(tNFA_HANDLE eeHandle, tNFA_PROTOCOL_MASK clearProtocol)
{
    const char* pChFuncName = "NfaEeClearDefaultProtoRouting";
    nfcEeClearDefaultProtoRoutingFuncHandle = (NFC_EE_CLEAR_DEFAULT_PROTO_ROUTING)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcEeClearDefaultProtoRoutingFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcEeClearDefaultProtoRoutingFuncHandle(eeHandle, clearProtocol);
}

tNFA_STATUS NfcNciAdaptor::NfcEeAddAidRouting(
    tNFA_HANDLE eeHandle, uint8_t aidLen, uint8_t* aid, tNFA_EE_PWR_STATE powerState, uint8_t aidInfo)
{
    const char* pChFuncName = "NfaEeAddAidRouting";
    nfcEeAddAidRoutingFuncHandle = (NFC_EE_ADD_AID_ROUTING)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcEeAddAidRoutingFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcEeAddAidRoutingFuncHandle(eeHandle, aidLen, aid, powerState, aidInfo);
}

tNFA_STATUS NfcNciAdaptor::NfcEeRemoveAidRouting(uint8_t aidLen, uint8_t* aid)
{
    const char* pChFuncName = "NfaEeRemoveAidRouting";
    nfcEeRemoveAidRoutingFuncHandle = (NFC_EE_REMOVE_AID_ROUTING)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcEeRemoveAidRoutingFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcEeRemoveAidRoutingFuncHandle(aidLen, aid);
}

tNFA_STATUS NfcNciAdaptor::NfcEeUpdateNow(void)
{
    const char* pChFuncName = "NfaEeUpdateNow";
    nfcEeUpdateNowFuncHandle = (NFC_EE_UPDATE_NOW)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcEeUpdateNowFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcEeUpdateNowFuncHandle();
}

uint16_t NfcNciAdaptor::NfcGetAidTableSize()
{
    const char* pChFuncName = "NfaGetAidTableSize";
    nfcGetAidTableSizeFuncHandle = (NFC_GET_AID_TABLE_SIZE)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcGetAidTableSizeFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return 0;
    }
    return nfcGetAidTableSizeFuncHandle();
}

tNFA_STATUS NfcNciAdaptor::NfcEeModeSet(tNFA_HANDLE eeHandle, tNFA_EE_MD mode)
{
    const char* pChFuncName = "NfaEeModeSet";
    nfcEeModeSetFuncHandle = (NFC_EE_MODE_SET)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcEeModeSetFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcEeModeSetFuncHandle(eeHandle, mode);
}

tNFA_STATUS NfcNciAdaptor::NfcCeSetIsoDepListenTech(tNFA_TECHNOLOGY_MASK techMask)
{
    const char* pChFuncName = "NfaCeSetIsoDepListenTech";
    nfcCeSetIsoDepListenTechFuncHandle = (NFC_CE_SET_ISO_DEP_LISTEN_TECH)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcCeSetIsoDepListenTechFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcCeSetIsoDepListenTechFuncHandle(techMask);
}

tNFA_STATUS NfcNciAdaptor::NfcCeRegisterAidOnDH(uint8_t aid[NFC_MAX_AID_LEN], uint8_t aidLen,
                                                tNFA_CONN_CBACK* connCback)
{
    const char* pChFuncName = "NfaCeRegisterAidOnDH";
    nfcCeRegisterAidOnDHFuncHandle = (NFC_CE_REGISTER_AID_ON_DH)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcCeRegisterAidOnDHFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcCeRegisterAidOnDHFuncHandle(aid, aidLen, connCback);
}

tNFA_STATUS NfcNciAdaptor::NfcSetPowerSubStateForScreenState(uint8_t screenState)
{
    const char* pChFuncName = "NfaSetPowerSubStateForScreenState";
    nfcSetPowerSubStateForScreenStateFuncHandle =
                                    (NFC_SET_POWER_SUB_STATE_FOR_SCREEN_STATE)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcSetPowerSubStateForScreenStateFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcSetPowerSubStateForScreenStateFuncHandle(screenState);
}

tNFA_STATUS NfcNciAdaptor::NfcSetConfig(tNFA_PMID paramId, uint8_t length, uint8_t* data)
{
    const char* pChFuncName = "NfaSetConfig";
    nfcSetConfigFuncHandle = (NFC_SET_CONFIG)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcSetConfigFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfcSetConfigFuncHandle(paramId, length, data);
}

bool NfcNciAdaptor::NfcConfigHasKey(const std::string& key)
{
    const char* pChFuncName = "NfcConfigHasKey";
    nfcConfigHasKeyFuncHandle = (NFC_CONFIG_HAS_KEY)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcConfigHasKeyFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return false;
    }
    return nfcConfigHasKeyFuncHandle(key);
}

unsigned NfcNciAdaptor::NfcConfigGetUnsigned(const std::string& key)
{
    const char* pChFuncName = "NfcConfigGetUnsigned";
    nfcConfigGetUnsignedFuncHandle = (NFC_CONFIG_GET_UNSIGNED)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcConfigGetUnsignedFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return 0;
    }
    return nfcConfigGetUnsignedFuncHandle(key);
}

unsigned NfcNciAdaptor::NfcConfigGetUnsignedWithDefaultValue(const std::string& key, unsigned defaultValue)
{
    const char* pChFuncName = "NfcConfigGetUnsignedWithDefaultValue";
    nfcConfigGetUnsignedWithDefaultValueFuncHandle =
                            (NFC_CONFIG_GET_UNSIGNED_WITH_DEFAULT_VALUE)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcConfigGetUnsignedWithDefaultValueFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return 0;
    }
    return nfcConfigGetUnsignedWithDefaultValueFuncHandle(key, defaultValue);
}

void NfcNciAdaptor::NfcConfigGetBytes(const std::string& key, std::vector<uint8_t>& value)
{
    const char* pChFuncName = "NfcConfigGetBytes";
    nfcConfigGetBytesFuncHandle = (NFC_CONFIG_GET_BYTES)dlsym(g_pLibHandle, pChFuncName);
    if (!nfcConfigGetBytesFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return;
    }
    nfcConfigGetBytesFuncHandle(key, value);
}

tNFA_STATUS NfcNciAdaptor::NfaCeConfigureUiccListenTech(tNFA_HANDLE eeHandle, tNFA_TECHNOLOGY_MASK techMask)
{
    const char* pChFuncName = "NfaCeConfigureUiccListenTech";
    nfaCeConfigureUiccListenTechFuncHandle = (NFA_CE_CONFIGURE_UICC_LISTEN_TECH)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaCeConfigureUiccListenTechFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaCeConfigureUiccListenTechFuncHandle(eeHandle, techMask);
}

tNFA_STATUS NfcNciAdaptor::NfaEeAddSystemCodeRouting(uint16_t systemCode,
                                                     tNFA_HANDLE eeHandle,
                                                     tNFA_EE_PWR_STATE powerState)
{
    const char* pChFuncName = "NfaEeAddSystemCodeRouting";
    nfaEeAddSystemCodeRoutingFuncHandle = (NFA_EE_ADD_SYSTEM_CODE_ROUTING)dlsym(g_pLibHandle, pChFuncName);
    if (!nfaEeAddSystemCodeRoutingFuncHandle) {
        ErrorLog("cannot find function %{public}s: %{public}s", pChFuncName, dlerror());
        return NFA_STATUS_FAILED;
    }
    return nfaEeAddSystemCodeRoutingFuncHandle(systemCode, eeHandle, powerState);
}

}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
