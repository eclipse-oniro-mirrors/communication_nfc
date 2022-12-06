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
#include "tag_nci_adapter.h"

#include "loghelper.h"
#include "nci_adaptations.h"
#include "nfc_brcm_defs.h"
#include "nfc_config.h"
#include "nfc_sdk_common.h"
#include "nfcc_host.h"
#include "nfcc_nci_adapter.h"
#include "rw_int.h"

namespace OHOS {
namespace NFC {
namespace NCI {
static const int DEFAULT_TIMEOUT = 1000;
static const int ISO14443_3A_DEFAULT_TIMEOUT = 618;   // NfcA
static const int ISO14443_3B_DEFAULT_TIMEOUT = 1000;  // NfcB
static const int ISO14443_4_DEFAULT_TIMEOUT = 618;    // ISO-DEP
static const int FELICA_DEFAULT_TIMEOUT = 255;        // Felica
static const int ISO15693_DEFAULT_TIMEOUT = 1000;     // NfcV
static const int NDEF_DEFAULT_TIMEOUT = 1000;
static const int NDEF_FORMATABLE_DEFAULT_TIMEOUT = 1000;
static const int MIFARE_CLASSIC_DEFAULT_TIMEOUT = 618;  // MifareClassic
static const int MIFARE_UL_DEFAULT_TIMEOUT = 618;       // MifareUltralight
static const int POS_NFCF_STSTEM_CODE_HIGH = 8;
static const int POS_NFCF_STSTEM_CODE_LOW = 9;
static const int TOPAZ512_MAX_MESSAGE_SIZE = 462;
static const int TOPAZ96_MAX_MESSAGE_SIZE = 90;
static const int SENSF_RES_LENGTH = 8;
static const int SENS_RES_LENGTH = 2;
static const int SENSB_RES_POLL_POS = 4;
static const int SYSTEM_CODE_SHIFT = 8;
static const int F_POLL_LENGTH = 10;
static const int I93_POLL_LENGTH = 2;
static const int I93_ACT_LENGTH = 2;
static const int MIFACE_DES_FIRE_RESPONSE_LENGTH = 9;
static const int NDEF_FORMATTABLE_1ST = 0x91;
static const int NDEF_FORMATTABLE_2ND = 0xAF;
static const int NDEF_MODE_READ_ONLY = 1;
static const int NDEF_MODE_READ_WRITE = 2;
static const int NDEF_MODE_UNKNOWN = 3;
static uint8_t RW_TAG_SLP_REQ[] = {0x50, 0x00};
static uint8_t RW_DESELECT_REQ[] = {0xC2};
static const unsigned int INVALID_TAG_INDEX = 0xFF;

std::mutex TagNciAdapter::rfDiscoveryMutex_;
OHOS::NFC::SynchronizeEvent TagNciAdapter::transceiveEvent_;
OHOS::NFC::SynchronizeEvent TagNciAdapter::filedCheckEvent_;
OHOS::NFC::SynchronizeEvent TagNciAdapter::readNdefEvent_;
OHOS::NFC::SynchronizeEvent TagNciAdapter::writeNdefEvent_;
OHOS::NFC::SynchronizeEvent TagNciAdapter::formatNdefEvent_;
OHOS::NFC::SynchronizeEvent TagNciAdapter::checkNdefEvent_;
OHOS::NFC::SynchronizeEvent TagNciAdapter::selectEvent_;
OHOS::NFC::SynchronizeEvent TagNciAdapter::activatedEvent_;
OHOS::NFC::SynchronizeEvent TagNciAdapter::deactivatedEvent_;

bool TagNciAdapter::isTagFieldOn_ = true;
int TagNciAdapter::connectedProtocol_ = NCI_PROTOCOL_UNKNOWN;
int TagNciAdapter::connectedTargetType_ = TagHost::TARGET_TYPE_UNKNOWN;
int TagNciAdapter::connectedTagDiscId_ = -1;
bool TagNciAdapter::isReconnecting_ = false;
bool TagNciAdapter::isInTransceive_ = false;
int TagNciAdapter::t1tMaxMessageSize_ = 0;
std::string TagNciAdapter::receivedData_ = "";
int TagNciAdapter::lastNdefCheckedStatus_ = NFA_STATUS_FAILED;
bool TagNciAdapter::isNdefCapable_ = false;
int TagNciAdapter::lastCheckedNdefSize_ = 0;
int TagNciAdapter::lastCheckedNdefMaxSize_ = 0;
int TagNciAdapter::lastCheckedNdefMode_ = NDEF_MODE_UNKNOWN;
bool TagNciAdapter::isNdefWriteSuccess_ = false;
bool TagNciAdapter::isNdefFormatSuccess_ = false;
unsigned short int TagNciAdapter::ndefTypeHandle_ = NFA_HANDLE_INVALID;
std::string TagNciAdapter::readNdefData = "";
std::shared_ptr<INfcNci> TagNciAdapter::nciAdaptations_ = std::make_shared<NciAdaptations>();

TagNciAdapter::TagNciAdapter()
    : techListIndex_(0),
      tagActivatedProtocol_(NCI_PROTOCOL_UNKNOWN),
      isFelicaLite_(false),
      isMifareUltralight_(false),
      isMifareDESFire_(false),
      presChkOption_(NFA_RW_PRES_CHK_DEFAULT),
      isMultiTag_(false),
      discRstEvtNum_(0),
      discNtfIndex_(0),
      multiTagTmpTechIdx_(0),
      selectedTagIdx_(0)
{
    ResetTimeout();
    if (NfcConfig::hasKey(NAME_PRESENCE_CHECK_ALGORITHM)) {
        presChkOption_ = NfcConfig::getUnsigned(NAME_PRESENCE_CHECK_ALGORITHM);
    } else {
        presChkOption_ = NFA_RW_PRES_CHK_ISO_DEP_NAK; // to be removed when read config from hdiimpl enabled
    }
}

TagNciAdapter::~TagNciAdapter()
{
    tagTechList_.clear();
    tagRfDiscIdList_.clear();
    tagActivatedProtocols_.clear();
    tagPollBytes_.clear();
    tagActivatedBytes_.clear();
    tagDiscIdListOfDiscResult_.clear();
    tagProtocolsOfDiscResult_.clear();
    techListIndex_ = 0;
    tagActivatedProtocol_ = NCI_PROTOCOL_UNKNOWN;
    isFelicaLite_ = false;
    isMifareUltralight_ = false;
    isMifareDESFire_ = false;
    isMultiTag_ = false;
    discRstEvtNum_ = 0;
    discNtfIndex_ = 0;
    multiTagTmpTechIdx_ = 0;
    selectedTagIdx_ = 0;
};

TagNciAdapter& TagNciAdapter::GetInstance()
{
    static TagNciAdapter tagNciAdapter;
    return tagNciAdapter;
}

void TagNciAdapter::NdefCallback(unsigned char event, tNFA_NDEF_EVT_DATA* eventData)
{
    DebugLog("TagNciAdapter::NdefCallback");
    switch (event) {
        case NFA_NDEF_REGISTER_EVT: {
            DebugLog("NdefCallback: NFA_NDEF_REGISTER_EVT; status=0x%{public}X; handle=0x%{public}X",
                     eventData->ndef_reg.status,
                     eventData->ndef_reg.ndef_type_handle);
            ndefTypeHandle_ = eventData->ndef_reg.ndef_type_handle;
            break;
        }
        case NFA_NDEF_DATA_EVT: {
            DebugLog("NdefCallback: NFA_NDEF_DATA_EVT; data_len = %u", eventData->ndef_data.len);
            uint32_t ndefDataLen = eventData->ndef_data.len;
            readNdefData = KITS::NfcSdkCommon::BytesVecToHexString(
                eventData->ndef_data.p_data, ndefDataLen);
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
    nciAdaptations_->NfaRegisterNDefTypeHandler(true, NFA_TNF_DEFAULT, (unsigned char*)"", 0, NdefCallback);
}

tNFA_STATUS TagNciAdapter::Connect(int discId, int protocol, int tech)
{
    DebugLog("TagNciAdapter::Connect: discId: %{public}d, protocol: %{public}d, tech: %{public}d",
        discId, protocol, tech);
    if (!IsTagActive()) {
        return NFA_STATUS_BUSY;
    }
    NFC::SynchronizeGuard guard(selectEvent_);
    tNFA_INTF_TYPE rfInterface = GetRfInterface(protocol);
    rfDiscoveryMutex_.lock();
    tNFA_STATUS status = nciAdaptations_->NfaSelect((uint8_t)discId, (tNFA_NFC_PROTOCOL)protocol, rfInterface);
    if (status != NFA_STATUS_OK) {
        ErrorLog("TagNciAdapter::Connect: select fail; error = 0x%{public}X", status);
        rfDiscoveryMutex_.unlock();
        return status;
    }
    if (selectEvent_.Wait(DEFAULT_TIMEOUT) == false) {
        ErrorLog("TagNciAdapter::Connect: Time out when select");
        status = nciAdaptations_->NfaDeactivate(false);
        if (status != NFA_STATUS_OK) {
            ErrorLog("TagNciAdapter::Connect: deactivate failed, error = 0x%{public}X", status);
        }
        rfDiscoveryMutex_.unlock();
        return NFA_STATUS_TIMEOUT;  // time out
    }
    connectedProtocol_ = protocol;
    connectedTagDiscId_ = discId;
    connectedTargetType_ = tech;
    rfDiscoveryMutex_.unlock();
    return NFA_STATUS_OK;
}

bool TagNciAdapter::Disconnect()
{
    DebugLog("TagNciAdapter::Disconnect");
    rfDiscoveryMutex_.lock();
    tNFA_STATUS status = nciAdaptations_->NfaDeactivate(false);
    if (status != NFA_STATUS_OK) {
        ErrorLog("TagNciAdapter::Disconnect: deactivate failed; error = 0x%{public}X", status);
    }
    connectedProtocol_ = NCI_PROTOCOL_UNKNOWN;
    connectedTagDiscId_ = -1;
    connectedTargetType_ = TagHost::TARGET_TYPE_UNKNOWN;
    isReconnecting_ = false;
    ResetTag();
    rfDiscoveryMutex_.unlock();
    return (status == NFA_STATUS_OK);
}

bool TagNciAdapter::Reselect(tNFA_INTF_TYPE rfInterface) // should set rfDiscoveryMutex_ outer when called
{
    tNFA_INTF_TYPE currInterface = GetRfInterface(connectedProtocol_);
    DebugLog("TagNciAdapter::Reselect: target interface: %{public}d, currInterface = %{public}d"
        "connectedProtocol_ = %{public}d", rfInterface, currInterface, connectedProtocol_);
    tNFA_STATUS status = NFA_STATUS_FAILED;
    if ((currInterface == NFA_INTERFACE_FRAME) &&
        (NfccNciAdapter::GetInstance().GetNciVersion() >= NCI_VERSION_2_0)) {
        NFC::SynchronizeGuard guard(activatedEvent_);
        if (connectedProtocol_ == NFA_PROTOCOL_T2T) {
            status = nciAdaptations_->NfaSendRawFrame(RW_TAG_SLP_REQ, sizeof(RW_TAG_SLP_REQ), 0);
        } else if (connectedProtocol_ == NFA_PROTOCOL_ISO_DEP) {
            status = nciAdaptations_->NfaSendRawFrame(RW_DESELECT_REQ, sizeof(RW_DESELECT_REQ), 0);
        } else {
            DebugLog("TagNciAdapter::Reselect: do nothing");
            return false;
        }
        DebugLog("TagNciAdapter::Reselect: SendRawFrame seccess, status = 0x%{public}X", status);
        activatedEvent_.Wait(4); // this request do not have response, so no need to wait for callback
        isReconnecting_ = true;
        status = nciAdaptations_->NfaDeactivate(true);
        if (status != NFA_STATUS_OK) {
            ErrorLog("TagNciAdapter::Reselect: deactivate failed, err = 0x%{public}X", status);
        }
        isReconnecting_ = false;
    }
    return (status == NFA_STATUS_OK);
}

bool TagNciAdapter::SendReselectReqIfNeed(int protocol, int tech)
{
    DebugLog("TagNciAdapter::SendReselectReqIfNeed: protocol = %{public}d, tech = %{public}d",
        protocol, tech);
    if (protocol != NCI_PROTOCOL_ISO_DEP && protocol != NCI_PROTOCOL_MIFARE) {
        DebugLog("TagNciAdapter::SendReselectReqIfNeed: do nothing for non isodep protocol");
        return false;
    }

    if (tech == TagHost::TARGET_TYPE_ISO14443_3A || tech == TagHost::TARGET_TYPE_ISO14443_3B) {
        return Reselect(NFA_INTERFACE_FRAME);
    } else if (tech == TagHost::TARGET_TYPE_MIFARE_CLASSIC) {
        return Reselect(NFA_INTERFACE_MIFARE);
    } else {
        return Reselect(NFA_INTERFACE_ISO_DEP);
    }
    return true;
}

bool TagNciAdapter::IsReconnecting()
{
    return isReconnecting_;
}

bool TagNciAdapter::NfaDeactivateAndSelect(int discId, int protocol)
{
    {
        NFC::SynchronizeGuard guard(deactivatedEvent_);
        tNFA_STATUS status = nciAdaptations_->NfaDeactivate(true);
        if (status != NFA_STATUS_OK) {
            ErrorLog("NfaDeactivateAndSelect, NfaDeactivate1 failed, status=0x%{public}X", status);
            return false;
        }
        deactivatedEvent_.Wait(DEFAULT_TIMEOUT);
    }
    {
        NFC::SynchronizeGuard guard(activatedEvent_);
        tNFA_STATUS status = nciAdaptations_->NfaSelect((uint8_t)discId, (tNFA_NFC_PROTOCOL)protocol,
            GetRfInterface(protocol));
        if (status != NFA_STATUS_OK) {
            ErrorLog("NfaDeactivateAndSelect NfaSelect failed, status=0x%{public}X", status);
            return false;
        }
        if (activatedEvent_.Wait(DEFAULT_TIMEOUT) == false) {
            ErrorLog("NfaDeactivateAndSelect, Timeout when NfaSelect.");
            status = nciAdaptations_->NfaDeactivate(false);
            if (status != NFA_STATUS_OK) {
                ErrorLog("NfaDeactivateAndSelect, NfaDeactivate2 failed, status=0x%{public}X", status);
            }
            return false;
        }
    }
    return true;
}

bool TagNciAdapter::Reconnect(int discId, int protocol, int tech, bool restart)
{
    if (!IsTagActive()) {
        return false;
    }
    rfDiscoveryMutex_.lock();
    if (connectedProtocol_ == protocol && !restart) {
        rfDiscoveryMutex_.unlock();
        return true;
    }
    if (!SendReselectReqIfNeed(protocol, tech)) {
        rfDiscoveryMutex_.unlock();
        return false;
    }
    isReconnecting_ = true;
    if (!NfaDeactivateAndSelect(discId, protocol)) {
        isReconnecting_ = false;
        rfDiscoveryMutex_.unlock();
        return false;
    }
    isReconnecting_ = false;
    {
        NFC::SynchronizeGuard guard(activatedEvent_);
        activatedEvent_.Wait(DEFAULT_TIMEOUT);
    }
    connectedProtocol_ = protocol;
    connectedTagDiscId_ = discId;
    connectedTargetType_ = tech;
    rfDiscoveryMutex_.unlock();
    return true;
}

int TagNciAdapter::Transceive(std::string& request, std::string& response)
{
    if (!IsTagActive()) {
        return NFA_STATUS_BUSY;
    }
    tNFA_STATUS status = NFA_STATUS_FAILED;
    isInTransceive_ = true;
    bool retry = false;
    do {
        NFC::SynchronizeGuard guard(transceiveEvent_);
        uint16_t length = KITS::NfcSdkCommon::GetHexStrBytesLen(request);
        std::vector<unsigned char> requestInCharVec;
        KITS::NfcSdkCommon::HexStringToBytes(request, requestInCharVec);
        InfoLog("TagNciAdapter::Transceive: requestLen = %{public}d", length);
        receivedData_ = "";
        status = nciAdaptations_->NfaSendRawFrame(static_cast<uint8_t *>(requestInCharVec.data()),
            length, NFA_DM_DEFAULT_PRESENCE_CHECK_START_DELAY);
        if (status != NFA_STATUS_OK) {
            ErrorLog("TagNciAdapter::Transceive: fail send; error=%{public}d", status);
            break;
        }
        int transceiveTimeout = GetTimeout(connectedTargetType_);
        bool wait = transceiveEvent_.Wait(transceiveTimeout);
        if (!wait) {
            ErrorLog("TagNciAdapter::Transceive: wait response timeout");
            status = NFA_STATUS_TIMEOUT;
            break;
        }
        response = receivedData_;
        InfoLog("TagNciAdapter::Transceive: rsp len = %{public}d", KITS::NfcSdkCommon::GetHexStrBytesLen(response));

        // not auth
        if (retry) {
            retry = false;
        } else if (connectedProtocol_ == NFA_PROTOCOL_MIFARE &&
            KITS::NfcSdkCommon::GetByteFromHexStr(request, 0) != 0x60 &&
            KITS::NfcSdkCommon::GetByteFromHexStr(request, 0) != 0x61 &&
            KITS::NfcSdkCommon::GetHexStrBytesLen(response) == 1 &&
            KITS::NfcSdkCommon::GetByteFromHexStr(response, 0) != 0x00) {
            DebugLog("NFA_PROTOCOL_MIFARE retry");
            retry = true;
        }

        if (!retry) {
            if (connectedProtocol_ == NFA_PROTOCOL_MIFARE &&
                KITS::NfcSdkCommon::GetHexStrBytesLen(response) == 1 &&
                KITS::NfcSdkCommon::GetByteFromHexStr(response, 0) != 0x00) {
                DebugLog("Ready to reconnect");
                Reconnect(connectedTagDiscId_, NFA_PROTOCOL_MIFARE, TagHost::TARGET_TYPE_MIFARE_CLASSIC, true);
            }
        }
    } while (retry);
    isInTransceive_ = false;
    return status;
}

void TagNciAdapter::HandleTranceiveData(unsigned char status, unsigned char* data, int dataLen)
{
    DebugLog("TagNciAdapter::HandleTranceiveData");
    NFC::SynchronizeGuard guard(transceiveEvent_);
    if (status == NFA_STATUS_OK || status == NFA_STATUS_CONTINUE) {
        receivedData_ = KITS::NfcSdkCommon::BytesVecToHexString(data, dataLen);
    }
    if (status == NFA_STATUS_OK) {
        transceiveEvent_.NotifyOne();
    }
}

bool TagNciAdapter::IsTagFieldOn()
{
    if (!IsTagActive()) {
        return false;
    }
    if (isInTransceive_) {
        return true;
    }
    if (!rfDiscoveryMutex_.try_lock()) {
        return true;
    }

    {
        NFC::SynchronizeGuard guard(filedCheckEvent_);
        tNFA_STATUS status = nciAdaptations_->NfaRwPresenceCheck(presChkOption_);
        if (status == NFA_STATUS_OK) {
            if (filedCheckEvent_.Wait(DEFAULT_TIMEOUT) == false) {
                DebugLog("filed on check timeout...");
                isTagFieldOn_ = false;
            }
        }
    }
    rfDiscoveryMutex_.unlock();
    return isTagFieldOn_;
}

void TagNciAdapter::HandleFieldCheckResult(unsigned char status)
{
    NFC::SynchronizeGuard guard(filedCheckEvent_);
    isTagFieldOn_ = (status == NFA_STATUS_OK);
    filedCheckEvent_.NotifyOne();
}

void TagNciAdapter::HandleSelectResult()
{
    DebugLog("TagNciAdapter::HandleSelectResult");
    {
        NFC::SynchronizeGuard guard(selectEvent_);
        selectEvent_.NotifyOne();
    }
}

void TagNciAdapter::HandleActivatedResult()
{
    DebugLog("TagNciAdapter::HandleActivatedResult");
    {
        NFC::SynchronizeGuard guard(activatedEvent_);
        activatedEvent_.NotifyOne();
    }
}

void TagNciAdapter::HandleDeactivatedResult()
{
    DebugLog("TagNciAdapter::HandleDeactivatedResult");
    {
        NFC::SynchronizeGuard guard(deactivatedEvent_);
        deactivatedEvent_.NotifyOne();
    }
}

void TagNciAdapter::ResetTagFieldOnFlag()
{
    DebugLog("TagNciAdapter::ResetTagFieldOnFlag");
    isTagFieldOn_ = true;
}

int TagNciAdapter::GetTimeout(int technology) const
{
    int timeout = DEFAULT_TIMEOUT;
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
    unsigned char status = nciAdaptations_->NfaRwSetTagReadOnly(true);
    if (status == NCI_STATUS_REJECTED) {
        status = nciAdaptations_->NfaRwSetTagReadOnly(false);
        if (status != NCI_STATUS_OK) {
            return false;
        }
    } else if (status != NCI_STATUS_OK) {
        return false;
    }
    return true;
}

void TagNciAdapter::ReadNdef(std::string& response)
{
    DebugLog("TagNciAdapter::ReadNdef");
    if (!IsTagActive()) {
        ErrorLog("ReadNdef, IsTagActive failed");
        return;
    }
    rfDiscoveryMutex_.lock();
    readNdefData = "";
    NFC::SynchronizeGuard guard(readNdefEvent_);
    if (lastCheckedNdefSize_ > 0) {
        tNFA_STATUS status = nciAdaptations_->NfaRwReadNdef();
        if (status != NFA_STATUS_OK) {
            ErrorLog("ReadNdef, Read ndef fail");
            return;
        }
        readNdefEvent_.Wait();

        if (KITS::NfcSdkCommon::GetHexStrBytesLen(readNdefData) > 0) {
            response = readNdefData;
        }
    }
    rfDiscoveryMutex_.unlock();
    return;
}

void TagNciAdapter::HandleReadComplete(unsigned char status)
{
    DebugLog("TagNciAdapter::HandleReadComplete");
    NFC::SynchronizeGuard guard(readNdefEvent_);
    if (status != NFA_STATUS_OK) {
        ErrorLog("Read ndef fail");
        readNdefData = "";
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
    unsigned char data[length];
    for (uint32_t i = 0; i < length; i++) {
        data[i] = KITS::NfcSdkCommon::GetByteFromHexStr(ndefMessage, i);
    }
    if (lastNdefCheckedStatus_ == NFA_STATUS_FAILED) {
        if (isNdefCapable_) {
            DebugLog("Format ndef first");
            this->FormatNdef();
        }
        status = nciAdaptations_->NfaRwWriteNdef(data, length);
    } else if (length == 0) {
        DebugLog("Create and write an empty ndef message");
        nciAdaptations_->NdefMsgInit(buffer, maxBufferSize, &curDataSize);
        nciAdaptations_->NdefMsgAddRec(buffer, maxBufferSize, &curDataSize, NDEF_TNF_EMPTY, NULL, 0, NULL, 0, NULL, 0);
        status = nciAdaptations_->NfaRwWriteNdef(buffer, curDataSize);
    } else {
        status = nciAdaptations_->NfaRwWriteNdef(data, length);
    }

    if (status == NCI_STATUS_OK) {
        writeNdefEvent_.Wait();
    } else {
        ErrorLog("WriteNdef, Write ndef fail");
    }
    rfDiscoveryMutex_.unlock();
    return isNdefWriteSuccess_;
}

void TagNciAdapter::HandleWriteComplete(unsigned char status)
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
    tNFA_STATUS status = nciAdaptations_->NfaRwFormatTag();
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

void TagNciAdapter::HandleFormatComplete(unsigned char status)
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

bool TagNciAdapter::IsNdefMsgContained(std::vector<int>& ndefInfo)
{
    DebugLog("TagNciAdapter::IsNdefMsgContained");
    if (!IsTagActive()) {
        return false;
    }
    rfDiscoveryMutex_.lock();
    NFC::SynchronizeGuard guard(checkNdefEvent_);
    tNFA_STATUS status = NFA_STATUS_FAILED;
    isReconnecting_ = false;

    status = nciAdaptations_->NfaRwDetectNdef();
    if (status != NFA_STATUS_OK) {
        ErrorLog("NFA_RwDetectNDef failed, status: %{public}d", status);
        rfDiscoveryMutex_.unlock();
        return false;
    }
    if (checkNdefEvent_.Wait(DEFAULT_TIMEOUT) == false) {
        ErrorLog("TagNciAdapter::IsNdefMsgContained time out");
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
    return isNdefCapable_;
}

void TagNciAdapter::HandleNdefCheckResult(unsigned char status, int currentSize, uint32_t flag, int maxSize)
{
    DebugLog("TagNciAdapter::HandleNdefCheckResult");
    auto uFlag = static_cast<unsigned char>(flag & 0xFF);
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

bool TagNciAdapter::IsDiscTypeA(char discType) const
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

bool TagNciAdapter::IsDiscTypeB(char discType) const
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

bool TagNciAdapter::IsDiscTypeF(char discType) const
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

bool TagNciAdapter::IsDiscTypeV(char discType) const
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
    int tech[MAX_NUM_TECHNOLOGY];
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
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_T3BT) {
        tech[techListIndex_] = TagHost::TARGET_TYPE_ISO14443_3B;
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_T3T) {
        tech[techListIndex_] = TagHost::TARGET_TYPE_FELICA;
    } else if (activated.activate_ntf.protocol == NCI_PROTOCOL_ISO_DEP) {
        tech[techListIndex_] = TagHost::TARGET_TYPE_ISO14443_4;
        // A OR B
        char discType = activated.activate_ntf.rf_tech_param.mode;
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
        tech[techListIndex_] = TagHost::TARGET_TYPE_ISO14443_3A;

        techListIndex_++;
        tech[techListIndex_] = TagHost::TARGET_TYPE_MIFARE_CLASSIC;
    } else {
        tech[techListIndex_] = TagHost::TARGET_TYPE_UNKNOWN;
    }
    techListIndex_++;

    int tagRfDiscId = activated.activate_ntf.rf_disc_id;
    int tagNtfProtocol = activated.activate_ntf.protocol;
    for (uint32_t i = multiTagTmpTechIdx_; i < techListIndex_; i++) {
        tagTechList_.push_back(tech[i]);
        tagRfDiscIdList_.push_back(tagRfDiscId);
        tagActivatedProtocols_.push_back(tagNtfProtocol);
        InfoLog("GetTechFromData: index = %{public}d, tech = %{public}d, RfDiscId = %{public}d, protocol = %{public}d",
                i, tech[i], tagRfDiscId, tagNtfProtocol);
    }
}

std::string TagNciAdapter::GetUidFromData(tNFA_ACTIVATED activated) const
{
    std::string uid;
    tNFC_RF_TECH_PARAMS nfcRfTechParams = activated.activate_ntf.rf_tech_param;
    char discType = nfcRfTechParams.mode;
    if (IsDiscTypeA(discType)) {
        int nfcid1Len = nfcRfTechParams.param.pa.nfcid1_len;
        uid = KITS::NfcSdkCommon::BytesVecToHexString(nfcRfTechParams.param.pa.nfcid1, nfcid1Len);
    } else if (IsDiscTypeB(discType)) {
        uid = KITS::NfcSdkCommon::BytesVecToHexString(nfcRfTechParams.param.pb.nfcid0, NFC_NFCID0_MAX_LEN);
    } else if (IsDiscTypeF(discType)) {
        uid = KITS::NfcSdkCommon::BytesVecToHexString(nfcRfTechParams.param.pf.nfcid2, NFC_NFCID2_LEN);
    } else if (IsDiscTypeV(discType)) {
        unsigned char* i93Uid = activated.params.i93.uid;
        unsigned char i93UidReverse[I93_UID_BYTE_LEN];
        for (int i = 0; i < I93_UID_BYTE_LEN; i++) {
            i93UidReverse[i] = i93Uid[I93_UID_BYTE_LEN - i - 1];
        }
        uid = KITS::NfcSdkCommon::BytesVecToHexString(i93UidReverse, I93_UID_BYTE_LEN);
    } else {
        uid = "";
    }
    return uid;
}

std::string TagNciAdapter::GetTechPollForTypeB(tNFC_RF_TECH_PARAMS nfcRfTechParams, int tech)
{
    std::string techPoll = "";
    if (tech == TagHost::TARGET_TYPE_ISO14443_3B) {
        int length = nfcRfTechParams.param.pb.sensb_res_len;
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
    char discType = nfcRfTechParams.mode;
    for (uint32_t i = multiTagTmpTechIdx_; i < techListIndex_; i++) {
        if (IsDiscTypeA(discType)) {
            techPoll = KITS::NfcSdkCommon::BytesVecToHexString(
                nfcRfTechParams.param.pa.sens_res, SENS_RES_LENGTH);
        } else if (IsDiscTypeB(discType)) {
            techPoll = GetTechPollForTypeB(nfcRfTechParams, tagTechList_[i]);
        } else if (IsDiscTypeF(discType)) {
            unsigned char cTechPoll[F_POLL_LENGTH];
            unsigned char *sensfRes = nfcRfTechParams.param.pf.sensf_res;

            // save the pmm value.
            for (int j = 0; j < SENSF_RES_LENGTH; j++) {
                cTechPoll[j] = static_cast<unsigned char>(sensfRes[j + SENSF_RES_LENGTH]);
            }

            // save the system code.
            if (activated.params.t3t.num_system_codes > 0) {
                unsigned short *pSystemCodes = activated.params.t3t.p_system_codes;
                cTechPoll[POS_NFCF_STSTEM_CODE_HIGH] =
                    static_cast<unsigned char>(*pSystemCodes >> SYSTEM_CODE_SHIFT);
                cTechPoll[POS_NFCF_STSTEM_CODE_LOW] = static_cast<unsigned char>(*pSystemCodes);
            }
            techPoll = KITS::NfcSdkCommon::BytesVecToHexString(cTechPoll, F_POLL_LENGTH);
        } else if (IsDiscTypeV(discType)) {
            unsigned char cTechPoll[2] = {activated.params.i93.afi, activated.params.i93.dsfid};
            techPoll = KITS::NfcSdkCommon::BytesVecToHexString(cTechPoll, I93_POLL_LENGTH);
        } else {
            techPoll = "";
        }
        tagPollBytes_.push_back(techPoll);
    }
}

std::string TagNciAdapter::GetTechActForIsoDep(tNFA_ACTIVATED activated,
                                               tNFC_RF_TECH_PARAMS nfcRfTechParams,
                                               int tech) const
{
    std::string techAct = "";
    if (tech == TagHost::TARGET_TYPE_ISO14443_4) {
        char discType = nfcRfTechParams.mode;
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
    unsigned char protocol = activated.activate_ntf.protocol;
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
            unsigned char techActivated[2] = {activated.params.i93.afi, activated.params.i93.dsfid};
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
        int i = 0;
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

void TagNciAdapter::BuildTagInfo(const tNFA_CONN_EVT_DATA* eventData)
{
    DebugLog("TagNciAdapter::BuildTagInfo, discRstEvtNum_ = %{public}d", discRstEvtNum_);
    if (techListIndex_ >= MAX_NUM_TECHNOLOGY) {
        return;
    }
    if (multiTagTmpTechIdx_ < (MAX_NUM_TECHNOLOGY - 1)) {
        techListIndex_ = multiTagTmpTechIdx_;
    }
    
    tNFA_ACTIVATED activated = eventData->activated;
    GetTechFromData(activated); // techListIndex_ is increased in this func
    std::string tagUid = GetUidFromData(activated);
    GetTechPollFromData(activated);
    GetTechActFromData(activated);

    tagActivatedProtocol_ = activated.activate_ntf.protocol;
    t1tMaxMessageSize_ = GetT1tMaxMessageSize(activated);
    ParseSpecTagType(activated);

    if (discRstEvtNum_ == 0) {
        multiTagTmpTechIdx_ = 0;
        std::unique_ptr<NCI::ITagHost> tagHost = std::make_unique<NCI::TagHost>(tagTechList_,
            tagRfDiscIdList_, tagActivatedProtocols_, tagUid, tagPollBytes_, tagActivatedBytes_);
        NfccHost::TagDiscovered(std::move(tagHost));
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
    tagTechList_.clear();
    tagRfDiscIdList_.clear();
    tagActivatedProtocols_.clear();
    tagPollBytes_.clear();
    tagActivatedBytes_.clear();
    tagDiscIdListOfDiscResult_.clear();
    tagProtocolsOfDiscResult_.clear();
    techListIndex_ = 0;
    multiTagTmpTechIdx_ = 0;
    tagActivatedProtocol_ = NCI_PROTOCOL_UNKNOWN;
    isFelicaLite_ = false;
    isMifareUltralight_ = false;
    isMifareDESFire_ = false;
    isMultiTag_ = false;
    discRstEvtNum_ = 0;
    discNtfIndex_ = 0;
    multiTagTmpTechIdx_ = 0;
    selectedTagIdx_ = 0;
    ResetTimeout();
}

void TagNciAdapter::HandleDiscResult(tNFA_CONN_EVT_DATA* eventData)
{
    tNFC_RESULT_DEVT& discoveryNtf = eventData->disc_result.discovery_ntf;
    DebugLog("TagNciAdapter::HandleDiscResult, discId: %{public}d, protocol: %{public}d",
        discoveryNtf.rf_disc_id, discoveryNtf.protocol);

    tagDiscIdListOfDiscResult_.push_back(discoveryNtf.rf_disc_id);
    tagProtocolsOfDiscResult_.push_back(discoveryNtf.protocol);
    if (discoveryNtf.more == NCI_DISCOVER_NTF_MORE) {
        return;
    }

    uint32_t index = MAX_NUM_TECHNOLOGY;
    for (std::size_t i = 0; i < tagProtocolsOfDiscResult_.size(); i++) {
        if (tagProtocolsOfDiscResult_[i] != NFA_PROTOCOL_NFC_DEP) {
            index = i;
            break;
        }
    }

    if (index >= MAX_NUM_TECHNOLOGY) {
        DebugLog("Has technology NFA_PROTOCOL_NFC_DEP only, don't handle it.");
        return;
    }

    // get the rf interface based on the found technology.
    tNFA_INTF_TYPE rfInterface = NFA_INTERFACE_FRAME;
    int foundTech = tagProtocolsOfDiscResult_[index];
    if (foundTech == NFA_PROTOCOL_ISO_DEP) {
        rfInterface = NFA_INTERFACE_ISO_DEP;
    } else if (foundTech == NFC_PROTOCOL_MIFARE) {
        rfInterface = NFA_INTERFACE_MIFARE;
    }

    // select the rf interface.
    rfDiscoveryMutex_.lock();
    tNFA_STATUS status = nciAdaptations_->NfaSelect(
        (uint8_t)tagDiscIdListOfDiscResult_[index], (tNFA_NFC_PROTOCOL)foundTech, rfInterface);
    if (status != NFA_STATUS_OK) {
        ErrorLog("TagNciAdapter::HandleDiscResult: NfaSelect error = 0x%{public}X", status);
    }
    connectedProtocol_ = foundTech;
    connectedTagDiscId_ = tagDiscIdListOfDiscResult_[index];
    rfDiscoveryMutex_.unlock();
}

void TagNciAdapter::OnRfDiscLock()
{
    rfDiscoveryMutex_.lock();
}

void TagNciAdapter::OffRfDiscLock()
{
    rfDiscoveryMutex_.unlock();
}

void TagNciAdapter::SetNciAdaptations(std::shared_ptr<INfcNci> nciAdaptations)
{
    nciAdaptations_ = nciAdaptations;
}

bool TagNciAdapter::IsNdefFormattable()
{
    DebugLog("check IsNdefFormattable");
    const int IDX_NDEF_FORMAT_1ST = 7;
    const int IDX_NDEF_FORMAT_2ND = 8;
    if (tagActivatedProtocol_ == NFA_PROTOCOL_T1T || tagActivatedProtocol_ == NFA_PROTOCOL_T5T ||
        tagActivatedProtocol_ == NFC_PROTOCOL_MIFARE) {
        return true;
    } else if (tagActivatedProtocol_ == NFA_PROTOCOL_T2T) {
        return isMifareUltralight_;
    } else if (tagActivatedProtocol_ == NFA_PROTOCOL_T3T) {
        return isFelicaLite_;
    } else if (tagActivatedProtocol_ == NFA_PROTOCOL_ISO_DEP && isMifareDESFire_) {
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
        NFC::SynchronizeGuard guard(filedCheckEvent_);
        filedCheckEvent_.NotifyOne();
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
}

int TagNciAdapter::GetT1tMaxMessageSize(tNFA_ACTIVATED activated) const
{
    int t1tMaxMessageSize;
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

tNFA_INTF_TYPE TagNciAdapter::GetRfInterface(int protocol) const
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
    return true;
}

void TagNciAdapter::SetIsMultiTag(bool isMultiTag)
{
    isMultiTag_ = isMultiTag;
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
    if (idx >= MAX_NUM_TECHNOLOGY) {
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

tNFA_STATUS TagNciAdapter::DoSelectForMultiTag(int currIdx)
{
    tNFA_STATUS result = NFA_STATUS_FAILED;
    if (currIdx == INVALID_TAG_INDEX) {
        ErrorLog("TagNciAdapter::DoSelectForMultiTag: is NFC_DEP");
        return result;
    }
    InfoLog("TagNciAdapter::DoSelectForMultiTag: protocol = 0x%{public}X", multiTagDiscProtocol_[currIdx]);

    if (multiTagDiscProtocol_[currIdx] == NFA_PROTOCOL_ISO_DEP) {
        result = NFA_Select(multiTagDiscId_[currIdx], multiTagDiscProtocol_[currIdx], NFA_INTERFACE_ISO_DEP);
    } else if (multiTagDiscProtocol_[currIdx] == NFA_PROTOCOL_MIFARE) {
        result = NFA_Select(multiTagDiscId_[currIdx], multiTagDiscProtocol_[currIdx], NFA_INTERFACE_MIFARE);
    } else {
        result = NFA_Select(multiTagDiscId_[currIdx], multiTagDiscProtocol_[currIdx], NFA_INTERFACE_FRAME);
    }
    return result;
}

void TagNciAdapter::SelectTheFirstTag()
{
    unsigned int currIdx = INVALID_TAG_INDEX;
    for (unsigned int i = 0; i < discNtfIndex_; i++) {
        InfoLog("TagNciAdapter::SelectTheFirstTag index = %{public}d discId = 0x%{public}X protocol = 0x%{public}X",
            i, multiTagDiscId_[i], multiTagDiscProtocol_[i]);
        if (multiTagDiscProtocol_[i] != NFA_PROTOCOL_NFC_DEP) {
            selectedTagIdx_ = i;
            currIdx = i;
            break;
        }
    }
    tNFA_STATUS result = DoSelectForMultiTag(currIdx);
    InfoLog("TagNciAdapter::SelectTheFirstTag result = %{public}d", result);
}

void TagNciAdapter::SelectTheNextTag()
{
    if (discRstEvtNum_ == 0) {
        ErrorLog("TagNciAdapter::SelectTheNextTag: next tag does not exist");
        return;
    }
    unsigned int currIdx = INVALID_TAG_INDEX;
    discRstEvtNum_--;
    for (unsigned int i = 0; i < discNtfIndex_; i++) {
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
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
