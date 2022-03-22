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

#include "nfc_napi_event.h"
#include <uv.h>
#include "nfc_napi_utils.h"
#include "log.h"
#include "iconnected_tag_callback.h"
#include "connected_tag_impl.h"

namespace OHOS {
namespace ConnectedTag {
const std::string EVENT_NOTIFY = "notify";

static std::set<std::string> g_supportEventList = {
    EVENT_NOTIFY,
};

bool EventRegister::isEventRegistered = false;

void NapiEvent::EventNotify(AsyncEventData *asyncEvent)
{
    HILOGI("Enter nfc event notify");
    if (asyncEvent == nullptr) {
        HILOGE("asyncEvent is null.");
        return;
    }
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(asyncEvent->env, &loop);

    uv_work_t* work = new uv_work_t;
    if (work == nullptr) {
        HILOGE("uv_work_t work is null.");
        delete asyncEvent;
        asyncEvent = nullptr;
        return;
    }

    HILOGI("Get the event loop, napi_env: %{public}p", asyncEvent->env);
    work->data = asyncEvent;
    uv_queue_work(
        loop,
        work,
        [](uv_work_t* work) {},
        [](uv_work_t* work, int status) {
            AsyncEventData *asyncData = static_cast<AsyncEventData*>(work->data);
            if (asyncData == nullptr) {
                HILOGE("asyncData is null.");
                return;
            }
            HILOGI("Napi event uv_queue_work, env: %{public}p, status: %{public}d", asyncData->env, status);
            napi_handle_scope scope = nullptr;
            napi_open_handle_scope(asyncData->env, &scope);
            if (scope == nullptr) {
                HILOGE("scope is nullptr");
                napi_close_handle_scope(asyncData->env, scope);
                return;
            }
            napi_value undefine;
            napi_get_undefined(asyncData->env, &undefine);
            napi_value handler = nullptr;
            napi_get_reference_value(asyncData->env, asyncData->callbackRef, &handler);

            HILOGI("Push event to js, env: %{public}p, ref : %{public}p", asyncData->env, &asyncData->callbackRef);
            if (napi_call_function(asyncData->env, nullptr, handler, 1, &asyncData->jsEvent, &undefine) != napi_ok) {
                HILOGE("Report event to Js failed");
            }
            napi_close_handle_scope(asyncData->env, scope);
            if (asyncData != nullptr) {
                delete asyncData;
                asyncData = nullptr;
            }
            delete work;
            work = nullptr;
        }
    );
}

bool NapiEvent::CheckIsRegister(const std::string& type)
{
    return g_eventRegisterInfo.find(type) != g_eventRegisterInfo.end();
}

class NfcListenerEvent : public IConnectedTagCallBack, public NapiEvent {
public:
    NfcListenerEvent() {
    }

