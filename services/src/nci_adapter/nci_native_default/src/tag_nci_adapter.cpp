/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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
#include "tag_nci_adapter.h"
#include <unistd.h>
#include "nfc_brcm_defs.h"
#include "nfc_config.h"
#include "nfc_sdk_common.h"
#include "nfcc_nci_adapter.h"
#include "loghelper.h"
#include "nfa_api.h"
#include "rw_int.h"
#include "securec.h"
#include "tag_native_impl.h"

namespace OHOS {
namespace NFC {
namespace NCI {
static const uint32_t DEFAULT_TIMEOUT = 1000;
static const uint32_t READ_NDEF_TIMEOUT = 5000;
static const uint32_t CHECK_NDEF_TIMEOUT = 3000;
static const uint32_t RETRY_RECONNECT_TIMEOUT = 500;
static const uint32_t T2TA_HALT_PICC_TIMEOUT = 10 * 1000; // us, 10S
static const uint32_t ISO3B_HALT_PICC_TIMEOUT = 100;
static const uint32_t ISO14443_3A_DEFAULT_TIMEOUT = 618;   // NfcA
static const uint32_t ISO14443_3B_DEFAULT_TIMEOUT = 1000;  // NfcB
static const uint32_t ISO14443_4_DEFAULT_TIMEOUT = 618;    // ISO-DEP
static const uint32_t FELICA_DEFAULT_TIMEOUT = 255;        // Felica
static const uint32_t ISO15693_DEFAULT_TIMEOUT = 1000;     // NfcV
static const uint32_t NDEF_DEFAULT_TIMEOUT = 1000;
static const uint32_t NDEF_FORMATABLE_DEFAULT_TIMEOUT = 1000;
static const uint32_t MIFARE_CLASSIC_DEFAULT_TIMEOUT = 618;  // MifareClassic
static const uint32_t MIFARE_UL_DEFAULT_TIMEOUT = 618;       // MifareUltralight
static const uint32_t POS_NFCF_STSTEM_CODE_HIGH = 8;
static const uint32_t POS_NFCF_STSTEM_CODE_LOW = 9;
static const uint32_t TOPAZ512_MAX_MESSAGE_SIZE = 462;
static const uint32_t TOPAZ96_MAX_MESSAGE_SIZE = 90;
static const uint32_t SENSF_RES_LENGTH = 8;
static const uint32_t SENS_RES_LENGTH = 2;
static const uint32_t SENSB_RES_POLL_POS = 4;
static const uint32_t SYSTEM_CODE_SHIFT = 8;
static const uint32_t F_POLL_LENGTH = 10;
static const uint32_t I93_POLL_LENGTH = 2;
static const uint32_t I93_ACT_LENGTH = 2;
static const uint32_t MIFACE_DES_FIRE_RESPONSE_LENGTH = 9;
static const uint32_t NDEF_FORMATTABLE_1ST = 0x91;
static const uint32_t NDEF_FORMATTABLE_2ND = 0xAF;
static const uint32_t NDEF_MODE_READ_ONLY = 1;
static const uint32_t NDEF_MODE_READ_WRITE = 2;
static const uint32_t WAIT_TIME_FOR_NO_RSP = 4;
static uint8_t RW_TAG_SLP_REQ[] = {0x50, 0x00};
#if (NXP_EXTNS == FALSE)
static uint8_t RW_DESELECT_REQ[] = {0xC2};
#endif

static const uint32_t INVALID_TAG_INDEX = 0xFF;
static const uint8_t MIFARE_RESPONSE_LEN = 0x10;  // Mifare response len
static const uint8_t T2T_ACK_RESPONSE = 0x0A; // T2T ack response
static const uint32_t TIME_MUL_100MS = 100; // ms
static const uint8_t MIN_FWI = 0;  // min waiting time integer for protocol frame
static const uint8_t MAX_FWI = 14; // max waiting time integer for protocol frame
static const uint8_t NON_STD_CARD_SAK = 0x13;

TagNciAdapter::TagNciAdapter()
    : presChkOption_(NFA_RW_PRES_CHK_DEFAULT),
      discNtfIndex_(0),
      isSkipNdefRead_(false),
      isMultiProtoMFC_(false)
{
    ResetTimeout();
    ResetTag();
    if (NfcConfig::hasKey(NAME_PRESENCE_CHECK_ALGORITHM)) {
        presChkOption_ = NfcConfig::getUnsigned(NAME_PRESENCE_CHECK_ALGORITHM);
    }
    if (NfcConfig::hasKey(NAME_LEGACY_MIFARE_READER)) {
        isLegacyMifareReader_ = (NfcConfig::getUnsigned(NAME_LEGACY_MIFARE_READER) != 0);
    } else {
        isLegacyMifareReader_ = true;
    }
    DebugLog("TagNciAdapter::TagNciAdapter: isLegacyMifareReader_ = %{public}d", isLegacyMifareReader_);
    if (NfcConfig::hasKey(NAME_NXP_NON_STD_CARD_TIMEDIFF)) {
        std::vector<uint8_t> nonStdTimeDiff = NfcConfig::getBytes(NAME_NXP_NON_STD_CARD_TIMEDIFF);
        for (uint8_t i = 0; i < nonStdTimeDiff.size(); i++) {
            multiTagTimeDiff_.push_back(nonStdTimeDiff.at(i) * TIME_MUL_100MS);
            DebugLog("TagNciAdapter::TagNciAdapter: timediff[%{public}d] = %{public}d", i, nonStdTimeDiff.at(i));
        }
    } else {
        DebugLog("TagNciAdapter::TagNciAdapter:timediff not configured, use default");
        multiTagTimeDiff_.push_back(100); // default time diff for Mifare Tag
        multiTagTimeDiff_.push_back(300); // default time diff for ISODEP
    }
#if (NXP_EXTNS == TRUE)
    errno_t err = memset_s(&g_multiTagParams, sizeof(g_multiTagParams), 0, sizeof(g_multiTagParams));
    if (err != EOK) {
        ErrorLog("TagNciAdapter::TagNciAdapter:memset_s for g_multiTagParams error: %{public}d", err);
    }
#endif
    if (NfcConfig::hasKey(NAME_NXP_SUPPORT_NON_STD_CARD)) {
        isMultiTagSupported_ = (NfcConfig::getUnsigned(NAME_LEGACY_MIFARE_READER) != 0);
    } else {
        isMultiTagSupported_ = false;
    }
}

TagNciAdapter::~TagNciAdapter()
{
    tagTechList_.clear();
    tagRfDiscIdList_.clear();
    tagRfProtocols_.clear();
    tagPollBytes_.clear();
    tagActivatedBytes_.clear();
    multiTagDiscId_.clear();
    multiTagDiscProtocol_.clear();
    receivedData_.clear();
    techListIndex_ = 0;
    connectedProtocol_ = NCI_PROTOCOL_UNKNOWN;
    isFelicaLite_ = false;
    isMifareUltralight_ = false;
    isMifareDESFire_ = false;
    isMultiTag_ = false;
    discRstEvtNum_ = 0;
    discNtfIndex_ = 0;
    multiTagTmpTechIdx_ = 0;
    selectedTagIdx_ = 0;
    isMultiProtoMFC_ = false;
    isSkipMifareActive_ = false;
};

TagNciAdapter& TagNciAdapter::GetInstance()
{
    static TagNciAdapter tagNciAdapter;
    return tagNciAdapter;
}

bool TagNciAdapter::IsMifareConnected()
{
    return (connectedProtocol_ == NFC_PROTOCOL_MIFARE);
}

void TagNciAdapter::DoNfaNdefRegisterEvt(tNFA_NDEF_EVT_DATA* eventData)
{
    DebugLog("NdefCallback: NFA_NDEF_REGISTER_EVT; status=0x%{public}X; handle=0x%{public}X",
        eventData->ndef_reg.status, eventData->ndef_reg.ndef_type_handle);
    ndefTypeHandle_ = eventData->ndef_reg.ndef_type_handle;
}

void TagNciAdapter::DoNfaNdefDataEvt(tNFA_NDEF_EVT_DATA* eventData)
{
    DebugLog("NdefCallback: NFA_NDEF_DATA_EVT; data_len = %u", eventData->ndef_data.len);
    uint32_t ndefDataLen = eventData->ndef_data.len;
    readNdefData_ = KITS::NfcSdkCommon::BytesVecToHexString(
        eventData->ndef_data.p_data, ndefDataLen);
}

void TagNciAdapter::NdefCallback(unsigned char event, tNFA_NDEF_EVT_DATA* eventData)
{
    DebugLog("TagNciAdapter::NdefCallback");
    switch (event) {
        case NFA_NDEF_REGISTER_EVT: {
            TagNciAdapter::GetInstance().DoNfaNdefRegisterEvt(eventData);
            break;
        }
        case NFA_NDEF_DATA_EVT: {
            TagNciAdapter::GetInstance().DoNfaNdefDataEvt(eventData);
            break;
        }
        default: {
            DebugLog("%{public}s: Unknown event %{public}u", "NdefCallback", event);
            break;
        }
    }
}

void TagNciAdapter::RegisterNdefHandler()
{
    DebugLog("TagNciAdapter::RegisterNdefHandler");
    ndefTypeHandle_ = NFA_HANDLE_INVALID;
    NFA_RegisterNDefTypeHandler(true, NFA_TNF_DEFAULT, (uint8_t*)"", 0, NdefCallback);
    if (isLegacyMifareReader_) {
        Extns::GetInstance().EXTNS_MfcRegisterNDefTypeHandler(NdefCallback);
    }
}

tNFA_STATUS TagNciAdapter::Connect(uint32_t idx)
{
    if (idx >= MAX_NUM_TECHNOLOGY) {
        return NFA_STATUS_FAILED;
    }
    if (!IsTagActive()) {
        return NFA_STATUS_FAILED;
    }
    connectedType_ = static_cast<uint32_t>(tagTechList_[idx]);
    connectedProtocol_ = tagRfProtocols_[idx];
    uint32_t discId = tagRfDiscIdList_[idx];
    connectedTechIdx_ = idx;
    InfoLog("TagNciAdapter::Connect: index: %{public}d, discId: %{public}d, "
        "targetProto_: %{public}d, targetType_: %{public}d",
        idx, discId, connectedProtocol_, connectedType_);
#if (NXP_EXTNS == TRUE)
    if (connectedProtocol_ == NFC_PROTOCOL_T3BT) {
        return NFA_STATUS_OK;
    }
#endif
    if (connectedProtocol_ != NFC_PROTOCOL_ISO_DEP && connectedProtocol_ != NFC_PROTOCOL_MIFARE) {
        DebugLog("TagNciAdapter::Connect: do nothing for non ISO_DEP");
        return NFA_STATUS_OK;
    }
    if (connectedType_ == TagHost::TARGET_TYPE_ISO14443_3A || connectedType_ == TagHost::TARGET_TYPE_ISO14443_3B) {
#if (NXP_EXTNS != TRUE)
        if (connectedProtocol_ != NFC_PROTOCOL_MIFARE)
#endif
        {
            DebugLog("TagNciAdapter::Connect: switch rf interface to frame");
            return (Reselect(NFA_INTERFACE_FRAME, true) ? NFA_STATUS_OK : NFA_STATUS_FAILED);
        }
    } else if (connectedType_ == TagHost::TARGET_TYPE_MIFARE_CLASSIC) {
        DebugLog("TagNciAdapter::Connect: switch rf interface to mifare classic");
        return (Reselect(NFA_INTERFACE_MIFARE, true) ? NFA_STATUS_OK : NFA_STATUS_FAILED);
    } else {
        DebugLog("TagNciAdapter::Connect: switch rf interface to ISODEP");
        return (Reselect(NFA_INTERFACE_ISO_DEP, true) ? NFA_STATUS_OK : NFA_STATUS_FAILED);
    }
}

bool TagNciAdapter::Disconnect()
{
    DebugLog("TagNciAdapter::Disconnect");
    rfDiscoveryMutex_.lock();
    tNFA_STATUS status = NFA_Deactivate(false);
    if (status != NFA_STATUS_OK) {
        WarnLog("TagNciAdapter::Disconnect: deactivate failed; error = 0x%{public}X", status);
    }
    connectedProtocol_ = NCI_PROTOCOL_UNKNOWN;
    connectedTechIdx_ = 0;
    connectedType_ = TagHost::TARGET_TYPE_UNKNOWN;
    connectedRfIface_ = NFA_INTERFACE_ISO_DEP;
    isReconnecting_ = false;
    ResetTag();
    rfDiscoveryMutex_.unlock();
    return (status == NFA_STATUS_OK);
}

tNFA_STATUS TagNciAdapter::SendRawFrameForHaltPICC()
{
    tNFA_STATUS status = NFA_STATUS_OK;
    NFC::SynchronizeGuard guardReconnectEvent(reconnectEvent_);
#if (NXP_EXTNS == TRUE)
    // skipped SRD
    if (connectedProtocol_ == NFA_PROTOCOL_T2T ||
        (connectedProtocol_ == NFA_PROTOCOL_ISO_DEP && connectedType_ == TagHost::TARGET_TYPE_ISO14443_3A)) {
        status = NFA_SendRawFrame(RW_TAG_SLP_REQ, sizeof(RW_TAG_SLP_REQ), 0);
        usleep(T2TA_HALT_PICC_TIMEOUT);
    } else if (connectedProtocol_ == NFA_PROTOCOL_ISO_DEP && connectedType_ == TagHost::TARGET_TYPE_ISO14443_3B) {
        uint8_t haltPiccBHead = 0x50;
        uint8_t rawHaltRqB[5] = {haltPiccBHead, nfcID0_[0], nfcID0_[1], nfcID0_[2], nfcID0_[3]};
        isInTransceive_ = true;
        NFC::SynchronizeGuard guardTransceiveEvent(transceiveEvent_);
        status = NFA_SendRawFrame(rawHaltRqB, sizeof(rawHaltRqB), 0);
        if (status != NFA_STATUS_OK) {
            ErrorLog("TagNciAdapter::SendRawFrameForHaltPICC: send rawHaltRqB error= %{public}d", status);
        } else {
            if (transceiveEvent_.Wait(ISO3B_HALT_PICC_TIMEOUT) == false) {
                status = NFA_STATUS_FAILED;
                ErrorLog("TagNciAdapter::SendRawFrameForHaltPICC: send rawHaltRqB timeout");
            }
        }
        isInTransceive_ = false;
    }
#else
    if (connectedProtocol_ == NFA_PROTOCOL_T2T) {
        status = NFA_SendRawFrame(RW_TAG_SLP_REQ, sizeof(RW_TAG_SLP_REQ), 0);
    } else if (connectedProtocol_ == NFA_PROTOCOL_ISO_DEP) {
        status = NFA_SendRawFrame(RW_DESELECT_REQ, sizeof(RW_DESELECT_REQ), 0);
    }
#endif
    reconnectEvent_.Wait(WAIT_TIME_FOR_NO_RSP);
    if (status != NFA_STATUS_OK) {
        ErrorLog("TagNciAdapter::Reselect: halt for iface frame error = %{public}d", status);
    }
    return status;
}

bool TagNciAdapter::Reselect(tNFA_INTF_TYPE rfInterface, bool isSwitchingIface)
{
    InfoLog("TagNciAdapter::Reselect: target interface = %{public}d, connected RfIface_ = %{public}d, "
            "connectedProtocol_ = %{public}d", rfInterface, connectedRfIface_, connectedProtocol_);
    rfDiscoveryMutex_.lock();
    if (isSwitchingIface && (rfInterface == connectedRfIface_)) {
        rfDiscoveryMutex_.unlock();
        return true;
    }
    tNFA_STATUS status = NFA_STATUS_OK;
    do {
        if (isNdefReadTimeOut_) {
            ErrorLog("TagNciAdapter::Reselect: ndef read timeout");
            status = NFA_STATUS_FAILED;
            break;
        }

        // send halt request for interface frame
        if ((connectedRfIface_ == NFA_INTERFACE_FRAME) &&
            (NfccNciAdapter::GetInstance().GetNciVersion() >= NCI_VERSION_2_0)) {
            status = SendRawFrameForHaltPICC();
            if (status != NFA_STATUS_OK) {
                ErrorLog("TagNciAdapter::Reselect: SendRawFrameForHaltPICC error status = %{public}d", status);
                break;
            }
        }

        // deactive to sleep, contains special process for cashbee
        {
            NFC::SynchronizeGuard guard(reconnectEvent_);
            isWaitingDeactRst_ = true;
#if (NXP_EXTNS == TRUE)
            if (isCashbee_) {
                InfoLog("TagNciAdapter::Reselect, Deactivate to IDLE for cashbee");
                status = NFA_StopRfDiscovery();
                if (status != NFA_STATUS_OK) {
                    ErrorLog("TagNciAdapter::Reselect: Deactivate to IDLE for cashbee failed, status = %{public}d",
                        status);
                    break;
                }
            } else {
                // skipped reset state for secureelement field on
#endif
                status = NFA_Deactivate(true);
                if (status != NFA_STATUS_OK) {
                    ErrorLog("TagNciAdapter::Reselect: deactivate to SLEEP failed, status = %{public}d", status);
                    break;
                }
            }
            if (reconnectEvent_.Wait(DEFAULT_TIMEOUT) == false) {
                ErrorLog("TagNciAdapter::Reselect: deactivate to SLEEP timeout");
            }
        }
#if (NXP_EXTNS == TRUE)
        if (tagState_ == IDLE) {
            InfoLog("TagNciAdapter::Reselect: tagState_ is IDLE state");
            if (connectedProtocol_ == NFC_PROTOCOL_ISO_DEP) {
                if (connectedType_ == NFC_DISCOVERY_TYPE_POLL_A) {
                    isCashbee_ = true;
                    InfoLog("TagNciAdapter::Reselect: CashBee Detected");
                }
            }
        }

        if (!isCashbee_) {
#endif
            if (tagState_ != SLEEP) {
                ErrorLog("TagNciAdapter::Reselect, tagState_ is not sleep");
                status = NFA_STATUS_FAILED;
                break;
            }
#if (NXP_EXTNS == TRUE)
        }
#endif
        // do reselect, contains special process for cashbee
        isWaitingDeactRst_ = false;
        {
            NFC::SynchronizeGuard guard(reconnectEvent_);
            isReconnecting_ = true;
            isSwitchingRfIface_ = true;
#if (NXP_EXTNS == TRUE)
            if (isCashbee_) {
                InfoLog("TagNciAdapter::Reselect: Start RF discovery for cash bee");
                if (!isIsoDepDhReqFailed_) {
                    status = NFA_StartRfDiscovery();
                    if (status != NFA_STATUS_OK) {
                        ErrorLog("TagNciAdapter::Reselect: start rf disc for cash bee failed, status = %{public}d",
                            status);
                        break;
                    }
                }
            } else {
#endif
                DebugLog("TagNciAdapter::Reselect:select interface %{public}u", rfInterface);
                status = NFA_Select(tagRfDiscIdList_[connectedTechIdx_],
                                    tagRfProtocols_[connectedTechIdx_],
                                    rfInterface);
                if (status != NFA_STATUS_OK) {
                    ErrorLog("TagNciAdapter::Reselect, NFA_Select failed, status = %{public}d", status);
                    status = NFA_Deactivate(false);
                    if (status != NFA_STATUS_OK) {
                        ErrorLog("TagNciAdapter::Reselect: deactivate failed; status = %{public}d", status);
                    }
                    break;
                }
            }

            // check reconnection
            isReconnected_ = false;
            if (reconnectEvent_.Wait(DEFAULT_TIMEOUT) == false) {
                ErrorLog("TagNciAdapter::Reselect: select timeout");
#if (NXP_EXTNS == TRUE)
                if (!isCashbee_) {
                    status = NFA_Deactivate(false);
                    if (status != NFA_STATUS_OK)
                        ErrorLog("TagNciAdapter::Reselect: deactivate after select timeout failed, "
                                 "status = %{public}d", status);
                }
#endif
                break;
            }
        }

        // Retry in case of Generic error
        int retry = 0;
        if (!isReconnected_) {
            ErrorLog("TagNciAdapter::Reselect retry: waiting for Card to be activated");
            isReconnecting_ = true;
#if (NXP_EXTNS == TRUE)
            if (IsMultiMFCTag() && isMultiTagSupported_) {
                isMultiProtoMFC_ = true;
            } else {
#endif
                do {
                    NFC::SynchronizeGuard guard(reconnectEvent_);
                    if (!reconnectEvent_.Wait(RETRY_RECONNECT_TIMEOUT)) {
                        ErrorLog("TagNciAdapter::Reselect: connect waiting retry timeout");
                    }
                    retry++;
                    ErrorLog("TagNciAdapter::Reselect:connect waiting retry "
                        "cnt = %{public}d, connect succ = %{public}d", retry, isReconnected_);
                } while (isReconnected_ == false && retry < 3);  // 3 represents the number of retries that occur
#if (NXP_EXTNS == TRUE)
            }
            if (discRstEvtNum_) {
                discRstEvtNum_ = 0;
            }
#endif
        }
        InfoLog("TagNciAdapter::Reselect: select completed; isReconnected_= %{public}d", isReconnected_);
        if (tagState_ != ACTIVE) {
            ErrorLog("TagNciAdapter::Reselect: tag is not active");
#if (NXP_EXTNS == TRUE)
            HandleDeactivatedResult(0);
#endif
            status = NFA_STATUS_FAILED;
            if (!isReconnected_ && retry >= 3) { // 3 connect retry times
                AbortWait();
            }
            break;
        }
#if (NXP_EXTNS == TRUE)
        if (isCashbee_) {
            isCashbee_ = false;
        }
#endif
        if (isReconnected_) {
            status = NFA_STATUS_OK; // success
#if (NXP_EXTNS != TRUE)
            connectedRfIface_ = rfInterface;
#endif
        } else {
            status = NFA_STATUS_FAILED;
        }
    } while (0);
    isReconnecting_ = false;
    isWaitingDeactRst_ = false;
    isSwitchingRfIface_ = false;
    rfDiscoveryMutex_.unlock();
    return (status == NFA_STATUS_OK) ? true : false;
}

bool TagNciAdapter::IsReconnecting()
{
    return isReconnecting_;
}

void TagNciAdapter::SetCurrRfInterface(uint32_t rfInterface)
{
    connectedRfIface_ = rfInterface;
}

void TagNciAdapter::SetCurrRfProtocol(uint32_t protocol)
{
    connectedProtocol_ = protocol;
}

void TagNciAdapter::SetCurrRfMode(uint8_t type)
{
    if (type == NFC_DISCOVERY_TYPE_POLL_A || type == NFC_DISCOVERY_TYPE_POLL_A_ACTIVE) {
        connectedType_ = TagHost::TARGET_TYPE_ISO14443_3A;
    } else if (type == NFC_DISCOVERY_TYPE_POLL_B || type == NFC_DISCOVERY_TYPE_POLL_B_PRIME) {
        connectedType_ = TagHost::TARGET_TYPE_ISO14443_3B;
    }
}

void TagNciAdapter::SetNfcID0ForTypeB(uint8_t* nfcID0)
{
    DebugLog("TagNciAdapter::SetNfcID0ForTypeB: nfcID0 = %{public}X%{public}X%{public}X%{public}X",
        nfcID0[0], nfcID0[1], nfcID0[2], nfcID0[3]);
    int nfcId0Len = 4;
    int err = memcpy_s(nfcID0_, nfcId0Len, &nfcID0[0], nfcId0Len);
    if (err != 0) {
        ErrorLog("TagNciAdapter::SetNfcID0ForTypeB: memcpy_s error: %{public}d", err);
    }
}

bool TagNciAdapter::IsMultiMFCTag()
{
    return isMultiTag_ && (connectedProtocol_ == NFC_PROTOCOL_MIFARE);
}

void TagNciAdapter::ClearMultiMFCTagState()
{
    isSkipNdefRead_ = false;
    isMultiProtoMFC_ = false;
    lastTagFoundTime_ = 0;
}

void TagNciAdapter::SetTagActivated()
{
    isNdefReadTimeOut_ = false;
    tagState_ = ACTIVE;
}

void TagNciAdapter::SetTagDeactivated(bool isSleep)
{
    tagState_ = (isSleep ? SLEEP : IDLE);
    isNdefReadTimeOut_ = false;
}

TagNciAdapter::TagState TagNciAdapter::GetTagState()
{
    return tagState_;
}

bool TagNciAdapter::IsSwitchingRfIface()
{
    return isSwitchingRfIface_;
}

bool TagNciAdapter::IsExpectedActRfProtocol(uint32_t protocol)
{
    InfoLog("IsExpectedActRfProtocol: currentProtocol = %{public}d, targetProtocol = %{public}d",
            connectedProtocol_, protocol);
    if (connectedProtocol_ != NFC_PROTOCOL_UNKNOWN && connectedProtocol_ != protocol) {
        NFA_Deactivate(false);
        return false;
    }
    return true;
}

bool TagNciAdapter::Reconnect()
{
    if (tagState_ != ACTIVE) {
        ErrorLog("TagNciAdapter::Reconnect, tag not active");
        return false;
    }
    // return for TARGET_TYPE_KOVIO_BARCODE
    // this is only supported for type 2 or 4 (ISO_DEP) tags
    if (connectedProtocol_ == NFA_PROTOCOL_ISO_DEP) {
        return Reselect(NFA_INTERFACE_ISO_DEP, false);
    } else if (connectedProtocol_ == NFA_PROTOCOL_T2T) {
        return Reselect(NFA_INTERFACE_FRAME, false);
    } else if (connectedProtocol_ == NFC_PROTOCOL_MIFARE) {
        return Reselect(NFA_INTERFACE_MIFARE, false);
    }
    return true;
}

/**
 * See NFC Digital Protocol Technical Specification(2010-11-17)
 * Chapter 9 (Type 2 Tag platform), section 9.6 (READ).
*/
bool TagNciAdapter::IsT2TNackRsp(const uint8_t* response, uint32_t responseLen)
{
    if (responseLen == 1) {
        return (response[0] != T2T_ACK_RESPONSE);
    }
    return false;
}

tNFA_STATUS TagNciAdapter::HandleMfcTransceiveData(std::string& response)
{
    tNFA_STATUS status = NFA_STATUS_FAILED;
    uint32_t len = static_cast<uint32_t>(receivedData_.size());
    uint8_t* data = (uint8_t*)receivedData_.data();
    bool shouldReconnect = false;

    if (isLegacyMifareReader_) {
        status = Extns::GetInstance().EXTNS_CheckMfcResponse(&data, &len);
        ErrorLog("TagNciAdapter::HandleMfcTransceiveData: status: %{public}d, isMfcTransRspErr_: %{public}d",
                 status, isMfcTransRspErr_);
        shouldReconnect = ((status == NFA_STATUS_FAILED) || isMfcTransRspErr_) ? true : false;
    } else {
        shouldReconnect = ((len == 1) && (data[0] != 0x00));
        if (!shouldReconnect) {
            status = NFA_STATUS_OK;
        } else {
            ErrorLog("TagNciAdapter::HandleMfcTransceiveData: data[0] = %{public}d, len = %{public}d",
                data[0], len);
        }
    }

    if (shouldReconnect) {
        Reconnect();
    } else {
        if (len != 0) {
            if (len == MIFARE_RESPONSE_LEN && data[0] != T2T_ACK_RESPONSE && isMfcTransRspErr_) {
                int err = (MIFARE_RESPONSE_LEN << 8) | data[0]; // 8 means offset one byte
                ErrorLog("TagNciAdapter::HandleMfcTransceiveData: rspProtocolErrData: %{public}d", err);
            }
            response = KITS::NfcSdkCommon::BytesVecToHexString(data, len);
            status = NFA_STATUS_OK;
        }
    }
    DebugLog("TagNciAdapter::HandleMfcTransceiveData: status = %{public}d", status);
    return status;
}

int TagNciAdapter::Transceive(const std::string& request, std::string& response)
{
    if (!IsTagActive()) {
        return NFA_STATUS_BUSY;
    }
    tNFA_STATUS status = NFA_STATUS_FAILED;
    isInTransceive_ = true;
    isTransceiveTimeout_ = false;
    bool wait = true;
    do {
        {
            NFC::SynchronizeGuard guard(transceiveEvent_);
            uint16_t length = KITS::NfcSdkCommon::GetHexStrBytesLen(request);
            std::vector<unsigned char> requestInCharVec;
            KITS::NfcSdkCommon::HexStringToBytes(request, requestInCharVec);
            InfoLog("TagNciAdapter::Transceive: requestLen = %{public}d", length);
            receivedData_.clear();
            if (IsMifareConnected() && isLegacyMifareReader_) {
                ErrorLog("TagNciAdapter::Transceive: is mifare");
                status = Extns::GetInstance().EXTNS_MfcTransceive(
                    static_cast<uint8_t *>(requestInCharVec.data()), length);
            } else {
                status = NFA_SendRawFrame(static_cast<uint8_t *>(requestInCharVec.data()),
                    length, NFA_DM_DEFAULT_PRESENCE_CHECK_START_DELAY);
            }
            if (status != NFA_STATUS_OK) {
                ErrorLog("TagNciAdapter::Transceive: fail send; error=%{public}d", status);
                break;
            }
            uint32_t transceiveTimeout = GetTimeout(connectedType_);
            wait = transceiveEvent_.Wait(transceiveTimeout);
        }
        if (!wait || isTransceiveTimeout_) {
            ErrorLog("TagNciAdapter::Transceive: wait response timeout transceiveTimeout: %{public}d,"
                "wait: %{public}d, isTimeout: %{public}d", GetTimeout(connectedType_), wait, isTransceiveTimeout_);
            status = NFA_STATUS_TIMEOUT;
            break;
        }
        if (tagState_ != ACTIVE) {
            ErrorLog("TagNciAdapter::Transceive, tag not active");
            status = NFA_STATUS_FAILED;
            break;
        }
        if (receivedData_.size() > 0) {
            if (connectedProtocol_ == NFA_PROTOCOL_T2T && IsT2TNackRsp(receivedData_.data(), receivedData_.size())) {
                // Do reconnect for mifareUL tag when it responses NACK and enters HALT state
                InfoLog("TagNciAdapter::Transceive:try reconnect for T2T NACK");
                Reconnect();
            } else if (IsMifareConnected() && isLegacyMifareReader_) {
                status = HandleMfcTransceiveData(response);
            } else {
                response = KITS::NfcSdkCommon::BytesVecToHexString(receivedData_.data(), receivedData_.size());
            }
        }
    } while (0);
    isInTransceive_ = false;
    InfoLog("TagNciAdapter::Transceive: exit rsp len = %{public}d", KITS::NfcSdkCommon::GetHexStrBytesLen(response));
    return status;
}

void TagNciAdapter::HandleTranceiveData(uint8_t status, uint8_t* data, uint32_t dataLen)
{
    if (IsMifareConnected() && isLegacyMifareReader_) {
        InfoLog("TagNciAdapter::HandleTranceiveData: is mifare");
        isMfcTransRspErr_ = (dataLen == 2 && data[0] == MIFARE_RESPONSE_LEN && data[1] != T2T_ACK_RESPONSE);
        if (!Extns::GetInstance().EXTNS_GetCallBackFlag()) {
            ErrorLog("TagNciAdapter::HandleTranceiveData: ExtnsGetCallBackFlag is false");
            Extns::GetInstance().EXTNS_MfcCallBack(data, dataLen);
            return;
        }
    }
    if (!isInTransceive_) {
        ErrorLog("TagNciAdapter::HandleTranceiveData: not in transceive");
        return;
    }
    NFC::SynchronizeGuard guard(transceiveEvent_);
    if (status == NFA_STATUS_OK || status == NFA_STATUS_CONTINUE) {
        receivedData_.append(data, dataLen);
    }
    if (status == NFA_STATUS_OK) {
        transceiveEvent_.NotifyOne();
    }
    DebugLog("TagNciAdapter::HandleTranceiveData: status = %{public}d", status);
}

bool TagNciAdapter::IsTagFieldOn()
{
    if (!IsTagActive()) {
        return false;
    }
    if (isInTransceive_ && IsMifareConnected()) {
        return true;
    }
    tNFA_STATUS status = NFA_STATUS_FAILED;
#if (NXP_EXTNS == TRUE)
    if (tagRfProtocols_[0] == NFA_PROTOCOL_T3BT) {
        uint8_t t3btPresenceCheckCmd[] = {0xB2};
        NFC::SynchronizeGuard guard(transceiveEvent_);
        isTransceiveTimeout_ = false;
        isInTransceive_ = true;
        status = NFA_SendRawFrame(t3btPresenceCheckCmd, sizeof(t3btPresenceCheckCmd),
                                                              NFA_DM_DEFAULT_PRESENCE_CHECK_START_DELAY);
        if (status != NFA_STATUS_OK) {
            ErrorLog("TagNciAdapter::IsTagFieldOn, send t3bt cmd failed; status = %{public}d, "
                "continue to do normal presence check", status);
        } else {
            if (!transceiveEvent_.Wait(GetTimeout(TagHost::TARGET_TYPE_ISO14443_3B)) || isTransceiveTimeout_) {
                isTagFieldOn_ = false;
                ErrorLog("TagNciAdapter::IsTagFieldOn, send t3bt cmd timeout");
            } else {
                isTagFieldOn_ = true;
            }
            isInTransceive_ = false;
            return isTagFieldOn_;
        }
        isInTransceive_ = false;
    }
#else
    if (IsMifareConnected() && isLegacyMifareReader_) {
        ErrorLog("TagNciAdapter::IsTagFieldOn: is mifare");
        status = Extns::GetInstance().EXTNS_MfcPresenceCheck();
        if (status == NFA_STATUS_OK) {
            status = Extns::GetInstance().EXTNS_GetPresenceCheckStatus();
        }
        isTagFieldOn_ = (status == NFA_STATUS_OK);
        rfDiscoveryMutex_.unlock();
        return isTagFieldOn_;
    }
#endif
    {
        NFC::SynchronizeGuard guard(fieldCheckEvent_);
        tNFA_STATUS status = NFA_RwPresenceCheck(presChkOption_);
        if (status == NFA_STATUS_OK) {
            if (fieldCheckEvent_.Wait(DEFAULT_TIMEOUT) == false) {
                ErrorLog("field on check timeout...");
                isTagFieldOn_ = false;
            }
        }
    }
    return isTagFieldOn_;
}

void TagNciAdapter::HandleFieldCheckResult(uint8_t status)
{
    NFC::SynchronizeGuard guard(fieldCheckEvent_);
    isTagFieldOn_ = (status == NFA_STATUS_OK);
    fieldCheckEvent_.NotifyOne();
}

bool TagNciAdapter::IsTagDeactivating()
{
    return isWaitingDeactRst_;
}

void TagNciAdapter::HandleSelectResult(uint8_t status)
{
    DebugLog("TagNciAdapter::HandleSelectResult");
    {
        NFC::SynchronizeGuard guard(selectEvent_);
        selectEvent_.NotifyOne();
    }
    if (status != NFA_STATUS_OK) {
        if (isSwitchingRfIface_) {
            SetConnectStatus(false);
        }
        ErrorLog("TagNciAdapter::HandleSelectResult error: %{public}d", status);
        NFA_Deactivate(false);
    }
}

void TagNciAdapter::ClearNonStdTagData()
{
    InfoLog("ClearNonStdTagData");
    errno_t err = memset_s(&g_multiTagParams, sizeof(g_multiTagParams), 0, sizeof(g_multiTagParams));
    if (err != EOK) {
        ErrorLog("TagNciAdapter::ClearNonStdTagData:memset_s for g_multiTagParams error: %{public}d", err);
    }
}

void TagNciAdapter::SetNonStdTagData()
{
    // skipped detecte time calculation
    tNFC_RESULT_DEVT& info = g_multiTagParams.discNtf;
    info.rf_disc_id = tagRfDiscIdList_[selectedTagIdx_];
    info.protocol = tagRfProtocols_[selectedTagIdx_];
    InfoLog("SetNonStdTagData: disc id: %{public}d", info.rf_disc_id);
}

void TagNciAdapter::HandleActivatedResult(tNFA_CONN_EVT_DATA* eventData)
{
    if (eventData->activated.activate_ntf.rf_tech_param.mode >= NCI_DISCOVERY_TYPE_LISTEN_A || //not poll mode
        eventData->activated.activate_ntf.intf_param.type == NFC_INTERFACE_EE_DIRECT_RF) {     // is EE direct rf
        return;
    }
    tNFA_ACTIVATED& activated = eventData->activated;
#if (NXP_EXTNS == TRUE)
    if (isMultiTag_) {
        InfoLog("TagNciAdapter::HandleActivatedResult: copy nonstd tag data");
        ClearNonStdTagData();
        errno_t err = EOK;
        err = memcpy_s(&g_multiTagParams.discNtf.rf_tech_param, sizeof(tNFC_RF_TECH_PARAMS),
                       &activated.activate_ntf.rf_tech_param, sizeof(tNFC_RF_TECH_PARAMS));
        if (err != EOK) {
            ErrorLog("TagNciAdapter::HandleActivatedResult, memcpy rf_tech_param error: %{public}d", err);
        }
        err = memcpy_s(&g_multiTagParams.intfParam, sizeof(tNFC_INTF_PARAMS),
                       &activated.activate_ntf.intf_param, sizeof(tNFC_INTF_PARAMS));
        if (err != EOK) {
            ErrorLog("TagNciAdapter::HandleActivatedResult, memcpy intfParam error: %{public}d", err);
        }
    }
#endif
    // skipped  same kovio detection
    connectedProtocol_ = activated.activate_ntf.protocol;
    t1tMaxMessageSize_ = GetT1tMaxMessageSize(activated);
    GetTechFromData(activated);
    BuildTagInfo(activated);
}

void TagNciAdapter::SetConnectStatus(bool isStatusOk)
{
    DebugLog("TagNciAdapter::SetConnectStatus");
    if (Extns::GetInstance().EXTNS_GetConnectFlag() && isLegacyMifareReader_) {
        DebugLog("TagNciAdapter::SetConnectStatus: ExtnsMfcActivated");
        Extns::GetInstance().EXTNS_MfcActivated();
        Extns::GetInstance().EXTNS_SetConnectFlag(false);
    }
    if (isReconnecting_) {
        isReconnected_ = isStatusOk;
        isReconnecting_ = false;
        NFC::SynchronizeGuard guard(reconnectEvent_);
        reconnectEvent_.NotifyOne();
    }
}

void TagNciAdapter::HandleDeactivatedResult(tNFA_DEACTIVATE_TYPE deactType)
{
    DebugLog("TagNciAdapter::HandleDeactivatedResult");
    connectedProtocol_ = NFC_PROTOCOL_UNKNOWN;
#if (NXP_EXTNS == TRUE)
    if (deactType == NFA_DEACTIVATE_TYPE_DISCOVERY) {
        // clear Activation Params
    }
#endif
    ResetTag();
}

void TagNciAdapter::SetDeactivatedStatus()
{
    if (Extns::GetInstance().EXTNS_GetDeactivateFlag() &&
        isLegacyMifareReader_) {
        DebugLog("TagNciAdapter::SetDeactivatedStatus mifare deactivate");
        Extns::GetInstance().EXTNS_MfcDisconnect();
        Extns::GetInstance().EXTNS_SetDeactivateFlag(false);
    }
    {
        NFC::SynchronizeGuard guard(reconnectEvent_);
        reconnectEvent_.NotifyOne();
    }
}

void TagNciAdapter::ResetTagFieldOnFlag()
{
    DebugLog("TagNciAdapter::ResetTagFieldOnFlag");
    isTagFieldOn_ = true;
}

void TagNciAdapter::SetTimeout(const uint32_t timeout, const uint32_t technology)
{
    DebugLog("SetTimeout timeout: %{public}d, tech: %{public}d", timeout, technology);
    if (technology > 0 && technology <= MAX_NUM_TECHNOLOGY) {
        technologyTimeoutsTable_[technology] = timeout;
    } else {
        WarnLog("TagNciAdapter::SetTimeout, Unknown technology");
    }
}

uint32_t TagNciAdapter::GetTimeout(uint32_t technology) const
{
    uint32_t timeout = DEFAULT_TIMEOUT;
    if (technology > 0 && technology <= MAX_NUM_TECHNOLOGY) {
        timeout = technologyTimeoutsTable_[technology];
    } else {
        WarnLog("TagNciAdapter::GetTimeout, Unknown technology");
    }
    return timeout;
}

void TagNciAdapter::ResetTimeout()
{
    technologyTimeoutsTable_[TagHost::TARGET_TYPE_ISO14443_3A] = ISO14443_3A_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TagHost::TARGET_TYPE_ISO14443_3B] = ISO14443_3B_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TagHost::TARGET_TYPE_ISO14443_4] = ISO14443_4_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TagHost::TARGET_TYPE_FELICA] = FELICA_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TagHost::TARGET_TYPE_V] = ISO15693_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TagHost::TARGET_TYPE_NDEF] = NDEF_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TagHost::TARGET_TYPE_NDEF_FORMATABLE] = NDEF_FORMATABLE_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TagHost::TARGET_TYPE_MIFARE_CLASSIC] = MIFARE_CLASSIC_DEFAULT_TIMEOUT;
    technologyTimeoutsTable_[TagHost::TARGET_TYPE_MIFARE_UL] = MIFARE_UL_DEFAULT_TIMEOUT;
}

