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
#ifndef NFC_NAPI_FOREGROUND_DISPATCH_H
#define NFC_NAPI_FOREGROUND_DISPATCH_H

#include <locale>
#include "element_name.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nfc_napi_common_utils.h"
#include "nfc_sdk_common.h"
#include "iforeground_callback.h"
#include "ireader_mode_callback.h"

namespace OHOS {
namespace NFC {
namespace KITS {
using OHOS::AppExecFwk::ElementName;
class AsyncEventData {
public:
    napi_env env;
    napi_ref callbackRef;
    std::function<napi_value ()> packResult;

    explicit AsyncEventData(napi_env e, napi_ref r, std::function<napi_value ()> v)
        :env(e),
        callbackRef(r),
        packResult(v) {};

    AsyncEventData() = delete;
    virtual ~AsyncEventData() {}
};

class RegObj {
public:
    RegObj() : regEnv(0), regHandlerRef(nullptr), regElement(), regDiscTech() {}

    explicit RegObj(const napi_env &env, const napi_ref &ref, ElementName &element, std::vector<uint32_t> &discTech)
        :regEnv(env),
        regHandlerRef(ref),
        regElement(element),
        regDiscTech(discTech) {};

    ~RegObj() {}

    bool IsEmpty() const { return (regEnv == 0) || (regHandlerRef == nullptr); }

    void Clear()
    {
        regEnv = 0;
        regHandlerRef = nullptr;
        regElement.ClearElement(&regElement);
        regDiscTech.clear();
    }

    napi_env regEnv;
    napi_ref regHandlerRef;
    ElementName regElement;
    std::vector<uint32_t> regDiscTech;
};

class ForegroundEventRegister {
public:
    ForegroundEventRegister() {}
    ~ForegroundEventRegister() {}
    static ForegroundEventRegister& GetInstance();
    ErrorCode Register(const napi_env &env, ElementName &element, std::vector<uint32_t> &discTech, napi_value handler);
    ErrorCode Unregister(const napi_env &env, ElementName &element, napi_value handler);

private:
    ErrorCode RegisterForegroundEvents(ElementName &element, std::vector<uint32_t> &discTech);
    ErrorCode UnregisterForegroundEvents(ElementName &element);
    void DeleteRegisterObj(const napi_env &env, RegObj &regObj, napi_value &handler);

    static bool isEvtRegistered;
};

class ReaderModeEvtRegister {
public:
    ReaderModeEvtRegister() {}
    ~ReaderModeEvtRegister() {}
    static ReaderModeEvtRegister& GetInstance();
    ErrorCode Register(const napi_env &env, std::string &type, ElementName &element,
                       std::vector<uint32_t> &discTech, napi_value handler);
    ErrorCode Unregister(const napi_env &env, std::string &type, ElementName &element, napi_value handler);

private:
    ErrorCode RegReaderModeEvt(std::string &type, ElementName &element, std::vector<uint32_t> &discTech);
    ErrorCode UnregReaderModeEvt(std::string &type, ElementName &element);
    void DeleteRegisteredObj(const napi_env &env, RegObj &regObj, napi_value &handler);

    static bool isReaderModeRegistered;
};

napi_value RegisterForegroundDispatch(napi_env env, napi_callback_info cbinfo);
napi_value UnregisterForegroundDispatch(napi_env env, napi_callback_info cbinfo);
napi_value On(napi_env env, napi_callback_info cbinfo);
napi_value Off(napi_env env, napi_callback_info cbinfo);
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif // NFC_NAPI_FOREGROUND_DISPATCH_H