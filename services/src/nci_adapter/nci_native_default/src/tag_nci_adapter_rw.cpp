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
#include "tag_nci_adapter_rw.h"
#include "tag_nci_adapter_common.h"
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
static const uint32_t CHECK_NDEF_TIMEOUT = 3000;
static const uint32_t DEFAULT_TIMEOUT = 1000;
static const uint32_t RETRY_RECONNECT_TIMEOUT = 500;
static const uint32_t READ_NDEF_TIMEOUT = 5000;
static const uint32_t WRITE_NDEF_TIMEOUT = 2000;
static const uint32_t T2TA_HALT_PICC_TIMEOUT = 10 * 1000; // us, 10S
static const uint32_t ISO3B_HALT_PICC_TIMEOUT = 100;
static const uint8_t MIFARE_RESPONSE_LEN = 0x10;  // Mifare response len
static const uint8_t T2T_ACK_RESPONSE = 0x0A; // T2T ack response
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

#define g_commonConnectedProtocol (TagNciAdapterCommon::GetInstance().connectedProtocol_)
#define g_commonIsLegacyMifareReader (TagNciAdapterCommon::GetInstance().isLegacyMifareReader_)
#define g_commonIsNdefReadTimeOut (TagNciAdapterCommon::GetInstance().isNdefReadTimeOut_)
#define g_commonReconnectEvent (TagNciAdapterCommon::GetInstance().reconnectEvent_)
#define g_commonIsReconnecting (TagNciAdapterCommon::GetInstance().isReconnecting_)
#define g_commonIsSwitchingRfIface (TagNciAdapterCommon::GetInstance().isSwitchingRfIface_)
#define g_commonIsIsoDepDhReqFailed (TagNciAdapterCommon::GetInstance().isIsoDepDhReqFailed_)
#define g_commonConnectedTechIdx (TagNciAdapterCommon::GetInstance().connectedTechIdx_)
#define g_commonIsReconnected (TagNciAdapterCommon::GetInstance().isReconnected_)
#define g_commonReadNdefData (TagNciAdapterCommon::GetInstance().readNdefData_)
#define g_commonReadNdefEvent (TagNciAdapterCommon::GetInstance().readNdefEvent_)
#define g_commonIsNdefReading (TagNciAdapterCommon::GetInstance().isNdefReading_)
#define g_commonT1tMaxMessageSize (TagNciAdapterCommon::GetInstance().t1tMaxMessageSize_)
#define g_commonIsNdefWriteSuccess (TagNciAdapterCommon::GetInstance().isNdefWriteSuccess_)
#define g_commonWriteNdefEvent (TagNciAdapterCommon::GetInstance().writeNdefEvent_)
#define g_commonFormatNdefEvent (TagNciAdapterCommon::GetInstance().formatNdefEvent_)
#define g_commonIsNdefFormatSuccess (TagNciAdapterCommon::GetInstance().isNdefFormatSuccess_)
#define g_commonSelectEvent (TagNciAdapterCommon::GetInstance().selectEvent_)
#define g_commonSetReadOnlyEvent (TagNciAdapterCommon::GetInstance().setReadOnlyEvent_)
#define g_commonConnectedType (TagNciAdapterCommon::GetInstance().connectedType_)
#define g_commonConnectedRfIface (TagNciAdapterCommon::GetInstance().connectedRfIface_)
#define g_commonNfcID0 (TagNciAdapterCommon::GetInstance().nfcID0_)

TagNciAdapterRw::TagNciAdapterRw()
    : presChkOption_(NFA_RW_PRES_CHK_DEFAULT)
{
    if (NfcConfig::hasKey(NAME_PRESENCE_CHECK_ALGORITHM)) {
        presChkOption_ = NfcConfig::getUnsigned(NAME_PRESENCE_CHECK_ALGORITHM);
    }
}

TagNciAdapterRw::~TagNciAdapterRw()
{
    receivedData_.clear();
};

TagNciAdapterRw& TagNciAdapterRw::GetInstance()
{
    static TagNciAdapterRw tagNciAdapterRw;
    return tagNciAdapterRw;
}

uint32_t TagNciAdapterRw::GetTimeout(uint32_t technology) const
{
    uint32_t timeout = DEFAULT_TIMEOUT;
    if (technology > 0 && technology <= MAX_NUM_TECHNOLOGY) {
        timeout = TagNciAdapterCommon::GetInstance().technologyTimeoutsTable_[technology];
    } else {
        WarnLog("TagNciAdapterRw::GetTimeout, Unknown technology");
    }
    return timeout;
}

bool TagNciAdapterRw::DeactiveForReselect()
{
    NFC::SynchronizeGuard guard(g_commonReconnectEvent);
    isWaitingDeactRst_ = true;
    tNFA_STATUS status = NFA_STATUS_OK;
#if (NXP_EXTNS == TRUE)
    // Need to deactive to idle for cashbee card
    if (isCashbee_) {
        InfoLog("TagNciAdapterRw::DeactiveForReselect, Deactivate to IDLE for cashbee");
        status = NFA_StopRfDiscovery();
        if (status != NFA_STATUS_OK) {
            ErrorLog("TagNciAdapterRw::Reselect: Deactivate to IDLE for cashbee failed, status = %{public}d",
                status);
            return false;
        }
    } else {
        if (NfccNciAdapter::GetInstance().isRfFieldOn()) {
            status = NFA_STATUS_FAILED;
            WarnLog("TagNciAdapterRw::DeactiveForReselect:Card Emulation is prior to read card");
            return false;
        }
#endif
        status = NFA_Deactivate(true);
        if (status != NFA_STATUS_OK) {
            ErrorLog("TagNciAdapterRw::Reselect: deactivate to SLEEP failed, status = %{public}d", status);
            return false;
        }
#if (NXP_EXTNS == TRUE)
    }
#endif
    if (g_commonReconnectEvent.Wait(DEFAULT_TIMEOUT) == false) {
        ErrorLog("TagNciAdapterRw::Reselect: deactivate to SLEEP timeout");
    }
    return true;
}