bool TagNciAdapter::SetReadOnly() const
{
    DebugLog("TagNciAdapter::SetReadOnly");
    uint8_t status = NFA_RwSetTagReadOnly(true);
    if (status == NCI_STATUS_REJECTED) {
        status = NFA_RwSetTagReadOnly(false);
        if (status != NCI_STATUS_OK) {
            return false;
        }
    } else if (status != NCI_STATUS_OK) {
        return false;
    }
    return true;
}

void TagNciAdapter::HandleSetReadOnlyResult(tNFA_STATUS status)
{
    NFC::SynchronizeGuard guard(setReadOnlyEvent_);
    setReadOnlyEvent_.NotifyOne();
}

void TagNciAdapter::ReadNdef(std::string& response)
{
    DebugLog("TagNciAdapter::ReadNdef");
    if (!IsTagActive()) {
        ErrorLog("ReadNdef, IsTagActive failed");
        return;
    }
    readNdefData_ = "";
    if (lastCheckedNdefSize_ > 0) {
        {
            NFC::SynchronizeGuard guard(readNdefEvent_);
            isNdefReading_ = true;
            tNFA_STATUS status = NFA_STATUS_FAILED;
            if (IsMifareConnected() && isLegacyMifareReader_) {
                status = Extns::GetInstance().EXTNS_MfcReadNDef();
            } else {
                status = NFA_RwReadNDef();
            }
            if (status != NFA_STATUS_OK) {
                isNdefReading_ = false;
                return;
            }
            if (!readNdefEvent_.Wait(READ_NDEF_TIMEOUT)) { // NFA_READ_CPLT_EVT will notify this
                ErrorLog("TagNciAdapter::ReadNdef, readNdefEvent_ timeout!");
                isNdefReading_ = false;
                return;
            }
        }
        isNdefReading_ = false;
        if (KITS::NfcSdkCommon::GetHexStrBytesLen(readNdefData_) > 0) {
            response = readNdefData_;
        }
    }
    rfDiscoveryMutex_.unlock();
    return;
}

