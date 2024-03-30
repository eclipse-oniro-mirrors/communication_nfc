/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#include "isodep_card_handler.h"

#include "cJSON.h"
#include "file_ex.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace TAG {
IsodepCardHandler::IsodepCardHandler(std::weak_ptr<NCI::INciTagInterface> nciTagProxy)
    : nciTagProxy_(nciTagProxy)
{
    InfoLog("IsodepCardHandler constructor enter.");
}

IsodepCardHandler::~IsodepCardHandler()
{
    InfoLog("IsodepCardHandler destructor enter.");
}

void IsodepCardHandler::InitTransportCardInfo()
{
    if (isInitialized_) {
        DebugLog("already initialized.");
        return;
    }
    cardInfoVec_.clear();
    if (DoJsonRead()) {
        InfoLog("transport card info initialized.");
        isInitialized_ = true;
    }
}

static bool GetCheckApduFromJson(cJSON *json, cJSON *cardInfoEach, TransportCardInfo *cardInfoList, int index)
{
    cJSON *checkApdus = cJSON_GetObjectItemCaseSensitive(cardInfoEach, KEY_APDU_CHECK_APDUS.c_str());
    if (checkApdus == nullptr || !cJSON_IsArray(checkApdus)) {
        ErrorLog("json param not array, or has no field \"checkApdus\", index = %{public}d", index);
        cJSON_Delete(json);
        return false;
    }
    int checkApduArraySize = cJSON_GetArraySize(checkApdus);
    if (checkApduArraySize == 0 || checkApduArraySize > MAX_APDU_ARRAY_SIZE) {
        ErrorLog("illegal array size [%{public}d]", checkApduArraySize);
        cJSON_Delete(json);
        return false;
    }
    for (int i = 0; i < checkApduArraySize; ++i) {
        cardInfoList[index].checkApdus.push_back(cJSON_GetArrayItem(checkApdus, i)->valuestring);
    }
    return true;
}

static bool GetBalanceApduFromJson(cJSON *json, cJSON *cardInfoEach, TransportCardInfo *cardInfoList, int index)
{
    cJSON *balanceApdus = cJSON_GetObjectItemCaseSensitive(cardInfoEach, KEY_APDU_BALANCE_APDUS.c_str());
    if (balanceApdus == nullptr || !cJSON_IsArray(balanceApdus)) {
        WarnLog("json param not array, or has no field \"balanceApdus\", index = %{public}d", index);
    } else {
        int balanceApduArraySize = cJSON_GetArraySize(balanceApdus);
        if (balanceApduArraySize == 0 || balanceApduArraySize > MAX_APDU_ARRAY_SIZE) {
            ErrorLog("illegal array size [%{public}d]", balanceApduArraySize);
            cJSON_Delete(json);
            return false;
        }
        for (int i = 0; i < balanceApduArraySize; ++i) {
            cardInfoList[index].balanceApdus.push_back(cJSON_GetArrayItem(balanceApdus, i)->valuestring);
        }
    }
    return true;
}