bool TagNciAdapterRw::IsCashbeeCard()
{
    if (tagState_ == IDLE) {
        InfoLog("TagNciAdapterRw::IsCashbeeCard: tagState_ is IDLE state");
        if (g_commonConnectedProtocol == NFC_PROTOCOL_ISO_DEP) {
            if (g_commonConnectedType == NFC_DISCOVERY_TYPE_POLL_A) {
                InfoLog("TagNciAdapterRw::IsCashbeeCard: CashBee Detected");
                return true;
            }
        }
    }
    return false;
}

tNFA_STATUS TagNciAdapterRw::SelectCard(tNFA_INTF_TYPE rfInterface)
{
    NFC::SynchronizeGuard guard(g_commonReconnectEvent);
    g_commonIsReconnecting = true;
    g_commonIsSwitchingRfIface = true;
    tNFA_STATUS status = NFA_STATUS_OK;
#if (NXP_EXTNS == TRUE)
    // Due to failed to enter sleep for cashbee, so cashbee card need to enter discovery to read card
    if (isCashbee_) {
        InfoLog("TagNciAdapterRw::SelectCard: Start RF discovery for cash bee");
        if (!g_commonIsIsoDepDhReqFailed) {
            status = NFA_StartRfDiscovery();
            if (status != NFA_STATUS_OK) {
                ErrorLog("SelectCard: start rf disc for cash bee failed, status = %{public}d", status);
                return NFA_STATUS_FAILED;
            }
        }
        // check reconnection
        g_commonIsReconnected = false;
        if (g_commonReconnectEvent.Wait(DEFAULT_TIMEOUT) == false) {
            ErrorLog("TagNciAdapterRw::SelectCard: select timeout for cashbee");
            return NFA_STATUS_FAILED;
        }
        return status;
    }
#endif
    InfoLog("TagNciAdapterRw::SelectCard:select interface %{public}u", rfInterface);
    status = NFA_Select(TagNciAdapterCommon::GetInstance().tagRfDiscIdList_[g_commonConnectedTechIdx],
                        TagNciAdapterCommon::GetInstance().tagRfProtocols_[g_commonConnectedTechIdx],
                        rfInterface);
    if (status != NFA_STATUS_OK) {
        ErrorLog("TagNciAdapterRw::SelectCard, NFA_Select failed, status = %{public}d", status);
        status = NFA_Deactivate(false);
        if (status != NFA_STATUS_OK) {
            ErrorLog("TagNciAdapterRw::SelectCard: deactivate failed; status = %{public}d", status);
        }
        return NFA_STATUS_FAILED;
    }
    // check reconnection
    g_commonIsReconnected = false;
    if (g_commonReconnectEvent.Wait(DEFAULT_TIMEOUT) == false) {
        ErrorLog("TagNciAdapterRw::SelectCard: select timeout for non-cashbee");
#if (NXP_EXTNS == TRUE)
        if (!isCashbee_) {
            status = NFA_Deactivate(false);
            if (status != NFA_STATUS_OK)
                ErrorLog("TagNciAdapterRw::Reselect: deactivate after select timeout failed, "
                    "status = %{public}d", status);
        }
#endif
        return NFA_STATUS_FAILED;
    }
    return NFA_STATUS_OK;
}

void TagNciAdapterRw::RetryThreeTimes(int retryIn)
{
    do {
        NFC::SynchronizeGuard guard(g_commonReconnectEvent);
        if (!g_commonReconnectEvent.Wait(RETRY_RECONNECT_TIMEOUT)) {
            ErrorLog("TagNciAdapterRw::RetryToWait: connect waiting retryIn timeout");
        }
        retryIn++;
        ErrorLog("TagNciAdapterRw::RetryToWait:connect waiting retryIn "
            "cnt = %{public}d, connect succ = %{public}d", retryIn, g_commonIsReconnected);
    } while (g_commonIsReconnected == false && retryIn < 3);  // 3 represents the number of retries that occur
}

/**
 * @brief When failed to activate card, wait more time to activate
 * @return True/false to be to successful/failed to activate card.
 */