void TagNciAdapter::HandleReadComplete(uint8_t status)
{
    DebugLog("TagNciAdapter::HandleReadComplete, isNdefReading_ = %{public}d", isNdefReading_);
    if (!isNdefReading_) {
        return;
    }
    NFC::SynchronizeGuard guard(readNdefEvent_);
    if (status != NFA_STATUS_OK) {
        ErrorLog("Read ndef fail");
        isNdefReadTimeOut_ = true;
        readNdefData_ = "";
    }
    readNdefEvent_.NotifyOne();
}

bool TagNciAdapter::WriteNdef(std::string& ndefMessage)
{
    DebugLog("TagNciAdapter::WriteNdef");
    if (!IsTagActive()) {
        ErrorLog("WriteNdef, IsTagActive failed");
        return false;
    }
    rfDiscoveryMutex_.lock();
    isNdefWriteSuccess_ = false;
    tNFA_STATUS status = NFA_STATUS_FAILED;
    const uint32_t maxBufferSize = 1024;
    uint8_t buffer[maxBufferSize] = {0};
    uint32_t curDataSize = 0;
    NFC::SynchronizeGuard guard(writeNdefEvent_);
    uint32_t length = KITS::NfcSdkCommon::GetHexStrBytesLen(ndefMessage);
    uint8_t data[length];
    for (uint32_t i = 0; i < length; i++) {
        data[i] = KITS::NfcSdkCommon::GetByteFromHexStr(ndefMessage, i);
    }
    if (lastNdefCheckedStatus_ == NFA_STATUS_FAILED) {
        if (isNdefCapable_) {
            DebugLog("Format ndef first");
            this->FormatNdef();
        }
        status = NFA_RwWriteNDef(data, length);
    } else if (length == 0) {
        DebugLog("Create and write an empty ndef message");
        NDEF_MsgInit(buffer, maxBufferSize, &curDataSize);
        NDEF_MsgAddRec(buffer, maxBufferSize, &curDataSize, NDEF_TNF_EMPTY, NULL, 0, NULL, 0, NULL, 0);
        status = NFA_RwWriteNDef(buffer, curDataSize);
    } else {
        status = NFA_RwWriteNDef(data, length);
    }

    if (status == NCI_STATUS_OK) {
        writeNdefEvent_.Wait();
    } else {
        ErrorLog("WriteNdef, Write ndef fail");
    }
    rfDiscoveryMutex_.unlock();
    return isNdefWriteSuccess_;
}

