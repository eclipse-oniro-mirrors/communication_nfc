/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
#include "nfc_service_mock.h"
#include "nfc_service.h"
#include <unistd.h>
#include "app_data_parser.h"
#include "infc_controller_callback.h"
#include "iservice_registry.h"
#include "loghelper.h"
#include "nfc_preferences.h"
#include "nfc_event_handler.h"
#include "nfc_event_publisher.h"
#include "nfc_hisysevent.h"
#include "nfc_polling_params.h"
#include "nfc_sdk_common.h"
#include "nfc_timer.h"
#include "nfc_watch_dog.h"
#include "tag_session.h"
#include "external_deps_proxy.h"
#include "want.h"
#include "nci_nfcc_proxy.h"
#include "nci_tag_proxy.h"
#include "nci_ce_proxy.h"
#include "hce_session.h"

namespace OHOS {
namespace NFC {
NfcService::NfcService() {}
NfcService::~NfcService() {}
bool NfcService::Initialize()
{
    nciTagProxy_ = std::make_shared<NFC::NCI::NciTagProxy>();
    return true;
}

std::weak_ptr<NfcService> NfcService::GetInstance() const
{
    auto nfcService = std::make_shared<NfcService>();
    std::weak_ptr<NfcService> ret = nfcService;
    return nfcService;
}

void NfcService::OnTagDiscovered(uint32_t tagDiscId)
{
}

void NfcService::OnTagLost(uint32_t tagDiscId)
{
}

void NfcService::FieldActivated()
{
}

void NfcService::FieldDeactivated()
{
}

void NfcService::OnVendorEvent(int eventType, int arg1, std::string arg2)
{
}

void NfcService::OnCardEmulationData(const std::vector<uint8_t>& data)
{
}

void NfcService::OnCardEmulationActivated()
{
}

void NfcService::OnCardEmulationDeactivated()
{
}

OHOS::sptr<IRemoteObject> NfcService::GetTagServiceIface()
{
    return nullptr;
}

OHOS::sptr<IRemoteObject> NfcService::GetHceServiceIface()
{
    return nullptr;
}

bool NfcService::IsNfcEnabled()
{
    return true;
}

int NfcService::GetNfcState()
{
    return 0;
}

int NfcService::GetScreenState()
{
    return 0;
}

int NfcService::GetNciVersion()
{
    return 0;
}

std::weak_ptr<NCI::INciNfccInterface> NfcService::GetNciNfccProxy(void)
{
    std::weak_ptr<NCI::INciNfccInterface> ret;
    return ret;
}

std::weak_ptr<NCI::INciTagInterface> NfcService::GetNciTagProxy(void)
{
    return nciTagProxy_;
}

std::weak_ptr<NfcPollingManager> NfcService::GetNfcPollingManager()
{
    std::weak_ptr<NfcPollingManager> ret;
    return ret;
}

std::weak_ptr<NfcRoutingManager> NfcService::GetNfcRoutingManager()
{
    std::weak_ptr<NfcRoutingManager> ret;
    return ret;
}

std::weak_ptr<CeService> NfcService::GetCeService()
{
    std::weak_ptr<CeService> ret;
    return ret;
}

std::string NfcService::GetSimVendorBundleName()
{
    return "";
}

std::weak_ptr<TAG::TagDispatcher> NfcService::GetTagDispatcher()
{
    std::weak_ptr<TAG::TagDispatcher> ret;
    return ret;
}

void NfcService::NotifyMessageToVendor(int key, const std::string &value)
{
}
}  // namespace NFC
}  // namespace OHOS