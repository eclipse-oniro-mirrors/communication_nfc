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

#include "nci_tag_proxy.h"

namespace OHOS {
namespace NFC {
namespace TEST {
using namespace testing::ext;
using namespace OHOS::NFC::NCI;
class NciTagProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void NciTagProxyTest::SetUpTestCase()
{
    std::cout << " SetUpTestCase NciTagProxyTest." << std::endl;
}

void NciTagProxyTest::TearDownTestCase()
{
    std::cout << " TearDownTestCase NciTagProxyTest." << std::endl;
}

void NciTagProxyTest::SetUp()
{
    std::cout << " SetUp NciTagProxyTest." << std::endl;
}

void NciTagProxyTest::TearDown()
{
    std::cout << " TearDown NciTagProxyTest." << std::endl;
}

/**
 * @tc.name: SetTagListener001
 * @tc.desc: Test NciTagProxyTest SetTagListener.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, SetTagListener001, TestSize.Level1)
{
    std::shared_ptr<NCI::INciTagInterface::ITagListener> listener = nullptr;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->SetTagListener(listener);
    uint32_t tagDiscId = 0;
    std::vector<AppExecFwk::PacMap> getTechExtrasData = nciTagProxy->GetTechExtrasData(tagDiscId);
    ASSERT_TRUE(getTechExtrasData.size() == 0);
}

/**
 * @tc.name: GetTechList001
 * @tc.desc: Test NciTagProxyTest GetTechList.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTechList001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    std::vector<int> getTechList = nciTagProxy->GetTechList(tagDiscId);
    ASSERT_TRUE(getTechList.size() == 0);
}

/**
 * @tc.name: GetConnectedTech001
 * @tc.desc: Test NciTagProxyTest GetConnectedTech.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetConnectedTech001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    int getConnectedTech = nciTagProxy->GetConnectedTech(tagDiscId);
    ASSERT_TRUE(getConnectedTech == 0);
}

/**
 * @tc.name: GetTechExtrasData001
 * @tc.desc: Test NciTagProxyTest GetTechExtrasData.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTechExtrasData001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    std::vector<AppExecFwk::PacMap> getTechExtrasData = nciTagProxy->GetTechExtrasData(tagDiscId);
    ASSERT_TRUE(getTechExtrasData.size() == 0);
}

/**
 * @tc.name: GetTagUid001
 * @tc.desc: Test NciTagProxyTest GetTagUid.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTagUid001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    std::string getTagUid = nciTagProxy->GetTagUid(tagDiscId);
    ASSERT_TRUE(getTagUid == "");
}

/**
 * @tc.name: Connect001
 * @tc.desc: Test NciTagProxyTest Connect.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Connect001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    uint32_t technology = 0;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    bool connect = nciTagProxy->Connect(tagDiscId, technology);
    ASSERT_TRUE(connect == false);
}

/**
 * @tc.name: Disconnect001
 * @tc.desc: Test NciTagProxyTest Disconnect.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Disconnect001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    bool disconnect = nciTagProxy->Disconnect(tagDiscId);
    ASSERT_TRUE(disconnect == false);
}

/**
 * @tc.name: Reconnect001
 * @tc.desc: Test NciTagProxyTest Reconnect.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Reconnect001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    bool reconnect = nciTagProxy->Reconnect(tagDiscId);
    ASSERT_TRUE(reconnect == false);
}

/**
 * @tc.name: Transceive001
 * @tc.desc: Test NciTagProxyTest Transceive.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, Transceive001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    std::string command = "";
    std::string response = "";
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    int transceive = nciTagProxy->Transceive(tagDiscId, command, response);
    ASSERT_TRUE(transceive == 0);
}

/**
 * @tc.name: ReadNdef001
 * @tc.desc: Test NciTagProxyTest ReadNdef.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, ReadNdef001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    std::string readNdef = nciTagProxy->ReadNdef(tagDiscId);
    ASSERT_TRUE(readNdef == "");
}

/**
 * @tc.name: FindNdefTech001
 * @tc.desc: Test NciTagProxyTest FindNdefTech.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, FindNdefTech001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    std::string findNdefTech = nciTagProxy->FindNdefTech(tagDiscId);
    ASSERT_TRUE(findNdefTech == "");
}

/**
 * @tc.name: WriteNdef001
 * @tc.desc: Test NciTagProxyTest WriteNdef.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, WriteNdef001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    std::string command = "";
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    bool writeNdef = nciTagProxy->WriteNdef(tagDiscId, command);
    ASSERT_TRUE(writeNdef == false);
}

/**
 * @tc.name: FormatNdef001
 * @tc.desc: Test NciTagProxyTest FormatNdef.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, FormatNdef001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    std::string key = "";
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    bool formatNdef = nciTagProxy->FormatNdef(tagDiscId, key);
    ASSERT_TRUE(formatNdef == false);
}

/**
 * @tc.name: CanMakeReadOnly001
 * @tc.desc: Test NciTagProxyTest CanMakeReadOnly.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, CanMakeReadOnly001, TestSize.Level1)
{
    uint32_t ndefType = 0;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    bool canMakeReadOnly = nciTagProxy->CanMakeReadOnly(ndefType);
    ASSERT_TRUE(canMakeReadOnly == false);
}

/**
 * @tc.name: SetNdefReadOnly001
 * @tc.desc: Test NciTagProxyTest SetNdefReadOnly.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, SetNdefReadOnly001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    bool setNdefReadOnly = nciTagProxy->SetNdefReadOnly(tagDiscId);
    ASSERT_TRUE(setNdefReadOnly == false);
}

/**
 * @tc.name: DetectNdefInfo001
 * @tc.desc: Test NciTagProxyTest DetectNdefInfo.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, DetectNdefInfo001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    std::vector<int> ndefInfo;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    bool detectNdefInfo = nciTagProxy->DetectNdefInfo(tagDiscId, ndefInfo);
    ASSERT_TRUE(detectNdefInfo == false);
}

/**
 * @tc.name: IsTagFieldOn001
 * @tc.desc: Test NciTagProxyTest IsTagFieldOn.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, IsTagFieldOn001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    bool isTagFieldOn = nciTagProxy->IsTagFieldOn(tagDiscId);
    ASSERT_TRUE(isTagFieldOn == false);
}

/**
 * @tc.name: StartFieldOnChecking001
 * @tc.desc: Test NciTagProxyTest StartFieldOnChecking.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, StartFieldOnChecking001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    uint32_t delayedMs = 0;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->StartFieldOnChecking(tagDiscId, delayedMs);
    std::vector<AppExecFwk::PacMap> getTechExtrasData = nciTagProxy->GetTechExtrasData(tagDiscId);
    ASSERT_TRUE(getTechExtrasData.size() == 0);
}

/**
 * @tc.name: SetTimeout001
 * @tc.desc: Test NciTagProxyTest SetTimeout.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, SetTimeout001, TestSize.Level1)
{
    uint32_t tagDiscId = 0;
    uint32_t timeout = 0;
    uint32_t technology = 0;
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    nciTagProxy->SetTimeout(tagDiscId, timeout, technology);
    std::vector<AppExecFwk::PacMap> getTechExtrasData = nciTagProxy->GetTechExtrasData(tagDiscId);
    ASSERT_TRUE(getTechExtrasData.size() == 0);
}

/**
 * @tc.name: GetIsoDepMaxTransceiveLength001
 * @tc.desc: Test NciTagProxyTest GetIsoDepMaxTransceiveLength.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetIsoDepMaxTransceiveLength001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    int getIsoDepMaxTransceiveLength = nciTagProxy->GetIsoDepMaxTransceiveLength();
    ASSERT_TRUE(getIsoDepMaxTransceiveLength != 0);
}

/**
 * @tc.name: IsExtendedLengthApduSupported001
 * @tc.desc: Test NciTagProxyTest IsExtendedLengthApduSupported.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, IsExtendedLengthApduSupported001, TestSize.Level1)
{
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    bool isExtendedLengthApduSupported = nciTagProxy->IsExtendedLengthApduSupported();
    ASSERT_TRUE(isExtendedLengthApduSupported == true);
}

/**
 * @tc.name: GetTechMaskFromTechList001
 * @tc.desc: Test NciTagProxyTest GetTechMaskFromTechList.
 * @tc.type: FUNC
 */
HWTEST_F(NciTagProxyTest, GetTechMaskFromTechList001, TestSize.Level1)
{
    std::vector<uint32_t> discTech = {0, 1, 2, 3, 4, 5};
    std::shared_ptr<NciTagProxy> nciTagProxy = std::make_shared<NciTagProxy>();
    int getTechMaskFromTechList = nciTagProxy->GetTechMaskFromTechList(discTech);
    ASSERT_TRUE(getTechMaskFromTechList != 0);
}
}
}
}