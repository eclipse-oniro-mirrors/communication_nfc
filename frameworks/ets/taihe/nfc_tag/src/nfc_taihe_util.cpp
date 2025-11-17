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

#include "nfc_taihe_util.h"

#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "securec.h"

using namespace taihe;
using namespace OHOS::NFC::KITS;

const uint16_t MAX_ARRAY_LENGTH = 512;
const uint16_t HEX_BYTE_LENGTH = 2;

std::string NfcTaiheUtil::TaiheArrayToHexString(const array_view<int32_t> &data)
{
    if (data.size() > MAX_ARRAY_LENGTH) {
        ErrorLog("data size exceed.");
        return "";
    }
    std::vector<uint8_t> dataBytes = {};
    for (uint16_t i = 0; i < data.size(); i++) {
        dataBytes.push_back(static_cast<uint8_t>(data[i]));
    }
    return NfcSdkCommon::BytesVecToHexString(&dataBytes[0], dataBytes.size());
}

std::vector<std::string> NfcTaiheUtil::TaiheStringArrayToStringVec(const array_view<::taihe::string> &data)
{
    std::vector<std::string> ret;
    if (data.size() > MAX_ARRAY_LENGTH) {
        ErrorLog("data size exceed.");
        return ret;
    }
    for (uint16_t i = 0; i < data.size(); i++) {
        ret.push_back(data[i].c_str());
    }
    return ret;
}

std::vector<int> NfcTaiheUtil::TaiheIntArrayToIntVec(const array<int32_t> &data)
{
    std::vector<int> ret;
    if (data.size() > MAX_ARRAY_LENGTH) {
        ErrorLog("data size exceed.");
        return ret;
    }
    for (uint16_t i = 0; i < data.size(); i++) {
        ret.push_back(data[i]);
    }
    return ret;
}

array<int32_t> NfcTaiheUtil::HexStringToTaiheArray(const std::string &src)
{
    std::vector<int32_t> dataVec;
    if (src.empty()) {
        return array<int32_t>(array_view<int32_t>(dataVec));
    }

    uint32_t bytesLen = src.length() / HEX_BYTE_LENGTH;
    if (bytesLen > MAX_ARRAY_LENGTH) {
        ErrorLog("data size exceed.");
        return array<int32_t>(array_view<int32_t>(dataVec));
    }
    std::string strByte;
    unsigned int srcIntValue;
    for (uint32_t i = 0; i < bytesLen; i++) {
        strByte = src.substr(i * HEX_BYTE_LENGTH, HEX_BYTE_LENGTH);
        if (sscanf_s(strByte.c_str(), "%x", &srcIntValue) <= 0) {
            ErrorLog("sscanf_s failed.");
            dataVec.clear();
            return array<int32_t>(array_view<int32_t>(dataVec));
        }
        dataVec.push_back(static_cast<unsigned char>(srcIntValue & 0xFF));
    }
    return array<int32_t>(array_view<int32_t>(dataVec));
}
