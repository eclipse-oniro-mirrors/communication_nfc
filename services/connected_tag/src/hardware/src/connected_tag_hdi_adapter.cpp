/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
#include "connected_tag_hdi_adapter.h"
#include "log.h"
#include "connected_tag_proxy.h"

namespace OHOS {
namespace ConnectedTag {
static sptr<ohos::hardware::nfc::v1_0::IConnectedTagHdi> g_iConnectedTagHdi;
NfcHdiAdapter::NfcHdiAdapter()
{
    HILOGI("NfcHdiAdapter: NfcHdiAdapter called.");
    sptr<ohos::hardware::nfc::v1_0::IConnectedTagHdi> nfcHdi = ohos::hardware::nfc::v1_0::IConnectedTagHdi::Get();
    if (nfcHdi == nullptr) {
        HILOGE("NfcHdiAdapter: IConnectedTagHdi::Get failed.");
    }
    g_iConnectedTagHdi = nfcHdi;
}

NfcHdiAdapter::~NfcHdiAdapter()
{
    HILOGI("NfcHdiAdapter: ~NfcHdiAdapter called.");
}
NfcHdiAdapter &NfcHdiAdapter::GetInstance()
{
    static NfcHdiAdapter instance;
    return instance;
}
int32_t NfcHdiAdapter::Init()
{
    HILOGI("NfcHdiAdapter::Init() starts");
    if (g_iConnectedTagHdi != nullptr) {
        g_iConnectedTagHdi->Init();
    }
    return 0;
}
int32_t NfcHdiAdapter::Uninit()
{
    HILOGI("NfcHdiAdapter::Uninit() starts");
    if (g_iConnectedTagHdi != nullptr) {
        g_iConnectedTagHdi->Uninit();
    }
    return 0;
}
std::string NfcHdiAdapter::ReadNdefTag()
{
    HILOGI("NfcHdiAdapter::ReadNdefTag() starts");
    std::string resp = "";
    if (g_iConnectedTagHdi != nullptr) {
        resp = g_iConnectedTagHdi->ReadNdefTag();
        HILOGI("NfcHdiAdapter::ReadNdefTag() resp = %{public}s", resp.c_str());
    }
    return resp;
}
int32_t NfcHdiAdapter::WriteNdefTag(std::string data)
{
    HILOGI("NfcHdiAdapter::WriteNdefTag() starts data = %{public}s", data.c_str());
    if (g_iConnectedTagHdi != nullptr) {
        g_iConnectedTagHdi->WriteNdefTag(data);
    }
    return 0;
}
}  // namespace ConnectedTag
}  // namespace OHOS