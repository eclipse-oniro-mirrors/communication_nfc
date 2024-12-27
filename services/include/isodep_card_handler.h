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

#ifndef ISODEP_CARD_HANDLER_H
#define ISODEP_CARD_HANDLER_H

#include <string>
#include <vector>

#include "inci_tag_interface.h"

namespace OHOS {
namespace NFC {
namespace TAG {
struct TransportCardInfo {
    std::string name;
    std::string aid;
    std::vector<std::string> checkApdus;
    std::vector<std::string> balanceApdus;
    std::string rspContain;
};

static const uint8_t INVALID_CARD_INDEX = 0xFF;
static const int INVALID_BALANCE = -1;
static const int APDU_RSP_OK_STR_LEN = 4;
static const int APDU_RSP_BALANCE_STR_LEN = 8;
static const int APDU_RSP_BALANCE_BYTES_LEN = 4;
static const int MAX_APDU_ARRAY_SIZE = 2;
static const int MAX_CARD_INFO_VEC_LEN = 7;

static const std::string KEY_CARD_INFO_LEN = "cardInfoLength";
static const std::string KEY_CARD_INFO = "cardInfo";
static const std::string KEY_APDU_NAME = "name";
static const std::string KEY_APDU_AID = "aid";
static const std::string KEY_APDU_CHECK_APDUS = "checkApdus";
static const std::string KEY_APDU_BALANCE_APDUS = "balanceApdus";
static const std::string KEY_APDU_RSP_CONTAINS = "rspContains";

class IsodepCardHandler {
public:
    explicit IsodepCardHandler(std::weak_ptr<NCI::INciTagInterface> nciTagProxy);
    ~IsodepCardHandler();
    IsodepCardHandler(const IsodepCardHandler&) = delete;
    IsodepCardHandler& operator=(const IsodepCardHandler&) = delete;

    void InitTransportCardInfo(void);
    bool IsSupportedTransportCard(uint32_t rfDiscId, uint8_t &cardIndex);
    void GetBalance(uint32_t rfDiscId, uint8_t cardIndex, int &balance);
    void GetCardName(uint8_t cardIndex, std::string &cardName);

private:
    bool MatchCity(uint32_t rfDiscId, uint8_t cardIndex);
    bool CheckApduResponse(const std::string &response, uint8_t cardIndex);
    bool CheckApduResponse(const std::string &response);
    void GetBalanceValue(const std::string &balanceStr, int &balanceValue);
    bool DoJsonRead();

    std::weak_ptr<NCI::INciTagInterface> nciTagProxy_ {};

    // transport card info
    std::vector<TransportCardInfo> cardInfoVec_;
    bool isInitialized_ = false;

    static const int BYTE_ZERO = 0;
    static const int BYTE_ONE = 1;
    static const int BYTE_TWO = 2;
    static const int BYTE_THREE = 3;
    static const int THREE_BYTES_SHIFT = 24;
    static const int TWO_BYTES_SHIFT = 16;
    static const int ONE_BYTES_SHIFT = 8;

    const std::string NFC_CARD_APDU_JSON_FILEPATH = "system/etc/nfc/nfc_card_apdu.json";
    const std::string APDU_RSP_OK = "9000";
    const std::string APDU_RSP_PREFIX = "9F0C";
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif  // ISODEP_CARD_HANDLER_H
