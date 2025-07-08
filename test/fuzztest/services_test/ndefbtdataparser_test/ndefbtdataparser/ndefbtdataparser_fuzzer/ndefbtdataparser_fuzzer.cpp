/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#define private public
#define protected public
#include "ndefbtdataparser_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "ndef_bt_data_parser.h"
#include "ndef_har_dispatch.h"
#include "nfc_sdk_common.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
    using namespace OHOS::NFC;
    using namespace OHOS::NFC::KITS;
    using namespace OHOS::NFC::TAG;

    constexpr const auto FUZZER_THRESHOLD = 4;
    constexpr const auto PAYLOAD_LEN = 600;

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    void FuzzCheckBtRecord(const uint8_t* data, size_t size)
    {
        std::string msg = std::string(reinterpret_cast<const char*>(data), size);
        std::shared_ptr<NdefBtDataParser> ndefBtDataParser = std::make_shared<NdefBtDataParser>();
        if (ndefBtDataParser == nullptr) {
            return;
        }
        ndefBtDataParser->CheckBtRecord(msg);
        ndefBtDataParser->CheckBtRecord("");
        ndefBtDataParser->CheckBtRecord("CheckbtRecord");
        ndefBtDataParser->CheckBtRecord("D220566170706C69636174696F6E2F766E642E626C7565746F6F74682E65"
                                        "702E6F6F625600BE17010E7F04050949435341040D14042C0B030B110C11"
                                        "0E111E11001236FF027D0320010240005A45303031810800113000190103"
                                        "021901010101020306047F0E0117BE020E52726364687A5238363739393532");
        ndefBtDataParser->CheckBtRecord("D220566170706C69636174696F6E2F766E642E626C7565746F6F74682E65"
                                        "702E6F6F625600");
        ndefBtDataParser->CheckBtRecord("D220566170706C69636174696F6E2F766E642E626C7565746F6F74682E65"
                                        "702E6F6F625600BE17010E7F04000849435341040D14042C0B030B110C11"
                                        "0E111E11001236FF027D0320010240005A45303031810800113000190103"
                                        "021901010101020306047F0E0117BE020E52726364687A5238363739393532");
        ndefBtDataParser->CheckBtRecord("D220566170706C69636174696F6E2F766E642E626C7565746F6F74682E65"
                                        "702E6F6F625600BE17010E7F04050849435341040D14042C0B030B110C11"
                                        "0E111E11001236FF027D0320010240005A45303031810800113000190103"
                                        "021901010101020306047F0E0117BE020E52726364687A5238363739393532");
        ndefBtDataParser->CheckBtRecord("D220566170706C69636174696F6E2F766E642E626C7565746F6F74682E65"
                                        "702E6F6F625600BE17010E7F04000949435341040D14042C0B030B110C11"
                                        "0E111E11001236FF027D0320010240005A45303031810800113000190103"
                                        "021901010101020306047F0E0117BE020E52726364687A5238363739393532");
        ndefBtDataParser->CheckBtRecord("D220566170706C69636174696F6E2F766E642E626C7565746F6F74682E65"
                                        "702E6F6F625600BE17010E7F04000049435341040D14042C0B030B110C11"
                                        "0E111E11001236FF027D0320010240005A45303031810800113000190103"
                                        "021901010101020306047F0E0117BE020E52726364687A5238363739393532");
        ndefBtDataParser->CheckBtRecord("D220566170706C69636174696F6E2F766E642E626C7565746F6F74682E65"
                                        "702E6F6F625600BE17010E7F04010049435341040D14042C0B030B110C11"
                                        "0E111E11001236FF027D0320010240005A45303031810800113000190103"
                                        "021901010101020306047F0E0117BE020E52726364687A5238363739393532");
        ndefBtDataParser->CheckBtRecord("D220566170706C69636174696F6E2F766E642E626C7565746F6F74682E65"
                                        "701E6F6F625600BE17010E7F04050949435341040D14042C0B030B110C11"
                                        "0E111E11001236FF027D0320010240005A45303031810800113000190103"
                                        "021901010101020306047F0E0117BE020E52726364687A5238363739393532");
        ndefBtDataParser->CheckBtRecord("D220566170706C69636174696F6E2F766E642E626C7565746F6F74682E60"
                                        "702E6F6F625600BE17010E7F04050949435341040D14042C0B030B110C11"
                                        "0E111E11001236FF027D0320010240005A45303031810800113000190103"
                                        "021901010101020306047F0E0117BE020E52726364687A5238363739393532");
        ndefBtDataParser->CheckBtRecord("D220566170706C69636174696F6E2F766E642E626C7565746F6F74682E6C"
                                        "652E6F6F625600BE17010E7F04050949435341040D14042C0B030B110C11"
                                        "0E111E11001236FF027D0320010240005A45303031810800113000190103"
                                        "021901010101020306047F0E0117BE020E52726364687A5238363739393532");
        ndefBtDataParser->CheckBtRecord("D220566170706C69636174696F6E2F766E642E626C7565746F6F74682E6C"
                                        "650E6F6F625600BE17010E7F04050949435341040D14042C0B030B110C11"
                                        "0E111E11001236FF027D0320010240005A45303031810800113000190103"
                                        "021901010101020306047F0E0117BE020E52726364687A5238363739393532");
        ndefBtDataParser->CheckBtRecord("D220566170706C69636174696F6E2F766E642E626C7565746F6F74682E60"
                                        "650E6F6F625600BE17010E7F04050949435341040D14042C0B030B110C11"
                                        "0E111E11001236FF027D0320010240005A45303031810800113000190103"
                                        "021901010101020306047F0E0117BE020E52726364687A5238363739393532");
        ndefBtDataParser->CheckBtRecord("D220566170706C69636174696F6E2F766E642E626C7565746F6F74682E60"
                                        "701E6F6F625600BE17010E7F04050949435341040D14042C0B030B110C11"
                                        "0E111E11001236FF027D0320010240005A45303031810800113000190103"
                                        "021901010101020306047F0E0117BE020E52726364687A5238363739393532");
    }

    void FuzzIsVendorPayloadValid(const uint8_t* data, size_t size)
    {
        std::string payload = std::string(reinterpret_cast<const char*>(data), size);
        std::shared_ptr<NdefBtDataParser> ndefBtDataParser = std::make_shared<NdefBtDataParser>();
        if (ndefBtDataParser == nullptr) {
            return;
        }
        ndefBtDataParser->IsVendorPayloadValid(payload);
        ndefBtDataParser->IsVendorPayloadValid("1");
        ndefBtDataParser->IsVendorPayloadValid("test");
        std::string payload1(PAYLOAD_LEN, '1');
        ndefBtDataParser->IsVendorPayloadValid(payload1);
    }

    void FuzzDispatchBundleAbility(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NCI::INciNfccInterface> testNfccInterface = nullptr;
        std::shared_ptr<TAG::NdefHarDispatch> ndefHarDispatchTest = std::make_shared<TAG::NdefHarDispatch>(
            testNfccInterface);

        std::shared_ptr<KITS::TagInfo> tagInfo = nullptr;
        std::string mimeType = "";
        std::string harPackage = "";
        std::string uri = std::string(reinterpret_cast<const char*>(data), size);
        std::string uriSchemeValue = "";
        OHOS::sptr<IRemoteObject> tagServiceIface;
        ndefHarDispatchTest->DispatchBundleAbility(harPackage, tagInfo, mimeType, uri, tagServiceIface);
        ndefHarDispatchTest->DispatchByAppLinkMode(uriSchemeValue, tagInfo, tagServiceIface);

        std::vector<int> tagTechList = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        std::vector<AppExecFwk::PacMap> tagTechExtras;
        AppExecFwk::PacMap tagTechExtrasData;
        AppExecFwk::PacMap isoDepExtrasData;
        tagTechExtras.push_back(tagTechExtrasData);
        tagTechExtras.push_back(isoDepExtrasData);
        std::string tagUid = "5B7FCFA9";
        int tagRfDisId = 1;
        tagInfo = std::make_shared<KITS::TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDisId, nullptr);
        harPackage = "ABC";
        mimeType = "A/B";
        ndefHarDispatchTest->DispatchBundleAbility(harPackage, tagInfo, mimeType, uri, tagServiceIface);

        uri = "";
        ndefHarDispatchTest->DispatchUriToBundleAbility(uri);
        uri = "ABC";
        ndefHarDispatchTest->DispatchUriToBundleAbility(uri);

        std::string type = "";
        ndefHarDispatchTest->DispatchMimeType(type, tagInfo);
        type = "ABC";
        ndefHarDispatchTest->DispatchMimeType(type, tagInfo);

        uriSchemeValue = "https://open.cmbchina.com/dispatch/"
            "go?url=web&version=v2&next=https%3A%2F%2Fpiao.o2o.cmbchina.com%2Fcmblife_fanpiao%2FstoreDetail%"
            "3FstrNo%3D051203673000025&deeplinkId=20241011";
        ndefHarDispatchTest->DispatchByAppLinkMode(uriSchemeValue, tagInfo, tagServiceIface);
    }

    void FuzzParseBleRecord(const uint8_t* data, size_t size)
    {
        std::string payload = std::string(reinterpret_cast<const char*>(data), size);
        std::shared_ptr<NdefBtDataParser> ndefBtDataParser = std::make_shared<NdefBtDataParser>();
        if (ndefBtDataParser == nullptr) {
            return;
        }
        ndefBtDataParser->ParseBleRecord(payload);
    }

    void FuzzParseBtHandoverSelect(const uint8_t* data, size_t size)
    {
        std::string msg = std::string(reinterpret_cast<const char*>(data), size);
        std::shared_ptr<NFC::KITS::NdefMessage> ndef = NdefMessage::GetNdefMessage(msg);
        std::string payload = std::string(reinterpret_cast<const char*>(data), size);
        std::shared_ptr<NdefBtDataParser> ndefBtDataParser = std::make_shared<NdefBtDataParser>();
        if (ndefBtDataParser == nullptr) {
            return;
        }
        ndefBtDataParser->ParseBtHandoverSelect(ndef);
        std::shared_ptr<NFC::KITS::NdefMessage> ndef1 = nullptr;
        ndefBtDataParser->ParseBtHandoverSelect(ndef1);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzCheckBtRecord(data, size);
    OHOS::FuzzIsVendorPayloadValid(data, size);
    OHOS::FuzzDispatchBundleAbility(data, size);
    OHOS::FuzzParseBleRecord(data, size);
    OHOS::FuzzParseBtHandoverSelect(data, size);
    return 0;
}