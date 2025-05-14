/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#include "nfc_sdk_common.h"

#include <algorithm>
#include <sstream>
#include <securec.h>
#include <sys/time.h>
#include "bundle_mgr_proxy.h"
#include "iservice_registry.h"
#include "cJSON.h"
#include "file_ex.h"

#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {

bool NfcSdkCommon::IsLittleEndian()
{
    const char LAST_DATA_BYTE = 0x78;
    union CheckData {
        int x;
        char y;
    };

    union CheckData data;
    data.x = 0x12345678;
    if (data.y == LAST_DATA_BYTE) {
        return true;
    }
    return false;
}

std::string NfcSdkCommon::BytesVecToHexString(const unsigned char* src, uint32_t length)
{
    std::string result = "";
    if (length <= 0) {
        return result;
    }
    const std::string hexKeys = "0123456789ABCDEF";
    for (uint32_t i = 0; i < length; i++) {
        result.push_back(hexKeys[(src[i] & 0xF0) >> HALF_BYTE_BITS]);
        result.push_back(hexKeys[src[i] & 0x0F]);
    }
    return result;
}

std::string NfcSdkCommon::UnsignedCharToHexString(const unsigned char src)
{
    std::string result = "";
    const std::string hexKeys = "0123456789ABCDEF";
    result.push_back(hexKeys[(src & 0xF0) >> HALF_BYTE_BITS]);
    result.push_back(hexKeys[src & 0x0F]);
    return result;
}

void NfcSdkCommon::HexStringToBytes(const std::string &src, std::vector<unsigned char> &bytes)
{
    if (src.empty()) {
        return;
    }

    uint32_t bytesLen = src.length() / HEX_BYTE_LEN;
    std::string strByte;
    unsigned int srcIntValue;
    for (uint32_t i = 0; i < bytesLen; i++) {
        strByte = src.substr(i * HEX_BYTE_LEN, HEX_BYTE_LEN);
        if (sscanf_s(strByte.c_str(), "%x", &srcIntValue) <= 0) {
            ErrorLog("HexStringToBytes, sscanf_s failed.");
            bytes.clear();
            return;
        }
        bytes.push_back(static_cast<unsigned char>(srcIntValue & 0xFF));
    }
}

uint32_t NfcSdkCommon::GetHexStrBytesLen(const std::string src)
{
    // 2 charactors consist of one byte.
    if (src.empty()) {
        return 0;
    }
    uint32_t length = src.length();
    if (length % HEX_BYTE_LEN == 0) {
        return (length / HEX_BYTE_LEN);
    } else {
        return ((length / HEX_BYTE_LEN) + 1);
    }
}

unsigned char NfcSdkCommon::GetByteFromHexStr(const std::string src, uint32_t index)
{
    // 2 charactors consist of one byte.
    if (src.empty() || (src.length() < index * HEX_BYTE_LEN + HEX_BYTE_LEN)) {
        ErrorLog("GetByteFromHexStr, src length error.");
        return 0;
    }
    std::string strByte = src.substr(index * HEX_BYTE_LEN, HEX_BYTE_LEN);
    unsigned int srcIntValue;
    if (sscanf_s(strByte.c_str(), "%x", &srcIntValue) <= 0) {
        ErrorLog("GetByteFromHexStr, sscanf_s failed.");
        return 0;
    }
    return static_cast<unsigned char>(srcIntValue & 0xFF);
}

uint32_t NfcSdkCommon::StringToInt(std::string src, bool bLittleEndian)
{
    uint32_t value = 0;
    if (bLittleEndian) {
        for (size_t i = SHIFT_TIME; i > 0; i--) {
            value += static_cast<uint32_t>((src.at(SHIFT_TIME - i)) << (i * SHIFT_SIZE - SHIFT_SIZE));
        }
    } else {
        for (size_t i = 0; i < SHIFT_TIME; i++) {
            value += static_cast<uint32_t>((src.at(i)) << (i * SHIFT_SIZE));
        }
    }
    return value;
}

std::string NfcSdkCommon::IntToHexString(uint32_t num)
{
    std::stringstream ss;
    ss << std::hex << num;
    std::string result = ss.str();
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    if (result.length() % HEX_BYTE_LEN > 0) { // expend "0" if string length is odd
        result = "0" + result;
    }
    return result;
}

void NfcSdkCommon::StringToAsciiBytes(const std::string &src, std::vector<unsigned char> &bytes)
{
    if (src.empty()) {
        return;
    }
    uint32_t bytesLen = src.length();
    for (uint32_t i = 0; i < bytesLen; i++) {
        unsigned int srcAsciiIntVal = static_cast<unsigned int>(src[i]);
        bytes.push_back(static_cast<unsigned char>(srcAsciiIntVal & 0xFF));
    }
}

std::string NfcSdkCommon::StringToHexString(const std::string &src)
{
    std::vector<unsigned char> bytes;
    StringToAsciiBytes(src, bytes);
    uint32_t len = src.length();
    std::string result = BytesVecToHexString(&bytes[0], len);
    return result;
}

