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
#include <unistd.h>
#include "app_data_parser.h"
#include "nfc_event_handler.h"
#include "loghelper.h"
#include "nfc_controller.h"
#include "nfc_polling_params.h"
#include "nfc_sdk_common.h"
#include "nfc_watch_dog.h"
#include "nfcc_host.h"
#include "want.h"
#include "nfc_database_helper.h"
#include "tag_session.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class NfcPollingManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    std::shared_ptr<NCI::INfccHost> nfccHost_ {};
    std::shared_ptr<NfcService> nfcService_ {};
};

void NfcPollingManagerTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcPollingManagerTest." << std::endl;
}

void NfcPollingManagerTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcPollingManagerTest." << std::endl;
}

void NfcPollingManagerTest::SetUp()
{
    std::cout << " SetUp NfcPollingManagerTest." << std::endl;
}

void NfcPollingManagerTest::TearDown()
{
    std::cout << " TearDown NfcPollingManagerTest." << std::endl;
}

/**
 * @tc.name: IsForegroundEnabled001
 * @tc.desc: Test NfcPollingManager IsForegroundEnabled.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, IsForegroundEnabled001, TestSize.Level1)
{
    std::shared_ptr<NFC::NfcPollingManager> nfcPollingManager = std::make_shared<NFC::NfcPollingManager>(nfccHost_,
        nfcService_);
    bool enable = nfcPollingManager->IsForegroundEnabled();
    ASSERT_TRUE(enable == false);
}
/**
 * @tc.name: DisableForegroundByDeathRcpt001
 * @tc.desc: Test NfcPollingManager DisableForegroundByDeathRcpt.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, DisableForegroundByDeathRcpt001, TestSize.Level1)
{
    std::shared_ptr<NFC::NfcPollingManager> nfcPollingManager = std::make_shared<NFC::NfcPollingManager>(nfccHost_,
        nfcService_);
    bool disable = nfcPollingManager->DisableForegroundByDeathRcpt();
    ASSERT_TRUE(disable == true);
}
/**
 * @tc.name: DisableForegroundDispatch001
 * @tc.desc: Test NfcPollingManager DisableForegroundDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, DisableForegroundDispatch001, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    std::shared_ptr<NFC::NfcPollingManager> nfcPollingManager = std::make_shared<NFC::NfcPollingManager>(nfccHost_,
        nfcService_);
    bool disable = nfcPollingManager->DisableForegroundDispatch(element);
    ASSERT_TRUE(disable == true);
}
/**
 * @tc.name: EnableForegroundDispatch001
 * @tc.desc: Test NfcPollingManager EnableForegroundDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(NfcPollingManagerTest, EnableForegroundDispatch001, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    std::vector<uint32_t> discTech = {1, 2, 4, 5, 10};
    const sptr<KITS::IForegroundCallback> callback = nullptr;
    std::shared_ptr<NFC::NfcPollingManager> nfcPollingManager = std::make_shared<NFC::NfcPollingManager>(nfccHost_,
        nfcService_);
    bool enable = nfcPollingManager->EnableForegroundDispatch(element, discTech, callback);
    ASSERT_TRUE(enable == false);
}
}
}
}