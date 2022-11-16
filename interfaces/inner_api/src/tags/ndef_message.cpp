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
#include "ndef_message.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace KITS {
using KITS::NfcSdkCommon;
NdefMessage::NdefMessage(std::vector<std::shared_ptr<NdefRecord>> ndefRecords)
    : ndefRecordList_(std::move(ndefRecords))
{
}

NdefMessage::~NdefMessage()
{
    ndefRecordList_.clear();
}

std::shared_ptr<NdefMessage> NdefMessage::GetNdefMessage(const std::string& data)
{
    std::vector<std::shared_ptr<NdefRecord>> ndefRecords = ParseRecord(data, false);
    if (ndefRecords.empty()) {
        ErrorLog("GetNdefMessage, ndefRecords invalid.");
        return std::shared_ptr<NdefMessage>();
    }
    return GetNdefMessage(ndefRecords);
}

std::shared_ptr<NdefMessage> NdefMessage::GetNdefMessage(std::vector<std::shared_ptr<NdefRecord>> ndefRecords)
{
    return std::make_shared<NdefMessage>(std::move(ndefRecords));
}

std::string NdefMessage::GetTagRtdType(EmRtdType rtdtype)
{
    std::string rtd;
    switch (rtdtype) {
        case EmRtdType::RTD_TEXT:
            rtd = "T";  // 0x54
            break;
        case EmRtdType::RTD_URI:
            rtd = "U";  // 0x55
            break;
        case EmRtdType::RTD_SMART_POSTER:
            rtd = "Sp";  // 0x53, 0x70
            break;
        case EmRtdType::RTD_ALTERNATIVE_CARRIER:
            rtd = "ac";  // 0x61, 0x63
            break;
        case EmRtdType::RTD_HANDOVER_CARRIER:
            rtd = "Hc";  // 0x48, 0x63
            break;
        case EmRtdType::RTD_HANDOVER_REQUEST:
            rtd = "Hr";  // 0x48, 0x72
            break;
        case EmRtdType::RTD_HANDOVER_SELECT:
            rtd = "Hs";  // 0x48, 0x73
            break;
        case EmRtdType::RTD_OHOS_APP:
            rtd = "ohos.com:pkg";  // "ohos.com:pkg"
            break;
        default:
            rtd.clear();
            break;
    }
    return rtd;
}

std::vector<std::shared_ptr<NdefRecord>> NdefMessage::GetNdefRecords() const
{
    return ndefRecordList_;
}

std::shared_ptr<NdefRecord> NdefMessage::MakeUriRecord(const std::string& uriString)
{
    if (uriString.empty()) {
        ErrorLog("MakeExternalRecord, uriString invalid.");
        return std::shared_ptr<NdefRecord>();
    }

    std::string payload = "00";
    std::string uri = uriString;
    for (size_t i = 1; i < gUriPrefix.size() - 1; i++) {
        if (!uriString.compare(0, gUriPrefix[i].size(), gUriPrefix[i])) {
            payload = NfcSdkCommon::UnsignedCharToHexString(i & 0xFF);
            uri = uriString.substr(gUriPrefix[i].size());
            break;
        }
    }

    payload += NfcSdkCommon::StringToHexString(uri);

    std::string id = "";
    std::string tagRtdType = NfcSdkCommon::StringToHexString(GetTagRtdType(EmRtdType::RTD_URI));
    return CreateNdefRecord(TNF_WELL_KNOWN, id, payload, tagRtdType);
}

std::shared_ptr<NdefRecord> NdefMessage::MakeTextRecord(const std::string& text, const std::string& locale)
{
    std::string tagRtdType = NfcSdkCommon::StringToHexString(GetTagRtdType(EmRtdType::RTD_TEXT));
    std::string id = "";
    int localeLen = locale.size() & 0xFF;
    std::string payload = NfcSdkCommon::UnsignedCharToHexString(localeLen);
    payload += NfcSdkCommon::StringToHexString(locale);
    payload += NfcSdkCommon::StringToHexString(text);
    return CreateNdefRecord(TNF_WELL_KNOWN, id, payload, tagRtdType);
}