tNFA_STATUS TagNciAdapterRw::RetryToWaitSuccess(tNFA_INTF_TYPE rfInterface)
{
    int retry = 0;
    if (!g_commonIsReconnected) {
        ErrorLog("TagNciAdapterRw::RetryToWait retry: waiting for Card to be activated");
        g_commonIsReconnecting = true;
#if (NXP_EXTNS == TRUE)
        if (IsMultiMFCTag() && TagNciAdapterCommon::GetInstance().isMultiTagSupported_) {
                TagNciAdapterCommon::GetInstance().isMultiProtoMFC_ = true;
        } else {
#endif
            RetryThreeTimes(retry);
#if (NXP_EXTNS == TRUE)
        }
        if (TagNciAdapterCommon::GetInstance().discRstEvtNum_) {
                TagNciAdapterCommon::GetInstance().discRstEvtNum_ = 0;
        }
#endif
    }
    InfoLog("TagNciAdapterRw::RetryToWait: select completed; g_commonIsReconnected= %{public}d", g_commonIsReconnected);
    if (tagState_ != ACTIVE) {
        ErrorLog("TagNciAdapterRw::RetryToWait: tag is not active");
#if (NXP_EXTNS == TRUE)
        HandleDeactivatedResult(0);
#endif
        if (!g_commonIsReconnected && retry >= 3) { // 3 connect retry times
            AbortWait();
        }
        return NFA_STATUS_FAILED;
    }
#if (NXP_EXTNS == TRUE)
        if (isCashbee_) {
            isCashbee_ = false;
        }
#endif
        if (g_commonIsReconnected) {
#if (NXP_EXTNS != TRUE)
            g_commonConnectedRfIface = rfInterface;
#endif
            return NFA_STATUS_OK; // success
        } else {
            return NFA_STATUS_FAILED;
        }
}

