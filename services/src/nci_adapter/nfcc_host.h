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
#ifndef NFCC_HOST_H
#define NFCC_HOST_H

#include <memory>
#include <string>

#include "infc_nci.h"
#include "infcc_host.h"
#include "itag_host.h"

namespace OHOS {
namespace NFC {
namespace NCI {
/* The maximum length of a default IsoDep frame consists of:
 * CLA, INS, P1, P2, LC, LE + 255 payload bytes = 261 bytes
 */
constexpr int ISO_DEP_FRAME_MAX_LEN = 261;

class NfccHost : public INfccHost {
public:
    explicit NfccHost(std::weak_ptr<INfccHostListener> listener);
    ~NfccHost() override;
    void SetNfccHostListener(std::weak_ptr<INfccHostListener> listener) override;
    bool Initialize() override;
    bool Deinitialize() override;
    void EnableDiscovery(uint16_t techMask, bool enableReaderMode, bool enableHostRouting, bool restart) override;
    void DisableDiscovery() override;
    bool SendRawFrame(std::string& rawData) override;
    bool SetScreenStatus(unsigned char screenStateMask) override;
    int GetNciVersion() override;
    bool SetSecureNfc(bool secure) override;
    int GetIsoDepMaxTransceiveLength() override;
    int RegisterT3tIdentifier(std::string& t3tIdentifier) override;
    void DeregisterT3tIdentifier(std::string& t3tIdentifier) override;
    void ClearT3tIdentifiersCache() override;
    int GetLfT3tMax() override;
    int GetLastError() override;
    void Abort() override;
    bool CheckFirmware() override;
    void Dump(int fd) override;
    void FactoryReset() override;
    void Shutdown() override;
    bool AddAidRouting(std::string& aid, int route, int aidInfo) override;
    bool RemoveAidRouting(std::string& aid) override;
    bool CommitRouting() override;
    int GetAidRoutingTableSize() override;
    int GetDefaultRoute() override;
    int GetDefaultOffHostRoute() override;
    std::vector<int> GetOffHostUiccRoute() override;
    std::vector<int> GetOffHostEseRoute() override;
    int GetAidMatchingMode() override;
    int GetDefaultIsoDepRouteDestination() override;
    bool CanMakeReadOnly(int ndefType) override;
    bool GetExtendedLengthApdusSupported() override;
    void SetNciAdaptation(std::shared_ptr<INfcNci> nciAdaptation);
    static void RemoteFieldActivated();
    static void RemoteFieldDeactivated();
    static void HostCardEmulationActivated(int technology);
    static void HostCardEmulationDeactivated(int technology);
    static void HostCardEmulationDataReceived(int technology, std::string& data);
    static void TagDiscovered(std::shared_ptr<NCI::ITagHost> tagHost);
    static void OffHostTransactionEvent(std::string& aid, std::string& data, std::string& seName);
    static void EeUpdate();
    bool ClearAidTable() override;
    int GetRemainRoutingTableSize() override;

private:
    static std::weak_ptr<INfccHostListener> nfccHostListener_;
};
}  // namespace NCI
}  // namespace NFC
}  // namespace OHOS
#endif /* NFCC_HOST_H */
