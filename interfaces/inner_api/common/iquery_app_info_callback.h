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
#ifndef OHOS_I_QUERY_APP_INFO_CALLBACK_H
#define OHOS_I_QUERY_APP_INFO_CALLBACK_H

#include <iremote_broker.h>

#include "element_name.h"
#include "want.h"

namespace OHOS {
namespace NFC {

const std::string KEY_TAG_APP = "tag";
const std::string KEY_HCE_APP = "hce";
using QueryApplicationByVendor = std::vector<AppExecFwk::ElementName> (*)(std::vector<int>);
using QueryHceAppByVendor = std::vector<AAFwk::Want> (*)();
class IQueryAppInfoCallback : public IRemoteBroker {
public:
    virtual bool OnQueryAppInfo(std::string type, std::vector<int> techList, std::vector<AAFwk::Want> &hceAppList,
        std::vector<AppExecFwk::ElementName> &elementNameList) = 0;

public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.nfc.IQueryAppInfoCallback");
};
}  // namespace NFC
}  // namespace OHOS
#endif  // OHOS_I_QUERY_APP_INFO_CALLBACK_H
