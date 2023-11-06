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
#include "nfc_service.h"
#include "nfcc_nci_adapter.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::NCI;

class NfccNciAdapterTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp();
    void TearDown();

    static const int ISO_DEP_MAX_TRANSEIVE_LENGTH = 0xFEFF;
};

void NfccNciAdapterTest::SetUp()
{
}

void NfccNciAdapterTest::TearDown()
{
}

/**
 * @tc.name: NfccNciAdapterTest001
 * @tc.desc: Test Constructor
 * @tc.type: FUNC
 */
HWTEST_F(NfccNciAdapterTest, NfccNciAdapterTest001, TestSize.Level1)
{
    NCI::NfccNciAdapter adapterObj = NCI::NfccNciAdapter::GetInstance();
    EXPECT_TRUE(!adapterObj.IsTagActive());
}

/**
 * @tc.name: NfccNciAdapterTest002
 * @tc.desc: Test Initialize or Deinitialize
 * @tc.type: FUNC
 */
HWTEST_F(NfccNciAdapterTest, NfccNciAdapterTest002, TestSize.Level1)
{
    NCI::NfccNciAdapter adapterObj = NCI::NfccNciAdapter::GetInstance();
    NCI::NfccNciAdapter::GetInstance().ClearT3tIdentifiersCache();
    EXPECT_TRUE(NCI::NfccNciAdapter::GetInstance().GetLfT3tMax() == 0);
    NCI::NfccNciAdapter::GetInstance().IsNfcActive();
    adapterObj.Deinitialize();
    std::string rawData = "00a40400";
    adapterObj.SendRawFrame(rawData);
    EXPECT_TRUE(NCI::NfccNciAdapter::GetInstance().GetLastError() == 0);
}

/**
 * @tc.name: NfccNciAdapterTest003
 * @tc.desc: Test Connect or Disconnect
 * @tc.type: FUNC
 */
HWTEST_F(NfccNciAdapterTest, NfccNciAdapterTest003, TestSize.Level1)
{
    NCI::NfccNciAdapter adapterObj = NCI::NfccNciAdapter::GetInstance();
    adapterObj.SetScreenStatus(0xFF);
    adapterObj.Initialize();
    adapterObj.SetScreenStatus(0xFF);
    adapterObj.SetScreenStatus(0xFF);
    adapterObj.SetScreenStatus(0xF0);
    adapterObj.SetScreenStatus(0xF2);
    adapterObj.SetScreenStatus(0xF4);
    adapterObj.SetScreenStatus(0xF8);
    adapterObj.GetNciVersion();
    std::string t3tIdentifier = "00a4";
    EXPECT_TRUE(adapterObj.RegisterT3tIdentifier(t3tIdentifier));
    adapterObj.DeregisterT3tIdentifier(0);
    EXPECT_TRUE(adapterObj.CheckFirmware());
    adapterObj.Dump(0);
    adapterObj.FactoryReset();
    adapterObj.Shutdown();
    EXPECT_TRUE(!adapterObj.IsTagActive());

    EXPECT_TRUE(!adapterObj.IsRfEbabled());
}
}
}
}
