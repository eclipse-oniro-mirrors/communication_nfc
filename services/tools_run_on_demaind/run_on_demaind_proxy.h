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
#ifndef RUN_ON_DEMAIND_PROXY_H
#define RUN_ON_DEMAIND_PROXY_H
#include <vector>

#include "app_data_parser.h"
#include "common_event_manager.h"
#include "nfc_sdk_common.h"
#include "nfc_data_share_impl.h"
#include "nfc_database_helper.h"
#include "nfc_event_publisher.h"
#include "nfc_hisysevent.h"
#include "permission_tools.h"
#include "tag_ability_dispatcher.h"
#include "taginfo.h"

namespace OHOS {
namespace NFC {
using OHOS::AppExecFwk::ElementName;
class RunOnDemaindProxy {
public:
    static RunOnDemaindProxy &GetInstance()
    {
        static RunOnDemaindProxy instance;
        return instance;
    }

    void HandleAppAddOrChangedEvent(std::shared_ptr<EventFwk::CommonEventData> data);
    void HandleAppRemovedEvent(std::shared_ptr<EventFwk::CommonEventData> data);
    void InitAppList();
    std::vector<ElementName> GetDispatchTagAppsByTech(std::vector<int> discTechList);

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
    NfcFailedParams* BuildFailedParams(MainErrorCode mainErrorCode, SubErrorCode subErrorCode);

    bool IsGranted(std::string permission);

    void DispatchTagAbility(std::shared_ptr<KITS::TagInfo> tagInfo, OHOS::sptr<IRemoteObject> tagServiceIface);
};
} // NFC
} // OHOS
#endif // RUN_ON_DEMAIND_PROXY_H