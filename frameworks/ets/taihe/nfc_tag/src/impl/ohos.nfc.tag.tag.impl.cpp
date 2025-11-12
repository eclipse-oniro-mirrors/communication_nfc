/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ohos.nfc.tag.tag.impl.hpp"
#include "ohos.nfc.tag.tag.ndef.impl.hpp"
#include "nfctech.impl.hpp"
#include "tagSession.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"

#include "ani_common_want.h"
#include "ani_data_ability_helper.h"
#include "ani_remote_object.h"
#include "loghelper.h"
#include "common_fun_ani.h"
#include "element_name.h"
#include "nfc_taihe_tag_event.h"
#include "taginfo.h"
#include "nfc_taihe_util.h"
#include "nfc_sdk_common.h"

#include "nfca_tag.h"
#include "nfcb_tag.h"
#include "nfcf_tag.h"
#include "iso15693_tag.h" // type V
#include "isodep_tag.h"
#include "ndef_tag.h"
#include "mifare_classic_tag.h"
#include "mifare_ultralight_tag.h"
#include "ndef_formatable_tag.h"
#include "barcode_tag.h"

using namespace taihe;
using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;
using namespace OHOS::NFC::KITS;
using namespace ohos::nfc::tag;

const uint16_t MAX_ARRAY_LEN = 512;

namespace {
class TagSessionImpl {
public:
    TagSessionImpl()
    {
        InfoLog("TagSessionImpl constructor enter");
    }

    void connect()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("TagSession nullptr");
            return;
        }
        tagSession_->Connect();
    }

    void resetConnection()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("TagSession nullptr");
            return;
        }
        tagSession_->Close();
    }

    bool isConnected()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("TagSession nullptr");
            return false;
        }
        return tagSession_->IsConnected();
    }

    void setTimeout(int32_t timeout)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("TagSession nullptr");
            return;
        }
        tagSession_->SetTimeout(timeout);
    }

    int32_t getTimeout()
    {
        int timeout = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("TagSession nullptr");
            return timeout;
        }
        tagSession_->GetTimeout(timeout);
        return timeout;
    }

    int32_t getMaxTransmitSize()
    {
        int maxSize = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("TagSession nullptr");
            return maxSize;
        }
        tagSession_->GetMaxSendCommandLength(maxSize);
        return maxSize;
    }

    array<int32_t> transmitImpl(array_view<int32_t> data)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("TagSession nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        std::string hexRespData;
        std::string hexCmdData = NfcTaiheUtil::TaiheArrayToHexString(data);
        tagSession_->SendCommand(hexCmdData, true, hexRespData);
        return NfcTaiheUtil::HexStringToTaiheArray(hexRespData);
    }

    int64_t getTagSessionImpl()
    {
        return reinterpret_cast<int64_t>(this);
    }

    void setTagSession(std::shared_ptr<BasicTagSession> tagSession)
    {
        tagSession_ = tagSession;
    }

private:
    std::shared_ptr<BasicTagSession> tagSession_ = nullptr;
};

::tagSession::TagSession MakeTagSession()
{
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return make_holder<TagSessionImpl, ::tagSession::TagSession>();
}

class NfcATagImpl {
public:
    NfcATagImpl()
    {
        InfoLog("tagA constructor enter");
    }

    int32_t getSak()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagA nullptr");
            return 0;
        }
        return tagSession_->GetSak();
    }

    array<int32_t> getAtqa()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagA nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        return NfcTaiheUtil::HexStringToTaiheArray(tagSession_->GetAtqa());
    }

    void connect()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagA nullptr");
            return;
        }
        tagSession_->Connect();
    }

    void resetConnection()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagA nullptr");
            return;
        }
        tagSession_->Close();
    }

    bool isConnected()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagA nullptr");
            return false;
        }
        return tagSession_->IsConnected();
    }

    void setTimeout(int32_t timeout)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagA nullptr");
            return;
        }
        tagSession_->SetTimeout(timeout);
    }

    int32_t getTimeout()
    {
        int timeout = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("tagA nullptr");
            return timeout;
        }
        tagSession_->GetTimeout(timeout);
        return timeout;
    }

    int32_t getMaxTransmitSize()
    {
        int maxSize = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("tagA nullptr");
            return maxSize;
        }
        tagSession_->GetMaxSendCommandLength(maxSize);
        return maxSize;
    }

    array<int32_t> transmitImpl(array_view<int32_t> data)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagA nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        std::string hexRespData;
        std::string hexCmdData = NfcTaiheUtil::TaiheArrayToHexString(data);
        tagSession_->SendCommand(hexCmdData, true, hexRespData);
        return NfcTaiheUtil::HexStringToTaiheArray(hexRespData);
    }

    int64_t getTagSessionImpl()
    {
        return reinterpret_cast<int64_t>(this);
    }

    void setTagSession(std::shared_ptr<NfcATag> tagSession)
    {
        tagSession_ = tagSession;
    }

private:
    std::shared_ptr<NfcATag> tagSession_ = nullptr;
};

class NfcBTagImpl {
public:
    NfcBTagImpl()
    {
        InfoLog("tagB constructor enter");
    }

    array<int32_t> getRespAppData()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagB nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        return NfcTaiheUtil::HexStringToTaiheArray(tagSession_->GetAppData());
    }

    array<int32_t> getRespProtocol()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagB nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        return NfcTaiheUtil::HexStringToTaiheArray(tagSession_->GetProtocolInfo());
    }

    void connect()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagB nullptr");
            return;
        }
        tagSession_->Connect();
    }

    void resetConnection()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagB nullptr");
            return;
        }
        tagSession_->Close();
    }

    bool isConnected()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagB nullptr");
            return false;
        }
        return tagSession_->IsConnected();
    }

    void setTimeout(int32_t timeout)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagB nullptr");
            return;
        }
        tagSession_->SetTimeout(timeout);
    }

    int32_t getTimeout()
    {
        int timeout = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("tagB nullptr");
            return timeout;
        }
        tagSession_->GetTimeout(timeout);
        return timeout;
    }

    int32_t getMaxTransmitSize()
    {
        int maxSize = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("tagB nullptr");
            return maxSize;
        }
        tagSession_->GetMaxSendCommandLength(maxSize);
        return maxSize;
    }

    array<int32_t> transmitImpl(array_view<int32_t> data)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagB nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        std::string hexRespData;
        std::string hexCmdData = NfcTaiheUtil::TaiheArrayToHexString(data);
        tagSession_->SendCommand(hexCmdData, true, hexRespData);
        return NfcTaiheUtil::HexStringToTaiheArray(hexRespData);
    }

    int64_t getTagSessionImpl()
    {
        return reinterpret_cast<int64_t>(this);
    }

    void setTagSession(std::shared_ptr<NfcBTag> tagSession)
    {
        tagSession_ = tagSession;
    }

