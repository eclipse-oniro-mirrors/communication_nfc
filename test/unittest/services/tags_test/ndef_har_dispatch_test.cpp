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
#include <gtest/gtest.h>
#include <thread>
#include "ndef_har_dispatch.h"
#include "iservice_registry.h"
#include "ndef_har_data_parser.h"
#include "tag_ability_dispatcher.h"
#include "ability_manager_client.h"
#include "loghelper.h"
#include "bundle_mgr_interface.h"
#include "if_system_ability_manager.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::KITS;
class NdefHarDispatchTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
public:
};

void NdefHarDispatchTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NdefHarDispatchTest." << std::endl;
}

void NdefHarDispatchTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NdefHarDispatchTest." << std::endl;
}

void NdefHarDispatchTest::SetUp()
{
    std::cout << " SetUp NdefHarDispatchTest." << std::endl;
}

void NdefHarDispatchTest::TearDown()
{
    std::cout << " TearDown NdefHarDispatchTest." << std::endl;
}

/**
 * @tc.name: GetNdefNdefHarDispatch001
 * @tc.desc: Test NdefHarDispatchTest GetNdefNdefHarDispatch.
 * @tc.type: FUNC
 */
HWTEST_F(NdefHarDispatchTest, GetNdefNdefHarDispatch001, TestSize.Level1)
{
    std::shared_ptr<NdefHarDispatch> ndefHarDispatchTest = std::make_shared<NdefHarDispatch>();
    std::string harPackage = "";
    ndefHarDispatchTest->DispatchBundleAbility(harPackage);
    harPackage = "ABC";
    ndefHarDispatchTest->DispatchBundleAbility(harPackage);

    std::string uri = "";
    ndefHarDispatchTest->DispatchUriToBundleAbility(uri);
    uri = "ABC";
    ndefHarDispatchTest->DispatchUriToBundleAbility(uri);

    std::string type = "";
    ndefHarDispatchTest->DispatchMimeType(type);
    type = "ABC";
    ndefHarDispatchTest->DispatchMimeType(type);

    std::string webAddress = "";
    std::string browserBundleName = "";
    ndefHarDispatchTest->DispatchWebLink(webAddress, browserBundleName);
    ASSERT_TRUE(ndefHarDispatchTest != nullptr);
}

}
}
}