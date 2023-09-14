/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "nfcc_host.h"
#include "nfc_service.h"
#include "tag_host.h"
#include "tag_nci_adapter.h"
#include "nfc_nci_adaptor.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::NCI;

class TagNciAdapterTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown();

    static const int DEFAULT_TIMEOUT = 1000;
    static const int ISO14443_3A_DEFAULT_TIMEOUT = 618;   // NfcA
    static const uint32_t MAX_NUM_TECHNOLOGY = 12;
    // const values for Mifare Ultralight
    static const int MANUFACTURER_ID_NXP = 0x04;
    static const int SAK_MIFARE_UL_1 = 0x00;
    static const int SAK_MIFARE_UL_2 = 0x04;
    static const int ATQA_MIFARE_UL_0 = 0x44;
    static const int ATQA_MIFARE_UL_1 = 0x00;

    // const values for Mifare DESFire
    static const int SAK_MIFARE_DESFIRE = 0x20;
    static const int ATQA_MIFARE_DESFIRE_0 = 0x44;
    static const int ATQA_MIFARE_DESFIRE_1 = 0x03;
};

void TagNciAdapterTest::SetUp()
{
}

void TagNciAdapterTest::TearDown()
{
}

/**
 * @tc.name: TagNciAdapterTest001
 * @tc.desc: Test IsNdefFormattable
 * @tc.type: FUNC
 */
HWTEST_F(TagNciAdapterTest, TagNciAdapterTest001, TestSize.Level1)
{
    NCI::TagNciAdapter adapterObj = NCI::TagNciAdapter::GetInstance();
    std::vector<uint16_t> systemCode = {0x88B4, 0, 0};
    tNFA_CONN_EVT_DATA eventData;
    tNFA_ACTIVATED activated;
    activated.params.t3t.num_system_codes = 1;
    activated.params.t3t.p_system_codes = &systemCode[0];
    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_A;
    activated.activate_ntf.rf_tech_param.param.pa.sel_rsp = SAK_MIFARE_UL_1;
    activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] = ATQA_MIFARE_UL_0;
    activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] = ATQA_MIFARE_UL_1;
    activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] = MANUFACTURER_ID_NXP;

    activated.activate_ntf.protocol = NFA_PROTOCOL_T2T;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);
    EXPECT_TRUE(adapterObj.IsNdefFormattable());

    activated.activate_ntf.protocol = NFA_PROTOCOL_T3T;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);
    EXPECT_TRUE(adapterObj.IsNdefFormattable());

    activated.activate_ntf.protocol = NFA_PROTOCOL_ISO_DEP;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);
    EXPECT_TRUE(!adapterObj.IsNdefFormattable());

    activated.activate_ntf.protocol = NFA_PROTOCOL_T1T;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);
    EXPECT_TRUE(adapterObj.IsNdefFormattable());

    activated.activate_ntf.protocol = NFA_PROTOCOL_INVALID;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);
    EXPECT_TRUE(!adapterObj.IsNdefFormattable());
}

/**
 * @tc.name: TagNciAdapterTest002
 * @tc.desc: Test Connect or Disconnect
 * @tc.type: FUNC
 */
HWTEST_F(TagNciAdapterTest, TagNciAdapterTest002, TestSize.Level1)
{
    NCI::TagNciAdapter adapterObj = NCI::TagNciAdapter::GetInstance();

    tNFA_STATUS statusConnect = adapterObj.Connect(0);
    EXPECT_TRUE(statusConnect == NFA_STATUS_BUSY);

    bool statusDisconnect = adapterObj.Disconnect();
    EXPECT_FALSE(!statusDisconnect);

    bool statusReconnect = adapterObj.Reconnect(0, 1, 1, false);
    EXPECT_TRUE(!statusReconnect);

    EXPECT_TRUE(!NCI::TagNciAdapter::IsReconnecting());

    adapterObj.ResetTag();
    EXPECT_TRUE(!NCI::TagNciAdapter::IsReconnecting());
}

