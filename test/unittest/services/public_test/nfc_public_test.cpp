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
#include <gtest/gtest.h>
#include <thread>

#include "nfc_sdk_common.h"
#include "nfc_timer.h"
#include "nfc_preferences.h"
#include "nfc_permission_checker.h"
#include "nfc_watch_dog.h"
#include "synchronize_event.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class NfcPublicTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:

    // An test array of byte types with a length of 6
    static constexpr char TEST_DATA_ANY_CHARS[4] = {'b', 'c', '0', '1'};

    // The length of the array to be tested is 4
    static constexpr auto TEST_DATA_ANY_HEX_STRING_BYTE_LEN = 4;

    // The hexadecimal string of the ascll table corresponding to the array to be tested is 62633031
    static constexpr auto TEST_DATA_ANY_HEX_STRING = "62633031";

    // A string "11111" used to test the conversion of characters to ints
    static constexpr auto TEST_DATA_STR = "11111";

    // "11111" to int is 825307441
    static constexpr auto TEST_DATA_STR_TO_INT = 825307441;
    static constexpr unsigned char TEST_DATA_UNSIGNED_CHAR = '0';

    // The character 0 corresponds to the number 30 of the hexadecimal ascll table
    static constexpr auto TEST_DATA_UNSIGNED_CHAR_STR = "30";
};

void NfcPublicTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase PublicTest." << std::endl;
}

void NfcPublicTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase PublicTest." << std::endl;
}

void NfcPublicTest::SetUp() {}

void NfcPublicTest::TearDown() {}