private:
    std::shared_ptr<NfcBTag> tagSession_ = nullptr;
};

class NfcFTagImpl {
public:
    NfcFTagImpl()
    {
        InfoLog("tagF constructor enter");
    }

    array<int32_t> getSystemCode()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagF nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        return NfcTaiheUtil::HexStringToTaiheArray(tagSession_->getSystemCode());
    }

    array<int32_t> getPmm()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagF nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        return NfcTaiheUtil::HexStringToTaiheArray(tagSession_->getPmm());
    }

    void connect()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagF nullptr");
            return;
        }
        tagSession_->Connect();
    }

    void resetConnection()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagF nullptr");
            return;
        }
        tagSession_->Close();
    }

    bool isConnected()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagF nullptr");
            return false;
        }
        return tagSession_->IsConnected();
    }

    void setTimeout(int32_t timeout)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagF nullptr");
            return;
        }
        tagSession_->SetTimeout(timeout);
    }

    int32_t getTimeout()
    {
        int timeout = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("tagF nullptr");
            return timeout;
        }
        tagSession_->GetTimeout(timeout);
        return timeout;
    }

    int32_t getMaxTransmitSize()
    {
        int maxSize = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("tagF nullptr");
            return maxSize;
        }
        tagSession_->GetMaxSendCommandLength(maxSize);
        return maxSize;
    }

    array<int32_t> transmitImpl(array_view<int32_t> data)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagF nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        std::string hexRespData;
        std::string hexCmdData = NfcTaiheUtil::TaiheArrayToHexString(data);
        tagSession_->SendCommand(hexCmdData, true, hexRespData);
        return NfcTaiheUtil::HexStringToTaiheArray(hexRespData);
    }

    int64_t getTagSessionImpl()
    {
        return reinterpret_cast<int64_t>(this);
    }

    void setTagSession(std::shared_ptr<NfcFTag> tagSession)
    {
        tagSession_ = tagSession;
    }

private:
    std::shared_ptr<NfcFTag> tagSession_ = nullptr;
};

class NfcVTagImpl {
public:
    NfcVTagImpl()
    {
        InfoLog("tagV constructor enter");
    }

    int32_t getResponseFlags()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagV nullptr");
            return 0;
        }
        return tagSession_->GetRespFlags();
    }

    int32_t getDsfId()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagV nullptr");
            return 0;
        }
        return tagSession_->GetDsfId();
    }

    void connect()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagV nullptr");
            return;
        }
        tagSession_->Connect();
    }

    void resetConnection()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagV nullptr");
            return;
        }
        tagSession_->Close();
    }

    bool isConnected()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagV nullptr");
            return false;
        }
        return tagSession_->IsConnected();
    }

    void setTimeout(int32_t timeout)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagV nullptr");
            return;
        }
        tagSession_->SetTimeout(timeout);
    }

    int32_t getTimeout()
    {
        int timeout = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("tagV nullptr");
            return timeout;
        }
        tagSession_->GetTimeout(timeout);
        return timeout;
    }

    int32_t getMaxTransmitSize()
    {
        int maxSize = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("tagV nullptr");
            return maxSize;
        }
        tagSession_->GetMaxSendCommandLength(maxSize);
        return maxSize;
    }

    array<int32_t> transmitImpl(array_view<int32_t> data)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("tagV nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        std::string hexRespData;
        std::string hexCmdData = NfcTaiheUtil::TaiheArrayToHexString(data);
        tagSession_->SendCommand(hexCmdData, true, hexRespData);
        return NfcTaiheUtil::HexStringToTaiheArray(hexRespData);
    }

    int64_t getTagSessionImpl()
    {
        return reinterpret_cast<int64_t>(this);
    }

    void setTagSession(std::shared_ptr<Iso15693Tag> tagSession)
    {
        tagSession_ = tagSession;
    }

private:
    std::shared_ptr<Iso15693Tag> tagSession_ = nullptr;
};

class IsoDepTagImpl {
public:
    IsoDepTagImpl()
    {
        InfoLog("IsoDepTag constructor enter");
    }

    array<int32_t> getHistoricalBytes()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("IsoDepTag nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        return NfcTaiheUtil::HexStringToTaiheArray(tagSession_->GetHistoricalBytes());
    }

    array<int32_t> getHiLayerResponse()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("IsoDepTag nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        return NfcTaiheUtil::HexStringToTaiheArray(tagSession_->GetHiLayerResponse());
    }

    bool isExtendedApduSupportedImpl()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("IsoDepTag nullptr");
            return false;
        }
        bool isSupported = false;
        tagSession_->IsExtendedApduSupported(isSupported);
        return isSupported;
    }

    void connect()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("IsoDepTag nullptr");
            return;
        }
        tagSession_->Connect();
    }

    void resetConnection()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("IsoDepTag nullptr");
            return;
        }
        tagSession_->Close();
    }

    bool isConnected()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("IsoDepTag nullptr");
            return false;
        }
        return tagSession_->IsConnected();
    }

    void setTimeout(int32_t timeout)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("IsoDepTag nullptr");
            return;
        }
        tagSession_->SetTimeout(timeout);
    }

    int32_t getTimeout()
    {
        int timeout = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("IsoDepTag nullptr");
            return timeout;
        }
        tagSession_->GetTimeout(timeout);
        return timeout;
    }

    int32_t getMaxTransmitSize()
    {
        int maxSize = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("IsoDepTag nullptr");
            return maxSize;
        }
        tagSession_->GetMaxSendCommandLength(maxSize);
        return maxSize;
    }

    array<int32_t> transmitImpl(array_view<int32_t> data)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("IsoDepTag nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        std::string hexRespData;
        std::string hexCmdData = NfcTaiheUtil::TaiheArrayToHexString(data);
        tagSession_->SendCommand(hexCmdData, true, hexRespData);
        return NfcTaiheUtil::HexStringToTaiheArray(hexRespData);
    }

    int64_t getTagSessionImpl()
    {
        return reinterpret_cast<int64_t>(this);
    }

    void setTagSession(std::shared_ptr<IsoDepTag> tagSession)
    {
        tagSession_ = tagSession;
    }

