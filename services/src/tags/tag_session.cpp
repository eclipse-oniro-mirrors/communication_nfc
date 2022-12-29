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
#include "tag_session.h"
#include "itag_host.h"
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace TAG {
const std::string DUMP_LINE = "---------------------------";
const std::string DUMP_END = "\n";

// NFC_A = 1 ~ NDEF_FORMATABLE = 10
const uint32_t MAX_TECH = 12;
uint32_t g_techTimeout[MAX_TECH] = {0};
uint32_t g_maxTransLength[MAX_TECH] = {0, 253, 253, 261, 255, 253, 0, 0, 253, 253, 0, 0};

TagSession::TagSession(std::shared_ptr<INfcService> service)
    : nfcService_(service)
{
    nfccHost_ = service->GetNfccHost();
    tagDispatcher_ = service->GetTagDispatcher();
}

TagSession::~TagSession()
{
}

/**
 * @brief To connect the tagRfDiscId by technology.
 * @param tagRfDiscId the rf disc id of tag
 * @param technology the tag technology
 * @return the result to connect the tag
 */
int TagSession::Connect(int tagRfDiscId, int technology)
{
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("Connect, IsNfcEnabled error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }

    /* find the tag in the hmap */
    std::weak_ptr<NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("Connect, tagRfDiscId not found");
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }

    if (!tag.lock()->IsTagFieldOn()) {
        ErrorLog("Connect, IsTagFieldOn error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_LOST;
    }

    if (tag.lock()->Connect(technology)) {
        return NFC::KITS::ErrorCode::ERR_NONE;
    } else {
        ErrorLog("Connect, unallowd call error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_IO_FAILED;
    }
}
/**
 * @brief To reconnect the tagRfDiscId.
 * @param tagRfDiscId the rf disc id of tag
 * @return the result to reconnect the tag
 */
int TagSession::Reconnect(int tagRfDiscId)
{
    // Check if NFC is enabled
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("Reconnect, IsTagFieldOn error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }

    /* find the tag in the hmap */
    std::weak_ptr<NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("Reconnect, tagRfDiscId not found");
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }

    if (tag.lock()->Reconnect()) {
        return NFC::KITS::ErrorCode::ERR_NONE;
    } else {
        ErrorLog("Reconnect, unallowd call error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_IO_FAILED;
    }
}
/**
 * @brief To disconnect the tagRfDiscId.
 * @param tagRfDiscId the rf disc id of tag
 */
void TagSession::Disconnect(int tagRfDiscId)
{
    // Check if NFC is enabled
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("Disconnect, IsTagFieldOn error");
        return;
    }

    /* find the tag in the hmap */
    std::weak_ptr<NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("Disconnect, tagRfDiscId not found");
        return;
    }
    tag.lock()->Disconnect();
}
int TagSession::SetTimeout(int timeout, int technology)
{
    if (technology < 0 || technology >= MAX_TECH) {
        ErrorLog("SetTimeout, invalid technology %{public}d", technology);
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    g_techTimeout[technology] = timeout;
    return NFC::KITS::ErrorCode::ERR_NONE;
}

int TagSession::GetTimeout(int technology, int &timeout)
{
    if (technology < 0 || technology >= MAX_TECH) {
        ErrorLog("GetTimeout, invalid technology %{public}d", technology);
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    timeout = g_techTimeout[technology];
    return NFC::KITS::ErrorCode::ERR_NONE;
}
/**
 * @brief Get the TechList of the tagRfDiscId.
 * @param tagRfDiscId the rf disc id of tag
 * @return TechList
 */
std::vector<int> TagSession::GetTechList(int tagRfDiscId)
{
    std::vector<int> techList;
    // Check if NFC is enabled
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("GetTechList, IsTagFieldOn error");
        return techList;
    }

    /* find the tag in the hmap */
    std::weak_ptr<NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("GetTechList, tagRfDiscId not found");
        return techList;
    }
    return tag.lock()->GetTechList();
}
/**
 * @brief Checking the tagRfDiscId is present.
 * @param tagRfDiscId the rf disc id of tag
 * @return true - Presnet; the other - No Presnet
 */
bool TagSession::IsTagFieldOn(int tagRfDiscId)
{
    // Check if NFC is enabled
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("IsTagFieldOn, IsTagFieldOn error");
        return false;
    }

    /* find the tag in the hmap */
    std::weak_ptr<NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("IsTagFieldOn, tagRfDiscId not found");
        return false;
    }
    return tag.lock()->IsTagFieldOn();
}
/**
 * @brief Checking the tagRfDiscId is a Ndef Tag.
 * @param tagRfDiscId the rf disc id of tag
 * @return true - Ndef Tag; the other - No Ndef Tag
 */
bool TagSession::IsNdef(int tagRfDiscId)
{
    // Check if NFC is enabled
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("IsNdef, IsTagFieldOn error");
        return false;
    }

    /* find the tag in the hmap */
    std::weak_ptr<NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("IsNdef, tagRfDiscId not found");
        return false;
    }
    std::vector<int> ndefInfo;
    return tag.lock()->IsNdefMsgContained(ndefInfo);
}