std::shared_ptr<NdefRecord> NdefMessage::MakeMimeRecord(const std::string& mimeType, const std::string& mimeData)
{
    if (mimeType.empty() || mimeData.empty()) {
        ErrorLog("MakeExternalRecord, mimeType or mimeData invalid.");
        return std::shared_ptr<NdefRecord>();
    }
    std::string id = "";
    size_t t = mimeType.find_first_of('/');
    if (t == 0 || t == (mimeType.size() - 1)) {
        ErrorLog("MakeExternalRecord, mimeType should have major and minor type if '/' exists.");
        return std::shared_ptr<NdefRecord>();
    }
    return CreateNdefRecord(TNF_MIME_MEDIA, id, mimeData, NfcSdkCommon::StringToHexString(mimeType));
}

std::shared_ptr<NdefRecord> NdefMessage::MakeExternalRecord(const std::string& domainName,
                                                            const std::string& serviceName,
                                                            const std::string& externalData)
{
    if (domainName.empty() || serviceName.empty() || externalData.empty()) {
        ErrorLog("MakeExternalRecord, domainName or serviceName invalid.");
        return std::shared_ptr<NdefRecord>();
    }

    std::string domain = domainName;
    std::string service = serviceName;
    domain.erase(0, domain.find_first_not_of("\r\t\n "));
    domain.erase(domain.find_last_not_of("\r\t\n ") + 1);
    transform(domain.begin(), domain.end(), domain.begin(), ::tolower);
    service.erase(0, service.find_first_not_of("\r\t\n "));
    service.erase(service.find_last_not_of("\r\t\n ") + 1);
    transform(service.begin(), service.end(), service.begin(), ::tolower);

    if (domain.empty() || service.empty()) {
        return std::shared_ptr<NdefRecord>();
    }

    std::string tagRtdType = NfcSdkCommon::StringToHexString(domain + ":" + service);
    std::string id = "";

    return CreateNdefRecord(TNF_EXTERNAL_TYPE, id, externalData, tagRtdType);
}

std::string NdefMessage::MessageToString(std::weak_ptr<NdefMessage> ndefMessage)
{
    std::string buffer;
    if (ndefMessage.expired()) {
        ErrorLog("MessageToString, ndefMessage invalid.");
        return buffer;
    }
    for (size_t i = 0; i < ndefMessage.lock()->ndefRecordList_.size(); i++) {
        bool bIsMB = (i == 0);                                                // first record
        bool bIsME = (i == ndefMessage.lock()->ndefRecordList_.size() - 1);  // last record
        NdefRecordToString(ndefMessage.lock()->ndefRecordList_.at(i), buffer, bIsMB, bIsME);
    }
    return buffer;
}

void NdefMessage::NdefRecordToString(std::weak_ptr<NdefRecord> record, std::string& buffer, bool bIsMB, bool bIsME)
{
    if (record.expired()) {
        ErrorLog("NdefRecordToString, record invalid.");
        return;
    }
    std::string payload = record.lock()->payload_;
    uint32_t tnf = record.lock()->tnf_;
    std::string id = record.lock()->id_;
    std::string rtdType = record.lock()->tagRtdType_;
    bool sr = NfcSdkCommon::GetHexStrBytesLen(payload) < SHORT_RECORD_SIZE;
    bool il = (tnf == TNF_EMPTY) ? true : (NfcSdkCommon::GetHexStrBytesLen(id) > 0);
    unsigned char flag = (unsigned char)((bIsMB ? FLAG_MB : 0) | (bIsME ? FLAG_ME : 0)
        | (sr ? FLAG_SR : 0) | (il ? FLAG_IL : 0)) | (char)tnf;
    buffer.append(NfcSdkCommon::UnsignedCharToHexString(flag));
    buffer.append(NfcSdkCommon::IntToHexString(NfcSdkCommon::GetHexStrBytesLen(rtdType)));
    if (sr) {
        buffer.append(NfcSdkCommon::IntToHexString(NfcSdkCommon::GetHexStrBytesLen(payload)));
    } else {
        buffer.append(NfcSdkCommon::IntToHexString(NfcSdkCommon::GetHexStrBytesLen(payload)));
    }
    if (il) {
        buffer.append(NfcSdkCommon::IntToHexString(NfcSdkCommon::GetHexStrBytesLen(id)));
    }

    buffer.append(rtdType);
    buffer.append(id);
    buffer.append(payload);
}