/**
 * @tc.name: TagNciAdapterTest003
 * @tc.desc: Test Transmit
 * @tc.type: FUNC
 */
HWTEST_F(TagNciAdapterTest, TagNciAdapterTest003, TestSize.Level1)
{
    NCI::TagNciAdapter adapterObj = NCI::TagNciAdapter::GetInstance();
    std::string request = "00a40400";
    std::string response;
    EXPECT_TRUE(adapterObj.Transceive(request, response) == NFA_STATUS_BUSY);

    unsigned char data[] = {0x00, 0xa4, 0x04, 0x00};
    NCI::TagNciAdapter::HandleTranceiveData(NFA_STATUS_OK, data, 4);
    NCI::TagNciAdapter::HandleTranceiveData(NFA_STATUS_CONTINUE, data, 4);

    NCI::TagNciAdapter::HandleFieldCheckResult(NFA_STATUS_OK);

    NCI::TagNciAdapter::HandleSelectResult();
    NCI::TagNciAdapter::HandleActivatedResult();
    NCI::TagNciAdapter::HandleDeactivatedResult();
    adapterObj.ResetTagFieldOnFlag();

    EXPECT_TRUE(adapterObj.GetTimeout((MAX_NUM_TECHNOLOGY + 1)) == DEFAULT_TIMEOUT);
    adapterObj.ResetTimeout();
    EXPECT_TRUE(adapterObj.GetTimeout(TagHost::TARGET_TYPE_ISO14443_3A) == ISO14443_3A_DEFAULT_TIMEOUT);
}

/**
 * @tc.name: TagNciAdapterTest004
 * @tc.desc: Test NDEF
 * @tc.type: FUNC
 */
HWTEST_F(TagNciAdapterTest, TagNciAdapterTest004, TestSize.Level1)
{
    NCI::TagNciAdapter adapterObj = NCI::TagNciAdapter::GetInstance();
    EXPECT_TRUE(!adapterObj.SetReadOnly());

    std::string response;
    adapterObj.ReadNdef(response);
    EXPECT_TRUE(response.empty());
    NCI::TagNciAdapter::HandleReadComplete(NFA_STATUS_BUSY);

    std::string command = "00a40400";
    EXPECT_TRUE(!adapterObj.WriteNdef(command));
    NCI::TagNciAdapter::HandleWriteComplete(NFA_STATUS_BUSY);

    EXPECT_TRUE(!adapterObj.FormatNdef());
    NCI::TagNciAdapter::HandleFormatComplete(NFA_STATUS_BUSY);

    EXPECT_TRUE(!adapterObj.IsNdefFormatable());

    adapterObj.HandleNdefCheckResult(NFA_STATUS_BUSY, 0, 0xFFFFFFFF, 0);
    adapterObj.HandleNdefCheckResult(NFA_STATUS_OK, 0, 0xFFFFFFFF, 0);
    adapterObj.HandleNdefCheckResult(NFA_STATUS_FAILED, 0, 0xFFFFFFFF, 0);
    adapterObj.HandleNdefCheckResult(NFA_STATUS_BUSY, 0, 0xFFFFFF00, 0);
    adapterObj.HandleNdefCheckResult(NFA_STATUS_FAILED, 0, 0xFFFFFF04, 0);
    adapterObj.HandleNdefCheckResult(NFA_STATUS_REJECTED, 0, 0xFFFFFFFF, 0);

    std::vector<int> ndefInfo{};
    EXPECT_TRUE(!adapterObj.IsNdefMsgContained(ndefInfo));
}

/**
 * @tc.name: TagNciAdapterTest005
 * @tc.desc: Test Field On/Off
 * @tc.type: FUNC
 */
