/*
* Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef NFC_NAPI_CONTROLLER_EVENT_H_
#define NFC_NAPI_CONTROLLER_EVENT_H_

#include <map>
#include <set>
#include <shared_mutex>
#include <string>
#include <uv.h>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nfc_sdk_common.h"
#include "infc_controller_callback.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class AsyncEventData {
public:
    napi_env env;
    napi_ref callbackRef;
    std::function<napi_value ()> packResult;

    explicit AsyncEventData(napi_env e, napi_ref r, std::function<napi_value ()> v)
    {
        env = e;
        callbackRef = r;
        packResult = v;
    }

    AsyncEventData() = delete;

    virtual ~AsyncEventData() {
    }
};

class RegObj {
public:
    RegObj() : m_regEnv(0), m_regHanderRef(nullptr) {
    }
    
    explicit RegObj(const napi_env& env, const napi_ref& ref)
    {
        m_regEnv = env;
        m_regHanderRef = ref;
    }

    ~RegObj() {
    }

    napi_env m_regEnv;
    napi_ref m_regHanderRef;
};


class EventRegister {
public:
    EventRegister() {
    }
    ~EventRegister() {
    }

    static EventRegister& GetInstance();

    void Register(const napi_env& env, const std::string& type, napi_value handler);
    void Unregister(const napi_env& env, const std::string& type, napi_value handler);

private:
    ErrorCode RegisterNfcStateChangedEvents(const std::string& type);
    ErrorCode UnRegisterNfcEvents(const std::string& type);
    bool IsEventSupport(const std::string& type);
    void DeleteRegisterObj(const napi_env& env, std::vector<RegObj>& vecRegObjs, napi_value& handler);
    void DeleteAllRegisterObj(const napi_env& env, std::vector<RegObj>& vecRegObjs);

    static bool isEventRegistered;
};

napi_value On(napi_env env, napi_callback_info cbinfo);
napi_value Off(napi_env env, napi_callback_info cbinfo);
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS

#endif
