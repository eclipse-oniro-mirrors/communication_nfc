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
        return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_NOT_INITIALIZED;
    }

    /* find the tag in the hmap */
    std::weak_ptr<NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("Connect, tagRfDiscId not found");
        return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_DISCONNECT;
    }

    if (!tag.lock()->IsTagFieldOn()) {
        ErrorLog("Connect, IsTagFieldOn error");
        return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_DISCONNECT;
    }

    if (tag.lock()->Connect(technology)) {
        return NFC::KITS::NfcErrorCode::NFC_SUCCESS;
    } else {
        ErrorLog("Connect, unallowd call error");
        return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_DISCONNECT;
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
        return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_NOT_INITIALIZED;
    }

    /* find the tag in the hmap */
    std::weak_ptr<NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("Reconnect, tagRfDiscId not found");
        return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_DISCONNECT;
    }

    if (tag.lock()->Reconnect()) {
        return NFC::KITS::NfcErrorCode::NFC_SUCCESS;
    } else {
        ErrorLog("Reconnect, unallowd call error");
        return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_DISCONNECT;
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
bool TagSession::SetTimeout(uint32_t timeout, int technology)
{
    if (technology < 0 || technology >= MAX_TECH) {
        ErrorLog("SetTimeout, invalid technology %{public}d", technology);
        return false;
    }
    g_techTimeout[technology] = timeout;
    return true;
}

uint32_t TagSession::GetTimeout(int technology)
{
    if (technology < 0 || technology >= MAX_TECH) {
        ErrorLog("GetTimeout, invalid technology %{public}d", technology);
        return 0;
    }
    return g_techTimeout[technology];
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
/**
 * @brief To send the data to the tagRfDiscId.
 * @param tagRfDiscId the rf disc id of tag
 * @param data the sent data
 * @param raw to send whether original data or un-original data
 * @return The response result from the host tag
 */
std::unique_ptr<TagRwResponse> TagSession::SendRawFrame(int tagRfDiscId, std::string data, bool raw)
{
    DebugLog("Send Raw(%{public}d) Frame", raw);
    // Check if NFC is enabled
    if (!nfcService_.lock()->IsNfcEnabled()) {
        ErrorLog("SendRawFrame, IsTagFieldOn error");
        return nullptr;
    }

    /* find the tag in the hmap */
    std::weak_ptr<NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("SendRawFrame, tagRfDiscId not found");
        return nullptr;
    }
    std::unique_ptr<TagRwResponse> resResult = std::make_unique<TagRwResponse>();
    // Check if length is within limits
    if (data.length() > static_cast<unsigned int>(GetMaxTransceiveLength(tag.lock()->GetConnectedTech()))) {
        resResult->SetResult(TagRwResponse::STATUS_EXCEEDED_LENGTH);
        return resResult;
    }
    std::string response;
    int result = tag.lock()->Transceive(data, response);
    if (!response.empty()) {
        resResult->SetResult(TagRwResponse::STATUS_SUCCESS);
    } else if (result == 1) {  // result == 1 means that Tag lost
        resResult->SetResult(TagRwResponse::STATUS_TAG_LOST);
    } else {
        ErrorLog("SendRawFrame, response error, result %{public}d", result);
        resResult->SetResult(TagRwResponse::STATUS_FAILURE);
    }
    resResult->SetResData(response);
    return resResult;
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
        return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_NOT_INITIALIZED;
    }

    /* find the tag in the hmap */
    std::weak_ptr<NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("NdefWrite, tagRfDiscId not found");
        return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_IO;
    }

    if (msg.empty()) {
        ErrorLog("NdefWrite, msg.empty error");
        return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_INVALID_PARAM;
    }

    if (tag.lock()->WriteNdef(msg)) {
        return NFC::KITS::NfcErrorCode::NFC_SUCCESS;
    }
    return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_IO;
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
        return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_NOT_INITIALIZED;
    }

    /* find the tag in the hmap */
    std::weak_ptr<NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("NdefMakeReadOnly, tagRfDiscId not found");
        return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_IO;
    }

    if (tag.lock()->SetNdefReadOnly()) {
        return NFC::KITS::NfcErrorCode::NFC_SUCCESS;
    }
    return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_IO;
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
        return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_NOT_INITIALIZED;
    }

    /* find the tag in the hmap */
    std::weak_ptr<NFC::NCI::ITagHost> tag = tagDispatcher_.lock()->FindTagHost(tagRfDiscId);
    if (tag.expired()) {
        ErrorLog("FormatNdef, tagRfDiscId not found");
        return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_IO;
    }

    if (tag.lock()->FormatNdef(key)) {
        return NFC::KITS::NfcErrorCode::NFC_SUCCESS;
    }
    return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_IO;
}
/**
 * @brief Checking the host tag is Read only
 * @param technology the tag technology
 * @return true - ReadOnly; false - No Read Only
 */
bool TagSession::CanMakeReadOnly(int technology)
{
    return nfccHost_.lock()->CanMakeReadOnly(technology);
}
/**
 * @brief Get Max Transceive Length
 * @param technology the tag technology
 * @return Max Transceive Length
 */
int TagSession::GetMaxTransceiveLength(int technology)
{
    return g_maxTransLength[technology];
}
/**
 * @brief Checking the NfccHost whether It supported the extended Apdus
 * @return true - yes; false - no
 */
bool TagSession::IsSupportedApdusExtended()
{
    return nfccHost_.lock()->GetExtendedLengthApdusSupported();
}

int32_t TagSession::Dump(int32_t fd, const std::vector<std::u16string>& args)
{
    std::string info = GetDumpInfo();
    int ret = dprintf(fd, "%s\n", info.c_str());
    if (ret < 0) {
        ErrorLog("TagSession Dump ret = %{public}d", ret);
        return NFC::KITS::NfcErrorCode::NFC_SER_ERROR_IO;
    }
    return NFC::KITS::NfcErrorCode::NFC_SUCCESS;
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
