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

#ifndef NFC_NAPI_HCE_SERVICE_H
#define NFC_NAPI_HCE_SERVICE_H

#include <map>
#include <set>
#include <shared_mutex>
#include <string>
#include <uv.h>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "ihce_cmd_callback.h"
#include "nfc_napi_common_utils.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class NfcNapiHceAdapter {
public:
    static napi_value Init(napi_env env, napi_value exports);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void* nativeObject, void* /*finalize_hint*/);
    static napi_value OnHceCmd(napi_env env, napi_callback_info info);
    static napi_value Transmit(napi_env env, napi_callback_info info);
};

struct NfcHceSessionContext : BaseContext {
    std::string value;     // out
    std::string dataBytes; // in
};

class AsyncEventData {
public:
    napi_env env;
    napi_ref callbackRef;
    std::function<napi_value()> packResult;

    explicit AsyncEventData(napi_env e, napi_ref r, std::function<napi_value()> v)
    {
        env = e;
        callbackRef = r;
        packResult = v;
    }

    AsyncEventData() = delete;

    virtual ~AsyncEventData() {}
};
/**
 * @brief 注册对象
 * @note   
 */
class RegObj {
public:
    RegObj() : m_regEnv(0), m_regHanderRef(nullptr) {}

    explicit RegObj(const napi_env& env, const napi_ref& ref)
    {
        m_regEnv = env;
        m_regHanderRef = ref;
    }

    ~RegObj() {}

    napi_env m_regEnv;
    napi_ref m_regHanderRef;
};
class EventRegister {
public:
    EventRegister() {}
    ~EventRegister() {}

    static EventRegister& GetInstance();

    void Register(const napi_env& env, const std::string& type, napi_value handler);

private:
    ErrorCode RegHceCmdCallbackEvents(const std::string& type);
    bool IsEventSupport(const std::string& type);

    static bool isEventRegistered;
};
} // namespace KITS
} // namespace NFC
} // namespace OHOS
#endif