HWTEST_F(TagNciAdapterTest, TagNciAdapterTest005, TestSize.Level1)
{
    NCI::TagNciAdapter adapterObj = NCI::TagNciAdapter::GetInstance();
    EXPECT_TRUE(!adapterObj.SetReadOnly());

    adapterObj.OnRfDiscLock();
    adapterObj.OffRfDiscLock();

    adapterObj.SetNciAdaptations(nullptr);
    NCI::TagNciAdapter::AbortWait();

    tNFA_DISC_RESULT discoveryData;
    adapterObj.GetMultiTagTechsFromData(discoveryData);

    tNFA_CONN_EVT_DATA eventData{};
    adapterObj.BuildTagInfo(&eventData);

    adapterObj.SelectTheFirstTag();
    adapterObj.SelectTheNextTag();

    adapterObj.SetIsMultiTag(true);
    EXPECT_TRUE(adapterObj.GetIsMultiTag());

    adapterObj.SetDiscRstEvtNum(TagHost::TARGET_TYPE_ISO14443_3A);
    EXPECT_TRUE(adapterObj.GetDiscRstEvtNum() == TagHost::TARGET_TYPE_ISO14443_3A);
}

/**
 * @tc.name: TagNciAdapterTest006
 * @tc.desc: Test HandleDiscResult
 * @tc.type: FUNC
 */
HWTEST_F(TagNciAdapterTest, TagNciAdapterTest006, TestSize.Level1)
{
    NCI::TagNciAdapter adapterObj = NCI::TagNciAdapter::GetInstance();
    std::shared_ptr<INfcNci> nciAdaptations = std::make_shared<NfcNciAdaptor>();
    adapterObj.SetNciAdaptations(nciAdaptations);
    adapterObj.HandleDiscResult(nullptr);
    tNFA_CONN_EVT_DATA eventData;
    tNFC_RESULT_DEVT discoveryNtf;
    discoveryNtf.more = NCI_DISCOVER_NTF_MORE;
    discoveryNtf.protocol = NFA_PROTOCOL_NFC_DEP;
    eventData.disc_result.discovery_ntf = discoveryNtf;
    adapterObj.HandleDiscResult(&eventData);

    discoveryNtf.more = NCI_DISCOVER_NTF_LAST;
    discoveryNtf.protocol = NFA_PROTOCOL_ISO_DEP;
    eventData.disc_result.discovery_ntf = discoveryNtf;
    adapterObj.HandleDiscResult(&eventData);

    discoveryNtf.more = NCI_DISCOVER_NTF_LAST;
    discoveryNtf.protocol = NFC_PROTOCOL_MIFARE;
    eventData.disc_result.discovery_ntf = discoveryNtf;
    adapterObj.HandleDiscResult(&eventData);
}

/**
 * @tc.name: TagNciAdapterTest007
 * @tc.desc: Test BuildTagInfo
 * @tc.type: FUNC
 */
HWTEST_F(TagNciAdapterTest, TagNciAdapterTest007, TestSize.Level1)
{
    NCI::TagNciAdapter adapterObj = NCI::TagNciAdapter::GetInstance();
    std::vector<uint16_t> systemCode = {0x88B4, 0, 0};
    tNFA_CONN_EVT_DATA eventData;
    tNFA_ACTIVATED activated;
    activated.activate_ntf.protocol = NCI_PROTOCOL_T1T;
    activated.params.t3t.num_system_codes = 1;
    activated.params.t3t.p_system_codes = &systemCode[0];
    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_A;
    activated.activate_ntf.rf_tech_param.param.pa.sel_rsp = 0004;
    activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] = ATQA_MIFARE_UL_0;
    activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] = ATQA_MIFARE_UL_1;
    activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] = MANUFACTURER_ID_NXP;
    eventData.activated = activated;
    adapterObj.SetDiscRstEvtNum(1);
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.protocol = NCI_PROTOCOL_T2T;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.protocol = NCI_PROTOCOL_T3BT;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.protocol = NCI_PROTOCOL_T3T;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.protocol = NCI_PROTOCOL_15693;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);
}

/**
 * @tc.name: TagNciAdapterTest008
 * @tc.desc: Test BuildTagInfo
 * @tc.type: FUNC
 */