void TagNciAdapter::HandleWriteComplete(uint8_t status)
{
    DebugLog("TagNciAdapter::HandleWriteComplete");
    NFC::SynchronizeGuard guard(writeNdefEvent_);
    isNdefWriteSuccess_ = (status == NFA_STATUS_OK);
    writeNdefEvent_.NotifyOne();
}

bool TagNciAdapter::FormatNdef()
{
    DebugLog("TagNciAdapter::FormatNdef");
    if (!IsTagActive()) {
        return false;
    }
    NFC::SynchronizeGuard guard(formatNdefEvent_);
    isNdefFormatSuccess_ = false;
    tNFA_STATUS status = NFA_RwFormatTag();
    if (status == NFA_STATUS_OK) {
        formatNdefEvent_.Wait();
        if (!isNdefFormatSuccess_) {
            status = NFA_STATUS_FAILED;
        }
    } else {
        ErrorLog("Format Ndef error, status= %{public}d", status);
    }
    return (status == NFA_STATUS_OK);
}

void TagNciAdapter::HandleFormatComplete(uint8_t status)
{
    DebugLog("TagNciAdapter::HandleFormatComplete");
    NFC::SynchronizeGuard guard(formatNdefEvent_);
    isNdefFormatSuccess_ = (status == NFA_STATUS_OK);
    formatNdefEvent_.NotifyOne();
}

