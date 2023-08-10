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
#include "common_event_handler.h"
#include "loghelper.h"
#include "nfc_controller.h"
#include "nfc_polling_params.h"
#include "nfc_sdk_common.h"
#include "nfc_watch_dog.h"
#include "nfcc_host.h"
#include "want.h"
#include "utils/preferences/nfc_pref_impl.h"
#include "tag_session.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC;
class NfcServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NfcServiceTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NfcServiceTest." << std::endl;
}

void NfcServiceTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NfcServiceTest." << std::endl;
}

void NfcServiceTest::SetUp()
{
    std::cout << " SetUp NfcServiceTest." << std::endl;
}

void NfcServiceTest::TearDown()
{
    std::cout << " TearDown NfcServiceTest." << std::endl;
}

/**
 * @tc.name: IsForegroundEnabled001
 * @tc.desc: Test NfcService IsForegroundEnabled.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, IsForegroundEnabled001, TestSize.Level1)
{
    std::shared_ptr<NFC::NfcService> nfcService = std::make_shared<NFC::NfcService>();
    bool enable = nfcService->IsForegroundEnabled();
    ASSERT_TRUE(enable == false);
}
/**
 * @tc.name: DisableForegroundByDeathRcpt001
 * @tc.desc: Test NfcService DisableForegroundByDeathRcpt.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, DisableForegroundByDeathRcpt001, TestSize.Level1)
{
    std::shared_ptr<NFC::NfcService> nfcService = std::make_shared<NFC::NfcService>();
    bool disable = nfcService->DisableForegroundByDeathRcpt();
    ASSERT_TRUE(disable == true);
}
/**
 * @tc.name: DisableForegroundDispatch001
 * @tc.desc: Test NfcService DisableForegroundDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, DisableForegroundDispatch001, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    std::shared_ptr<NFC::NfcService> nfcService = std::make_shared<NFC::NfcService>();
    bool disable = nfcService->DisableForegroundDispatch(element);
    ASSERT_TRUE(disable == true);
}
/**
 * @tc.name: EnableForegroundDispatch001
 * @tc.desc: Test NfcService EnableForegroundDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(NfcServiceTest, EnableForegroundDispatch001, TestSize.Level1)
{
    AppExecFwk::ElementName element;
    std::vector<uint32_t> discTech = {1, 2, 4, 5, 10};
    const sptr<KITS::IForegroundCallback> callback = nullptr;
    std::shared_ptr<NFC::NfcService> nfcService = std::make_shared<NFC::NfcService>();
    bool enable = nfcService->EnableForegroundDispatch(element, discTech, callback);
    ASSERT_TRUE(enable == false);
}
}
}
}