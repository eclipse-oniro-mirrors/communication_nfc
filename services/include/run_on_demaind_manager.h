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
#ifndef RUN_ON_DEMAIND_MANAGER_H
#define RUN_ON_DEMAIND_MANAGER_H
#include "run_on_demaind_proxy.h"

namespace OHOS {
namespace NFC {
class RunOnDemaindManager {
public:
    RunOnDemaindManager();
    ~RunOnDemaindManager();

    static RunOnDemaindManager &GetInstance()
    {
        static RunOnDemaindManager instance;
        return instance;
    }

    void HandleAppAddOrChangedEvent(std::shared_ptr<EventFwk::CommonEventData> data);
    void HandleAppRemovedEvent(std::shared_ptr<EventFwk::CommonEventData> data);
    void InitAppList();
    std::vector<ElementName> GetDispatchTagAppsByTech(std::vector<int> discTechList);
    std::vector<ElementName> GetVendorDispatchTagAppsByTech(std::vector<int> discTechList);
    void RegQueryApplicationCb(QueryApplicationByVendor callback);

    KITS::ErrorCode NfcDataGetValue(Uri &uri, const std::string &column, int32_t &value);
    KITS::ErrorCode NfcDataSetValue(Uri &uri, const std::string &column, int &value);

    void NfcDataSetString(const std::string& key, const std::string& value);
    std::string NfcDataGetString(const std::string& key);
    void NfcDataSetInt(const std::string& key, const int value);
    int NfcDataGetInt(const std::string& key);
    void NfcDataClear();
    void NfcDataDelete(const std::string& key);
    void UpdateNfcState(int newState);

    void PublishNfcStateChanged(int newState);
    void PublishNfcFieldStateChanged(bool isFieldOn);

    void WriteNfcFailedHiSysEvent(const NfcFailedParams* failedParams);
    void WriteOpenAndCloseHiSysEvent(int openRequestCnt, int openFailCnt,
                                     int closeRequestCnt, int closeFailCnt);
    void WriteTagFoundHiSysEvent(int tagFoundCnt, int typeACnt,
                                 int typeBCnt, int typeFCnt, int typeVCnt);
    void WritePassiveListenHiSysEvent(int requestCnt, int failCnt);
    void WriteFirmwareUpdateHiSysEvent(int requestCnt, int failCnt);
    void BuildFailedParams(NfcFailedParams &nfcFailedParams, MainErrorCode mainErrorCode, SubErrorCode subErrorCode);

    bool IsGranted(std::string permission);

    void DispatchTagAbility(std::shared_ptr<KITS::TagInfo> tagInfo, OHOS::sptr<IRemoteObject> tagServiceIface);
    void StartVibratorOnce();
};
} // NFC
} // OHOS
#endif // RUN_ON_DEMAIND_MANAGER_H