private:
    std::shared_ptr<IsoDepTag> tagSession_ = nullptr;
};

::tag::NdefRecord makeNdefRecord(std::shared_ptr<NdefRecord> ndefRecord)
{
    ::tag::NdefRecord recordRet{};
    if (ndefRecord == nullptr) {
        ErrorLog("ndefRecord nullptr.");
        return recordRet;
    }
    recordRet.tnf = static_cast<int32_t>(ndefRecord->tnf_);
    recordRet.rtdType = NfcTaiheUtil::HexStringToTaiheArray(ndefRecord->tagRtdType_);
    recordRet.id = NfcTaiheUtil::HexStringToTaiheArray(ndefRecord->id_);
    recordRet.payload = NfcTaiheUtil::HexStringToTaiheArray(ndefRecord->payload_);
    return recordRet;
}

class NdefMessageImpl {
public:
    NdefMessageImpl()
    {
        InfoLog("NdefMessage constructor enter");
    }

    array<::tag::NdefRecord> getNdefRecords()
    {
        std::vector<::tag::NdefRecord> records {};
        if (ndefMessage_ == nullptr) {
            ErrorLog("ndef message nullptr");
            return array<::tag::NdefRecord>(array_view<::tag::NdefRecord>(records));
        }
        std::vector<std::shared_ptr<NdefRecord>> ndefRecords = ndefMessage_->GetNdefRecords();
        for (uint16_t i = 0; i < ndefRecords.size(); i++) {
            ::tag::NdefRecord record = makeNdefRecord(ndefRecords[i]);
            records.push_back(record);
        }
        return array<::tag::NdefRecord>(array_view<::tag::NdefRecord>(records));
    }

    int64_t getNdefMessageImpl()
    {
        return reinterpret_cast<int64_t>(this);
    }

    void setNdefMessage(std::shared_ptr<NdefMessage> ndefMessage)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        ndefMessage_ = ndefMessage;
    }

    std::shared_ptr<NdefMessage> getNdefMessage()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return ndefMessage_;
    }

private:
    std::mutex mutex_ {};
    std::shared_ptr<NdefMessage> ndefMessage_ = nullptr;
};

class NdefTagImpl {
public:
    NdefTagImpl()
    {
        InfoLog("NdefTag constructor enter");
    }

    ::tag::NfcForumType getNdefTagType()
    {
        InfoLog("enter");
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return ::tag::NfcForumType::from_value(static_cast<int>(EmNfcForumType::NFC_FORUM_TYPE_UNKNOWN));
        }
        
        EmNfcForumType type = tagSession_->GetNdefTagType();
        InfoLog("type %{public}d", static_cast<int>(type));
        return ::tag::NfcForumType::from_value(1);
    }

    ::nfctech::NdefMessage getNdefMessage()
    {
        InfoLog("enter");
        ::nfctech::NdefMessage ndefMessage = make_holder<NdefMessageImpl, ::nfctech::NdefMessage>();
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return ndefMessage;
        }
        std::shared_ptr<NdefMessage> ndefMsg = tagSession_->GetCachedNdefMsg();

        auto implPtr = reinterpret_cast<NdefMessageImpl *>(ndefMessage->getNdefMessageImpl());
        if (implPtr == nullptr) {
            ErrorLog("implPtr nullptr");
            return ndefMessage;
        }
        implPtr->setNdefMessage(ndefMsg);
        return ndefMessage;
    }

    bool isNdefWritable()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return false;
        }
        return tagSession_->IsNdefWritable();
    }

    ::nfctech::NdefMessage readNdefImpl()
    {
        InfoLog("enter");
        ::nfctech::NdefMessage ndefMessage = make_holder<NdefMessageImpl, ::nfctech::NdefMessage>();
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return ndefMessage;
        }
        std::shared_ptr<NdefMessage> ndefMsg = nullptr;
        tagSession_->ReadNdef(ndefMsg);

        auto implPtr = reinterpret_cast<NdefMessageImpl *>(ndefMessage->getNdefMessageImpl());
        if (implPtr == nullptr) {
            ErrorLog("implPtr nullptr");
            return ndefMessage;
        }
        implPtr->setNdefMessage(ndefMsg);
        return ndefMessage;
    }

    void writeNdefImpl(::nfctech::weak::NdefMessage msg)
    {
        InfoLog("enter");
        auto implPtr = reinterpret_cast<NdefMessageImpl *>(msg->getNdefMessageImpl());
        if (implPtr == nullptr) {
            ErrorLog("implPtr nullptr");
            return;
        }
        std::shared_ptr<NdefMessage> ndefMsg = implPtr->getNdefMessage();
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return;
        }
        tagSession_->WriteNdef(ndefMsg);
    }

    bool canSetReadOnly()
    {
        bool canSetReadOnly = false;
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return canSetReadOnly;
        }
        tagSession_->IsEnableReadOnly(canSetReadOnly);
        return canSetReadOnly;
    }

    void setReadOnlyImpl()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return;
        }
        tagSession_->EnableReadOnly();
    }

    ::taihe::string getNdefTagTypeString(::tag::NfcForumType type)
    {
        std::string typeStr;
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return ::taihe::string(typeStr);
        }
        typeStr = tagSession_->GetNdefTagTypeString(static_cast<EmNfcForumType>(type.get_value()));
        return ::taihe::string(typeStr);
    }

    void connect()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return;
        }
        tagSession_->Connect();
    }

    void resetConnection()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return;
        }
        tagSession_->Close();
    }

    bool isConnected()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return false;
        }
        return tagSession_->IsConnected();
    }

    void setTimeout(int32_t timeout)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return;
        }
        tagSession_->SetTimeout(timeout);
    }

    int32_t getTimeout()
    {
        int timeout = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return timeout;
        }
        tagSession_->GetTimeout(timeout);
        return timeout;
    }

    int32_t getMaxTransmitSize()
    {
        int maxSize = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return maxSize;
        }
        tagSession_->GetMaxSendCommandLength(maxSize);
        return maxSize;
    }

    array<int32_t> transmitImpl(array_view<int32_t> data)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        std::string hexRespData;
        std::string hexCmdData = NfcTaiheUtil::TaiheArrayToHexString(data);
        tagSession_->SendCommand(hexCmdData, true, hexRespData);
        return NfcTaiheUtil::HexStringToTaiheArray(hexRespData);
    }

    int64_t getTagSessionImpl()
    {
        return reinterpret_cast<int64_t>(this);
    }

    void setTagSession(std::shared_ptr<NdefTag> tagSession)
    {
        tagSession_ = tagSession;
    }

