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
};

void TagNciAdapterTest::SetUp()
{
}

void TagNciAdapterTest::TearDown()
{
}

/**
 * @tc.name: TagNciAdapterTest001
 * @tc.desc: Test Constructor
 * @tc.type: FUNC
 */
HWTEST_F(TagNciAdapterTest, TagNciAdapterTest001, TestSize.Level1)
{
    NCI::TagNciAdapter adapterObj = NCI::TagNciAdapter::GetInstance();
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

    tNFA_STATUS statusConnect = adapterObj.Connect(0, 1, 1);
    EXPECT_TRUE(statusConnect == NFA_STATUS_BUSY);

    bool statusDisconnect = adapterObj.Disconnect();
    EXPECT_TRUE(!statusDisconnect);

    bool statusReconnect = adapterObj.Reconnect(0, 1, 1, false);
    EXPECT_TRUE(!statusReconnect);

    EXPECT_TRUE(!NCI::TagNciAdapter::IsReconnecting());

    EXPECT_TRUE(!adapterObj.NfaDeactivateAndSelect(0, 1));

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
}
}
}
