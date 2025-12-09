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
#define private public
#define protected public

#include <gtest/gtest.h>
#include <thread>

#include "isodep_card_handler.h"
#include "nci_native_selector.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::TAG;
using namespace OHOS::NFC::NCI;
class IsodepCardHandlerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void IsodepCardHandlerTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase IsodepCardHandlerTest." << std::endl;
}

void IsodepCardHandlerTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase IsodepCardHandlerTest." << std::endl;
}

void IsodepCardHandlerTest::SetUp()
{
    std::cout << " SetUp IsodepCardHandlerTest." << std::endl;
}

void IsodepCardHandlerTest::TearDown()
{
    std::cout << " TearDown IsodepCardHandlerTest." << std::endl;
}

/**
 * @tc.name: InitTransportCardInfo001
 * @tc.desc: Test IsodepCardHandlerTest InitTransportCardInfo.
 * @tc.type: FUNC
 */
HWTEST_F(IsodepCardHandlerTest, InitTransportCardInfo001, TestSize.Level1)
{
    std::shared_ptr<NCI::INciTagInterface> nciTagProxy = nullptr;
    std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
    isodepCardHandler->InitTransportCardInfo();
    ASSERT_TRUE(isodepCardHandler != nullptr);
}

/**
 * @tc.name: IsSupportedTransportCard001
 * @tc.desc: Test IsodepCardHandlerTest IsSupportedTransportCard.
 * @tc.type: FUNC
 */
HWTEST_F(IsodepCardHandlerTest, IsSupportedTransportCard001, TestSize.Level1)
{
    std::shared_ptr<NCI::INciTagInterface> nciTagProxy = nullptr;
    uint32_t rfDiscId = 0;
    uint8_t cardIndex = 0;
    std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
    bool res = isodepCardHandler->IsSupportedTransportCard(rfDiscId, cardIndex);
    ASSERT_TRUE(!res);
}

/**
 * @tc.name: IsSupportedTransportCard002
 * @tc.desc: Test IsodepCardHandlerTest IsSupportedTransportCard.
 * @tc.type: FUNC
 */
HWTEST_F(IsodepCardHandlerTest, IsSupportedTransportCard002, TestSize.Level1)
{
    std::shared_ptr<NCI::INciTagInterface> nciTagProxy = NciNativeSelector::GetInstance().GetNciTagInterface();
    uint32_t rfDiscId = 0;
    uint8_t cardIndex = 0;
    std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
    bool res = isodepCardHandler->IsSupportedTransportCard(rfDiscId, cardIndex);
    ASSERT_TRUE(!res);
}

/**
 * @tc.name: GetBalance001
 * @tc.desc: Test IsodepCardHandlerTest GetBalance.
 * @tc.type: FUNC
 */
HWTEST_F(IsodepCardHandlerTest, GetBalance001, TestSize.Level1)
{
    std::shared_ptr<NCI::INciTagInterface> nciTagProxy = nullptr;
    uint32_t rfDiscId = 0;
    uint8_t cardIndex = 0;
    int balance = 0;
    std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
    isodepCardHandler->GetBalance(rfDiscId, cardIndex, balance);
    ASSERT_TRUE(isodepCardHandler != nullptr);
}

/**
 * @tc.name: GetCardName001
 * @tc.desc: Test IsodepCardHandlerTest GetCardName.
 * @tc.type: FUNC
 */
HWTEST_F(IsodepCardHandlerTest, GetCardName001, TestSize.Level1)
{
    std::shared_ptr<NCI::INciTagInterface> nciTagProxy = nullptr;
    uint8_t cardIndex = 0;
    std::string cardName = "";
    std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
    isodepCardHandler->GetCardName(cardIndex, cardName);
    ASSERT_TRUE(isodepCardHandler != nullptr);
}

/**
 * @tc.name: MatchCity
 * @tc.desc: Test IsodepCardHandlerTest MatchCity.
 * @tc.type: FUNC
 */
HWTEST_F(IsodepCardHandlerTest, MatchCity, TestSize.Level1)
{
    std::shared_ptr<NCI::INciTagInterface> nciTagProxy = nullptr;
    std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
    isodepCardHandler->MatchCity(0, 0);
    ASSERT_TRUE(isodepCardHandler != nullptr);
}

/**
 * @tc.name: GetBalanceValue
 * @tc.desc: Test IsodepCardHandlerTest GetBalanceValue.
 * @tc.type: FUNC
 */
HWTEST_F(IsodepCardHandlerTest, GetBalanceValue, TestSize.Level1)
{
    std::string balanceStr = "test";
    int balanceValue = 0;
    std::weak_ptr<INciTagInterface> nciTagProxy;
    std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
    isodepCardHandler->GetBalanceValue(balanceStr, balanceValue);
    ASSERT_TRUE(isodepCardHandler != nullptr);
}

/**
 * @tc.name: GetCardName
 * @tc.desc: Test IsodepCardHandlerTest GetCardName.
 * @tc.type: FUNC
 */
HWTEST_F(IsodepCardHandlerTest, GetCardName, TestSize.Level1)
{
    uint8_t cardIndex = 1;
    std::string cardName = "test";
    std::weak_ptr<INciTagInterface> nciTagProxy;
    std::shared_ptr<IsodepCardHandler> isodepCardHandler = std::make_shared<IsodepCardHandler>(nciTagProxy);
    isodepCardHandler->GetCardName(cardIndex, cardName);
    ASSERT_TRUE(isodepCardHandler != nullptr);
}

} // namespace TEST
} // namespace NFC
} // namespace OHOS