/**
 * @tc.name: IsLittleEndian001
 * @tc.desc: Test NfcPublic IsLittleEndian.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, IsLittleEndian001, TestSize.Level1)
{
    bool isLittleEndian = NfcSdkCommon::IsLittleEndian();
    ASSERT_TRUE(isLittleEndian == true);
}

/**
 * @tc.name: BytesVecToHexString001
 * @tc.desc: Test NfcPublic BytesVecToHexString.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, BytesVecToHexString001, TestSize.Level1)
{
    unsigned char *bytes = new unsigned char[0];
    std::string hexString = NfcSdkCommon::BytesVecToHexString(bytes, 0);
    ASSERT_TRUE(strcmp(hexString.c_str(), "") == 0);
}

/**
 * @tc.name: BytesVecToHexString002
 * @tc.desc: Test NfcPublic BytesVecToHexString.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, BytesVecToHexString002, TestSize.Level1)
{
    unsigned char bytes[4] = {'b', 'c', '0', '1'};
    uint32_t size = sizeof(bytes) / sizeof(bytes[0]);
    std::string hexString = NfcSdkCommon::BytesVecToHexString(bytes, size);
    ASSERT_TRUE(strcmp(hexString.c_str(), TEST_DATA_ANY_HEX_STRING) == 0);
}

/**
 * @tc.name: UnsignedCharToHexString001
 * @tc.desc: Test NfcPublic UnsignedCharToHexString.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, UnsignedCharToHexString001, TestSize.Level1)
{
    std::string hexString = NfcSdkCommon::UnsignedCharToHexString(TEST_DATA_UNSIGNED_CHAR);
    ASSERT_TRUE(strcmp(hexString.c_str(), TEST_DATA_UNSIGNED_CHAR_STR) == 0);
}

/**
 * @tc.name: GetHexStrBytesLen001
 * @tc.desc: Test NfcPublic GetHexStrBytesLen.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, GetHexStrBytesLen001, TestSize.Level1)
{
    std::string src = "";
    uint32_t hexStrBytesLen = NfcSdkCommon::GetHexStrBytesLen(src);
    ASSERT_TRUE(hexStrBytesLen == 0);
}

/**
 * @tc.name: GetHexStrBytesLen002
 * @tc.desc: Test NfcPublic GetHexStrBytesLen.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, GetHexStrBytesLen002, TestSize.Level1)
{
    uint32_t hexStrBytesLen = NfcSdkCommon::GetHexStrBytesLen(TEST_DATA_ANY_HEX_STRING);
    ASSERT_TRUE(hexStrBytesLen == TEST_DATA_ANY_HEX_STRING_BYTE_LEN);
}

/**
 * @tc.name: GetByteFromHexStr001
 * @tc.desc: Test NfcPublic GetByteFromHexStr.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, GetByteFromHexStr001, TestSize.Level1)
{
    uint32_t index = 0;
    unsigned char byteFromHexStr = NfcSdkCommon::GetByteFromHexStr(TEST_DATA_UNSIGNED_CHAR_STR, index);
    ASSERT_TRUE(byteFromHexStr == TEST_DATA_UNSIGNED_CHAR);
}

/**
 * @tc.name: StringToInt001
 * @tc.desc: Test NfcPublic StringToInt.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, StringToInt001, TestSize.Level1)
{
    bool bLittleEndian = true;
    uint32_t srcToInt = NfcSdkCommon::StringToInt(TEST_DATA_STR, bLittleEndian);
    ASSERT_TRUE(srcToInt == TEST_DATA_STR_TO_INT);
}

/**
 * @tc.name: StringToInt002
 * @tc.desc: Test NfcPublic StringToInt.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, StringToInt002, TestSize.Level1)
{
    bool bLittleEndian = false;
    uint32_t srcToInt = NfcSdkCommon::StringToInt(TEST_DATA_STR, bLittleEndian);
    ASSERT_TRUE(srcToInt == TEST_DATA_STR_TO_INT);
}

/**
 * @tc.name: IntToHexString001
 * @tc.desc: Test NfcPublic IntToHexString.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, IntToHexString001, TestSize.Level1)
{
    // 255 corresponds to hexadecimal a
    uint32_t num = 255;
    std::string intToStr = NfcSdkCommon::IntToHexString(num);
    ASSERT_TRUE(intToStr == "FF");
}

/**
 * @tc.name: StringToHexString001
 * @tc.desc: Test NfcPublic StringToHexString.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, StringToHexString001, TestSize.Level1)
{
    std::string str = "0";
    std::string strToHexStr = NfcSdkCommon::StringToHexString(str);
    ASSERT_TRUE(strcmp(strToHexStr.c_str(), TEST_DATA_UNSIGNED_CHAR_STR) == 0);
}
/**
 * @tc.name: HexStringToBytes001
 * @tc.desc: Test NfcPublic HexStringToBytes.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, HexStringToBytes001, TestSize.Level1)
{
    const std::string src = "";
    std::vector<unsigned char> bytes;
    NfcSdkCommon::HexStringToBytes(src, bytes);
    ASSERT_TRUE(bytes.empty() == true);
}
/**
 * @tc.name: HexStringToBytes002
 * @tc.desc: Test NfcPublic HexStringToBytes.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, HexStringToBytes002, TestSize.Level1)
{
    const std::string src = "01";
    std::vector<unsigned char> bytes;
    NfcSdkCommon::HexStringToBytes(src, bytes);
    ASSERT_TRUE(bytes.size() == FeatureType::UICC);
}
/**
 * @tc.name: GetHexStrBytesLen003
 * @tc.desc: Test NfcPublic GetHexStrBytesLen.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, GetHexStrBytesLen003, TestSize.Level1)
{
    const std::string src = "43020";
    uint32_t hexStrBytesLen = NfcSdkCommon::GetHexStrBytesLen(src);
    ASSERT_TRUE(hexStrBytesLen == 3);
}
/**
 * @tc.name: GetByteFromHexStr002
 * @tc.desc: Test NfcPublic GetByteFromHexStr.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, GetByteFromHexStr002, TestSize.Level1)
{
    const std::string src = "%x";
    uint32_t index = 0;
    unsigned char byteFromHexStr = NfcSdkCommon::GetByteFromHexStr(src, index);
    ASSERT_TRUE(byteFromHexStr == 0);
}
/**
 * @tc.name: GetCurrentTime001
 * @tc.desc: Test NfcPublic GetCurrentTime.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, GetCurrentTime001, TestSize.Level1)
{
    uint64_t getCurrentTime = NfcSdkCommon::GetCurrentTime();
    ASSERT_TRUE(getCurrentTime != 0);
}
/**
 * @tc.name: GetByteFromHexStr003
 * @tc.desc: Test NfcPublic GetByteFromHexStr.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, GetByteFromHexStr003, TestSize.Level1)
{
    const std::string src = TEST_DATA_ANY_HEX_STRING;
    uint32_t index = TEST_DATA_ANY_HEX_STRING_BYTE_LEN;
    unsigned char byteFromHexStr = NfcSdkCommon::GetByteFromHexStr(src, index);
    ASSERT_TRUE(byteFromHexStr == 0);
}
/**
 * @tc.name: IntToHexString002
 * @tc.desc: Test NfcPublic IntToHexString.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, IntToHexString002, TestSize.Level1)
{
    uint32_t num = ErrorCode::ERR_NONE;
    std::string intToStr = NfcSdkCommon::IntToHexString(num);
    ASSERT_TRUE(intToStr == "00");
}
/**
 * @tc.name: HexStringToBytes003
 * @tc.desc: Test NfcPublic HexStringToBytes.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, HexStringToBytes003, TestSize.Level1)
{
    const std::string src = "%x";
    std::vector<unsigned char> bytes;
    NfcSdkCommon::HexStringToBytes(src, bytes);
    ASSERT_TRUE(bytes.empty() == true);
}

/**
 * @tc.name: CodeMiddlePart001
 * @tc.desc: Test NfcPublic CodeMiddlePart001.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, CodeMiddlePart001, TestSize.Level1)
{
    std::string src = "";
    std::string res = NfcSdkCommon::CodeMiddlePart(src);
    ASSERT_TRUE(res.empty() == true);
}

/**
 * @tc.name: CodeMiddlePart002
 * @tc.desc: Test NfcPublic CodeMiddlePart002.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, CodeMiddlePart002, TestSize.Level1)
{
    std::string src(1025, 'a');
    std::string res = NfcSdkCommon::CodeMiddlePart(src);
    ASSERT_TRUE(res.empty() == true);
}

/**
 * @tc.name: CodeMiddlePart003
 * @tc.desc: Test NfcPublic CodeMiddlePart003.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, CodeMiddlePart003, TestSize.Level1)
{
    std::string src = "CodeMiddlePart";
    std::string res = NfcSdkCommon::CodeMiddlePart(src);
    ASSERT_TRUE(res.empty() == false);
}

/**
 * @tc.name: NfcPrefImpl001
 * @tc.desc: Test NfcPublic NfcPrefImpl001.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, NfcPrefImpl001, TestSize.Level1)
{
    OHOS::NFC::NfcPreferences *impl = new OHOS::NFC::NfcPreferences();
    impl->SetString("test_key", "test_value");
    std::string value = impl->GetString("test_key");
    ASSERT_TRUE(value == "test_value");

    impl->Delete("test_key");
    value = impl->GetString("test_key");
    ASSERT_TRUE(value == "");

    impl->Clear();
    value = impl->GetString("test_key");
    delete impl;
    ASSERT_TRUE(value == "");
}

/**
 * @tc.name: NfcPermissionChecker001
 * @tc.desc: Test NfcPermissionChecker001
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, NfcPermissionChecker001, TestSize.Level1)
{
    bool granted = NfcPermissionChecker::IsGranted("unitest.permission.nfc");
    ASSERT_TRUE(granted == true);
}

/**
 * @tc.name: NfcWatchDog001
 * @tc.desc: Test NfcWatchDog001
 * @tc.type: FUNC
 */
HWTEST_F(NfcPublicTest, NfcWatchDog001, TestSize.Level1)
{
    NfcTimer::GetInstance()->UnRegister(0);
    std::shared_ptr<NCI::INciNfccInterface> nciNfccProxy = nullptr;
    NfcWatchDog nfcWatchDog("Test", 90 * 1000, nciNfccProxy);
    nfcWatchDog.Run();
    nfcWatchDog.Cancel();
    uint64_t currentTime = NfcSdkCommon::GetCurrentTime();
    ASSERT_TRUE(currentTime != 0);
}

}
}
}