void NdefMessage::ParseRecordLayoutHead(RecordLayout& layout, unsigned char head)
{
    layout.mb = (head & FLAG_MB) != 0;
    layout.me = (head & FLAG_ME) != 0;
    layout.cf = (head & FLAG_CF) != 0;
    layout.sr = (head & FLAG_SR) != 0;
    layout.il = (head & FLAG_IL) != 0;
    layout.tnf = static_cast<short>(head & FLAG_TNF);
}

bool NdefMessage::IsInvalidRecordLayoutHead(RecordLayout& layout, bool isChunkFound,
    uint32_t parsedRecordSize, bool isMbMeIgnored)
{
    if (!layout.mb && parsedRecordSize == 0 && !isChunkFound && !isMbMeIgnored) {
        ErrorLog("IsInvalidRecordLayoutHead, 1st error for mb and size.");
        return true;
    } else if (layout.mb && (parsedRecordSize != 0 || isChunkFound) && !isMbMeIgnored) {
        ErrorLog("IsInvalidRecordLayoutHead, 2nd error for mb and size");
        return true;
    } else if (isChunkFound && layout.il) {
        ErrorLog("IsInvalidRecordLayoutHead, 3rd error for il");
        return true;
    } else if (layout.cf && layout.me) {
        ErrorLog("IsInvalidRecordLayoutHead, 4th error for cf and me");
        return true;
    } else if (isChunkFound && layout.tnf != TNF_UNCHANGED) {
        ErrorLog("IsInvalidRecordLayoutHead, 5th error for tnf");
        return true;
    } else if (!isChunkFound && layout.tnf == TNF_UNCHANGED) {
        ErrorLog("IsInvalidRecordLayoutHead, 6th error for tnf");
        return true;
    }
    return false;
}

void NdefMessage::ParseRecordLayoutLength(RecordLayout& layout, bool isChunkFound,
    const std::string& data, uint32_t& parsedDataIndex)
{
    layout.typeLength = NfcSdkCommon::GetByteFromHexStr(data, parsedDataIndex++) & 0xFF;
    if (layout.sr) {
        layout.payloadLength = NfcSdkCommon::GetByteFromHexStr(data, parsedDataIndex++) & 0xFF;
    } else {
        if (NfcSdkCommon::GetHexStrBytesLen(data) < parsedDataIndex + int(sizeof(int))) {
            layout.payloadLength = 0;
        } else {
            std::string lenString = data.substr(parsedDataIndex * HEX_BYTE_LEN, sizeof(int) * HEX_BYTE_LEN);
            layout.payloadLength = NfcSdkCommon::StringToInt(lenString, NfcSdkCommon::IsLittleEndian());
            parsedDataIndex += sizeof(int);
        }
    }
    layout.idLength = layout.il ? (NfcSdkCommon::GetByteFromHexStr(data, parsedDataIndex++) & 0xFF) : 0;
}