private:
    std::shared_ptr<NdefTag> tagSession_ = nullptr;
};

class MifareClassicTagImpl {
public:
    MifareClassicTagImpl()
    {
        InfoLog("MifareClassicTag constructor enter");
    }

    void authenticateSectorImpl(int32_t sectorIndex, array_view<int32_t> key, bool isKeyA)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return;
        }
        std::string keyStr = NfcTaiheUtil::TaiheArrayToHexString(key);
        tagSession_->AuthenticateSector(sectorIndex, keyStr, isKeyA);
    }

    array<int32_t> readSingleBlockImpl(uint32_t blockIndex)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        std::string hexRespData;
        tagSession_->ReadSingleBlock(blockIndex, hexRespData);
        return NfcTaiheUtil::HexStringToTaiheArray(hexRespData);
    }

    void writeSingleBlockImpl(uint32_t blockIndex, array_view<int32_t> data)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return;
        }
        std::string hexData = NfcTaiheUtil::TaiheArrayToHexString(data);
        tagSession_->WriteSingleBlock(blockIndex, hexData);
    }

    void incrementBlockImpl(uint32_t blockIndex, int32_t value)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return;
        }
        tagSession_->IncrementBlock(blockIndex, value);
    }

    void decrementBlockImpl(uint32_t blockIndex, int32_t value)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return;
        }
        tagSession_->DecrementBlock(blockIndex, value);
    }

    void transferToBlockImpl(uint32_t blockIndex)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return;
        }
        tagSession_->TransferToBlock(blockIndex);
    }

    void restoreFromBlockImpl(uint32_t blockIndex)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return;
        }
        tagSession_->RestoreFromBlock(blockIndex);
    }

    int32_t getSectorCount()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return 0;
        }
        return tagSession_->GetSectorCount();
    }

    int32_t getBlockCountInSector(int32_t sectorIndex)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return 0;
        }
        return tagSession_->GetBlockCountInSector(sectorIndex);
    }

    ::tag::MifareClassicType getType()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return ::tag::MifareClassicType::from_value(static_cast<int>(MifareClassicTag::EmType::TYPE_UNKNOWN));
        }
        return ::tag::MifareClassicType::from_value(static_cast<int>(tagSession_->GetMifareTagType()));
    }

    int32_t getTagSize()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return 0;
        }
        return tagSession_->GetSize();
    }

    bool isEmulatedTag()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return false;
        }
        return tagSession_->IsEmulated();
    }

    int32_t getBlockIndex(int32_t sectorIndex)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return 0;
        }
        return tagSession_->GetBlockIndexFromSector(sectorIndex);
    }

    int32_t getSectorIndex(int32_t blockIndex)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return 0;
        }
        return tagSession_->GetSectorIndexFromBlock(blockIndex);
    }

    void connect()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return;
        }
        tagSession_->Connect();
    }

    void resetConnection()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return;
        }
        tagSession_->Close();
    }

    bool isConnected()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return false;
        }
        return tagSession_->IsConnected();
    }

    void setTimeout(int32_t timeout)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return;
        }
        tagSession_->SetTimeout(timeout);
    }

    int32_t getTimeout()
    {
        int timeout = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return timeout;
        }
        tagSession_->GetTimeout(timeout);
        return timeout;
    }

    int32_t getMaxTransmitSize()
    {
        int maxSize = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return maxSize;
        }
        tagSession_->GetMaxSendCommandLength(maxSize);
        return maxSize;
    }

    array<int32_t> transmitImpl(array_view<int32_t> data)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("MifareClassicTag nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        std::string hexRespData;
        std::string hexCmdData = NfcTaiheUtil::TaiheArrayToHexString(data);
        tagSession_->SendCommand(hexCmdData, true, hexRespData);
        return NfcTaiheUtil::HexStringToTaiheArray(hexRespData);
    }

    int64_t getTagSessionImpl()
    {
        return reinterpret_cast<int64_t>(this);
    }

    void setTagSession(std::shared_ptr<MifareClassicTag> tagSession)
    {
        tagSession_ = tagSession;
    }

private:
    std::shared_ptr<MifareClassicTag> tagSession_ = nullptr;
};

class MifareUltralightTagImpl {
public:
    MifareUltralightTagImpl()
    {
        InfoLog("mifareul constructor enter");
    }

    array<int32_t> readMultiplePagesImpl(uint32_t pageIndex)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("mifareul nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        std::string hexRespData;
        tagSession_->ReadMultiplePages(pageIndex, hexRespData);
        return NfcTaiheUtil::HexStringToTaiheArray(hexRespData);
    }