std::string NfcSdkCommon::HexStringToAsciiString(const std::string &src)
{
    if (src.size() % HEX_BYTE_LEN != 0 || src.empty()) {  // 2 is Even number judgement
        ErrorLog("HexStringToAsciiString length error");
        return "";
    }
    std::string result = "";
    for (size_t i = 0; i < src.size() / HEX_BYTE_LEN; i++) {
        unsigned char byteVal = GetByteFromHexStr(src, i);
        const char minPrintChar = ' ';
        const char maxPrintChar = '~';
        /* ' ' to '~' is the printable char range */
        if (static_cast<char>(byteVal) < minPrintChar || static_cast<char>(byteVal) > maxPrintChar) {
            return "";
        }
        result.push_back(static_cast<char>(byteVal));
    }
    return result;
}

/*
 * transfer Hex array to String without checking Ascii validation, compatible with Chinese characters
 */
std::string NfcSdkCommon::HexArrayToStringWithoutChecking(const std::string &src)
{
    if (src.size() % HEX_BYTE_LEN != 0 || src.empty()) {  // 2 is Even number judgement
        ErrorLog("HexStringToAsciiString length error");
        return "";
    }
    std::string result = "";
    for (size_t i = 0; i < src.size() / HEX_BYTE_LEN; i++) {
        unsigned char byteVal = GetByteFromHexStr(src, i);
        result.push_back(static_cast<char>(byteVal));
    }
    return result;
}

uint64_t NfcSdkCommon::GetCurrentTime()
{
    // get the time since 1970/1/1
    constexpr int timeRate = 1000;
    struct timeval time = {0};
    gettimeofday(&time, nullptr);
    return static_cast<uint64_t>(time.tv_sec * timeRate + time.tv_usec / timeRate);
}

uint64_t NfcSdkCommon::GetRelativeTime()
{
    // get the time since the system was booted
    constexpr int64_t msPerSecond = 1000;
    constexpr int64_t nsPerMs = 1000000;

    struct timespec times = {0};
    clock_gettime(CLOCK_MONOTONIC, &times);
    return ((times.tv_sec * msPerSecond) + (times.tv_nsec / nsPerMs));
}

std::string NfcSdkCommon::CodeMiddlePart(const std::string &src)
{
    std::string res = "";
    if (src.empty()) {
        return res;
    }
    const char code = 'X';
    const uint32_t maxStrLen = 1024;
    uint32_t len = src.length();
    if (len > maxStrLen) {
        return res;
    }

    uint32_t head = (len / 2) / 2; // Divide the string evenly into 2 * 2 parts
    if (len <= head * 2) {  // The length of the head * 2 is greater than src
        return src;
    }
    for (uint32_t i = 0; i < head; i++) {
        res.push_back(src[i]);
    }
    for (uint32_t i = head; i < (len - head); i++) {
        res.push_back(code);
    }
    for (uint32_t i = (len - head); i < len; i++) {
        res.push_back(src[i]);
    }
    return res;
}

bool NfcSdkCommon::SecureStringToInt(const std::string &str, int32_t &value, int base)
{
    errno = 0;
    char *endptr = nullptr;
    const char *ptr = str.c_str();
    value = std::strtol(str.c_str(), &endptr, base);
    if (errno == ERANGE || endptr == ptr || *endptr != '\0') { // ERANGE: integer overflow
        ErrorLog("SecureStringToInt errno str = %{public}s", str.c_str());
        return false;
    }
    return true;
}

int NfcSdkCommon::GetSdkVersion(void)
{
    int version = SDK_VERSION_UNKNOWN;

    auto systemAbilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        ErrorLog("fail to get system ability mgr.");
        return version;
    }
    auto remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        ErrorLog("fail to get bundle manager proxy.");
        return version;
    }
    sptr<AppExecFwk::BundleMgrProxy> bundleMgrProxy = iface_cast<AppExecFwk::BundleMgrProxy>(remoteObject);
    if (bundleMgrProxy == nullptr) {
        ErrorLog("failed to get bundle manager proxy.");
        return version;
    }
    AppExecFwk::BundleInfo bundleInfo;
    auto flags = AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_APPLICATION;
    auto ret = bundleMgrProxy->GetBundleInfoForSelf(static_cast<int32_t>(flags), bundleInfo);
    if (ret != ERR_OK) {
        ErrorLog("GetBundleInfoForSelf: get fail.");
        return version;
    }

    version = static_cast<int>(bundleInfo.targetVersion % 100); // %100 to get the real version
    return version;
}

bool NfcSdkCommon::GetConfigFromJson(const std::string &key, std::string &value)
{
    std::string content;
    if (!LoadStringFromFile(NFC_SERVICE_CONFIG_PATH, content) || content.empty()) {
        ErrorLog("fail to load string from nfc_service_config.json");
        return false;
    }
    cJSON *json = cJSON_Parse(content.c_str());
    if (json == nullptr) {
        ErrorLog("json nullptr");
        return false;
    }
    if (!cJSON_IsObject(json)) {
        ErrorLog("reader is not cJSON object");
        cJSON_Delete(json);
        return false;
    }
    if (!NFC_SERVICE_CONFIG_KEY_SET.count(key)) {
        WarnLog("current key[%{public}s] not exists!", key.c_str());
        cJSON_Delete(json);
        return false;
    }

    cJSON *cJsonObject = cJSON_GetObjectItem(json, key.c_str());
    if (cJsonObject == nullptr) {
        ErrorLog("cJsonObject is nullptr.");
        cJSON_Delete(json);
        return false;
    }
    value = cJSON_GetStringValue(cJsonObject);
    cJSON_Delete(json);
    return true;
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