bool TagNciAdapter::IsNdefFormatable()
{
    DebugLog("TagNciAdapter::IsNdefFormatable");
    return isNdefFormatSuccess_;
}

bool TagNciAdapter::DetectNdefInfo(std::vector<int>& ndefInfo)
{
    DebugLog("TagNciAdapter::DetectNdefInfo");
    if (!IsTagActive()) {
        return false;
    }
    // special for MULTI MFC and t3tB
    if (connectedProtocol_ == NFA_PROTOCOL_T3BT || (isMultiProtoMFC_ && isSkipNdefRead_)) {
        ClearMultiMFCTagState();
        ndefInfo.push_back(0);
        ndefInfo.push_back(NDEF_MODE_READ_ONLY);
        InfoLog("TagNciAdapter:: ReadNdef, skip ndef read for NFA_PROTOCOL_T3BT or MultiProtoMFC");
        return false;
    }

    // special for kovio

    // special for mifare
    if (connectedProtocol_ == NFC_PROTOCOL_MIFARE) {
        Reconnect();
    }
    rfDiscoveryMutex_.lock();
    NFC::SynchronizeGuard guard(checkNdefEvent_);
    tNFA_STATUS status = NFA_STATUS_FAILED;
    isNdefChecking_ = true;
    if (IsMifareConnected() && isLegacyMifareReader_) {
        status = Extns::GetInstance().EXTNS_MfcCheckNDef();
    } else {
        status = NFA_RwDetectNDef();
    }
    if (status != NFA_STATUS_OK) {
        ErrorLog("NFA_RwDetectNDef failed, status: %{public}d", status);
        rfDiscoveryMutex_.unlock();
        return false;
    }
    if (checkNdefEvent_.Wait(CHECK_NDEF_TIMEOUT) == false) {
        ErrorLog("TagNciAdapter::DetectNdefInfo time out");
        rfDiscoveryMutex_.unlock();
        return false;
    }

    if (isNdefCapable_) {
        if (connectedProtocol_ == NFA_PROTOCOL_T1T) {
            ndefInfo.push_back(t1tMaxMessageSize_);
        } else {
            ndefInfo.push_back(lastCheckedNdefMaxSize_);
        }
        ndefInfo.push_back(lastCheckedNdefMode_);
    }
    rfDiscoveryMutex_.unlock();

    if (connectedProtocol_ == NFC_PROTOCOL_MIFARE) {
        Reconnect();
    }
    return isNdefCapable_;
}

void TagNciAdapter::HandleNdefCheckResult(uint8_t status, uint32_t currentSize, uint32_t flag, uint32_t maxSize)
{
    DebugLog("TagNciAdapter::HandleNdefCheckResult");
    auto uFlag = static_cast<uint8_t>(flag & 0xFF);
    if (uFlag & RW_NDEF_FL_FORMATED) {
        DebugLog("Ndef check: Tag formated for NDEF");
    }
    if (uFlag & RW_NDEF_FL_SUPPORTED) {
        DebugLog("Ndef check: NDEF supported by the tag");
    }
    if (uFlag & RW_NDEF_FL_UNKNOWN) {
        DebugLog("Ndef check: Unable to find if tag is ndef capable/formated/read only");
    }
    if (uFlag & RW_NDEF_FL_FORMATABLE) {
        DebugLog("Ndef check: Tag supports format operation");
    }
    NFC::SynchronizeGuard guard(checkNdefEvent_);
    if (uFlag & RW_NDEF_FL_READ_ONLY) {
        DebugLog("Ndef check: Tag is read only");
        lastCheckedNdefMode_ = NDEF_MODE_READ_ONLY;
    } else {
        lastCheckedNdefMode_ = NDEF_MODE_READ_WRITE;
    }

    lastNdefCheckedStatus_ = status;
    if (lastNdefCheckedStatus_ != NFA_STATUS_OK && lastNdefCheckedStatus_ != NFA_STATUS_TIMEOUT) {
        lastNdefCheckedStatus_ = NFA_STATUS_FAILED;
        isNdefCapable_ = false;
    }

    isNdefCapable_ = false;
    if (lastNdefCheckedStatus_ == NFA_STATUS_OK) {
        lastCheckedNdefSize_ = currentSize;
        lastCheckedNdefMaxSize_ = maxSize;
        isNdefCapable_ = true;
    } else if (lastNdefCheckedStatus_ == NFA_STATUS_FAILED) {
        lastCheckedNdefSize_ = 0;
        lastCheckedNdefMaxSize_ = 0;
        if ((uFlag & RW_NDEF_FL_SUPPORTED) && ((uFlag & RW_NDEF_FL_UNKNOWN) == 0)) {
            DebugLog("Tag is ndef capable");
            isNdefCapable_ = true;
        }
    } else {
        lastCheckedNdefSize_ = 0;
        lastCheckedNdefMaxSize_ = 0;
    }
    checkNdefEvent_.NotifyOne();
}

bool TagNciAdapter::IsDiscTypeA(uint8_t discType) const
{
    if (discType == NCI_DISCOVERY_TYPE_POLL_A) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_POLL_A_ACTIVE) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_LISTEN_A) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_LISTEN_A_ACTIVE) {
        return true;
    }
    return false;
}

bool TagNciAdapter::IsDiscTypeB(uint8_t discType) const
{
    if (discType == NCI_DISCOVERY_TYPE_POLL_B) {
        return true;
    }
    if (discType == NFC_DISCOVERY_TYPE_POLL_B_PRIME) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_LISTEN_B) {
        return true;
    }
    if (discType == NFC_DISCOVERY_TYPE_LISTEN_B_PRIME) {
        return true;
    }
    return false;
}

bool TagNciAdapter::IsDiscTypeF(uint8_t discType) const
{
    if (discType == NCI_DISCOVERY_TYPE_POLL_F) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_POLL_F_ACTIVE) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_LISTEN_F) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_LISTEN_F_ACTIVE) {
        return true;
    }
    return false;
}

bool TagNciAdapter::IsDiscTypeV(uint8_t discType) const
{
    if (discType == NCI_DISCOVERY_TYPE_POLL_V) {
        return true;
    }
    if (discType == NCI_DISCOVERY_TYPE_LISTEN_ISO15693) {
        return true;
    }
    return false;
}

void TagNciAdapter::GetTechFromData(tNFA_ACTIVATED activated)
{
    uint32_t tech[MAX_NUM_TECHNOLOGY];
    if (activated.activate_ntf.protocol == NCI_PROTOCOL_T1T) {
        tech[techListIndex_] = TagHost::TARGET_TYPE_ISO14443_3A;
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_T2T) {
        tech[techListIndex_] = TagHost::TARGET_TYPE_ISO14443_3A;
        // can also be mifare
        if (activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] == MANUFACTURER_ID_NXP &&
            (activated.activate_ntf.rf_tech_param.param.pa.sel_rsp == SAK_MIFARE_UL_1 ||
            activated.activate_ntf.rf_tech_param.param.pa.sel_rsp == SAK_MIFARE_UL_2)) {
            InfoLog("TagNciAdapter::GetTechFromData: MifareUltralight");
            techListIndex_++;
            tech[techListIndex_] = TagHost::TARGET_TYPE_MIFARE_UL;
        }
    }
#if (NXP_EXTNS == TRUE)
    else if (activated.activate_ntf.protocol == NCI_PROTOCOL_T3BT) {
        tech[techListIndex_] = TagHost::TARGET_TYPE_ISO14443_3B;
    }