    void writeSinglePageImpl(uint32_t pageIndex, array_view<int32_t> data)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("mifareul nullptr");
            return;
        }
        std::string dataStr = NfcTaiheUtil::TaiheArrayToHexString(data);
        tagSession_->WriteSinglePage(pageIndex, dataStr);
    }

    ::tag::MifareUltralightType getType()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("mifareul nullptr");
            return ::tag::MifareUltralightType::from_value(static_cast<int>(MifareUltralightTag::EmType::TYPE_UNKNOWN));
        }
        return ::tag::MifareUltralightType::from_value(static_cast<int>(tagSession_->GetType()));
    }

    void connect()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("mifareul nullptr");
            return;
        }
        tagSession_->Connect();
    }

    void resetConnection()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("mifareul nullptr");
            return;
        }
        tagSession_->Close();
    }

    bool isConnected()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("mifareul nullptr");
            return false;
        }
        return tagSession_->IsConnected();
    }

    void setTimeout(int32_t timeout)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("mifareul nullptr");
            return;
        }
        tagSession_->SetTimeout(timeout);
    }

    int32_t getTimeout()
    {
        int timeout = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("mifareul nullptr");
            return timeout;
        }
        tagSession_->GetTimeout(timeout);
        return timeout;
    }

    int32_t getMaxTransmitSize()
    {
        int maxSize = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("mifareul nullptr");
            return maxSize;
        }
        tagSession_->GetMaxSendCommandLength(maxSize);
        return maxSize;
    }

    array<int32_t> transmitImpl(array_view<int32_t> data)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("mifareul nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        std::string hexRespData;
        std::string hexCmdData = NfcTaiheUtil::TaiheArrayToHexString(data);
        tagSession_->SendCommand(hexCmdData, true, hexRespData);
        return NfcTaiheUtil::HexStringToTaiheArray(hexRespData);
    }

    int64_t getTagSessionImpl()
    {
        return reinterpret_cast<int64_t>(this);
    }

    void setTagSession(std::shared_ptr<MifareUltralightTag> tagSession)
    {
        tagSession_ = tagSession;
    }

private:
    std::shared_ptr<MifareUltralightTag> tagSession_ = nullptr;
};

class NdefFormatableTagImpl {
public:
    NdefFormatableTagImpl()
    {
        InfoLog("NdefFormatableTag constructor enter");
    }

    void formatImpl(::nfctech::weak::NdefMessage message)
    {
        InfoLog("enter");
        auto implPtr = reinterpret_cast<NdefMessageImpl *>(message->getNdefMessageImpl());
        if (implPtr == nullptr) {
            ErrorLog("implPtr nullptr");
            return;
        }
        std::shared_ptr<NdefMessage> ndefMsg = implPtr->getNdefMessage();
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return;
        }
        tagSession_->Format(ndefMsg);
    }

    void formatReadOnlyImpl(::nfctech::weak::NdefMessage message)
    {
        InfoLog("enter");
        auto implPtr = reinterpret_cast<NdefMessageImpl *>(message->getNdefMessageImpl());
        if (implPtr == nullptr) {
            ErrorLog("implPtr nullptr");
            return;
        }
        std::shared_ptr<NdefMessage> ndefMsg = implPtr->getNdefMessage();
        if (tagSession_ == nullptr) {
            ErrorLog("NdefTag nullptr");
            return;
        }
        tagSession_->FormatReadOnly(ndefMsg);
    }

    void connect()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("NdefFormatableTag nullptr");
            return;
        }
        tagSession_->Connect();
    }

    void resetConnection()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("NdefFormatableTag nullptr");
            return;
        }
        tagSession_->Close();
    }

    bool isConnected()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("NdefFormatableTag nullptr");
            return false;
        }
        return tagSession_->IsConnected();
    }

    void setTimeout(int32_t timeout)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("NdefFormatableTag nullptr");
            return;
        }
        tagSession_->SetTimeout(timeout);
    }

    int32_t getTimeout()
    {
        int timeout = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("NdefFormatableTag nullptr");
            return timeout;
        }
        tagSession_->GetTimeout(timeout);
        return timeout;
    }

    int32_t getMaxTransmitSize()
    {
        int maxSize = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("NdefFormatableTag nullptr");
            return maxSize;
        }
        tagSession_->GetMaxSendCommandLength(maxSize);
        return maxSize;
    }

    array<int32_t> transmitImpl(array_view<int32_t> data)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("NdefFormatableTag nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        std::string hexRespData;
        std::string hexCmdData = NfcTaiheUtil::TaiheArrayToHexString(data);
        tagSession_->SendCommand(hexCmdData, true, hexRespData);
        return NfcTaiheUtil::HexStringToTaiheArray(hexRespData);
    }

    int64_t getTagSessionImpl()
    {
        return reinterpret_cast<int64_t>(this);
    }

    void setTagSession(std::shared_ptr<NdefFormatableTag> tagSession)
    {
        tagSession_ = tagSession;
    }

private:
    std::shared_ptr<NdefFormatableTag> tagSession_ = nullptr;
};

class BarcodeTagImpl {
public:
    BarcodeTagImpl()
    {
        InfoLog("BarcodeTagImpl constructor enter");
    }

    array_view<uint8_t> getBarcodeImpl()
    {
        InfoLog("enter");
        if (tagSession_ == nullptr) {
            ErrorLog("BarcodeTag nullptr");
            return array_view<uint8_t>();
        }
        std::string barcodeData = tagSession_->GetBarcode();
        std::vector<unsigned char> bytes;
        NfcSdkCommon::HexStringToBytes(barcodeData, bytes);
        return array_view<uint8_t>(bytes);
    }

    void connect()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("BarcodeTag nullptr");
            return;
        }
        tagSession_->Connect();
    }

    void resetConnection()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("BarcodeTag nullptr");
            return;
        }
        tagSession_->Close();
    }

    bool isConnected()
    {
        if (tagSession_ == nullptr) {
            ErrorLog("BarcodeTag nullptr");
            return false;
        }
        return tagSession_->IsConnected();
    }

    void setTimeout(int32_t timeout)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("BarcodeTag nullptr");
            return;
        }
        tagSession_->SetTimeout(timeout);
    }

    int32_t getTimeout()
    {
        int timeout = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("BarcodeTag nullptr");
            return timeout;
        }
        tagSession_->GetTimeout(timeout);
        return timeout;
    }

    int32_t getMaxTransmitSize()
    {
        int maxSize = 0;
        if (tagSession_ == nullptr) {
            ErrorLog("BarcodeTag nullptr");
            return maxSize;
        }
        tagSession_->GetMaxSendCommandLength(maxSize);
        return maxSize;
    }

    array<int32_t> transmitImpl(array_view<int32_t> data)
    {
        if (tagSession_ == nullptr) {
            ErrorLog("BarcodeTag nullptr");
            return array<int32_t>(array_view<int32_t>());
        }
        std::string hexRespData;
        std::string hexCmdData = NfcTaiheUtil::TaiheArrayToHexString(data);
        tagSession_->SendCommand(hexCmdData, true, hexRespData);
        return NfcTaiheUtil::HexStringToTaiheArray(hexRespData);
    }

    int64_t getTagSessionImpl()
    {
        return reinterpret_cast<int64_t>(this);
    }

    void setTagSession(std::shared_ptr<BarcodeTag> tagSession)
    {
        tagSession_ = tagSession;
    }