bool NdefMessage::IsRecordLayoutLengthInvalid(RecordLayout& layout, bool isChunkFound)
{
    // for the middle chunks record, need the type length is zero.
    if (isChunkFound && layout.typeLength != 0) {
        ErrorLog("IsInvalidRecordLayoutHead, 1st error for typeLength");
        return true;
    }

    // for the first chunk, expected has type.
    if (layout.cf && !isChunkFound) {
        if (layout.typeLength == 0 && layout.tnf != TNF_UNKNOWN) {
            ErrorLog("IsInvalidRecordLayoutHead, 2nd error for typeLength and tnf");
            return true;
        }
    }

    if (layout.payloadLength > MAX_PAYLOAD_SIZE) {
        ErrorLog("IsInvalidRecordLayoutHead, 3rd error for payloadLength");
        return true;
    }
    return false;
}

std::string NdefMessage::ParseRecordType(RecordLayout& layout, const std::string& data, uint32_t& parsedDataIndex)
{
    if (layout.typeLength <= 0) {
        ErrorLog("IsInvalidRecordLayoutHead, typeLength less than 0.");
        return "";
    }
    if (NfcSdkCommon::GetHexStrBytesLen(data) < parsedDataIndex + layout.typeLength) {
        ErrorLog("data len.%{public}d index.%{public}d rtdtype len.%{public}d error",
            NfcSdkCommon::GetHexStrBytesLen(data), parsedDataIndex, layout.typeLength);
        return "";
    }
    std::string type = data.substr(parsedDataIndex * HEX_BYTE_LEN, layout.typeLength * HEX_BYTE_LEN);
    parsedDataIndex += layout.typeLength;
    return type;
}

std::string NdefMessage::ParseRecordId(RecordLayout& layout, const std::string& data, uint32_t& parsedDataIndex)
{
    if (layout.idLength <= 0) {
        ErrorLog("ParseRecordId, idLength <= 0");
        return "";
    }
    if (NfcSdkCommon::GetHexStrBytesLen(data) < parsedDataIndex + layout.idLength) {
        ErrorLog("data len.%{public}d index.%{public}d id len.%{public}d error",
            NfcSdkCommon::GetHexStrBytesLen(data), parsedDataIndex, layout.idLength);
        return "";
    }
    std::string id = data.substr(parsedDataIndex * HEX_BYTE_LEN, layout.idLength * HEX_BYTE_LEN);
    parsedDataIndex += layout.idLength;
    return id;
}

std::string NdefMessage::ParseRecordPayload(RecordLayout& layout, const std::string& data, uint32_t& parsedDataIndex)
{
    if (layout.payloadLength <= 0) {
        ErrorLog("ParseRecordPayload, payloadLength <= 0");
        return "";
    }
    if (NfcSdkCommon::GetHexStrBytesLen(data) < (parsedDataIndex + layout.payloadLength)) {
        ErrorLog("data len.%{public}d index.%{public}d payload len.%{public}d error",
            NfcSdkCommon::GetHexStrBytesLen(data), parsedDataIndex, layout.payloadLength);
        return "";
    }
    std::string payload = data.substr(parsedDataIndex * HEX_BYTE_LEN, layout.payloadLength * HEX_BYTE_LEN);
    parsedDataIndex += layout.payloadLength;
    return payload;
}

void NdefMessage::SaveRecordChunks(RecordLayout& layout, bool isChunkFound,
    std::vector<std::string>& chunks, char& chunkTnf, const std::string& payload)
{
    // handle for the first chunk.
    if (layout.cf && !isChunkFound) {
        chunks.clear();
        chunkTnf = layout.tnf;
    }

    // save the payload for all(first/middle/last) chunk.
    if (layout.cf || isChunkFound) {
        chunks.push_back(payload);
    }
}

std::string NdefMessage::MergePayloadByChunks(RecordLayout& layout, bool isChunkFound,
    std::vector<std::string>& chunks, char chunkTnf, const std::string& payload)
{
    // it's the last chunk, merge the payload for NdefRecord.
    if (!layout.cf && isChunkFound) {
        std::string mergedPayload;
        for (std::string n : chunks) {
            mergedPayload += n;
        }
        layout.tnf = chunkTnf;
        return mergedPayload;
    }
    return payload;
}