#endif
    else if (activated.activate_ntf.protocol == NCI_PROTOCOL_T3T) {
        tech[techListIndex_] = TagHost::TARGET_TYPE_FELICA;
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_ISO_DEP) {
        tech[techListIndex_] = TagHost::TARGET_TYPE_ISO14443_4;
        if ((activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) ||
            (activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A_ACTIVE)) {
            // get frame Waiting time Integer(fwi) from activated data
            uint8_t fwi = activated.activate_ntf.intf_param.intf_param.pa_iso.fwi;
            if (fwi >= MIN_FWI && fwi <= MAX_FWI) {
                // 2^MIN_FWI * 256 * 16 * 1000 / 13560000 is approximately 618
                int fwt = (1 << (fwi - MIN_FWI)) * 618;
                InfoLog("TagNciAdapter::GetTechFromData timeout = %{public}d, fwi = %{public}0#x", fwt, fwi);
                SetTimeout(fwt, tech[techListIndex_]);
            }
        }
        // A OR B
        uint8_t discType = activated.activate_ntf.rf_tech_param.mode;
        if (IsDiscTypeA(discType)) {
            techListIndex_++;
            tech[techListIndex_] = TagHost::TARGET_TYPE_ISO14443_3A;
        } else if (IsDiscTypeB(discType)) {
            techListIndex_++;
            tech[techListIndex_] = TagHost::TARGET_TYPE_ISO14443_3B;
        }
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_15693) {
        tech[techListIndex_] = TagHost::TARGET_TYPE_V;
    } else if (activated.activate_ntf.protocol == NFC_PROTOCOL_MIFARE) {
        InfoLog("TagNciAdapter::GetTechFromData: MifareClassic");
        Extns::GetInstance().EXTNS_MfcInit(activated);
        tech[techListIndex_] = TagHost::TARGET_TYPE_ISO14443_3A;

        techListIndex_++;
        tech[techListIndex_] = TagHost::TARGET_TYPE_MIFARE_CLASSIC;
    } else {
        tech[techListIndex_] = TagHost::TARGET_TYPE_UNKNOWN;
    }
    techListIndex_++;

    uint32_t tagRfDiscId = activated.activate_ntf.rf_disc_id;
    uint32_t tagNtfProtocol = activated.activate_ntf.protocol;
    for (uint32_t i = multiTagTmpTechIdx_; i < techListIndex_; i++) {
        tagTechList_.push_back(tech[i]);
        tagRfDiscIdList_.push_back(tagRfDiscId);
        tagRfProtocols_.push_back(tagNtfProtocol);
        InfoLog("GetTechFromData: index = %{public}d, tech = %{public}d, RfDiscId = %{public}d, protocol = %{public}d",
                i, tech[i], tagRfDiscId, tagNtfProtocol);
    }
}

std::string TagNciAdapter::GetUidFromData(tNFA_ACTIVATED activated) const
{
    std::string uid;
    tNFC_RF_TECH_PARAMS nfcRfTechParams = activated.activate_ntf.rf_tech_param;
    uint8_t discType = nfcRfTechParams.mode;
    if (IsDiscTypeA(discType)) {
        uint32_t nfcid1Len = nfcRfTechParams.param.pa.nfcid1_len;
        uid = KITS::NfcSdkCommon::BytesVecToHexString(nfcRfTechParams.param.pa.nfcid1, nfcid1Len);
    } else if (IsDiscTypeB(discType)) {
        if (activated.activate_ntf.protocol == NFA_PROTOCOL_T3BT) {
            uid = KITS::NfcSdkCommon::BytesVecToHexString(nfcRfTechParams.param.pb.pupiid, NFC_PUPIID_MAX_LEN);
        } else {
            uid = KITS::NfcSdkCommon::BytesVecToHexString(nfcRfTechParams.param.pb.nfcid0, NFC_NFCID0_MAX_LEN);
        }
    } else if (IsDiscTypeF(discType)) {
        uid = KITS::NfcSdkCommon::BytesVecToHexString(nfcRfTechParams.param.pf.nfcid2, NFC_NFCID2_LEN);
    } else if (IsDiscTypeV(discType)) {
        uint8_t* i93Uid = activated.params.i93.uid;
        uint8_t i93UidReverse[I93_UID_BYTE_LEN];
        for (uint32_t i = 0; i < I93_UID_BYTE_LEN; i++) {
            i93UidReverse[i] = i93Uid[I93_UID_BYTE_LEN - i - 1];
        }
        uid = KITS::NfcSdkCommon::BytesVecToHexString(i93UidReverse, I93_UID_BYTE_LEN);
    } else {
        uid = "";
    }
    return uid;
}

std::string TagNciAdapter::GetTechPollForTypeB(tNFC_RF_TECH_PARAMS nfcRfTechParams, uint32_t tech)
{
    std::string techPoll = "";
    if (tech == TagHost::TARGET_TYPE_ISO14443_3B) {
        uint32_t length = nfcRfTechParams.param.pb.sensb_res_len;
        if (length > NFC_NFCID0_MAX_LEN) {
            length = length - NFC_NFCID0_MAX_LEN;
        } else {
            WarnLog("sensb_res_len %{public}d error", length);
            length = 0;
        }
        techPoll = KITS::NfcSdkCommon::BytesVecToHexString(
            nfcRfTechParams.param.pb.sensb_res + SENSB_RES_POLL_POS, length);
    }
    return techPoll;
}

void TagNciAdapter::GetTechPollFromData(tNFA_ACTIVATED activated)
{
    std::string techPoll = "";
    tNFC_RF_TECH_PARAMS nfcRfTechParams = activated.activate_ntf.rf_tech_param;
    uint8_t discType = nfcRfTechParams.mode;
    for (uint32_t i = multiTagTmpTechIdx_; i < techListIndex_; i++) {
        if (IsDiscTypeA(discType)) {
            techPoll = KITS::NfcSdkCommon::BytesVecToHexString(
                nfcRfTechParams.param.pa.sens_res, SENS_RES_LENGTH);
        } else if (IsDiscTypeB(discType)) {
            techPoll = GetTechPollForTypeB(nfcRfTechParams, tagTechList_[i]);
        } else if (IsDiscTypeF(discType)) {
            uint8_t fTechPoll[F_POLL_LENGTH];
            uint8_t *sensfRes = nfcRfTechParams.param.pf.sensf_res;

            // save the pmm value.
            for (uint32_t j = 0; j < SENSF_RES_LENGTH; j++) {
                fTechPoll[j] = static_cast<uint8_t>(sensfRes[j + SENSF_RES_LENGTH]);
            }

            // save the system code.
            if (activated.params.t3t.num_system_codes > 0) {
                unsigned short *pSystemCodes = activated.params.t3t.p_system_codes;
                fTechPoll[POS_NFCF_STSTEM_CODE_HIGH] =
                    static_cast<uint8_t>(*pSystemCodes >> SYSTEM_CODE_SHIFT);
                fTechPoll[POS_NFCF_STSTEM_CODE_LOW] = static_cast<uint8_t>(*pSystemCodes);
            }
            techPoll = KITS::NfcSdkCommon::BytesVecToHexString(fTechPoll, F_POLL_LENGTH);
        } else if (IsDiscTypeV(discType)) {
            uint8_t vTechPoll[2] = {activated.params.i93.afi, activated.params.i93.dsfid};
            techPoll = KITS::NfcSdkCommon::BytesVecToHexString(vTechPoll, I93_POLL_LENGTH);
        } else {
            techPoll = "";
        }
        tagPollBytes_.push_back(techPoll);
    }
}

std::string TagNciAdapter::GetTechActForIsoDep(tNFA_ACTIVATED activated,
                                               tNFC_RF_TECH_PARAMS nfcRfTechParams,
                                               uint32_t tech) const
{
    std::string techAct = "";
    if (tech == TagHost::TARGET_TYPE_ISO14443_4) {
        uint8_t discType = nfcRfTechParams.mode;
        if (IsDiscTypeA(discType)) {
            if (activated.activate_ntf.intf_param.type == NFC_INTERFACE_ISO_DEP) {
                tNFC_INTF_PA_ISO_DEP paIso = activated.activate_ntf.intf_param.intf_param.pa_iso;
                techAct = (paIso.his_byte_len > 0) ? KITS::NfcSdkCommon::BytesVecToHexString(
                    paIso.his_byte, paIso.his_byte_len) : "";
            }
        } else if (IsDiscTypeB(discType)) {
            if (activated.activate_ntf.intf_param.type == NFC_INTERFACE_ISO_DEP) {
                tNFC_INTF_PB_ISO_DEP pbIso = activated.activate_ntf.intf_param.intf_param.pb_iso;
                techAct = (pbIso.hi_info_len > 0) ? KITS::NfcSdkCommon::BytesVecToHexString(
                    pbIso.hi_info, pbIso.hi_info_len) : "";
            }
        }
    } else if (tech == TagHost::TARGET_TYPE_ISO14443_3A) {
        techAct = KITS::NfcSdkCommon::UnsignedCharToHexString(nfcRfTechParams.param.pa.sel_rsp);
    } else {
        // do nothing
    }
    return techAct;
}

void TagNciAdapter::GetTechActFromData(tNFA_ACTIVATED activated)
{
    uint8_t protocol = activated.activate_ntf.protocol;
    tNFC_RF_TECH_PARAMS nfcRfTechParams = activated.activate_ntf.rf_tech_param;
    for (uint32_t i = multiTagTmpTechIdx_; i < techListIndex_; i++) {
        std::string techAct = "";
        if (protocol == NCI_PROTOCOL_T1T) {
            techAct = KITS::NfcSdkCommon::UnsignedCharToHexString(nfcRfTechParams.param.pa.sel_rsp);
        } else if (protocol == NCI_PROTOCOL_T2T) {
            techAct = KITS::NfcSdkCommon::UnsignedCharToHexString(nfcRfTechParams.param.pa.sel_rsp);
        } else if (protocol == NCI_PROTOCOL_T3T) {
            techAct = "";
        } else if (protocol == NCI_PROTOCOL_ISO_DEP) {
            techAct = GetTechActForIsoDep(activated, nfcRfTechParams, tagTechList_[i]);
        } else if (protocol == NCI_PROTOCOL_15693) {
            uint8_t techActivated[2] = {activated.params.i93.afi, activated.params.i93.dsfid};
            techAct = KITS::NfcSdkCommon::BytesVecToHexString(techActivated, I93_ACT_LENGTH);
        } else if (protocol == NFC_PROTOCOL_MIFARE) {
            techAct = KITS::NfcSdkCommon::UnsignedCharToHexString(nfcRfTechParams.param.pa.sel_rsp);
        } else {
            // do nothing
        }
        tagActivatedBytes_.push_back(techAct);
    }
}

