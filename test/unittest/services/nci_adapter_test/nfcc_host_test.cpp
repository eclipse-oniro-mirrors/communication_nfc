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

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::NCI;

enum EmNfcForumType {
    NFC_FORUM_TYPE_UNKNOWN = 0,
    NFC_FORUM_TYPE_1 = 1,
    NFC_FORUM_TYPE_2 = 2,
    NFC_FORUM_TYPE_3 = 3,
    NFC_FORUM_TYPE_4 = 4,
    MIFARE_CLASSIC = 101,
    ICODE_SLI = 102
};

class NfccHostTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown();

    std::shared_ptr<NCI::NfccHost> nfccHostTest_ {};
    std::shared_ptr<NfcService> nfcService_;
};

void NfccHostTest::SetUp()
{
    std::shared_ptr<NfcService> nfcService_ = std::make_shared<NfcService>();
    nfccHostTest_ = std::make_shared<NFC::NCI::NfccHost>(nfcService_);
}

void NfccHostTest::TearDown()
{
    nfccHostTest_ = nullptr;
    nfcService_ = nullptr;
}

/**
 * @tc.name: SendRawFrameTest001
 * @tc.desc: Test SendRawFrame
 * @tc.type: FUNC
 */
HWTEST_F(NfccHostTest, SendRawFrameTest001, TestSize.Level1)
{
    std::string getCplc = "80CA9F7F00";
    nfccHostTest_->SendRawFrame(getCplc);
    EXPECT_TRUE(nfccHostTest_->GetNciVersion() >= 0);
}

/**
 * @tc.name: SetSecureNfcTest001
 * @tc.desc: Test SetSecureNfc
 * @tc.type: FUNC
 */
HWTEST_F(NfccHostTest, SetSecureNfcTest001, TestSize.Level1)
{
    EXPECT_TRUE(nfccHostTest_->GetIsoDepMaxTransceiveLength() >= 0);
    EXPECT_TRUE(nfccHostTest_->SetSecureNfc(false));
    EXPECT_EQ(nfccHostTest_->GetLfT3tMax(), 0);
    EXPECT_EQ(nfccHostTest_->GetLastError(), 0);
    nfccHostTest_->Dump(0);
    nfccHostTest_->FactoryReset();
    nfccHostTest_->Shutdown();
}

/**
 * @tc.name: RegisterT3tIdentifierTest001
 * @tc.desc: Test RegisterT3tIdentifier
 * @tc.type: FUNC
 */
HWTEST_F(NfccHostTest, RegisterT3tIdentifierTest001, TestSize.Level1)
{
    std::string identfier = "t3t";
    nfccHostTest_->RegisterT3tIdentifier(identfier);
    nfccHostTest_->DeregisterT3tIdentifier(identfier);
    nfccHostTest_->ClearT3tIdentifiersCache();
    identfier = "";
    nfccHostTest_->RegisterT3tIdentifier(identfier);
}

/**
 * @tc.name: AddAidRoutingTest001
 * @tc.desc: Test AddAidRouting
 * @tc.type: FUNC
 */
HWTEST_F(NfccHostTest, AddAidRoutingTest001, TestSize.Level1)
{
    std::string aid = "A0000001150000";
    int route = 0;
    int aidInfo = 0;
    EXPECT_TRUE(nfccHostTest_->AddAidRouting(aid, route, aidInfo));
    EXPECT_TRUE(nfccHostTest_->RemoveAidRouting(aid));
    EXPECT_TRUE(nfccHostTest_->ClearAidTable());
}

/**
 * @tc.name: CommitRoutingTest001
 * @tc.desc: Test CommitRouting
 * @tc.type: FUNC
 */
HWTEST_F(NfccHostTest, CommitRoutingTest001, TestSize.Level1)
{
    EXPECT_EQ(nfccHostTest_->GetAidRoutingTableSize(), 0);
    EXPECT_EQ(nfccHostTest_->GetDefaultRoute(), 0);
    EXPECT_EQ(nfccHostTest_->GetDefaultOffHostRoute(), 0);
    EXPECT_EQ(nfccHostTest_->GetAidMatchingMode(), 0);
    EXPECT_EQ(nfccHostTest_->GetRemainRoutingTableSize(), 0);
    EXPECT_EQ(nfccHostTest_->GetDefaultIsoDepRouteDestination(), 0);
    EXPECT_TRUE(nfccHostTest_->GetOffHostUiccRoute().empty());
    EXPECT_TRUE(nfccHostTest_->GetOffHostEseRoute().empty());
    EXPECT_TRUE(nfccHostTest_->CheckFirmware());
    unsigned char screenStateMask = {0};
    EXPECT_TRUE(nfccHostTest_->SetScreenStatus(screenStateMask));
    nfccHostTest_->SetNciAdaptation(nullptr);
    nfccHostTest_->EeUpdate();
    std::string aid = "";
    std::string data = "";
    std::string seName = "";
    nfccHostTest_->OffHostTransactionEvent(aid, data, seName);
    nfccHostTest_->HostCardEmulationActivated(0);
    std::string res = "";
    nfccHostTest_->HostCardEmulationDataReceived(0, res);
    nfccHostTest_->HostCardEmulationDeactivated(0);
}

/**
 * @tc.name: CanMakeReadOnlyTest001
 * @tc.desc: Test CanMakeReadOnly
 * @tc.type: FUNC
 */
HWTEST_F(NfccHostTest, CanMakeReadOnlyTest001, TestSize.Level1)
{
    int ndefType = EmNfcForumType::NFC_FORUM_TYPE_1;
    EXPECT_TRUE(nfccHostTest_->CanMakeReadOnly(ndefType));
    ndefType = EmNfcForumType::NFC_FORUM_TYPE_3;
    EXPECT_FALSE(nfccHostTest_->CanMakeReadOnly(ndefType));
}

/**
 * @tc.name: TagDiscoveredTest001
 * @tc.desc: Test TagDiscovered
 * @tc.type: FUNC
 */
HWTEST_F(NfccHostTest, TagDiscoveredTest001, TestSize.Level1)
{
    std::shared_ptr<NCI::ITagHost> tagHost = nullptr;
    nfccHostTest_->TagDiscovered(tagHost);
}

/**
 * @tc.name: RemoteFieldActivatedTest001
 * @tc.desc: Test RemoteFieldActivated
 * @tc.type: FUNC
 */
HWTEST_F(NfccHostTest, RemoteFieldActivatedTest001, TestSize.Level1)
{
    std::shared_ptr<OHOS::NFC::NfcService> listener = nullptr;
    nfccHostTest_->SetNfccHostListener(listener);
    nfccHostTest_->RemoteFieldActivated();
    nfccHostTest_->RemoteFieldDeactivated();

    nfccHostTest_->SetNfccHostListener(nfcService_);
    nfccHostTest_->RemoteFieldActivated();
    nfccHostTest_->RemoteFieldDeactivated();
}
}
}
}