private:
    std::shared_ptr<BarcodeTag> tagSession_ = nullptr;
};

std::shared_ptr<TagInfo> GetTagInfo(::tag::TagInfo const& tagInfo)
{
    std::vector<int> tagTechList = NfcTaiheUtil::TaiheIntArrayToIntVec(tagInfo.technology);
    std::string tagUid = NfcTaiheUtil::TaiheArrayToHexString(tagInfo.uid);
    std::vector<PacMap> tagTechExtrasData;
    if (tagInfo.extrasData.size() > MAX_ARRAY_LEN) {
        ErrorLog("extrasData size exceed");
        return nullptr;
    }
    for (uint16_t i = 0; i < tagInfo.extrasData.size(); i++) {
        PacMap pacMap;
        AnalysisPacMap(pacMap, get_env(), reinterpret_cast<ani_object>(tagInfo.extrasData[i]));
        tagTechExtrasData.push_back(pacMap);
    }
    OHOS::sptr<OHOS::IRemoteObject> tagServiceIface =
        AniGetNativeRemoteObject(get_env(), reinterpret_cast<ani_object>(tagInfo.remoteTagService));
    return std::make_shared<TagInfo>
        (tagTechList, tagTechExtrasData, tagUid, tagInfo.tagRfDiscId, tagServiceIface);
}

::nfctech::NfcATag getNfcA(::tag::TagInfo const& tagInfo)
{
    InfoLog("getNfcA enter");
    std::shared_ptr<NfcATag> nfcATag = std::make_shared<NfcATag> (GetTagInfo(tagInfo));
    ::nfctech::NfcATag nfcATagTaihe = make_holder<NfcATagImpl, ::nfctech::NfcATag>();
    auto implPtr = reinterpret_cast<NfcATagImpl *>(nfcATagTaihe->getTagSessionImpl());
    if (implPtr == nullptr) {
        ErrorLog("fail to get nfc A tag impl ptr.");
        return nfcATagTaihe;
    }
    implPtr->setTagSession(nfcATag);
    return nfcATagTaihe;
}

::nfctech::NfcBTag getNfcB(::tag::TagInfo const& tagInfo)
{
    InfoLog("getNfcB enter");
    std::shared_ptr<NfcBTag> nfcBTag = std::make_shared<NfcBTag> (GetTagInfo(tagInfo));
    ::nfctech::NfcBTag nfcBTagTaihe = make_holder<NfcBTagImpl, ::nfctech::NfcBTag>();
    auto implPtr = reinterpret_cast<NfcBTagImpl *>(nfcBTagTaihe->getTagSessionImpl());
    if (implPtr == nullptr) {
        ErrorLog("fail to get nfc B tag impl ptr.");
        return nfcBTagTaihe;
    }
    implPtr->setTagSession(nfcBTag);
    return nfcBTagTaihe;
}

::nfctech::NfcFTag getNfcF(::tag::TagInfo const& tagInfo)
{
    InfoLog("getNfcF enter");
    std::shared_ptr<NfcFTag> nfcFTag = std::make_shared<NfcFTag> (GetTagInfo(tagInfo));
    ::nfctech::NfcFTag nfcFTagTaihe = make_holder<NfcFTagImpl, ::nfctech::NfcFTag>();
    auto implPtr = reinterpret_cast<NfcFTagImpl *>(nfcFTagTaihe->getTagSessionImpl());
    if (implPtr == nullptr) {
        ErrorLog("fail to get nfc F tag impl ptr.");
        return nfcFTagTaihe;
    }
    implPtr->setTagSession(nfcFTag);
    return nfcFTagTaihe;
}

::nfctech::NfcVTag getNfcV(::tag::TagInfo const& tagInfo)
{
    InfoLog("getNfcV enter");
    std::shared_ptr<Iso15693Tag> nfcVTag = std::make_shared<Iso15693Tag> (GetTagInfo(tagInfo));
    ::nfctech::NfcVTag nfcVTagTaihe = make_holder<NfcVTagImpl, ::nfctech::NfcVTag>();
    auto implPtr = reinterpret_cast<NfcVTagImpl *>(nfcVTagTaihe->getTagSessionImpl());
    if (implPtr == nullptr) {
        ErrorLog("fail to get nfc V tag impl ptr.");
        return nfcVTagTaihe;
    }
    implPtr->setTagSession(nfcVTag);
    return nfcVTagTaihe;
}

::nfctech::IsoDepTag getIsoDep(::tag::TagInfo const& tagInfo)
{
    InfoLog("getIsoDep enter");
    std::shared_ptr<IsoDepTag> isoDepTag = std::make_shared<IsoDepTag> (GetTagInfo(tagInfo));
    ::nfctech::IsoDepTag isoDepTagTaihe = make_holder<IsoDepTagImpl, ::nfctech::IsoDepTag>();
    auto implPtr = reinterpret_cast<IsoDepTagImpl *>(isoDepTagTaihe->getTagSessionImpl());
    if (implPtr == nullptr) {
        ErrorLog("fail to get nfc IsoDep tag impl ptr.");
        return isoDepTagTaihe;
    }
    implPtr->setTagSession(isoDepTag);
    return isoDepTagTaihe;
}

::nfctech::NdefTag getNdef(::tag::TagInfo const& tagInfo)
{
    InfoLog("getNdef enter");
    std::shared_ptr<NdefTag> ndefTag = std::make_shared<NdefTag> (GetTagInfo(tagInfo));
    ::nfctech::NdefTag ndefTagTaihe = make_holder<NdefTagImpl, ::nfctech::NdefTag>();
    auto implPtr = reinterpret_cast<NdefTagImpl *>(ndefTagTaihe->getTagSessionImpl());
    if (implPtr == nullptr) {
        ErrorLog("fail to get nfc Ndef tag impl ptr.");
        return ndefTagTaihe;
    }
    implPtr->setTagSession(ndefTag);
    return ndefTagTaihe;
}