HWTEST_F(TagNciAdapterTest, TagNciAdapterTest008, TestSize.Level1)
{
    NCI::TagNciAdapter adapterObj = NCI::TagNciAdapter::GetInstance();
    std::vector<uint16_t> systemCode = {0x88B4, 0, 0};
    tNFA_CONN_EVT_DATA eventData;
    tNFA_ACTIVATED activated;
    activated.activate_ntf.protocol = NCI_PROTOCOL_ISO_DEP;
    activated.params.t3t.num_system_codes = 1;
    activated.params.t3t.p_system_codes = &systemCode[0];
    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_A;
    activated.activate_ntf.rf_tech_param.param.pa.sel_rsp = 0004;
    activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] = ATQA_MIFARE_UL_0;
    activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] = ATQA_MIFARE_UL_1;
    activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] = MANUFACTURER_ID_NXP;
    adapterObj.SetDiscRstEvtNum(1);
    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_B;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_F;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.rf_tech_param.mode = 0x0F;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.rf_tech_param.mode = NFC_DISCOVERY_TYPE_LISTEN_A_ACTIVE;
    activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] = ATQA_MIFARE_DESFIRE_0;
    activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] = ATQA_MIFARE_DESFIRE_1;
    activated.activate_ntf.rf_tech_param.param.pa.sel_rsp = SAK_MIFARE_DESFIRE;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);
}

/**
 * @tc.name: TagNciAdapterTest009
 * @tc.desc: Test BuildTagInfo NCI_DISCOVERY_TYPE_POLL_A
 * @tc.type: FUNC
 */
HWTEST_F(TagNciAdapterTest, TagNciAdapterTest009, TestSize.Level1)
{
    NCI::TagNciAdapter adapterObj = NCI::TagNciAdapter::GetInstance();
    adapterObj.ResetTag();
    adapterObj.SetDiscRstEvtNum(1);
    std::vector<uint16_t> systemCode = {0x88B4, 0, 0};
    tNFA_CONN_EVT_DATA eventData;
    tNFA_ACTIVATED activated;
    activated.params.t3t.p_system_codes = &systemCode[0];

    activated.activate_ntf.protocol = NCI_PROTOCOL_ISO_DEP;
    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_A;
    activated.activate_ntf.intf_param.type = NFC_INTERFACE_ISO_DEP;
    activated.activate_ntf.intf_param.intf_param.pa_iso.his_byte[0] = 0;
    activated.activate_ntf.intf_param.intf_param.pa_iso.his_byte_len = 1;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.intf_param.intf_param.pa_iso.his_byte[0] = 0;
    activated.activate_ntf.intf_param.intf_param.pa_iso.his_byte_len = 0;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.intf_param.type = NFC_INTERFACE_FRAME;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);
}

/**
 * @tc.name: TagNciAdapterTest0010
 * @tc.desc: Test BuildTagInfo NCI_DISCOVERY_TYPE_POLL_B
 * @tc.type: FUNC
 */
HWTEST_F(TagNciAdapterTest, TagNciAdapterTest0010, TestSize.Level1)
{
    NCI::TagNciAdapter adapterObj = NCI::TagNciAdapter::GetInstance();
    adapterObj.ResetTag();
    adapterObj.SetDiscRstEvtNum(1);
    std::vector<uint16_t> systemCode = {0x88B4, 0, 0};
    std::vector<uint8_t> hisByte = {0};
    tNFA_CONN_EVT_DATA eventData;
    tNFA_ACTIVATED activated;
    activated.params.t3t.p_system_codes = &systemCode[0];

    activated.activate_ntf.protocol = NCI_PROTOCOL_ISO_DEP;
    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_B;
    activated.activate_ntf.rf_tech_param.param.pb.sensb_res_len = 0;
    activated.activate_ntf.intf_param.type = NFC_INTERFACE_ISO_DEP;
    activated.activate_ntf.intf_param.intf_param.pa_iso.his_byte[0] = 0;
    activated.activate_ntf.intf_param.intf_param.pa_iso.his_byte_len = 1;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);
    activated.activate_ntf.rf_tech_param.param.pb.sensb_res_len = NFC_NFCID0_MAX_LEN + 1;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);
    activated.activate_ntf.intf_param.type = NFC_INTERFACE_NFC_DEP;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.protocol = NCI_PROTOCOL_ISO_DEP;
    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_F;
    activated.params.t3t.num_system_codes = 1;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);
    activated.params.t3t.num_system_codes = 0;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.protocol = NCI_PROTOCOL_T3BT;
    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_B;
    activated.activate_ntf.intf_param.intf_param.pa_iso.his_byte_len = 0;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);
}

