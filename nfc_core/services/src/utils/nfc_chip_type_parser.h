/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef NFC_CHIP_TYPE_PARSER_H
#define NFC_CHIP_TYPE_PARSER_H
#include <string>

namespace OHOS {
namespace NFC {
static const std::string NFC_CHIP_ST = "st21nfc";
static const std::string NFC_CHIP_THN31 = "thn31";
static const std::string NFC_CHIP_ST54K = "st54k";
static const std::string NFC_CHIP_PN553 = "pn553";
static const std::string NFC_CHIP_PN80T = "pn80t";
static const std::string NFC_CHIP_SN110U = "sn110u";
static const std::string NFC_CHIP_PATH = "/sys/nfc/nfc_chip_type";

enum NFC_CHIPTYPE {
    NFCTYPE_INVALID = 0,
    NFCTYPE_ST21NFC = 1, // ST54H or ST21
    NFCTYPE_NXP     = 2, // PN80T
    NFCTYPE_SN110   = 3,
    NFCTYPE_ST54K   = 4,
    NFCTYPE_THN31   = 5,
    NFCTYPE_PN553   = 6,
};

class NfcChipTypeParser {
public:
    static bool IsSn110();

private:
    static int ParseNfcChipType();
    static const int NFC_CHIP_LEN = 16;
    static const int NFC_CHIP_DEFAULT = -1;
    static int nfcChipType_;
};
} // NFC
} // OHOS
#endif // NFC_CHIP_TYPE_PARSER_H