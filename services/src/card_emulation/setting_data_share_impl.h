/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#ifndef SETTING_DATA_SHARE_IMPL_H
#define SETTING_DATA_SHARE_IMPL_H

#include <singleton.h>
#include "datashare_helper.h"
#include "nfc_sdk_common.h"
#include "uri.h"
#include "element_name.h"

namespace OHOS {
namespace NFC {
using OHOS::AppExecFwk::ElementName;
class SettingDataShareImpl : public DelayedSingleton<SettingDataShareImpl> {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"nfc.setting.data");
    SettingDataShareImpl();
    ~SettingDataShareImpl();
    KITS::ErrorCode RegisterDataObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver);
    KITS::ErrorCode ReleaseDataObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver);
    KITS::ErrorCode GetElementName(Uri &uri, const std::string &column, ElementName &value);
    KITS::ErrorCode SetElementName(Uri &uri, const std::string &column, ElementName &value);
    std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper();

private:
    void Initialize();
    bool ParseElementURI(const std::string &uri, ElementName &value);
    void Split(const std::string &str, const std::string &delim, std::vector<std::string> &vec);

    sptr<IRemoteObject> remoteObj_;
    std::shared_ptr<DataShare::DataShareHelper> dataShareHelper_;
};

class ISettingData : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"nfc.setting.data");
};
} // namespace NFC
} // namespace OHOS
#endif // SETTING_DATA_SHARE_IMPL_H