static bool GetEachCardInfoFromJson(cJSON *json, cJSON *cardInfo, TransportCardInfo *cardInfoList)
{
    cJSON *cardInfoEach = nullptr;
    int index = 0;
    cJSON_ArrayForEach(cardInfoEach, cardInfo) {
        if (index >= MAX_CARD_INFO_VEC_LEN) {
            ErrorLog("index exceeds");
            cJSON_Delete(json);
            return false;
        }
        cJSON *name = cJSON_GetObjectItemCaseSensitive(cardInfoEach, KEY_APDU_NAME.c_str());
        if (name == nullptr || !cJSON_IsString(name)) {
            ErrorLog("json param not string, or has no field \"name\", index = %{public}d", index);
            cJSON_Delete(json);
            return false;
        }
        cardInfoList[index].name = name->valuestring;

        cJSON *aid = cJSON_GetObjectItemCaseSensitive(cardInfoEach, KEY_APDU_AID.c_str());
        if (aid == nullptr || !cJSON_IsString(aid)) {
            WarnLog("json param not string, or has no field \"aid\", index = %{public}d", index);
        } else {
            cardInfoList[index].aid = aid->valuestring;
        }

        if (!GetCheckApduFromJson(json, cardInfoEach, cardInfoList, index)) {
            ErrorLog("fail to get check apdu array from json.");
            return false;
        }

        if (!GetBalanceApduFromJson(json, cardInfoEach, cardInfoList, index)) {
            ErrorLog("fail to get balance apdu array from json.");
            return false;
        }

        cJSON *rspContains = cJSON_GetObjectItemCaseSensitive(cardInfoEach, KEY_APDU_RSP_CONTAINS.c_str());
        if (rspContains == nullptr || !cJSON_IsString(rspContains)) {
            WarnLog("json param not string, or has no fild \"rspContain\", index = %{public}d", index);
        } else {
            cardInfoList[index].rspContain = rspContains->valuestring;
        }

        index++;
    }
    return true;
}

bool IsodepCardHandler::DoJsonRead()
{
    InfoLog("Reading apdu from json config.");
    TransportCardInfo cardInfoList[MAX_CARD_INFO_VEC_LEN];
    std::string content;
    LoadStringFromFile(NFC_CARD_APDU_JSON_FILEPATH, content);
    cJSON *json = cJSON_Parse(content.c_str());
    if (json == nullptr) {
        ErrorLog("json nullptr.");
        return false;
    }

    cJSON *cardInfo = cJSON_GetObjectItemCaseSensitive(json, KEY_CARD_INFO.c_str());
    if (cardInfo == nullptr || cJSON_GetArraySize(cardInfo) != MAX_CARD_INFO_VEC_LEN) {
        ErrorLog("fail to parse cardinfo");
        cJSON_Delete(json);
        return false;
    }

    if (!GetEachCardInfoFromJson(json, cardInfo, cardInfoList)) {
        ErrorLog("fail to get each cardinfo from json");
        cJSON_Delete(json);
        return false;
    }

    for (uint8_t i = 0; i < MAX_CARD_INFO_VEC_LEN; ++i) {
        cardInfoVec_.push_back(cardInfoList[i]);
    }
    cJSON_Delete(json);
    return true;
}

bool IsodepCardHandler::IsSupportedTransportCard(uint32_t rfDiscId, uint8_t &cardIndex)
{
    InfoLog("IsSupportedTransportCard, cardInfoVec_ size = [%{public}lu]", cardInfoVec_.size());
    if (nciTagProxy_.expired()) {
        WarnLog("nciTagProxy_ expired.");
        return false;
    }
    nciTagProxy_.lock()->Connect(rfDiscId, static_cast<int>(KITS::TagTechnology::NFC_ISODEP_TECH));
    for (uint8_t index = 0; index < cardInfoVec_.size(); ++index) {
        if (MatchCity(rfDiscId, index)) {
            InfoLog("card match \"%{public}s\"", cardInfoVec_[index].name.c_str());
            cardIndex = index;
            return true;
        }
    }
    InfoLog("no matching city, ignore.");
    return false;
}

bool IsodepCardHandler::MatchCity(uint32_t rfDiscId, uint8_t cardIndex)
{
    InfoLog("trying to match card type = \"%{public}s\"", cardInfoVec_[cardIndex].name.c_str());
    std::string checkCmdApdu = "";
    std::string rspApdu = "";
    for (uint8_t i = 0; i < cardInfoVec_[cardIndex].checkApdus.size(); ++i) {
        checkCmdApdu = cardInfoVec_[cardIndex].checkApdus[i];
        if (nciTagProxy_.expired()) {
            WarnLog("nciTagProxy_ expired.");
            return false;
        }
        nciTagProxy_.lock()->Transceive(rfDiscId, checkCmdApdu, rspApdu);
        InfoLog("rspApdu = %{public}s", rspApdu.c_str());
        if (!CheckApduResponse(rspApdu, cardIndex)) {
            InfoLog("check result false");
            return false;
        }
    }
    InfoLog("check result true");
    return true;
}