::nfctech::MifareClassicTag getMifareClassic(::tag::TagInfo const& tagInfo)
{
    InfoLog("getMifareClassic enter");
    std::shared_ptr<MifareClassicTag> mifareClassicTag = std::make_shared<MifareClassicTag> (GetTagInfo(tagInfo));
    ::nfctech::MifareClassicTag mifareClassicTaihe = make_holder<MifareClassicTagImpl, ::nfctech::MifareClassicTag>();
    auto implPtr = reinterpret_cast<MifareClassicTagImpl *>(mifareClassicTaihe->getTagSessionImpl());
    if (implPtr == nullptr) {
        ErrorLog("fail to get nfc MifareClassic tag impl ptr.");
        return mifareClassicTaihe;
    }
    implPtr->setTagSession(mifareClassicTag);
    return mifareClassicTaihe;
}

::nfctech::MifareUltralightTag getMifareUltralight(::tag::TagInfo const& tagInfo)
{
    InfoLog("getMifareUltralight enter");
    std::shared_ptr<MifareUltralightTag> mifareUlTag = std::make_shared<MifareUltralightTag> (GetTagInfo(tagInfo));
    ::nfctech::MifareUltralightTag mifareUlTaihe =
        make_holder<MifareUltralightTagImpl, ::nfctech::MifareUltralightTag>();
    auto implPtr = reinterpret_cast<MifareUltralightTagImpl *>(mifareUlTaihe->getTagSessionImpl());
    if (implPtr == nullptr) {
        ErrorLog("fail to get nfc MifareUltralight tag impl ptr.");
        return mifareUlTaihe;
    }
    implPtr->setTagSession(mifareUlTag);
    return mifareUlTaihe;
}

::nfctech::NdefFormatableTag getNdefFormatable(::tag::TagInfo const& tagInfo)
{
    InfoLog("getNdefFormatable enter");
    std::shared_ptr<NdefFormatableTag> ndefFormatableTag = std::make_shared<NdefFormatableTag> (GetTagInfo(tagInfo));
    ::nfctech::NdefFormatableTag ndefFormatableTaihe =
        make_holder<NdefFormatableTagImpl, ::nfctech::NdefFormatableTag>();
    auto implPtr = reinterpret_cast<NdefFormatableTagImpl *>(ndefFormatableTaihe->getTagSessionImpl());
    if (implPtr == nullptr) {
        ErrorLog("fail to get nfc NdefFormatable tag impl ptr.");
        return ndefFormatableTaihe;
    }
    implPtr->setTagSession(ndefFormatableTag);
    return ndefFormatableTaihe;
}

::nfctech::BarcodeTag getBarcodeTag(::tag::TagInfo const& tagInfo)
{
    InfoLog("getBarcodeTag enter");
    std::shared_ptr<BarcodeTag> barcodeTag = std::make_shared<BarcodeTag> (GetTagInfo(tagInfo));
    ::nfctech::BarcodeTag barcodeTagTaihe = make_holder<BarcodeTagImpl, ::nfctech::BarcodeTag>();
    auto implPtr = reinterpret_cast<BarcodeTagImpl *>(barcodeTagTaihe->getTagSessionImpl());
    if (implPtr == nullptr) {
        ErrorLog("fail to get nfc Barcode tag impl ptr.");
        return barcodeTagTaihe;
    }
    implPtr->setTagSession(barcodeTag);
    return barcodeTagTaihe;
}

::tag::TagInfo getTagInfo(uintptr_t want)
{
    InfoLog("getTagInfo enter");
    ::tag::TagInfo tagInfo = {};
    Want tagInfoWant;
    bool result = UnwrapWant(get_env(), reinterpret_cast<ani_object>(want), tagInfoWant);
    if (!result) {
        ErrorLog("fail to unwrap want");
        return tagInfo;
    }
    return tagInfo;
}

void registerForegroundDispatch(uintptr_t elementName, array_view<uint32_t> discTech,
    callback_view<void(uintptr_t err, ::tag::TagInfo const& tagInfo)> callback)
{
    InfoLog("enter");

    ElementName element;
    CommonFunAni::ParseElementName(get_env(), reinterpret_cast<ani_object>(elementName), element);

    if (discTech.size() > MAX_ARRAY_LEN) {
        ErrorLog("discTech size exceed.");
        return;
    }
    std::vector<uint32_t> dataVec = {};
    for (uint16_t i = 0; i < discTech.size(); i++) {
        dataVec.push_back(discTech[i]);
    }
    TagFgEventRegister::GetInstance().Register(element, dataVec, callback);
}

void unregisterForegroundDispatch(uintptr_t elementName)
{
    InfoLog("enter");

    ElementName element;
    CommonFunAni::ParseElementName(get_env(), reinterpret_cast<ani_object>(elementName), element);
    TagFgEventRegister::GetInstance().Unregister(element);
}

void onReaderMode(uintptr_t elementName, array_view<uint32_t> discTech,
    callback_view<void(uintptr_t err, ::tag::TagInfo const& tagInfo)> callback)
{
    InfoLog("enter");

    ElementName element;
    CommonFunAni::ParseElementName(get_env(), reinterpret_cast<ani_object>(elementName), element);

    if (discTech.size() > MAX_ARRAY_LEN) {
        ErrorLog("discTech size exceed.");
        return;
    }
    std::vector<uint32_t> dataVec = {};
    for (uint16_t i = 0; i < discTech.size(); i++) {
        dataVec.push_back(discTech[i]);
    }
    TagRmEventRegister::GetInstance().Register(element, dataVec, callback);
}

void offReaderMode(uintptr_t elementName, optional_view<
    callback<void(uintptr_t err, ::tag::TagInfo const& tagInfo)>> callback)
{
    InfoLog("enter");

    ElementName element;
    CommonFunAni::ParseElementName(get_env(), reinterpret_cast<ani_object>(elementName), element);
    TagRmEventRegister::GetInstance().Unregister(element);
}

::tag::NdefRecord makeUriRecord(::taihe::string_view uri)
{
    InfoLog("enter");
    std::shared_ptr<NdefRecord> ndefRecord = NdefMessage::MakeUriRecord(uri.c_str());
    return makeNdefRecord(ndefRecord);
}

::tag::NdefRecord makeTextRecord(::taihe::string_view text, ::taihe::string_view locale)
{
    InfoLog("enter");
    std::shared_ptr<NdefRecord> ndefRecord = NdefMessage::MakeTextRecord(text.c_str(), locale.c_str());
    return makeNdefRecord(ndefRecord);
}