void TagNciAdapter::ParseSpecTagType(tNFA_ACTIVATED activated)
{
    // parse for FelicaLite
    if (activated.activate_ntf.protocol == NFC_PROTOCOL_T3T) {
        uint32_t i = 0;
        while (i < activated.params.t3t.num_system_codes) {
            if (activated.params.t3t.p_system_codes[i++] == T3T_SYSTEM_CODE_FELICA_LITE) {
                isFelicaLite_ = true;
                break;
            }
        }
    }
    // parse for MifareUltralight, NFC Digital Protocol, see SENS_RES and SEL_RES
    if (activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) {
        if ((activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] == ATQA_MIFARE_UL_0) &&
            (activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] == ATQA_MIFARE_UL_1) &&
            ((activated.activate_ntf.rf_tech_param.param.pa.sel_rsp == SAK_MIFARE_UL_1) ||
            (activated.activate_ntf.rf_tech_param.param.pa.sel_rsp == SAK_MIFARE_UL_2)) &&
            (activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] == MANUFACTURER_ID_NXP)) {
            isMifareUltralight_ = true;
        }
    }

    // parse for MifareDESFire, one sak byte and 2 ATQA bytes
    if ((activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) ||
        (activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_LISTEN_A) ||
        (activated.activate_ntf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_LISTEN_A_ACTIVE)) {
        if ((activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] == ATQA_MIFARE_DESFIRE_0) &&
            (activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] == ATQA_MIFARE_DESFIRE_1) &&
            (activated.activate_ntf.rf_tech_param.param.pa.sel_rsp == SAK_MIFARE_DESFIRE)) {
            isMifareDESFire_ = true;
        }
    }
    InfoLog("isFelicaLite_ = %{public}d, isMifareUltralight_ = %{public}d, isMifareDESFire_ = %{public}d",
        isFelicaLite_, isMifareUltralight_, isMifareDESFire_);
}

void TagNciAdapter::BuildTagInfo(tNFA_ACTIVATED activated)
{
    DebugLog("TagNciAdapter::BuildTagInfo, discRstEvtNum_ = %{public}d", discRstEvtNum_);
    std::string tagUid = GetUidFromData(activated);
    GetTechPollFromData(activated);
    GetTechActFromData(activated);
    ParseSpecTagType(activated);

    if (discRstEvtNum_ == 0) {
        multiTagTmpTechIdx_ = 0;
        std::shared_ptr<NCI::TagHost> tagHost = std::make_shared<NCI::TagHost>(tagTechList_,
            tagRfDiscIdList_, tagRfProtocols_, tagUid, tagPollBytes_, tagActivatedBytes_,
            connectedTechIdx_);
        TagNativeImpl::GetInstance().OnTagDiscovered(tagRfDiscIdList_[0], tagHost);
    } else {
        multiTagTmpTechIdx_ = techListIndex_;
        InfoLog("TagNciAdapter::BuildTagInfo, select next tag if exists");
    }
    InfoLog("TagNciAdapter::BuildTagInfo, multiTagTmpTechIdx_ = %{public}d, techListIndex_ = %{public}d",
        multiTagTmpTechIdx_, techListIndex_);
}

void TagNciAdapter::ResetTag()
{
    DebugLog("TagNciAdapter::ResetTag");
    // tag data
    tagTechList_.clear();
    tagRfDiscIdList_.clear();
    tagRfProtocols_.clear();
    tagPollBytes_.clear();
    tagActivatedBytes_.clear();
    multiTagDiscId_.clear();
    multiTagDiscProtocol_.clear();

    // disc idxes
    techListIndex_ = 0;
    multiTagTmpTechIdx_ = 0;
    discRstEvtNum_ = 0;
    discNtfIndex_ = 0;
    multiTagTmpTechIdx_ = 0;
    selectedTagIdx_ = 0;

    // connection datas
    connectedProtocol_ = NCI_PROTOCOL_UNKNOWN;

    isFelicaLite_ = false;
    isMifareUltralight_ = false;
    isMifareDESFire_ = false;
    isMultiTag_ = false;

    ResetTimeout();

    //  special data
#if (NXP_EXTNS == TRUE)
    Extns::GetInstance().EXTNS_SetConnectFlag(false);
#endif
}

bool TagNciAdapter::IsTagDetectedInTimeDiff(uint32_t timeDiff)
{
    return true;
}

void TagNciAdapter::SetMultiTagData(tNFC_RESULT_DEVT& discNtf)
{
    if (discNtf.rf_tech_param.param.pa.sel_rsp == NON_STD_CARD_SAK) {
        InfoLog("TagNciAdapter::SetMultiTagData: sak 13 tag detechted, set protocol to ISODEP");
        multiTagDiscProtocol_[discRstEvtNum_] = NFC_PROTOCOL_ISO_DEP;
    } else {
        if (discNtf.protocol == NFC_PROTOCOL_MIFARE) {
            if (isMultiProtoMFC_ && IsTagDetectedInTimeDiff(multiTagTimeDiff_[0])) { // 0 for Mifare
                isSkipNdefRead_ = true;
            } else {
                ClearMultiMFCTagState();
            }
        } else if (discNtf.protocol == NFC_PROTOCOL_ISO_DEP) {
            if (isIsoDepDhReqFailed_ && IsTagDetectedInTimeDiff(multiTagTimeDiff_[1])) { // 1 for ISODEP
                g_multiTagParams.isSkipIsoDepAct = true;
            } else {
                ClearMultiMFCTagState();
            }
        } else if (discNtf.more == NCI_DISCOVER_NTF_LAST) {
            bool isMFCDetected = false;
            for (uint32_t i = 0; i < techListIndex_; i++) {
                if (tagRfProtocols_[i] == NFC_PROTOCOL_MIFARE) {
                    isMFCDetected = true;
                }
            }
            if (!isMFCDetected) {
                ClearMultiMFCTagState();
            }
        }
    }
}

void TagNciAdapter::HandleDiscResult(tNFA_CONN_EVT_DATA* eventData)
{
    if (eventData == nullptr) {
        WarnLog("HandleDiscResult invalid eventData.");
        return;
    }
    if (eventData->disc_result.status != NFA_STATUS_OK) {
        ErrorLog("TagNciAdapter::HandleDiscResult, status error: %{public}d", eventData->disc_result.status);
        return;
    }
    tNFC_RESULT_DEVT& discoveryNtf = eventData->disc_result.discovery_ntf;
    DebugLog("TagNciAdapter::HandleDiscResult, discId: %{public}d, protocol: %{public}d, discNtfIndex_: %{public}d",
        discoveryNtf.rf_disc_id, discoveryNtf.protocol, discNtfIndex_);
    uint8_t nfcID2[NCI_NFCID1_MAX_LEN] = {0};
    errno_t err = EOK;

    if (discoveryNtf.rf_disc_id == 1) { // first UID
        (void)memset_s(nfcID1_, sizeof(nfcID1_), 0, sizeof(nfcID1_));
        if (discoveryNtf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) {
            err = memcpy_s(nfcID1_, sizeof(nfcID1_), discoveryNtf.rf_tech_param.param.pa.nfcid1,
                           discoveryNtf.rf_tech_param.param.pa.nfcid1_len);
            if (err != EOK) {
                ErrorLog("TagNciAdapter::HandleDiscResult, memcpy nfcid1 error: %{public}d", err);
            }
        }
    } else if (discoveryNtf.rf_disc_id == 2) {  // 2 represents the second uid
        if (discoveryNtf.rf_tech_param.mode == NFC_DISCOVERY_TYPE_POLL_A) {
            err = memcpy_s(nfcID2, sizeof(nfcID2), discoveryNtf.rf_tech_param.param.pa.nfcid1,
                           discoveryNtf.rf_tech_param.param.pa.nfcid1_len);
            if (err != EOK) {
                ErrorLog("TagNciAdapter::HandleDiscResult, memcpy nfcid2 error: %{public}d", err);
            }
        }
    }
    if (discNtfIndex_ >= MAX_NUM_TECHNOLOGY) {
        ErrorLog("TagNciAdapter::HandleDiscResult, invalid discNtfIndex_: %{public}d", discNtfIndex_);
        return;
    }
    discNtfIndex_++;
    multiTagDiscId_.push_back(discoveryNtf.rf_disc_id);
    multiTagDiscProtocol_.push_back(discoveryNtf.protocol);
#if (NXP_EXTNS == TRUE)
    SetMultiTagData(discoveryNtf);
#endif
    if (discoveryNtf.more == NCI_DISCOVER_NTF_MORE) {
        return;
    }
    for (uint32_t i = 0; i < discNtfIndex_; i++) {
        InfoLog("TagNciAdapter::HandleDiscResult, index: %{public}d, discId: %{public}d, protocl: %{public}d",
                i, multiTagDiscId_[i], multiTagDiscProtocol_[i]);
    }
    if (discoveryNtf.rf_disc_id > 2) {
        InfoLog("TagNciAdapter::HandleDiscResult, this multiTag has more than 2 uids");
    } else if (discoveryNtf.rf_disc_id == 2) { // this multiTag has 2 uids
        if (memcmp(nfcID1_, nfcID2, sizeof(nfcID1_)) == 0) {
            InfoLog("TagNciAdapter::HandleDiscResult, this multiTag has 2 same uids");
            isMultiTag_ = false;
        } else {
            InfoLog("TagNciAdapter::HandleDiscResult, this multiTag has 2 different uids");
        }
    } else {
        InfoLog("TagNciAdapter::HandleDiscResult, this multiTag has 1 uid");
    }
}

void TagNciAdapter::OnRfDiscLock()
{
    rfDiscoveryMutex_.lock();
}

void TagNciAdapter::OffRfDiscLock()
{
    rfDiscoveryMutex_.unlock();
}

bool TagNciAdapter::IsNdefFormattable()
{
    DebugLog("check IsNdefFormattable");
    const uint32_t IDX_NDEF_FORMAT_1ST = 7;
    const uint32_t IDX_NDEF_FORMAT_2ND = 8;
    if (connectedProtocol_ == NFA_PROTOCOL_T1T || connectedProtocol_ == NFA_PROTOCOL_T5T ||
        connectedProtocol_ == NFC_PROTOCOL_MIFARE) {
        return true;
    } else if (connectedProtocol_ == NFA_PROTOCOL_T2T) {
        return isMifareUltralight_;
    } else if (connectedProtocol_ == NFA_PROTOCOL_T3T) {
        return isFelicaLite_;
    } else if (connectedProtocol_ == NFA_PROTOCOL_ISO_DEP && isMifareDESFire_) {
        std::string hexRequest = "9060000000";
        std::string response;
        Transceive(hexRequest, response);
        if (KITS::NfcSdkCommon::GetHexStrBytesLen(response) == MIFACE_DES_FIRE_RESPONSE_LENGTH &&
            KITS::NfcSdkCommon::GetByteFromHexStr(response, IDX_NDEF_FORMAT_1ST) == NDEF_FORMATTABLE_1ST &&
            KITS::NfcSdkCommon::GetByteFromHexStr(response, IDX_NDEF_FORMAT_2ND) == NDEF_FORMATTABLE_2ND) {
            return true;
        }
    }
    return false;
}