int TagSession::SendRawFrame(int tagRfDiscId, std::string hexCmdData, bool raw, std::string &hexRespData)
{
    DebugLog("Send Raw(%{public}d) Frame", raw);
    // Check if NFC is enabled
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("SendRawFrame, IsTagFieldOn error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }

    std::weak_ptr<NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("SendRawFrame, tagRfDiscId not found");
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }

    // Check if length is within limits
    int maxSize = 0;
    GetMaxTransceiveLength(tag.lock()->GetConnectedTech(), maxSize);
    if (KITS::NfcSdkCommon::GetHexStrBytesLen(hexCmdData) > maxSize) {
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }

    int result = tag.lock()->Transceive(hexCmdData, hexRespData);
    if (!hexRespData.empty()) {
        return NFC::KITS::ErrorCode::ERR_NONE;
    } else if (result == 1) {  // result == 1 means that Tag lost
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_LOST;
    }
    return NFC::KITS::ErrorCode::ERR_TAG_STATE_IO_FAILED;
}
/**
 * @brief Reading from the host tag
 * @param tagRfDiscId the rf disc id of tag
 * @return the read data
 */
std::string TagSession::NdefRead(int tagRfDiscId)
{
    // Check if NFC is enabled
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("NdefRead, IsTagFieldOn error");
        return "";
    }
    /* find the tag in the hmap */
    std::weak_ptr<NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("NdefRead, tagRfDiscId not found");
        return "";
    }
    return tag.lock()->ReadNdef();
}
/**
 * @brief Writing the data into the host tag.
 * @param tagRfDiscId the rf disc id of tag
 * @param msg the wrote data
 * @return the Writing Result
 */
int TagSession::NdefWrite(int tagRfDiscId, std::string msg)
{
    // Check if NFC is enabled
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("NdefWrite, IsTagFieldOn error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }

    /* find the tag in the hmap */
    std::weak_ptr<NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("NdefWrite, tagRfDiscId not found");
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }

    if (msg.empty()) {
        ErrorLog("NdefWrite, msg.empty error");
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }

    if (tag.lock()->WriteNdef(msg)) {
        return NFC::KITS::ErrorCode::ERR_NONE;
    }
    return NFC::KITS::ErrorCode::ERR_TAG_STATE_IO_FAILED;
}
/**
 * @brief Making the host tag to read only.
 * @param tagRfDiscId the rf disc id of tag
 * @return the making result
 */
int TagSession::NdefMakeReadOnly(int tagRfDiscId)
{
    // Check if NFC is enabled
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("NdefMakeReadOnly, IsTagFieldOn error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }

    /* find the tag in the hmap */
    std::weak_ptr<NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("NdefMakeReadOnly, tagRfDiscId not found");
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }

    if (tag.lock()->SetNdefReadOnly()) {
        return NFC::KITS::ErrorCode::ERR_NONE;
    }
    return NFC::KITS::ErrorCode::ERR_TAG_STATE_IO_FAILED;
}
/**
 * @brief format the tag by Ndef
 * @param tagRfDiscId the rf disc id of tag
 * @param key the format key
 * @return the format result
 */
int TagSession::FormatNdef(int tagRfDiscId, const std::string& key)
{
    // Check if NFC is enabled
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("FormatNdef, IsTagFieldOn error");
        return NFC::KITS::ErrorCode::ERR_TAG_STATE_NFC_CLOSED;
    }

    /* find the tag in the hmap */
    std::weak_ptr<NFC::NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("FormatNdef, tagRfDiscId not found");
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }

    if (tag.lock()->FormatNdef(key)) {
        return NFC::KITS::ErrorCode::ERR_NONE;
    }
    return NFC::KITS::ErrorCode::ERR_TAG_STATE_IO_FAILED;
}

int TagSession::CanMakeReadOnly(int ndefType, bool &canSetReadOnly)
{
    canSetReadOnly = nfccHost_.lock()->CanMakeReadOnly(ndefType);
    return NFC::KITS::ErrorCode::ERR_NONE;
}
/**
 * @brief Get Max Transceive Length
 * @param technology the tag technology
 * @return Max Transceive Length
 */
int TagSession::GetMaxTransceiveLength(int technology, int &maxSize)
{
    maxSize = g_maxTransLength[technology];
    return NFC::KITS::ErrorCode::ERR_NONE;
}

int TagSession::IsSupportedApdusExtended(bool &isSupported)
{
    isSupported = nfccHost_.lock()->GetExtendedLengthApdusSupported();
    return NFC::KITS::ErrorCode::ERR_NONE;
}

int32_t TagSession::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    std::string info = GetDumpInfo();
    int ret = dprintf(fd, "%s\n", info.c_str());
    if (ret < 0) {
        ErrorLog("TagSession Dump ret = %{public}d", ret);
        return NFC::KITS::ErrorCode::ERR_TAG_PARAMETERS;
    }
    return NFC::KITS::ErrorCode::ERR_NONE;
}
std::string TagSession::GetDumpInfo()
{
    std::string info;
    return info.append(DUMP_LINE)
        .append(" TAG DUMP ")
        .append(DUMP_LINE)
        .append(DUMP_END)
        .append("NFC_STATE          : ")
        .append(std::to_string(nfcService_.lock()->GetNfcState()))
        .append(DUMP_END)
        .append("SCREEN_STATE       : ")
        .append(std::to_string(nfcService_.lock()->GetScreenState()))
        .append(DUMP_END)
        .append("NCI_VERSION        : ")
        .append(std::to_string(nfcService_.lock()->GetNciVersion()))
        .append(DUMP_END);
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