::tag::NdefRecord makeApplicationRecord(::taihe::string_view bundleName)
{
    InfoLog("enter");
    std::shared_ptr<NdefRecord> ndefRecord = NdefMessage::MakeApplicationRecord(bundleName.c_str());
    return makeNdefRecord(ndefRecord);
}

::tag::NdefRecord makeMimeRecord(::taihe::string_view mimeType, array_view<int32_t> mimeData)
{
    InfoLog("enter");
    std::string mimeDataStr = NfcTaiheUtil::TaiheArrayToHexString(mimeData);
    std::shared_ptr<NdefRecord> ndefRecord = NdefMessage::MakeMimeRecord(mimeType.c_str(), mimeDataStr);
    return makeNdefRecord(ndefRecord);
}

::tag::NdefRecord makeExternalRecord(
    ::taihe::string_view domainName, ::taihe::string_view type, array_view<int32_t> externalData)
{
    InfoLog("enter");
    std::string externalDataStr = NfcTaiheUtil::TaiheArrayToHexString(externalData);
    std::shared_ptr<NdefRecord> ndefRecord =
        NdefMessage::MakeExternalRecord(domainName.c_str(), type.c_str(), externalDataStr);
    return makeNdefRecord(ndefRecord);
}

array<int32_t> messageToBytes(::nfctech::weak::NdefMessage ndefMessage)
{
    InfoLog("enter");
    auto implPtr = reinterpret_cast<NdefMessageImpl *>(ndefMessage->getNdefMessageImpl());
    if (implPtr == nullptr) {
        ErrorLog("implPtr nullptr");
        return array<int32_t>(array_view<int32_t>());
    }
    std::string buffer = NdefMessage::MessageToString(implPtr->getNdefMessage());
    return NfcTaiheUtil::HexStringToTaiheArray(buffer);
}

::nfctech::NdefMessage createNdefMessageByData(array_view<int32_t> data)
{
    InfoLog("enter");
    ::nfctech::NdefMessage ndefMessage = make_holder<NdefMessageImpl, ::nfctech::NdefMessage>();
    auto implPtr = reinterpret_cast<NdefMessageImpl *>(ndefMessage->getNdefMessageImpl());
    if (implPtr == nullptr) {
        ErrorLog("implPtr nullptr");
        return ndefMessage;
    }
    std::string rawData = NfcTaiheUtil::TaiheArrayToHexString(data);
    std::shared_ptr<NdefMessage> ndefMsg = NdefMessage::GetNdefMessage(rawData);
    implPtr->setNdefMessage(ndefMsg);
    return ndefMessage;
}

std::vector<std::shared_ptr<NdefRecord>> parseTaiheNdefRecord(array_view<::tag::NdefRecord> record)
{
    std::vector<std::shared_ptr<NdefRecord>> records{};
    if (record.size() > MAX_ARRAY_LEN) {
        ErrorLog("record length exceed.");
        return records;
    }
    for (uint16_t i = 0; i < record.size(); i++) {
        std::shared_ptr<NdefRecord> ndefRecord = std::make_shared<NdefRecord>();
        ndefRecord->id_ = NfcTaiheUtil::TaiheArrayToHexString(record[i].id);
        ndefRecord->payload_ = NfcTaiheUtil::TaiheArrayToHexString(record[i].payload);
        ndefRecord->tagRtdType_ = NfcTaiheUtil::TaiheArrayToHexString(record[i].rtdType);
        ndefRecord->tnf_ = static_cast<short>(record[i].tnf);
        records.push_back(ndefRecord);
    }
    return records;
}

::nfctech::NdefMessage createNdefMessageByRecords(array_view<::tag::NdefRecord> record)
{
    InfoLog("enter");
    ::nfctech::NdefMessage ndefMessage = make_holder<NdefMessageImpl, ::nfctech::NdefMessage>();
    auto implPtr = reinterpret_cast<NdefMessageImpl *>(ndefMessage->getNdefMessageImpl());
    if (implPtr == nullptr) {
        ErrorLog("implPtr nullptr");
        return ndefMessage;
    }
    std::vector<std::shared_ptr<NdefRecord>> records = parseTaiheNdefRecord(record);
    std::shared_ptr<NdefMessage> ndefMsg = NdefMessage::GetNdefMessage(records);
    implPtr->setNdefMessage(ndefMsg);
    return ndefMessage;
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_MakeTagSession(MakeTagSession);
TH_EXPORT_CPP_API_getNfcA(getNfcA);
TH_EXPORT_CPP_API_getNfcB(getNfcB);
TH_EXPORT_CPP_API_getNfcF(getNfcF);
TH_EXPORT_CPP_API_getNfcV(getNfcV);
TH_EXPORT_CPP_API_getIsoDep(getIsoDep);
TH_EXPORT_CPP_API_getNdef(getNdef);
TH_EXPORT_CPP_API_getMifareClassic(getMifareClassic);
TH_EXPORT_CPP_API_getMifareUltralight(getMifareUltralight);
TH_EXPORT_CPP_API_getNdefFormatable(getNdefFormatable);
TH_EXPORT_CPP_API_getBarcodeTag(getBarcodeTag);
TH_EXPORT_CPP_API_getTagInfo(getTagInfo);
TH_EXPORT_CPP_API_registerForegroundDispatch(registerForegroundDispatch);
TH_EXPORT_CPP_API_unregisterForegroundDispatch(unregisterForegroundDispatch);
TH_EXPORT_CPP_API_onReaderMode(onReaderMode);
TH_EXPORT_CPP_API_offReaderMode(offReaderMode);
TH_EXPORT_CPP_API_makeUriRecord(makeUriRecord);
TH_EXPORT_CPP_API_makeTextRecord(makeTextRecord);
TH_EXPORT_CPP_API_makeApplicationRecord(makeApplicationRecord);
TH_EXPORT_CPP_API_makeMimeRecord(makeMimeRecord);
TH_EXPORT_CPP_API_makeExternalRecord(makeExternalRecord);
TH_EXPORT_CPP_API_messageToBytes(messageToBytes);
TH_EXPORT_CPP_API_createNdefMessageByData(createNdefMessageByData);
TH_EXPORT_CPP_API_createNdefMessageByRecords(createNdefMessageByRecords);
// NOLINTEND
