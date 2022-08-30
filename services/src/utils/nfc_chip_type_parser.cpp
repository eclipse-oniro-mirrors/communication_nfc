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

#include "nfc_chip_type_parser.h"

#include <fstream>
#include <iostream>
#include <string>
#include "loghelper.h"

using namespace std;

namespace OHOS {
namespace NFC {
int NfcChipTypeParser::nfcChipType_ = NfcChipTypeParser::NFC_CHIP_DEFAULT;

int NfcChipTypeParser::ParseNfcChipType()
{
    DebugLog("ParseNfcChipType: enter");
    if (nfcChipType_ >= 0) {
        InfoLog("ParseNfcChipType: chip type = %{public}d", nfcChipType_);
        return nfcChipType_;
    }

    ifstream ifs;
    ifs.open(NFC_CHIP_PATH, ios::in);
    if (!ifs.is_open()) {
        ErrorLog("ParseNfcChipType: cannot open nfc chip file");
        return NFC_CHIP_DEFAULT;
    }
    char chipBuf[NFC_CHIP_LEN] = {0};
    ifs.getline(chipBuf, sizeof(chipBuf));
    ifs.close();

    if (NFC_CHIP_ST.compare(chipBuf) == 0) {
        nfcChipType_ = NFCTYPE_ST21NFC;
    } else if (NFC_CHIP_ST54K.compare(chipBuf) == 0) {
        nfcChipType_ = NFCTYPE_ST54K;
    } else if (NFC_CHIP_THN31.compare(chipBuf) == 0) {
        nfcChipType_ = NFCTYPE_THN31;
    } else if (NFC_CHIP_SN110U.compare(chipBuf) == 0) {
        nfcChipType_ = NFCTYPE_SN110;
    } else if (NFC_CHIP_PN80T.compare(chipBuf) == 0) {
        nfcChipType_ = NFCTYPE_NXP;
    } else if (NFC_CHIP_PN553.compare(chipBuf) == 0) {
        nfcChipType_ = NFCTYPE_PN553;
    } else {
        nfcChipType_ = NFCTYPE_INVALID;
    }
    InfoLog("ParseNfcChipType: exit, chip type = %{public}d", nfcChipType_);
    return nfcChipType_;
}

bool NfcChipTypeParser::IsSn110()
{
    return (ParseNfcChipType() == NFCTYPE_SN110);
}
} // NFC
} // OHOS