bool TagNciAdapterRw::Reselect(tNFA_INTF_TYPE rfInterface, bool isSwitchingIface)
{
    InfoLog("TagNciAdapterRw::Reselect: target interface = %{public}d, connected RfIface_ = %{public}d, "
            "g_commonConnectedProtocol = %{public}d", rfInterface, g_commonConnectedRfIface, g_commonConnectedProtocol);
    rfDiscoveryMutex_.lock();
    if (isSwitchingIface && (rfInterface == g_commonConnectedRfIface)) {
        rfDiscoveryMutex_.unlock();
        return true;
    }
    tNFA_STATUS status = NFA_STATUS_OK;
    do {
        if (g_commonIsNdefReadTimeOut) {
            ErrorLog("TagNciAdapterRw::Reselect: ndef read timeout");
            tagState_ = INACTIVE;
            status = NFA_STATUS_FAILED;
            break;
        }

        // send halt request for interface frame
        if (SendRawFrameForHaltPICC() != NFA_STATUS_OK) {
            ErrorLog("TagNciAdapterRw::Reselect: SendRawFrameForHaltPICC error status = %{public}d", status);
            break;
        }

        // deactive to sleep, contains special process for cashbee
        if (!DeactiveForReselect()) {
            break;
        }
#if (NXP_EXTNS == TRUE)
        isCashbee_ = IsCashbeeCard();
        // Cashbee card needed to be read even if failed in sleep
        if ((!isCashbee_) && (tagState_ != SLEEP)) {
#else
        if (tagState_ != SLEEP) {
#endif
            ErrorLog("TagNciAdapterRw::Reselect, tagState_ is not sleep");
            status = NFA_STATUS_FAILED;
            break;
        }
        // do reselect, contains special process for cashbee
        isWaitingDeactRst_ = false;
        // enter discovery or select status to read card
        if (SelectCard(rfInterface) != NFA_STATUS_OK) {
            break;
        }

        // Retry in case of Generic error
        if (RetryToWaitSuccess(rfInterface) != NFA_STATUS_OK) {
            break;
        }
    } while (0);
    g_commonIsReconnecting = false;
    isWaitingDeactRst_ = false;
    g_commonIsSwitchingRfIface = false;
    rfDiscoveryMutex_.unlock();
    return (status == NFA_STATUS_OK) ? true : false;
}

tNFA_STATUS TagNciAdapterRw::Connect(uint32_t idx)
{
    if (idx >= MAX_NUM_TECHNOLOGY) {
        ErrorLog("TagNciAdapterRw::Connect: tag %{public}X is out-of-range", idx);
        return NFA_STATUS_FAILED;
    }
    if (!IsTagActive()) {
        ErrorLog("TagNciAdapterRw::Connect: tag %{public}X is not active", idx);
        return NFA_STATUS_FAILED;
    }
    g_commonConnectedType = static_cast<uint32_t>(TagNciAdapterCommon::GetInstance().tagTechList_[idx]);
    g_commonConnectedProtocol = TagNciAdapterCommon::GetInstance().tagRfProtocols_[idx];
    uint32_t discId = TagNciAdapterCommon::GetInstance().tagRfDiscIdList_[idx];
    g_commonConnectedTechIdx = idx;
    InfoLog("TagNciAdapterRw::Connect: index: %{public}d, discId: %{public}d, "
        "targetProto_: %{public}d, targetType_: %{public}d",
        idx, discId, g_commonConnectedProtocol, g_commonConnectedType);
#if (NXP_EXTNS == TRUE)
    if (g_commonConnectedProtocol == NFC_PROTOCOL_T3BT) {
        return NFA_STATUS_OK;
    }
#endif
    if (g_commonConnectedProtocol != NFC_PROTOCOL_ISO_DEP && g_commonConnectedProtocol != NFC_PROTOCOL_MIFARE) {
        InfoLog("TagNciAdapterRw::Connect: do nothing for non ISO_DEP");
        return NFA_STATUS_OK;
    }
    if (g_commonConnectedType == TagNciAdapterCommon::TARGET_TYPE_ISO14443_3A ||
        g_commonConnectedType == TagNciAdapterCommon::TARGET_TYPE_ISO14443_3B) {
#if (NXP_EXTNS != TRUE)
        if (g_commonConnectedProtocol != NFC_PROTOCOL_MIFARE)
#endif
        {
            DebugLog("TagNciAdapterRw::Connect: switch rf interface to frame");
            return (Reselect(NFA_INTERFACE_FRAME, true) ? NFA_STATUS_OK : NFA_STATUS_FAILED);
        }
    } else if (g_commonConnectedType == TagNciAdapterCommon::TARGET_TYPE_MIFARE_CLASSIC) {
        DebugLog("TagNciAdapterRw::Connect: switch rf interface to mifare classic");
        return (Reselect(NFA_INTERFACE_MIFARE, true) ? NFA_STATUS_OK : NFA_STATUS_FAILED);
    } else {
        DebugLog("TagNciAdapterRw::Connect: switch rf interface to ISODEP");
        return (Reselect(NFA_INTERFACE_ISO_DEP, true) ? NFA_STATUS_OK : NFA_STATUS_FAILED);
    }
}

bool TagNciAdapterRw::Disconnect()
{
    if (tagState_ != ACTIVE) {
        ErrorLog("TagNciAdapterRw::Disconnect : tag has been deactived.");
        return NFA_STATUS_OK;
    }
    DebugLog("TagNciAdapterRw::Disconnect");
    rfDiscoveryMutex_.lock();
    tNFA_STATUS status = NFA_Deactivate(false);
    if (status != NFA_STATUS_OK) {
        WarnLog("TagNciAdapterRw::Disconnect: deactivate failed; error = 0x%{public}X", status);
    }
    g_commonConnectedProtocol = NCI_PROTOCOL_UNKNOWN;
    g_commonConnectedTechIdx = 0;
    g_commonConnectedType = TagNciAdapterCommon::TARGET_TYPE_UNKNOWN;
    g_commonConnectedRfIface = NFA_INTERFACE_ISO_DEP;
    g_commonIsReconnecting = false;
    TagNciAdapterCommon::GetInstance().ResetTag();
    rfDiscoveryMutex_.unlock();
    return (status == NFA_STATUS_OK);
}

bool TagNciAdapterRw::Reconnect()
{
    if (tagState_ != ACTIVE) {
        ErrorLog("TagNciAdapterRw::Reconnect, tag not active");
        return false;
    }
    // return for TARGET_TYPE_KOVIO_BARCODE
    // this is only supported for type 2 or 4 (ISO_DEP) tags
    if (g_commonConnectedProtocol == NFA_PROTOCOL_ISO_DEP) {
        return Reselect(NFA_INTERFACE_ISO_DEP, false);
    } else if (g_commonConnectedProtocol == NFA_PROTOCOL_T2T) {
        return Reselect(NFA_INTERFACE_FRAME, false);
    } else if (g_commonConnectedProtocol == NFC_PROTOCOL_MIFARE) {
        return Reselect(NFA_INTERFACE_MIFARE, false);
    }
    return true;
}

bool TagNciAdapterRw::IsMifareConnected()
{
    return (g_commonConnectedProtocol == NFC_PROTOCOL_MIFARE);
}

/**
 * See NFC Digital Protocol Technical Specification(2010-11-17)
 * Chapter 9 (Type 2 Tag platform), section 9.6 (READ).
*/
bool TagNciAdapterRw::IsT2TNackRsp(const uint8_t* response, uint32_t responseLen)
{
    if (responseLen == 1) {
        return (response[0] != T2T_ACK_RESPONSE);
    }
    return false;
}

int TagNciAdapterRw::Transceive(const std::string& request, std::string& response)
{
    if (!IsTagActive() || (tagState_ != ACTIVE)) {
        ErrorLog("Transceive, IsTagActive:%{public}d, tagState_::%{public}d",
            IsTagActive(), tagState_);
        return NFA_STATUS_BUSY;
    }
    tNFA_STATUS status = NFA_STATUS_FAILED;
    isInTransceive_ = true;
    isTransceiveTimeout_ = false;
    do {
        bool wait = true;
        {
            NFC::SynchronizeGuard guard(transceiveEvent_);
            uint16_t length = KITS::NfcSdkCommon::GetHexStrBytesLen(request);
            std::vector<unsigned char> requestInCharVec;
            KITS::NfcSdkCommon::HexStringToBytes(request, requestInCharVec);
            InfoLog("TagNciAdapterRw::Transceive: requestLen = %{public}d", length);
            receivedData_.clear();
            if (IsMifareConnected() && g_commonIsLegacyMifareReader) {
                ErrorLog("TagNciAdapterRw::Transceive: is mifare");
                status = Extns::GetInstance().EXTNS_MfcTransceive(
                    static_cast<uint8_t *>(requestInCharVec.data()), length);
            } else {
                status = NFA_SendRawFrame(static_cast<uint8_t *>(requestInCharVec.data()),
                    length, NFA_DM_DEFAULT_PRESENCE_CHECK_START_DELAY);
            }
            if (status != NFA_STATUS_OK) {
                ErrorLog("TagNciAdapterRw::Transceive: fail send; error=%{public}d", status);
                break;
            }
            wait = transceiveEvent_.Wait(GetTimeout(g_commonConnectedType));
        }
        if (!wait || isTransceiveTimeout_) {
            ErrorLog("TagNciAdapterRw::Transceive: wait response timeout transceiveTimeout: %{public}d,"
                "wait: %{public}d, isTimeout: %{public}d", GetTimeout(g_commonConnectedType),
                wait, isTransceiveTimeout_);
            status = NFA_STATUS_TIMEOUT;
            break;
        }
        if (receivedData_.size() > 0) {
            if (g_commonConnectedProtocol == NFA_PROTOCOL_T2T &&
                IsT2TNackRsp(receivedData_.data(), receivedData_.size())) {
                // Do reconnect for mifareUL tag when it responses NACK and enters HALT state
                InfoLog("TagNciAdapterRw::Transceive:try reconnect for T2T NACK");
                Reconnect();
            } else if (IsMifareConnected() && g_commonIsLegacyMifareReader) {
                status = HandleMfcTransceiveData(response);
            } else {
                response = KITS::NfcSdkCommon::BytesVecToHexString(receivedData_.data(), receivedData_.size());
            }
        }
    } while (0);
    isInTransceive_ = false;
    InfoLog("TagNciAdapterRw::Transceive: exit rsp len = %{public}d", KITS::NfcSdkCommon::GetHexStrBytesLen(response));
    return status;
}

void TagNciAdapterRw::HandleFieldCheckResult(uint8_t status)
{
    NFC::SynchronizeGuard guard(fieldCheckEvent_);
    isTagFieldOn_ = (status == NFA_STATUS_OK);
    fieldCheckEvent_.NotifyOne();
}

void TagNciAdapterRw::ResetTagFieldOnFlag()
{
    DebugLog("TagNciAdapterRw::ResetTagFieldOnFlag");
    isTagFieldOn_ = true;
}

void TagNciAdapterRw::HandleTranceiveData(uint8_t status, uint8_t* data, uint32_t dataLen)
{
    if (IsMifareConnected() && g_commonIsLegacyMifareReader) {
        InfoLog("TagNciAdapterRw::HandleTranceiveData: is mifare");
        isMfcTransRspErr_ = (dataLen == 2 && data[0] == MIFARE_RESPONSE_LEN && data[1] != T2T_ACK_RESPONSE);
        if (!Extns::GetInstance().EXTNS_GetCallBackFlag()) {
            ErrorLog("TagNciAdapterRw::HandleTranceiveData: ExtnsGetCallBackFlag is false");
            Extns::GetInstance().EXTNS_MfcCallBack(data, dataLen);
            return;
        }
    }
    if (!isInTransceive_) {
        ErrorLog("TagNciAdapterRw::HandleTranceiveData: not in transceive");
        return;
    }
    NFC::SynchronizeGuard guard(transceiveEvent_);
    if (status == NFA_STATUS_OK || status == NFA_STATUS_CONTINUE) {
        receivedData_.append(data, dataLen);
    }
    if (status == NFA_STATUS_OK) {
        transceiveEvent_.NotifyOne();
    }
    DebugLog("TagNciAdapterRw::HandleTranceiveData: status = %{public}d", status);
}

bool TagNciAdapterRw::IsTagFieldOn()
{
    if (!IsTagActive()) {
        return false;
    }
    if (isInTransceive_ && IsMifareConnected()) {
        return true;
    }
    tNFA_STATUS status = NFA_STATUS_FAILED;
#if (NXP_EXTNS == TRUE)
    if (TagNciAdapterCommon::GetInstance().tagRfProtocols_[0] == NFA_PROTOCOL_T3BT) {
        uint8_t t3btPresenceCheckCmd[] = {0xB2};
        NFC::SynchronizeGuard guard(transceiveEvent_);
        isTransceiveTimeout_ = false;
        isInTransceive_ = true;
        status = NFA_SendRawFrame(t3btPresenceCheckCmd, sizeof(t3btPresenceCheckCmd),
            NFA_DM_DEFAULT_PRESENCE_CHECK_START_DELAY);
        if (status != NFA_STATUS_OK) { // when failed to send cmd, continue to do normal presence check
            ErrorLog("TagNciAdapterRw::IsTagFieldOn, status = %{public}d", status);
        } else {
            if (!transceiveEvent_.Wait(GetTimeout(TagNciAdapterCommon::TARGET_TYPE_ISO14443_3B)) ||
                isTransceiveTimeout_) {
                isTagFieldOn_ = false;
                ErrorLog("TagNciAdapterRw::IsTagFieldOn, send t3bt cmd timeout");
            } else {
                isTagFieldOn_ = true;
            }
            isInTransceive_ = false;
            return isTagFieldOn_;
        }
        isInTransceive_ = false;
    }
#else
    if (IsMifareConnected() && g_commonIsLegacyMifareReader) {
        ErrorLog("TagNciAdapterRw::IsTagFieldOn: is mifare");
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
        if (NFA_RwPresenceCheck(presChkOption_) == NFA_STATUS_OK) {
            if (fieldCheckEvent_.Wait(DEFAULT_TIMEOUT) == false) {
                ErrorLog("field on check timeout...");
                isTagFieldOn_ = false;
            }
        }
    }
    return isTagFieldOn_;
}

void TagNciAdapterRw::SetTimeout(const uint32_t timeout, const uint32_t technology)
{
    DebugLog("SetTimeout timeout: %{public}d, tech: %{public}d", timeout, technology);
    if (technology > 0 && technology <= MAX_NUM_TECHNOLOGY) {
        TagNciAdapterCommon::GetInstance().technologyTimeoutsTable_[technology] = timeout;
    } else {
        WarnLog("TagNciAdapterRw::SetTimeout, Unknown technology");
    }
}

bool TagNciAdapterRw::SetReadOnly() const
{
    DebugLog("TagNciAdapterRw::SetReadOnly");
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

void TagNciAdapterRw::HandleNdefCheckResult(uint8_t status, uint32_t currentSize, uint32_t flag, uint32_t maxSize)
{
    DebugLog("TagNciAdapterRw::HandleNdefCheckResult");
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

void TagNciAdapterRw::ReadNdef(std::string& response)
{
    DebugLog("TagNciAdapterRw::ReadNdef");
    if (!IsTagActive()) {
        ErrorLog("ReadNdef, IsTagActive failed");
        return;
    }
    g_commonReadNdefData = "";
    if (lastCheckedNdefSize_ > 0) {
        {
            NFC::SynchronizeGuard guard(g_commonReadNdefEvent);
            g_commonIsNdefReading = true;
            tNFA_STATUS status = NFA_STATUS_FAILED;
            if (IsMifareConnected() && g_commonIsLegacyMifareReader) {
                status = Extns::GetInstance().EXTNS_MfcReadNDef();
            } else {
                status = NFA_RwReadNDef();
            }
            if (status != NFA_STATUS_OK) {
                g_commonIsNdefReading = false;
                return;
            }
            if (!g_commonReadNdefEvent.Wait(READ_NDEF_TIMEOUT)) { // NFA_READ_CPLT_EVT will notify this
                ErrorLog("TagNciAdapterRw::ReadNdef, g_commonReadNdefEvent timeout!");
                g_commonIsNdefReading = false;
                return;
            }
        }
        g_commonIsNdefReading = false;
        if (KITS::NfcSdkCommon::GetHexStrBytesLen(g_commonReadNdefData) > 0) {
            response = g_commonReadNdefData;
        }
    }
    rfDiscoveryMutex_.unlock();
    return;
}

bool TagNciAdapterRw::IsNdefFormattable()
{
    DebugLog("check IsNdefFormattable");
    const uint32_t IDX_NDEF_FORMAT_1ST = 7;
    const uint32_t IDX_NDEF_FORMAT_2ND = 8;
    if (g_commonConnectedProtocol == NFA_PROTOCOL_T1T || g_commonConnectedProtocol == NFA_PROTOCOL_T5T ||
        g_commonConnectedProtocol == NFC_PROTOCOL_MIFARE) {
        return true;
    } else if (g_commonConnectedProtocol == NFA_PROTOCOL_T2T) {
        return TagNciAdapterCommon::GetInstance().isMifareUltralight_;
    } else if (g_commonConnectedProtocol == NFA_PROTOCOL_T3T) {
        return TagNciAdapterCommon::GetInstance().isFelicaLite_;
    } else if (g_commonConnectedProtocol == NFA_PROTOCOL_ISO_DEP &&
        TagNciAdapterCommon::GetInstance().isMifareDESFire_) {
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

bool TagNciAdapterRw::DetectNdefInfo(std::vector<int>& ndefInfo)
{
    DebugLog("TagNciAdapterRw::DetectNdefInfo");
    if (!IsTagActive()) {
        return false;
    }
    // special for MULTI MFC and t3tB
    if (g_commonConnectedProtocol == NFA_PROTOCOL_T3BT || (TagNciAdapterCommon::GetInstance().isMultiProtoMFC_
        && TagNciAdapterCommon::GetInstance().isSkipNdefRead_)) {
        TagNciAdapterCommon::GetInstance().ClearMultiMFCTagState();
        ndefInfo.push_back(0);
        ndefInfo.push_back(NDEF_MODE_READ_ONLY);
        InfoLog("TagNciAdapterRw:: ReadNdef, skip ndef read for NFA_PROTOCOL_T3BT or MultiProtoMFC");
        return false;
    }
    // special for kovio
    // special for mifare
    if (g_commonConnectedProtocol == NFC_PROTOCOL_MIFARE) {
        Reconnect();
    }
    rfDiscoveryMutex_.lock();
    NFC::SynchronizeGuard guard(checkNdefEvent_);
    tNFA_STATUS status = NFA_STATUS_FAILED;
    isNdefChecking_ = true;
    if (IsMifareConnected() && g_commonIsLegacyMifareReader) {
        status = Extns::GetInstance().EXTNS_MfcCheckNDef();
    } else {
        status = NFA_RwDetectNDef();
    }
    if ((status != NFA_STATUS_OK) || (checkNdefEvent_.Wait(CHECK_NDEF_TIMEOUT) == false)) {
        ErrorLog("NFA_RwDetectNDef failed or timeout, status: %{public}d", status);
        rfDiscoveryMutex_.unlock();
        return false;
    }

    if (isNdefCapable_) {
        if (g_commonConnectedProtocol == NFA_PROTOCOL_T1T) {
            ndefInfo.push_back(g_commonT1tMaxMessageSize);
        } else {
            ndefInfo.push_back(lastCheckedNdefMaxSize_);
        }
        ndefInfo.push_back(lastCheckedNdefMode_);
    }
    rfDiscoveryMutex_.unlock();

    if (g_commonConnectedProtocol == NFC_PROTOCOL_MIFARE) {
        Reconnect();
    }
    return isNdefCapable_;
}

bool TagNciAdapterRw::WriteNdef(std::string& ndefMessage)
{
    DebugLog("TagNciAdapterRw::WriteNdef");
    if (!IsTagActive()) {
        ErrorLog("WriteNdef, IsTagActive failed");
        return false;
    }
    rfDiscoveryMutex_.lock();
    g_commonIsNdefWriteSuccess = false;
    tNFA_STATUS status = NFA_STATUS_FAILED;
    const uint32_t maxBufferSize = 1024;
    uint8_t buffer[maxBufferSize] = {0};
    uint32_t curDataSize = 0;
    NFC::SynchronizeGuard guard(g_commonWriteNdefEvent);
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
        if (g_commonWriteNdefEvent.Wait(WRITE_NDEF_TIMEOUT) == false) {
            ErrorLog("WriteNdef tmeout.");
            status = NFA_STATUS_FAILED;
        }
    } else {
        ErrorLog("WriteNdef, Write ndef fail");
    }
    rfDiscoveryMutex_.unlock();
    return g_commonIsNdefWriteSuccess;
}

bool TagNciAdapterRw::FormatNdef()
{
    DebugLog("TagNciAdapterRw::FormatNdef");
    if (!IsTagActive()) {
        return false;
    }
    NFC::SynchronizeGuard guard(g_commonFormatNdefEvent);
    g_commonIsNdefFormatSuccess = false;
    tNFA_STATUS status = NFA_RwFormatTag();
    if (status == NFA_STATUS_OK) {
        if (g_commonFormatNdefEvent.Wait(WRITE_NDEF_TIMEOUT) == false) {
            ErrorLog("FormatNdef tmeout.");
            status = NFA_STATUS_FAILED;
        }
        if (!g_commonIsNdefFormatSuccess) {
            status = NFA_STATUS_FAILED;
        }
    } else {
        ErrorLog("Format Ndef error, status= %{public}d", status);
    }
    return (status == NFA_STATUS_OK);
}

bool TagNciAdapterRw::IsNdefFormatable()
{
    DebugLog("TagNciAdapterRw::IsNdefFormatable");
    return g_commonIsNdefFormatSuccess;
}

tNFA_STATUS TagNciAdapterRw::HandleMfcTransceiveData(std::string& response)
{
    tNFA_STATUS status = NFA_STATUS_FAILED;
    uint32_t len = static_cast<uint32_t>(receivedData_.size());
    uint8_t* data = static_cast<uint8_t *>(receivedData_.data());
    bool shouldReconnect = false;

    if (g_commonIsLegacyMifareReader) {
        status = Extns::GetInstance().EXTNS_CheckMfcResponse(&data, &len);
        ErrorLog("TagNciAdapterRw::HandleMfcTransceiveData: status: %{public}d, isMfcTransRspErr_: %{public}d",
                 status, isMfcTransRspErr_);
        shouldReconnect = ((status == NFA_STATUS_FAILED) || isMfcTransRspErr_) ? true : false;
    } else {
        shouldReconnect = ((len == 1) && (data[0] != 0x00));
        if (!shouldReconnect) {
            status = NFA_STATUS_OK;
        } else {
            ErrorLog("TagNciAdapterRw::HandleMfcTransceiveData: data[0] = %{public}d, len = %{public}d",
                data[0], len);
        }
    }

    if (shouldReconnect) {
        Reconnect();
    } else {
        if (len != 0) {
            if (len == MIFARE_RESPONSE_LEN && data[0] != T2T_ACK_RESPONSE && isMfcTransRspErr_) {
                int err = (MIFARE_RESPONSE_LEN << 8) | data[0]; // 8 means offset one byte
                ErrorLog("TagNciAdapterRw::HandleMfcTransceiveData: rspProtocolErrData: %{public}d", err);
            }
            response = KITS::NfcSdkCommon::BytesVecToHexString(data, len);
            status = NFA_STATUS_OK;
        }
    }
    DebugLog("TagNciAdapterRw::HandleMfcTransceiveData: status = %{public}d", status);
    return status;
}

void TagNciAdapterRw::OnRfDiscLock()
{
    rfDiscoveryMutex_.lock();
}

void TagNciAdapterRw::OffRfDiscLock()
{
    rfDiscoveryMutex_.unlock();
}

void TagNciAdapterRw::AbortWait()
{
    DebugLog("TagNciAdapterRw::AbortWait");
    {
        NFC::SynchronizeGuard guard(transceiveEvent_);
        transceiveEvent_.NotifyOne();
    }
    {
        NFC::SynchronizeGuard guard(fieldCheckEvent_);
        fieldCheckEvent_.NotifyOne();
    }
    {
        NFC::SynchronizeGuard guard(g_commonReadNdefEvent);
        g_commonReadNdefEvent.NotifyOne();
    }
    {
        NFC::SynchronizeGuard guard(g_commonWriteNdefEvent);
        g_commonWriteNdefEvent.NotifyOne();
    }
    {
        NFC::SynchronizeGuard guard(g_commonFormatNdefEvent);
        g_commonFormatNdefEvent.NotifyOne();
    }
    {
        NFC::SynchronizeGuard guard(checkNdefEvent_);
        checkNdefEvent_.NotifyOne();
    }
    {
        NFC::SynchronizeGuard guard(g_commonSelectEvent);
        g_commonSelectEvent.NotifyOne();
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
        NFC::SynchronizeGuard guard(g_commonSetReadOnlyEvent);
        g_commonSetReadOnlyEvent.NotifyOne();
    }
    g_commonConnectedRfIface = NFA_INTERFACE_ISO_DEP;
    g_commonConnectedProtocol = NFC_PROTOCOL_UNKNOWN;
    g_commonConnectedType = TagNciAdapterCommon::TARGET_TYPE_UNKNOWN;
}

tNFA_STATUS TagNciAdapterRw::SendRawFrameForHaltPICC()
{
    if (!((g_commonConnectedRfIface == NFA_INTERFACE_FRAME) &&
        (NfccNciAdapter::GetInstance().GetNciVersion() >= NCI_VERSION_2_0))) {
        WarnLog("No need to halt picc");
        return NFA_STATUS_OK;
    }
    tNFA_STATUS status = NFA_STATUS_OK;
    NFC::SynchronizeGuard guardReconnectEvent(g_commonReconnectEvent);
#if (NXP_EXTNS == TRUE)
    // skipped SRD
    if (g_commonConnectedProtocol == NFA_PROTOCOL_T2T ||
        (g_commonConnectedProtocol == NFA_PROTOCOL_ISO_DEP &&
        g_commonConnectedType == TagNciAdapterCommon::TARGET_TYPE_ISO14443_3A)) {
        status = NFA_SendRawFrame(RW_TAG_SLP_REQ, sizeof(RW_TAG_SLP_REQ), 0);
        usleep(T2TA_HALT_PICC_TIMEOUT);
    } else if (g_commonConnectedProtocol == NFA_PROTOCOL_ISO_DEP &&
        g_commonConnectedType == TagNciAdapterCommon::TARGET_TYPE_ISO14443_3B) {
        uint8_t haltPiccBHead = 0x50;
        uint8_t rawHaltRqB[5] = {haltPiccBHead, g_commonNfcID0[0], g_commonNfcID0[1],
            g_commonNfcID0[2], g_commonNfcID0[3]};
        isInTransceive_ = true;
        NFC::SynchronizeGuard guardTransceiveEvent(transceiveEvent_);
        status = NFA_SendRawFrame(rawHaltRqB, sizeof(rawHaltRqB), 0);
        if (status != NFA_STATUS_OK) {
            ErrorLog("TagNciAdapterRw::SendRawFrameForHaltPICC: send rawHaltRqB error= %{public}d", status);
        } else {
            if (transceiveEvent_.Wait(ISO3B_HALT_PICC_TIMEOUT) == false) {
                status = NFA_STATUS_FAILED;
                ErrorLog("TagNciAdapterRw::SendRawFrameForHaltPICC: send rawHaltRqB timeout");
            }
        }
        isInTransceive_ = false;
    }
#else
    if (g_commonConnectedProtocol == NFA_PROTOCOL_T2T) {
        status = NFA_SendRawFrame(RW_TAG_SLP_REQ, sizeof(RW_TAG_SLP_REQ), 0);
    } else if (g_commonConnectedProtocol == NFA_PROTOCOL_ISO_DEP) {
        status = NFA_SendRawFrame(RW_DESELECT_REQ, sizeof(RW_DESELECT_REQ), 0);
    }
#endif
    g_commonReconnectEvent.Wait(WAIT_TIME_FOR_NO_RSP);
    if (status != NFA_STATUS_OK) {
        ErrorLog("TagNciAdapterRw::Reselect: halt for iface frame error = %{public}d", status);
    }
    return status;
}

bool TagNciAdapterRw::IsTagDeactivating()
{
    return isWaitingDeactRst_;
}

bool TagNciAdapterRw::IsTagActive() const
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

void TagNciAdapterRw::SetTagActivated()
{
    g_commonIsNdefReadTimeOut = false;
    tagState_ = ACTIVE;
}

void TagNciAdapterRw::SetTagDeactivated(bool isSleep)
{
    tagState_ = (isSleep ? SLEEP : IDLE);
    g_commonIsNdefReadTimeOut = false;
}

TagNciAdapterRw::TagState TagNciAdapterRw::GetTagState()
{
    return tagState_;
}

bool TagNciAdapterRw::IsMultiMFCTag()
{
    return TagNciAdapterCommon::GetInstance().isMultiTag_ && (g_commonConnectedProtocol == NFC_PROTOCOL_MIFARE);
}

void TagNciAdapterRw::HandleDeactivatedResult(tNFA_DEACTIVATE_TYPE deactType)
{
    DebugLog("TagNciAdapterRw::HandleDeactivatedResult");
    g_commonConnectedProtocol = NFC_PROTOCOL_UNKNOWN;
#if (NXP_EXTNS == TRUE)
    if (deactType == NFA_DEACTIVATE_TYPE_DISCOVERY) {
        // clear Activation Params
    }
#endif
    TagNciAdapterCommon::GetInstance().ResetTag();
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
