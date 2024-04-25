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

#include "isodep_card_handler.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::TAG;
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
} // namespace TEST
} // namespace NFC
} // namespace OHOS