void TagNciAdapter::AbortWait()
{
    DebugLog("TagNciAdapter::AbortWait");
    {
        NFC::SynchronizeGuard guard(transceiveEvent_);
        transceiveEvent_.NotifyOne();
    }
    {
        NFC::SynchronizeGuard guard(fieldCheckEvent_);
        fieldCheckEvent_.NotifyOne();
    }
    {
        NFC::SynchronizeGuard guard(readNdefEvent_);
        readNdefEvent_.NotifyOne();
    }
    {
        NFC::SynchronizeGuard guard(writeNdefEvent_);
        writeNdefEvent_.NotifyOne();
    }
    {
        NFC::SynchronizeGuard guard(formatNdefEvent_);
        formatNdefEvent_.NotifyOne();
    }
    {
        NFC::SynchronizeGuard guard(checkNdefEvent_);
        checkNdefEvent_.NotifyOne();
    }
    {
        NFC::SynchronizeGuard guard(selectEvent_);
        selectEvent_.NotifyOne();
    }
    {
        NFC::SynchronizeGuard guard(activatedEvent_);
        activatedEvent_.NotifyOne();
    }
    {
        NFC::SynchronizeGuard guard(deactivatedEvent_);
        deactivatedEvent_.NotifyOne();
    }
    {
        NFC::SynchronizeGuard guard(setReadOnlyEvent_);
        setReadOnlyEvent_.NotifyOne();
    }
    connectedRfIface_ = NFA_INTERFACE_ISO_DEP;
    connectedProtocol_ = NFC_PROTOCOL_UNKNOWN;
    connectedType_ = TagHost::TARGET_TYPE_UNKNOWN;
}

uint32_t TagNciAdapter::GetT1tMaxMessageSize(tNFA_ACTIVATED activated) const
{
    uint32_t t1tMaxMessageSize;
    DebugLog("GetT1tMaxMessageSize");
    if (activated.activate_ntf.protocol != NFC_PROTOCOL_T1T) {
        t1tMaxMessageSize = 0;
        return t1tMaxMessageSize;
    }
    // examine the first byte of header ROM bytes
    switch (activated.params.t1t.hr[0]) {
        case RW_T1T_IS_TOPAZ96:
            t1tMaxMessageSize = TOPAZ96_MAX_MESSAGE_SIZE;
            break;
        case RW_T1T_IS_TOPAZ512:
            t1tMaxMessageSize = TOPAZ512_MAX_MESSAGE_SIZE;
            break;
        default:
            ErrorLog("GetT1tMaxMessageSize: unknown T1T HR0=%u", activated.params.t1t.hr[0]);
            t1tMaxMessageSize = 0;
            break;
    }
    return t1tMaxMessageSize;
}

tNFA_INTF_TYPE TagNciAdapter::GetRfInterface(uint32_t protocol) const
{
    tNFA_INTF_TYPE rfInterface;
    if (protocol == NFA_PROTOCOL_ISO_DEP) {
        rfInterface = NFA_INTERFACE_ISO_DEP;
    } else if (protocol == NFA_PROTOCOL_MIFARE) {
        rfInterface = NFA_INTERFACE_MIFARE;
    } else {
        rfInterface = NFA_INTERFACE_FRAME;
    }
    return rfInterface;
}

bool TagNciAdapter::IsTagActive() const
{
    if (!NfccNciAdapter::GetInstance().IsNfcActive()) {
        DebugLog("Nfc is not active");
        return false;
    }
    if (!NfccNciAdapter::GetInstance().IsTagActive()) {
        DebugLog("Tag already deactive");
        return false;
    }
    if (tagState_ != ACTIVE) {
        DebugLog("tagState_ not ACTIVE");
        return false;
    }
    return true;
}

void TagNciAdapter::SetIsMultiTag(bool isMultiTag)
{
    isMultiTag_ = isMultiTag && isMultiTagSupported_;
}

bool TagNciAdapter::GetIsMultiTag() const
{
    return isMultiTag_;
}

void TagNciAdapter::SetDiscRstEvtNum(uint32_t num)
{
    if (num < MAX_NUM_TECHNOLOGY) {
        discRstEvtNum_ = num;
    }
}

uint32_t TagNciAdapter::GetDiscRstEvtNum() const
{
    return discRstEvtNum_;
}

void TagNciAdapter::GetMultiTagTechsFromData(const tNFA_DISC_RESULT& discoveryData)
{
    uint32_t idx = discRstEvtNum_;
    if (idx >= MAX_NUM_TECHNOLOGY || idx >= multiTagDiscProtocol_.size() || idx >= multiTagDiscId_.size()) {
        ErrorLog("TagNciAdapter::GetMultiTagTechsFromData: index error, index = %{public}d", idx);
        return;
    }
    multiTagDiscId_[idx] = discoveryData.discovery_ntf.rf_disc_id;
    multiTagDiscProtocol_[idx] = discoveryData.discovery_ntf.protocol;
    if (discNtfIndex_ < MAX_NUM_TECHNOLOGY) {
        discNtfIndex_++;
    }
    DebugLog("TagNciAdapter::GetMultiTagTechsFromData: discRstEvtNum_ = %{public}d, discNtfIndex_ = %{public}d"
        "discId = 0x%{public}X, protocol = 0x%{public}X",
        discRstEvtNum_, discNtfIndex_, multiTagDiscId_[idx], multiTagDiscProtocol_[idx]);
}

tNFA_STATUS TagNciAdapter::DoSelectForMultiTag(uint32_t currIdx)
{
    tNFA_STATUS result = NFA_STATUS_FAILED;
    if (currIdx == INVALID_TAG_INDEX) {
        ErrorLog("TagNciAdapter::DoSelectForMultiTag: is NFC_DEP");
        return result;
    }
    InfoLog("TagNciAdapter::DoSelectForMultiTag: protocol = 0x%{public}X", multiTagDiscProtocol_[currIdx]);

    if (multiTagDiscProtocol_[currIdx] == NFA_PROTOCOL_ISO_DEP) {
        result = NFA_Select(
            multiTagDiscId_[currIdx], multiTagDiscProtocol_[currIdx], NFA_INTERFACE_ISO_DEP);
    } else if (multiTagDiscProtocol_[currIdx] == NFA_PROTOCOL_MIFARE) {
        result = NFA_Select(
            multiTagDiscId_[currIdx], multiTagDiscProtocol_[currIdx], NFA_INTERFACE_MIFARE);
    } else {
        result = NFA_Select(
            multiTagDiscId_[currIdx], multiTagDiscProtocol_[currIdx], NFA_INTERFACE_FRAME);
    }
    return result;
}

bool TagNciAdapter::SkipProtoActivateIfNeed(tNFC_PROTOCOL protocol)
{
    if ((protocol == NFA_PROTOCOL_ISO_DEP) && g_multiTagParams.isSkipIsoDepAct) {
        tNFA_CONN_EVT_DATA eventData;
        tNFC_ACTIVATE_DEVT& actNtf = eventData.activated.activate_ntf;
        tNFC_RESULT_DEVT& info = g_multiTagParams.discNtf;
        actNtf.rf_disc_id = info.rf_disc_id;
        actNtf.protocol = info.protocol;
        errno_t err = EOK;
        err = memcpy_s(&actNtf.rf_tech_param, sizeof(tNFC_RF_TECH_PARAMS),
                       &info.rf_tech_param, sizeof(tNFC_RF_TECH_PARAMS));
        if (err != EOK) {
            ErrorLog("TagNciAdapter::SkipProtoActivateIfNeed, memcpy rf_tech_param error: %{public}d", err);
        }
        err = memcpy_s(&actNtf.intf_param, sizeof(tNFC_INTF_PARAMS),
                       &g_multiTagParams.intfParam, sizeof(tNFC_INTF_PARAMS));
        if (err != EOK) {
            ErrorLog("TagNciAdapter::SkipProtoActivateIfNeed, memcpy intfParam error: %{public}d", err);
        }
        InfoLog("TagNciAdapter::SkipProtoActivateIfNeed,(SAK28) discID:%{public}u is skipped", actNtf.rf_disc_id);
        NfccNciAdapter::GetInstance().SendActEvtForSak28Tag(NFA_ACTIVATED_EVT, &eventData);
        discRstEvtNum_--;
        return true;
    }
    return false;
}

void TagNciAdapter::SelectTheFirstTag()
{
    uint32_t currIdx = INVALID_TAG_INDEX;
    for (uint32_t i = 0; i < discNtfIndex_; i++) {
        InfoLog("TagNciAdapter::SelectTheFirstTag index = %{public}d discId = 0x%{public}X protocol = 0x%{public}X",
            i, multiTagDiscId_[i], multiTagDiscProtocol_[i]);
        // logic for SAK28 issue
        if (isSkipMifareActive_) {
            if ((multiTagDiscProtocol_[i] != NFA_PROTOCOL_NFC_DEP) &&
                (multiTagDiscProtocol_[i] != NFA_PROTOCOL_MIFARE)) {
#if (NXP_EXTNS == TRUE)
                if (!SkipProtoActivateIfNeed(multiTagDiscProtocol_[i])) {
                    g_selectedIdx = i;
#endif
                    selectedTagIdx_ = i;
                    currIdx = i;
                    break;
#if (NXP_EXTNS == TRUE)
                }
#endif
            }
        } else if (multiTagDiscProtocol_[i] != NFA_PROTOCOL_NFC_DEP) {
#if (NXP_EXTNS == TRUE)
            if (!SkipProtoActivateIfNeed(multiTagDiscProtocol_[i])) {
                g_selectedIdx = i;
#endif
                selectedTagIdx_ = i;
                currIdx = i;
                break;
#if (NXP_EXTNS == TRUE)
            }
#endif
        }
    }
    isSkipMifareActive_ = false;

    // logic for normal tag
    tNFA_STATUS result = DoSelectForMultiTag(currIdx);
    InfoLog("TagNciAdapter::SelectTheFirstTag result = %{public}d", result);
}

void TagNciAdapter::SelectTheNextTag()
{
    if (discRstEvtNum_ == 0) {
        InfoLog("TagNciAdapter::SelectTheNextTag: next tag does not exist");
        return;
    }
    uint32_t currIdx = INVALID_TAG_INDEX;
    discRstEvtNum_--;
    for (uint32_t i = 0; i < discNtfIndex_; i++) {
        InfoLog("TagNciAdapter::SelectTheNextTag index = %{public}d discId = 0x%{public}X protocol = 0x%{public}X",
            i, multiTagDiscId_[i], multiTagDiscProtocol_[i]);
        if (multiTagDiscId_[i] != multiTagDiscId_[selectedTagIdx_] ||
            (multiTagDiscProtocol_[i] != multiTagDiscProtocol_[selectedTagIdx_] &&
            (multiTagDiscProtocol_[i] != NFA_PROTOCOL_NFC_DEP))) {
            selectedTagIdx_ = i;
            currIdx = i;
            break;
        }
    }
    tNFA_STATUS result = DoSelectForMultiTag(currIdx);
    InfoLog("TagNciAdapter::DoSelectForMultiTag result = %{public}d", result);
}

/* method for SAK28 issue */
void TagNciAdapter::SetSkipMifareInterface()
{
    InfoLog("TagNciAdapter::SetSkipMifareInterface");
    isSkipMifareActive_ = true;
    discRstEvtNum_ = 1;
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