std::shared_ptr<NdefRecord> NdefMessage::CreateNdefRecord(short tnf, const std::string& id,
    const std::string& payload, const std::string& tagRtdType)
{
    bool isValidTnf = CheckTnf(tnf, tagRtdType, id, payload);
    if (!isValidTnf) {
        ErrorLog("CreateNdefRecord, isValidTnf failed.");
        return std::shared_ptr<NdefRecord>();
    }
    std::shared_ptr<NdefRecord> ndefRecord = std::make_shared<NdefRecord>();
    ndefRecord->tnf_ = tnf;
    ndefRecord->id_ = id;
    ndefRecord->payload_ = payload;
    ndefRecord->tagRtdType_ = tagRtdType;
    return ndefRecord;
}

bool NdefMessage::CheckTnf(short tnf, const std::string& tagRtdType, const std::string& id, const std::string& payload)
{
    switch (tnf) {
        case TNF_EMPTY:
            if (!tagRtdType.empty() || !id.empty() || !payload.empty()) {
                ErrorLog("CheckTnf, TNF_EMPTY error.");
                return false;
            }
            break;
        case TNF_WELL_KNOWN:
        case TNF_MIME_MEDIA:
        case TNF_ABSOLUTE_URI:
        case TNF_EXTERNAL_TYPE:
            return true;
        case TNF_UNKNOWN:
        case TNF_RESERVED:
            if (tagRtdType.empty()) {
                return false;
            }
            return true;
        case TNF_UNCHANGED:
            return false;
        default:
            break;
    }
    return false;
}

std::vector<std::shared_ptr<NdefRecord>> NdefMessage::ParseRecord(const std::string& data, bool isMbMeIgnored)
{
    std::vector<std::shared_ptr<NdefRecord>> recordList;
    if (data.empty()) {
        ErrorLog("ParseRecord, raw data empty.");
        return recordList;
    }

    std::string tagRtdType, id;
    std::vector<std::string> chunks;
    bool isChunkFound = false;
    char chunkTnf = 0;
    bool isMessageEnd = false;
    uint32_t parsedDataIndex = 0;
    while (!isMessageEnd) {
        RecordLayout layout;
        ParseRecordLayoutHead(layout, NfcSdkCommon::GetByteFromHexStr(data, parsedDataIndex++));
        isMessageEnd = layout.me;

        if (IsInvalidRecordLayoutHead(layout, isChunkFound, recordList.size(), isMbMeIgnored)) {
            return recordList;
        }

        ParseRecordLayoutLength(layout, isChunkFound, data, parsedDataIndex);

        if (IsRecordLayoutLengthInvalid(layout, isChunkFound)) {
            return recordList;
        }
        
        if (!isChunkFound) {
            // don't parse the type and id for the middle chunks record, allowed them tobe empty.
            tagRtdType = ParseRecordType(layout, data, parsedDataIndex);
            id = ParseRecordId(layout, data, parsedDataIndex);
        }

        // parse the payload.
        std::string payload = ParseRecordPayload(layout, data, parsedDataIndex);
        SaveRecordChunks(layout, isChunkFound, chunks, chunkTnf, payload);
        payload = MergePayloadByChunks(layout, isChunkFound, chunks, chunkTnf, payload);
        if (NfcSdkCommon::GetHexStrBytesLen(payload) > MAX_PAYLOAD_SIZE) {
            ErrorLog("ParseRecord, payload > MAX_PAYLOAD_SIZE");
            return recordList;
        }

        // if not the last chunk, continue to parse again.
        isChunkFound = layout.cf;
        if (isChunkFound) {
            continue;
        }

        // all chunks parsed end, add a new NdefRecord.
        std::shared_ptr<NdefRecord> record = CreateNdefRecord(layout.tnf, id, payload, tagRtdType);
        recordList.push_back(record);

        // isMbMeIgnored is true, means that single record need tobe parsed.
        if (isMbMeIgnored) {
            break;
        }
    }
    return recordList;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
