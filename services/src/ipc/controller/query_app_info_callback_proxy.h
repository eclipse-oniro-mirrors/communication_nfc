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
#ifndef OHOS_QUERY_APP_INFO_CALLBACK_PROXY_H
#define OHOS_QUERY_APP_INFO_CALLBACK_PROXY_H

#include "iquery_app_info_callback.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace NFC {
class QueryAppInfoCallbackProxy : public IRemoteProxy<IQueryAppInfoCallback> {
public:
    explicit QueryAppInfoCallbackProxy(const sptr<IRemoteObject> &remote);
    virtual ~QueryAppInfoCallbackProxy() {}

    bool OnQueryAppInfo(std::string type, std::vector<int> techList, std::vector<AAFwk::Want> &hceAppList,
        std::vector<AppExecFwk::ElementName> &elementNameList) override;

private:
    static inline BrokerDelegator<QueryAppInfoCallbackProxy> g_delegator;
};
}  // namespace NFC
}  // namespace OHOS
#endif  // OHOS_QUERY_APP_INFO_CALLBACK_PROXY_H