bool IsodepCardHandler::CheckApduResponse(const std::string &response, uint8_t cardIndex)
{
    if (response.length() < APDU_RSP_OK_STR_LEN) {
        ErrorLog("invalid response length");
        return false;
    }
    if (cardInfoVec_[cardIndex].rspContain == "") {
        return CheckApduResponse(response);
    }
    if (response.find(APDU_RSP_PREFIX) != std::string::npos &&
        response.find(cardInfoVec_[cardIndex].rspContain) != std::string::npos) {
        return true;
    }
    return false;
}

bool IsodepCardHandler::CheckApduResponse(const std::string &response)
{
    std::string rspStr = response.substr(response.length() - APDU_RSP_OK_STR_LEN, APDU_RSP_OK_STR_LEN);
    if (rspStr == APDU_RSP_OK) {
        return true;
    }
    return false;
}

void IsodepCardHandler::GetBalance(uint32_t rfDiscId, uint8_t cardIndex, int &balance)
{
    if (cardIndex >= cardInfoVec_.size()) {
        ErrorLog("invalid input cardIndex[%{public}u]", cardIndex);
        return;
    }
    InfoLog("start to get balance, card type = \"%{public}s\"", cardInfoVec_[cardIndex].name.c_str());
    std::string getBalanceCmdApdu = "";
    std::string rspApdu = "";
    uint8_t apduNum = cardInfoVec_[cardIndex].balanceApdus.size();
    for (uint8_t i = 0; i < apduNum; ++i) {
        getBalanceCmdApdu = cardInfoVec_[cardIndex].balanceApdus[i];
        if (nciTagProxy_.expired()) {
            WarnLog("nciTagProxy_ expired.");
            return;
        }
        nciTagProxy_.lock()->Transceive(rfDiscId, getBalanceCmdApdu, rspApdu);
        InfoLog("rspApdu = %{public}s", rspApdu.c_str());
        if (CheckApduResponse(rspApdu)) {
            if (i != apduNum - 1) {
                continue;
            }
            std::string balanceStr = rspApdu.substr(0, APDU_RSP_BALANCE_STR_LEN);
            DebugLog("balanceStr = %{public}s", balanceStr.c_str());
            GetBalanceValue(balanceStr, balance);
            return;
        }
    }
    ErrorLog("fail to get balance infomation from traffic card.");
}

void IsodepCardHandler::GetBalanceValue(const std::string &balanceStr, int &balanceValue)
{
    if (balanceStr.length() != APDU_RSP_BALANCE_STR_LEN) {
        ErrorLog("illegal balance string input.");
        return;
    }
    std::vector<unsigned char> bytes;
    KITS::NfcSdkCommon::HexStringToBytes(balanceStr, bytes);
    if (bytes.size() != APDU_RSP_BALANCE_BYTES_LEN) {
        ErrorLog("bytes size error.");
        return;
    }
    balanceValue = ((bytes[BYTE_ONE] & 0xFF) << TWO_BYTES_SHIFT)
                + ((bytes[BYTE_TWO] & 0xFF) << ONE_BYTES_SHIFT)
                + (bytes[BYTE_THREE] & 0xFF); // ignore BYTE_ZERO, in case of large balance
}

void IsodepCardHandler::GetCardName(uint8_t cardIndex, std::string &cardName)
{
    if (cardIndex >= cardInfoVec_.size()) {
        ErrorLog("invalid input cardIndex[%{public}u]", cardIndex);
        return;
    }
    cardName = cardInfoVec_[cardIndex].name;
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
