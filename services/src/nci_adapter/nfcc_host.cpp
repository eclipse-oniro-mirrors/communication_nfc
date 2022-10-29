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
#include "nfcc_host.h"

#include "infcc_host.h"
#include "loghelper.h"
#include "ndef_tag.h"
#include "nfcc_nci_adapter.h"
#include "nfc_chip_type_parser.h"

namespace OHOS {
namespace NFC {
namespace NCI {
std::weak_ptr<NfccHost::INfccHostListener> NfccHost::nfccHostListener_;

NfccHost::NfccHost(std::weak_ptr<INfccHostListener> listener)
{
    nfccHostListener_ = listener;
}

void NfccHost::SetNfccHostListener(std::weak_ptr<INfccHostListener> listener)
{
    nfccHostListener_ = listener;
}

NfccHost::~NfccHost()
{
    this->Deinitialize();
}

bool NfccHost::Initialize()
{
    DebugLog("NfccHost::Initialize");
    if (!NfcChipTypeParser::IsSn110()) {
        WarnLog("NfccHost::Initialize(): unsupported chip type");
        return true;
    }
    return NfccNciAdapter::GetInstance().Initialize();
}

bool NfccHost::Deinitialize()
{
    DebugLog("NfccHost::Deinitialize");
    if (!NfcChipTypeParser::IsSn110()) {
        WarnLog("NfccHost::Deinitialize(): unsupported chip type");
        return true;
    }
    return NfccNciAdapter::GetInstance().Deinitialize();
}

void NfccHost::EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart)
{
    DebugLog("NfccHost::EnableDiscovery");
    if (!NfcChipTypeParser::IsSn110()) {
        WarnLog("NfccHost::EnableDiscovery(): unsupported chip type");
        return;
    }
    NfccNciAdapter::GetInstance().EnableDiscovery(techMask, enableReaderMode, enableHostRouting, restart);
}

void NfccHost::DisableDiscovery()
{
    DebugLog("NfccHost::DisableDiscovery");
    if (!NfcChipTypeParser::IsSn110()) {
        WarnLog("NfccHost::DisableDiscovery(): unsupported chip type");
        return;
    }
    NfccNciAdapter::GetInstance().DisableDiscovery();
}

bool NfccHost::SendRawFrame(std::string& rawData)
{
    DebugLog("NfccHost::SendRawFrame");
    return NfccNciAdapter::GetInstance().SendRawFrame(rawData);
}

bool NfccHost::SetScreenStatus(unsigned char screenStateMask)
{
    DebugLog("NfccHost::SetScreenStatus");
    if (!NfcChipTypeParser::IsSn110()) {
        WarnLog("NfccHost::SetScreenStatus(): unsupported chip type");
        return true;
    }
    NfccNciAdapter::GetInstance().SetScreenStatus(screenStateMask);
    return true;
}

int NfccHost::GetNciVersion()
{
    DebugLog("NfccHost::GetNciVersion");
    if (!NfcChipTypeParser::IsSn110()) {
        WarnLog("NfccHost::GetNciVersion(): unsupported chip type");
        return 0;
    }
    return NfccNciAdapter::GetInstance().GetNciVersion();
}

bool NfccHost::SetSecureNfc(bool secure)
{
    DebugLog("NfccHost::SetSecureNfc");
#ifdef _NFC_SERVICE_HCE_
    NciBalCe::GetInstance().SetSecureNfc(secure);
#endif
    return true;
}

int NfccHost::GetIsoDepMaxTransceiveLength()
{
    DebugLog("NfccHost::GetIsoDepMaxTransceiveLength");
    return NfccNciAdapter::GetInstance().GetIsoDepMaxTransceiveLength();
}

int NfccHost::RegisterT3tIdentifier(std::string& t3tIdentifier)
{
    DebugLog("NfccHost::RegisterT3tIdentifier");
    return NfccNciAdapter::GetInstance().RegisterT3tIdentifier(t3tIdentifier);
}

void NfccHost::DeregisterT3tIdentifier(std::string& t3tIdentifier)
{
    DebugLog("NfccHost::DeregisterT3tIdentifier");
    // get handle from mT3tIdentifiers
    if (!t3tIdentifier.empty()) {
        int handle = -1;
        NfccNciAdapter::GetInstance().DeregisterT3tIdentifier(handle);
    }
}

void NfccHost::ClearT3tIdentifiersCache()
{
    DebugLog("NfccHost::ClearT3tIdentifiersCache");
    NfccNciAdapter::GetInstance().ClearT3tIdentifiersCache();
}

int NfccHost::GetLfT3tMax()
{
    DebugLog("NfccHost::GetLfT3tMax");
    return NfccNciAdapter::GetInstance().GetLfT3tMax();
}

int NfccHost::GetLastError()
{
    DebugLog("NfccHost::GetLastError");
    return NfccNciAdapter::GetInstance().GetLastError();
}

void NfccHost::Abort()
{
    DebugLog("NfccHost::Abort");
    NfccNciAdapter::GetInstance().Abort();
}

bool NfccHost::CheckFirmware()
{
    DebugLog("NfccHost::CheckFirmware");
    return NfccNciAdapter::GetInstance().CheckFirmware();
}

void NfccHost::Dump(int fd)
{
    DebugLog("NfccHost::Dump");
    NfccNciAdapter::GetInstance().Dump(fd);
}

void NfccHost::FactoryReset()
{
    DebugLog("NfccHost::FactoryReset");
    NfccNciAdapter::GetInstance().FactoryReset();
}

void NfccHost::Shutdown()
{
    DebugLog("NfccHost::Shutdown");
    NfccNciAdapter::GetInstance().Shutdown();
}

bool NfccHost::AddAidRouting(std::string& aid, int route, int aidInfo)
{
    DebugLog("NfccHost::AddAidRouting");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().AddAidRouting(aid, route, aidInfo);
#else
    return true;
#endif
}

bool NfccHost::RemoveAidRouting(std::string& aid)
{
    DebugLog("NfccHost::RemoveAidRouting");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().RemoveAidRouting(aid);
#else
    return true;
#endif
}

bool NfccHost::CommitRouting()
{
    DebugLog("NfccHost::CommitRouting");
#ifdef _NFC_SERVICE_HCE_
    bool restart = NfccNciAdapter::GetInstance().IsRfEbabled();
    if (restart) {
        NfccNciAdapter::GetInstance().StartRfDiscovery(false);
    }
    bool commitResult = NciBalCe::GetInstance().CommitRouting();
    if (restart) {
        NfccNciAdapter::GetInstance().StartRfDiscovery(true);
    }
    return commitResult;
#else
    return true;
#endif
}

int NfccHost::GetAidRoutingTableSize()
{
    DebugLog("NfccHost::GetAidRoutingTableSize");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetAidRoutingTableSize();
#endif
    return 0;
}

int NfccHost::GetDefaultRoute()
{
    DebugLog("NfccHost::GetDefaultRoute");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetDefaultRoute();
#endif
    return 0;
}

int NfccHost::GetDefaultOffHostRoute()
{
    DebugLog("NfccHost::GetDefaultOffHostRoute");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetDefaultOffHostRoute();
#endif
    return 0;
}

std::vector<int> NfccHost::GetOffHostUiccRoute()
{
    DebugLog("NfccHost::GetOffHostUiccRoute");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetOffHostUiccRoute();
#endif
    return {};
}

std::vector<int> NfccHost::GetOffHostEseRoute()
{
    DebugLog("NfccHost::GetOffHostEseRoute");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetOffHostEseRoute();
#else
    return {};
#endif
}

int NfccHost::GetAidMatchingMode()
{
    DebugLog("NfccHost::GetAidMatchingMode");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetAidMatchingMode();
#else
    return 0;
#endif
}

int NfccHost::GetDefaultIsoDepRouteDestination()
{
    DebugLog("NfccHost::GetDefaultIsoDepRouteDestination");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetDefaultIsoDepRouteDestination();
#else
    return 0;
#endif
}

bool NfccHost::CanMakeReadOnly(int ndefType)
{
    return ndefType == KITS::NdefTag::EmNfcForumType::NFC_FORUM_TYPE_1 ||
        ndefType == KITS::NdefTag::EmNfcForumType::NFC_FORUM_TYPE_2;
}

bool NfccHost::GetExtendedLengthApdusSupported()
{
    if (NfccNciAdapter::GetInstance().GetIsoDepMaxTransceiveLength() > ISO_DEP_FRAME_MAX_LEN) {
        return true;
    }
    return false;
}

void NfccHost::SetNciAdaptation(std::shared_ptr<INfcNci> nciAdaptation)
{
    NfccNciAdapter::GetInstance().SetNciAdaptation(nciAdaptation);
#ifdef _NFC_SERVICE_HCE_
    NciBalCe::GetInstance().SetNciAdaptation(nciAdaptation);
    HciManager::GetInstance().SetNciAdaptation(nciAdaptation);
#endif
}

void NfccHost::RemoteFieldActivated()
{
    DebugLog("NfccHost::RemoteFieldActivated");
    if (nfccHostListener_.expired()) {
        ErrorLog("Nfcc host listener is null");
        return;
    }
}

void NfccHost::RemoteFieldDeactivated()
{
    DebugLog("NfccHost::RemoteFieldDeactivated");
    if (nfccHostListener_.expired()) {
        ErrorLog("Nfcc host listener is null");
        return;
    }
}

void NfccHost::HostCardEmulationActivated(int technology)
{
    DebugLog("NfccHost::HostCardEmulationActivated");
    if (nfccHostListener_.expired()) {
        ErrorLog("Nfcc host listener is null");
        return;
    }
}

void NfccHost::HostCardEmulationDeactivated(int technology)
{
    DebugLog("NfccHost::HostCardEmulationDeactivated");
    if (nfccHostListener_.expired()) {
        ErrorLog("Nfcc host listener is null");
        return;
    }
}

void NfccHost::HostCardEmulationDataReceived(int technology, std::string& data)
{
    DebugLog("NfccHost::HostCardEmulationDataReceived");
    if (nfccHostListener_.expired()) {
        ErrorLog("Nfcc host listener is null");
        return;
    }
}

void NfccHost::TagDiscovered(std::shared_ptr<NCI::ITagHost> tagHost)
{
    DebugLog("NfccHost::TagDiscovered");
    if (nfccHostListener_.expired()) {
        ErrorLog("Nfcc host listener is null");
        return;
    }
    nfccHostListener_.lock()->OnTagDiscovered(tagHost);
}

void NfccHost::OffHostTransactionEvent(std::string& aid, std::string& data, std::string& seName)
{
    DebugLog("NfccHost::OffHostTransactionEvent");
    if (nfccHostListener_.expired()) {
        ErrorLog("Nfcc host listener is null");
        return;
    }
}

void NfccHost::EeUpdate()
{
    DebugLog("NfccHost::EeUpdate");
    if (nfccHostListener_.expired()) {
        ErrorLog("Nfcc host listener is null");
        return;
    }
}

bool NfccHost::ClearAidTable()
{
    DebugLog("NfccHost::ClearAidTable");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().ClearAidTable();
#else
    return true;
#endif
}

int NfccHost::GetRemainRoutingTableSize()
{
    DebugLog("NfccHost::GetRemainRoutingTableSize");
#ifdef _NFC_SERVICE_HCE_
    return NciBalCe::GetInstance().GetRemainRoutingTableSize();
#endif
    return 0;
}
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