    virtual ~NfcListenerEvent() {
    }

public:
    void OnNotify(int nfcRfState) override
    {
        HILOGI("OnNotify rcvd nfcRfState: %{public}d", nfcRfState);
        CheckAndNotify(EVENT_NOTIFY, nfcRfState);
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

sptr<NfcListenerEvent> nfcListenerEvent =
    sptr<NfcListenerEvent>(new (std::nothrow) NfcListenerEvent());

napi_value On(napi_env env, napi_callback_info cbinfo)
{
    TRACE_FUNC_CALL;
    size_t requireArgc = 2;
    size_t argc = 2;
    napi_value argv[2] = {0};
    napi_value thisVar = 0;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    napi_valuetype eventName = napi_undefined;
    napi_typeof(env, argv[0], &eventName);
    napi_valuetype handler = napi_undefined;
    napi_typeof(env, argv[1], &handler);

    if (argc != requireArgc || eventName != napi_string || handler != napi_function) {
        HILOGE("On args invalid, failed!");
        napi_value result;
        napi_get_boolean(env, false, &result);
        return result;
    }

    char type[64] = {0};
    size_t typeLen = 0;
    napi_get_value_string_utf8(env, argv[0], type, sizeof(type), &typeLen);
    EventRegister::GetInstance().Register(env, type, argv[1]);
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value Off(napi_env env, napi_callback_info cbinfo)
{
    TRACE_FUNC_CALL;
    size_t requireArgc = 1;
    size_t requireArgcWithCb = 2;
    size_t argc = 2;
    napi_value argv[2] = {0};
    napi_value thisVar = 0;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    napi_valuetype eventName = napi_undefined;
    napi_typeof(env, argv[0], &eventName);

    if ((argc != requireArgc && argc != requireArgcWithCb) || eventName != napi_string) {
        HILOGE("On args invalid, failed!");
        napi_value result;
        napi_get_boolean(env, false, &result);
        return result;
    }

    if (argc == requireArgcWithCb) {
        napi_valuetype handler = napi_undefined;
        napi_typeof(env, argv[1], &handler);
        if (handler != napi_function) {
            HILOGE("On args invalid napi_function, failed!");
            napi_value result;
            napi_get_boolean(env, false, &result);
            return result;
        }
    }

    char type[64] = {0};
    size_t typeLen = 0;
    napi_get_value_string_utf8(env, argv[0], type, sizeof(type), &typeLen);
    EventRegister::GetInstance().Unregister(env, type, argc >= requireArgcWithCb ? argv[1] : nullptr);
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

ErrCode EventRegister::RegisterNfcEvents()
{
    ConnectedTagImpl connectedTag = OHOS::ConnectedTag::ConnectedTagImpl::GetInstance();
    ErrCode ret = connectedTag.RegListener(nfcListenerEvent);
    if (ret != NFC_OPT_SUCCESS) {
        HILOGE("RegisterNfcEvents nfcListenerEvent failed!");
        return ret;
    }
    return ret;
}

EventRegister& EventRegister::GetInstance()
{
    static EventRegister inst;
    return inst;
}

bool EventRegister::IsEventSupport(const std::string& type)
{
    return g_supportEventList.find(type) != g_supportEventList.end();
}

void EventRegister::Register(const napi_env& env, const std::string& type, napi_value handler)
{
    HILOGI("Register event: %{public}s, env: %{public}p", type.c_str(), env);

    if (!IsEventSupport(type)) {
        HILOGE("Register type error or not support!");
        return;
    }
    std::unique_lock<std::shared_mutex> guard(g_regInfoMutex);
    if (!isEventRegistered) {
        if (RegisterNfcEvents() != NFC_OPT_SUCCESS) {
            return;
        }
        isEventRegistered = true;
    }
    napi_ref handlerRef = nullptr;
    napi_create_reference(env, handler, 1, &handlerRef);
    RegObj regObj(env, handlerRef);
    auto iter = g_eventRegisterInfo.find(type);
    if (iter == g_eventRegisterInfo.end()) {
        g_eventRegisterInfo[type] = std::vector<RegObj> {regObj};
    } else {
        iter->second.emplace_back(regObj);
    }
}

void EventRegister::DeleteRegisterObj(std::vector<RegObj>& vecRegObjs, napi_value& handler)
{
    auto iter = vecRegObjs.begin();
    for (;iter != vecRegObjs.end();) {
        napi_value handlerTemp = nullptr;
        napi_get_reference_value(iter->m_regEnv, iter->m_regHanderRef, &handlerTemp);
        bool isEqual = false;
        napi_strict_equals(iter->m_regEnv, handlerTemp, handler, &isEqual);
        if (isEqual) {
            napi_delete_reference(iter->m_regEnv, iter->m_regHanderRef);
            HILOGI("Delete register object ref.");
            iter = vecRegObjs.erase(iter);
        } else {
            ++iter;
        }
    }
}

void EventRegister::DeleteAllRegisterObj(std::vector<RegObj>& vecRegObjs)
{
    for (auto& each : vecRegObjs) {
        napi_delete_reference(each.m_regEnv, each.m_regHanderRef);
    }
    vecRegObjs.clear();
}

void EventRegister::Unregister(const napi_env& env, const std::string& type, napi_value handler)
{
    HILOGI("Unregister event: %{public}s, env: %{public}p", type.c_str(), env);

    if (!IsEventSupport(type)) {
        HILOGE("Unregister type error or not support!");
        return;
    }

    std::unique_lock<std::shared_mutex> guard(g_regInfoMutex);
    auto iter = g_eventRegisterInfo.find(type);
    if (iter == g_eventRegisterInfo.end()) {
        HILOGE("Unregister type not registered!");
        return;
    }
    if (handler != nullptr) {
        DeleteRegisterObj(iter->second, handler);
    } else {
        HILOGW("All callback is unsubscribe for event: %{public}s", type.c_str());
        DeleteAllRegisterObj(iter->second);
    }
    if (iter->second.empty()) {
        g_eventRegisterInfo.erase(iter);
    }
}
}  // namespace ConnectedTag
}  // namespace OHOS