/**
 * @tc.name: TagNciAdapterTest0011
 * @tc.desc: Test BuildTagInfo: IsDiscTypeA/IsDiscTypeB/IsDiscTypeF/IsDiscTypeV
 * @tc.type: FUNC
 */
HWTEST_F(TagNciAdapterTest, TagNciAdapterTest0011, TestSize.Level1)
{
    NCI::TagNciAdapter adapterObj = NCI::TagNciAdapter::GetInstance();
    std::vector<uint16_t> systemCode = {0x88B4, 0, 0};
    tNFA_CONN_EVT_DATA eventData;
    tNFA_ACTIVATED activated;
    activated.activate_ntf.protocol = NCI_PROTOCOL_T1T;
    activated.params.t3t.num_system_codes = 1;
    activated.params.t3t.p_system_codes = &systemCode[0];
    activated.activate_ntf.rf_tech_param.param.pa.sel_rsp = SAK_MIFARE_DESFIRE;
    activated.activate_ntf.rf_tech_param.param.pa.sens_res[0] = ATQA_MIFARE_UL_0;
    activated.activate_ntf.rf_tech_param.param.pa.sens_res[1] = ATQA_MIFARE_UL_1;
    activated.activate_ntf.rf_tech_param.param.pa.nfcid1[0] = MANUFACTURER_ID_NXP;

    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_A_ACTIVE;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_LISTEN_A;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_LISTEN_A_ACTIVE;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.rf_tech_param.mode = NFC_DISCOVERY_TYPE_POLL_B_PRIME;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_LISTEN_B;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.rf_tech_param.mode = NFC_DISCOVERY_TYPE_LISTEN_B_PRIME;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_F_ACTIVE;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_LISTEN_F;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_LISTEN_F_ACTIVE;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_POLL_V;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);

    activated.activate_ntf.rf_tech_param.mode = NCI_DISCOVERY_TYPE_LISTEN_ISO15693;
    eventData.activated = activated;
    adapterObj.BuildTagInfo(&eventData);
}

/**
 * @tc.name: TagNciAdapterTest0012
 * @tc.desc: Test SelectTheNextTag
 * @tc.type: FUNC
 */
HWTEST_F(TagNciAdapterTest, TagNciAdapterTest0012, TestSize.Level1)
{
    NCI::TagNciAdapter adapterObj = NCI::TagNciAdapter::GetInstance();
    adapterObj.SetDiscRstEvtNum(1);
    tNFA_DISC_RESULT discoveryData;
    discoveryData.discovery_ntf.rf_disc_id = NFA_PROTOCOL_NFC_DEP;
    discoveryData.discovery_ntf.protocol = NFA_PROTOCOL_NFC_DEP;
    adapterObj.GetMultiTagTechsFromData(discoveryData);
    adapterObj.SelectTheFirstTag();

    adapterObj.SetDiscRstEvtNum(2);
    discoveryData.discovery_ntf.rf_disc_id = NFA_PROTOCOL_ISO_DEP;
    discoveryData.discovery_ntf.protocol = NFA_PROTOCOL_ISO_DEP;
    adapterObj.GetMultiTagTechsFromData(discoveryData);
    adapterObj.SelectTheFirstTag();
    adapterObj.SelectTheNextTag();
}

}
}
}
