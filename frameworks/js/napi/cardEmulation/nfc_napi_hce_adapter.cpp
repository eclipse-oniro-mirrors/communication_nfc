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

#include "nfc_napi_hce_adapter.h"
#include "loghelper.h"
#include "hce_service.h"
#include <uv.h>

namespace OHOS {
namespace NFC {
namespace KITS {
static const uint16_t DEFAULT_REF_COUNT = 1;
constexpr uint32_t INVALID_REF_COUNT = 0xFF;
static std::set<std::string> g_supportEventList = {
    KITS::EVENT_HCE_CMD,
};

bool EventRegister::isEventRegistered = false;

static std::mutex g_regInfoMutex;
static std::map<std::string, RegObj> g_eventRegisterInfo;

class NapiEvent {
public:
    napi_value CreateResult(const napi_env& env, const std::vector<uint8_t>& data);
    bool CheckIsRegister(const std::string& type);
    void EventNotify(AsyncEventData* asyncEvent);

    template <typename T>
    void CheckAndNotify(const std::string& type, const T& obj)
    {
        std::lock_guard<std::mutex> guard(g_regInfoMutex);
        if (!CheckIsRegister(type)) {
            return;
        }

        const RegObj& regObj = g_eventRegisterInfo[type];

        auto result = [this, env = regObj.m_regEnv, obj]() -> napi_value { return CreateResult(env, obj); };
        AsyncEventData* asyncEvent =
            new (std::nothrow) AsyncEventData(regObj.m_regEnv, regObj.m_regHanderRef, result);
        if (asyncEvent == nullptr) {
            return;
        }
        EventNotify(asyncEvent);
    }
};

class HceCmdListenerEvent : public IHceCmdCallback, public NapiEvent {
public:
    HceCmdListenerEvent() {}

    virtual ~HceCmdListenerEvent() {}

public:
    void OnCeApduData(const std::vector<uint8_t>& data) override
    {
        std::string dataStr(data.begin(), data.end());
        InfoLog("OnNotify rcvd ce adpu data: Data Length = %{public}zu; Data "
                "as String = %{public}s",
                data.size(), dataStr.c_str());
        CheckAndNotify(KITS::EVENT_HCE_CMD, data);
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override { return nullptr; }
};

sptr<HceCmdListenerEvent> hceCmdListenerEvent =
    sptr<HceCmdListenerEvent>(new (std::nothrow) HceCmdListenerEvent());

napi_value NfcNapiHceAdapter::Init(napi_env env, napi_value exports)
{
    napi_status status;
    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("on", NfcNapiHceAdapter::OnHceCmd),
        DECLARE_NAPI_FUNCTION("transmit", NfcNapiHceAdapter::Transmit),
        DECLARE_NAPI_FUNCTION("stop", NfcNapiHceAdapter::StopHce),
        DECLARE_NAPI_FUNCTION("stopHCE", NfcNapiHceAdapter::StopHCEDeprecated),
        DECLARE_NAPI_FUNCTION("startHCE", NfcNapiHceAdapter::StartHCEDeprecated),
        DECLARE_NAPI_FUNCTION("start", NfcNapiHceAdapter::StartHCE),
        DECLARE_NAPI_FUNCTION("sendResponse", NfcNapiHceAdapter::SendResponse),
    };

    char hceClassName[] = "HceService";

    napi_value cons;
    status = napi_define_class(env, hceClassName, NAPI_AUTO_LENGTH, NfcNapiHceAdapter::Constructor, nullptr,
                               sizeof(properties) / sizeof(napi_property_descriptor), properties, &cons);
    NAPI_ASSERT(env, status == napi_ok, "NfcNapiHceAdapter define class failed");

    status = napi_set_named_property(env, exports, hceClassName, cons);
    NAPI_ASSERT(env, status == napi_ok, "NfcNapiHceAdapter set name property failed");
    return cons;
}

napi_value NfcNapiHceAdapter::Constructor(napi_env env, napi_callback_info info)
{
    DebugLog("NfcNapiHceAdapter Constructor");
    napi_status status;
    napi_value jsHceService;
    size_t argc = 1;
    napi_value args[1];
    status = napi_get_cb_info(env, info, &argc, args, &jsHceService, nullptr);
    NAPI_ASSERT(env, status == napi_ok, "NfcNapiHceAdapter Constructor get_cb_info failed");
    NfcNapiHceAdapter* hceService = new NfcNapiHceAdapter();
    status = napi_wrap(env, jsHceService, hceService, NfcNapiHceAdapter::Destructor, nullptr, nullptr);
    NAPI_ASSERT(env, status == napi_ok, "NfcNapiHceAdapter Constructor wrap failed");
    return jsHceService;
}

void NfcNapiHceAdapter::Destructor(napi_env env, void* nativeObject, void* hint)
{
    NfcNapiHceAdapter* nfcNapiHceAdapter = static_cast<NfcNapiHceAdapter*>(nativeObject);
    nfcNapiHceAdapter->~NfcNapiHceAdapter();
    delete nfcNapiHceAdapter;
}

napi_value NfcNapiHceAdapter::OnHceCmd(napi_env env, napi_callback_info info)
{
    // js method on("hce",callback)
    size_t requireArgc = ARGV_NUM_2;
    size_t argc = ARGV_NUM_2;
    napi_value argv[2] = {0};
    napi_value thisVar = 0;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    NAPI_ASSERT(env, argc == requireArgc, "requires 2 parameter");

    napi_valuetype eventName = napi_undefined;
    napi_typeof(env, argv[0], &eventName);
    NAPI_ASSERT(env, eventName == napi_string, "type mismatch for parameter 1");

    napi_valuetype handler = napi_undefined;
    napi_typeof(env, argv[1], &handler);
    NAPI_ASSERT(env, handler == napi_function, "type mismatch for parameter 2");

    char type[64] = {0};
    size_t typeLen = 0;
    napi_get_value_string_utf8(env, argv[0], type, sizeof(type), &typeLen);
    EventRegister::GetInstance().Register(env, type, argv[1]);
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}
static bool CheckTransmitParametersAndThrow(napi_env env, const napi_value parameters[],
                                            size_t parameterCount)
{
    if (parameterCount == ARGV_NUM_1) {
        if (!CheckParametersAndThrow(env, parameters, {napi_object}, "data", "number[]")) {
            return false;
        }
        return true;
    } else if (parameterCount == ARGV_NUM_2) {
        if (!CheckParametersAndThrow(env, parameters, {napi_object, napi_function}, "data & callback",
                                     "number[] & function") ||
            !CheckArrayNumberAndThrow(env, parameters[ARGV_NUM_0], "data", "number[]")) {
            return false;
        }
        return true;
    } else {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM,
                                              BuildErrorMessage(BUSI_ERR_PARAM, "", "", "", "")));
        return false;
    }
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
    InfoLog("Register eventType: %{public}s", type.c_str());
    if (!IsEventSupport(type)) {
        DebugLog("Register eventType error or not support!");
        return;
    }
    std::lock_guard<std::mutex> guard(g_regInfoMutex);
    if (!isEventRegistered) {
        if (RegHceCmdCallbackEvents(env, type) != KITS::ERR_NONE) {
            return;
        }
        isEventRegistered = true;
    }
    napi_ref handlerRef = nullptr;
    napi_create_reference(env, handler, 1, &handlerRef);
    RegObj regObj(env, handlerRef);
    auto iter = g_eventRegisterInfo.find(type);
    if (iter == g_eventRegisterInfo.end()) {
        g_eventRegisterInfo[type] = regObj;
        DebugLog("Register, add new type.");
        return;
    }

    auto oldRegObj = iter->second;
    if (env == oldRegObj.m_regEnv) {
        DebugLog("handler env is same");
        napi_value oldHandler = nullptr;
        napi_get_reference_value(oldRegObj.m_regEnv, oldRegObj.m_regHanderRef, &oldHandler);
        bool isEqual = false;
        napi_strict_equals(oldRegObj.m_regEnv, oldHandler, handler, &isEqual);
        if (isEqual) {
            DebugLog("handler function is same");
        } else {
            iter->second = regObj;
        }
    } else {
        DebugLog("handler env is different");
        iter->second = regObj;
    }
}

ErrorCode EventRegister::RegHceCmdCallbackEvents(const napi_env& env, const std::string& type)
{
    HceService hceService = HceService::GetInstance();
    ErrorCode ret = hceService.RegHceCmdCallback(hceCmdListenerEvent, type);
    if (!CheckHceStatusCodeAndThrow(env, ret, "on")) {
        ErrorLog("RegHceCmdCallback, statusCode = %{public}d", ret);
    }
    if (ret != KITS::ERR_NONE) {
        DebugLog("RegHceCmdCallbackEvents failed!");
        return ret;
    }
    return ret;
}

void EventRegister::Unregister(const napi_env& env, ElementName& element)
{
    std::lock_guard<std::mutex> guard(g_regInfoMutex);
    if (!g_eventRegisterInfo.empty()) {
        if (UnregisterHceEvents(env, element) != KITS::ERR_NONE) {
            ErrorLog("hce EventRegister::Unregister, unreg event failed.");
            return;
        }
    }

    DeleteHceCmdRegisterObj(env);

    if (!g_eventRegisterInfo.empty()) {
        g_eventRegisterInfo.erase(KITS::EVENT_HCE_CMD);
        isEventRegistered = false;
    }
    InfoLog("hce EventRegister, isEvtRegistered = %{public}d", isEventRegistered);
}

void EventRegister::DeleteHceCmdRegisterObj(const napi_env& env)
{
    auto iter = g_eventRegisterInfo.find(KITS::EVENT_HCE_CMD);
    if (iter == g_eventRegisterInfo.end()) {
        InfoLog("no hce cmd register info.");
        return;
    }

    auto oldRegObj = iter->second;
    if (env == oldRegObj.m_regEnv) {
        DebugLog("env is same");
        uint32_t refCount = INVALID_REF_COUNT;
        napi_reference_unref(oldRegObj.m_regEnv, oldRegObj.m_regHanderRef, &refCount);
        InfoLog(
            "DeleteHceCmdRegisterObj, m_regEnv: %{private}p, m_regHanderRef: %{private}p, refCount: %{public}d",
            oldRegObj.m_regEnv, oldRegObj.m_regHanderRef, refCount);
        if (refCount == 0) {
            InfoLog("DeleteHceCmdRegisterObj, ref count is zero");
            napi_delete_reference(oldRegObj.m_regEnv, oldRegObj.m_regHanderRef);
        }
    } else {
        InfoLog("DeleteHceCmdRegisterObj, env is different, env: %{private}p m_regEnv:: %{private}p", env,
                oldRegObj.m_regEnv);
    }
}

ErrorCode EventRegister::UnregisterHceEvents(const napi_env& env, ElementName& element)
{
    HceService hceService = HceService::GetInstance();
    ErrorCode ret = hceService.StopHce(element);
    if (!CheckHceStatusCodeAndThrow(env, ret, "stop")) {
        ErrorLog("StopHce, statusCode = %{public}d", ret);
    }
    if (ret != KITS::ERR_NONE) {
        ErrorLog("UnregisterHceEvents  failed!");
        return ret;
    }
    return ret;
}

static void after_work_cb(uv_work_t* work, int status)
{
    AsyncEventData* asyncData = static_cast<AsyncEventData*>(work->data);
    InfoLog("Napi event uv_queue_work, env: %{private}p, status: %{public}d", asyncData->env, status);
    napi_value handler = nullptr;
    napi_handle_scope scope = nullptr;
    uint32_t refCount = INVALID_REF_COUNT;
    napi_open_handle_scope(asyncData->env, &scope);
    if (scope == nullptr) {
        ErrorLog("after_work_cb: scope is nullptr");
        goto EXIT;
    }

    napi_get_reference_value(asyncData->env, asyncData->callbackRef, &handler);
    if (handler == nullptr) {
        ErrorLog("after_work_cb: handler is nullptr");
        goto EXIT;
    }
    napi_value resArgs[ARGV_INDEX_2];
    napi_get_undefined(asyncData->env, &resArgs[ARGV_INDEX_0]);
    resArgs[ARGV_INDEX_1] = asyncData->packResult();
    napi_value returnVal;
    napi_get_undefined(asyncData->env, &returnVal);
    if (napi_call_function(asyncData->env, nullptr, handler, ARGV_INDEX_2, resArgs, &returnVal) != napi_ok) {
        DebugLog("Report event to Js failed");
    } else {
        DebugLog("Report event to Js success");
    }

EXIT:
    napi_close_handle_scope(asyncData->env, scope);
    napi_reference_unref(asyncData->env, asyncData->callbackRef, &refCount);
    InfoLog("after_work_cb unref, env: %{private}p, callbackRef: %{private}p, "
            "refCount: %{public}d",
            asyncData->env, asyncData->callbackRef, refCount);
    if (refCount == 0) {
        napi_delete_reference(asyncData->env, asyncData->callbackRef);
    }
    delete asyncData;
    delete work;
    asyncData = nullptr;
    work = nullptr;
}

void NapiEvent::EventNotify(AsyncEventData* asyncEvent)
{
    DebugLog("Enter hce cmd event notify");
    if (asyncEvent == nullptr) {
        DebugLog("hce asyncEvent is null.");
        return;
    }
    uv_loop_s* loop = nullptr;
    napi_get_uv_event_loop(asyncEvent->env, &loop);

    uv_work_t* work = new uv_work_t;
    if (work == nullptr) {
        DebugLog("hce uv_work_t work is null.");
        delete asyncEvent;
        asyncEvent = nullptr;
        return;
    }

    uint32_t refCount = INVALID_REF_COUNT;
    napi_reference_ref(asyncEvent->env, asyncEvent->callbackRef, &refCount);
    work->data = asyncEvent;
    uv_after_work_cb tmp_after_work_cb = after_work_cb;
    int ret = uv_queue_work(loop, work, [](uv_work_t* work) {}, tmp_after_work_cb);
    if (ret != 0) {
        ErrorLog("uv_queue_work failed!");
        delete asyncEvent;
        delete work;
    }
}

napi_value NapiEvent::CreateResult(const napi_env& env, const std::vector<uint8_t>& data)
{
    napi_value result;
    napi_create_array_with_length(env, data.size(), &result);
    for (uint32_t i = 0; i < data.size(); i++) {
        napi_value item;
        napi_create_uint32(env, static_cast<uint32_t>(data[i]), &item);
        napi_set_element(env, result, i, item);
    }
    return result;
}

bool NapiEvent::CheckIsRegister(const std::string& type)
{
    return g_eventRegisterInfo.find(type) != g_eventRegisterInfo.end();
}

static void NativeTransmit(napi_env env, void* data)
{
    auto context = static_cast<NfcHceSessionContext*>(data);
    context->errorCode = BUSI_ERR_TAG_STATE_INVALID;
    std::string hexRespData;
    HceService hceService = HceService::GetInstance();
    context->errorCode = hceService.SendRawFrame(context->dataBytes, true, hexRespData);
    context->value = hexRespData;
    context->resolved = true;
}

static void TransmitCallback(napi_env env, napi_status status, void* data)
{
    auto context = static_cast<NfcHceSessionContext*>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok && context->resolved && context->errorCode == ErrorCode::ERR_NONE) {
        napi_get_undefined(env, &callbackValue);
        DoAsyncCallbackOrPromise(env, context, callbackValue);
    } else {
        int errCode = BuildOutputErrorCodeHce(context->errorCode);
        std::string errMessage = BuildErrorMessage(errCode, "transmit", CARD_EMULATION_PERM_DESC, "", "");
        ThrowAsyncError(env, context, errCode, errMessage);
    }
}

napi_value NfcNapiHceAdapter::Transmit(napi_env env, napi_callback_info info)
{
    // JS API define1: Transmit(data: number[]): Promise<number[]>
    // JS API define2: Transmit(data: number[], callback:
    // AsyncCallback<number[]>): void
    size_t paramsCount = ARGV_NUM_2;
    napi_value params[ARGV_NUM_2] = {0};
    void* data = nullptr;
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &thisVar, &data);

    if (!CheckTransmitParametersAndThrow(env, params, paramsCount)) {
        return CreateUndefined(env);
    }

    auto context = std::make_unique<NfcHceSessionContext>().release();
    if (!CheckContextAndThrow(env, context, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }

    // parse the params
    int32_t hexCmdData = 0;
    napi_value hexCmdDataValue = nullptr;
    uint32_t arrayLength = 0;
    std::vector<unsigned char> dataBytes = {};
    NAPI_CALL(env, napi_get_array_length(env, params[ARGV_INDEX_0], &arrayLength));
    for (uint32_t i = 0; i < arrayLength; ++i) {
        NAPI_CALL(env, napi_get_element(env, params[ARGV_INDEX_0], i, &hexCmdDataValue));
        NAPI_CALL(env, napi_get_value_int32(env, hexCmdDataValue, &hexCmdData));
        dataBytes.push_back(hexCmdData);
    }
    context->dataBytes =
        NfcSdkCommon::BytesVecToHexString(static_cast<unsigned char*>(dataBytes.data()), dataBytes.size());
    if (paramsCount == ARGV_NUM_2) {
        napi_create_reference(env, params[ARGV_INDEX_1], DEFAULT_REF_COUNT, &context->callbackRef);
    }

    napi_value result = HandleAsyncWork(env, context, "Transmit", NativeTransmit, TransmitCallback);
    return result;
}

napi_value NfcNapiHceAdapter::StopHce(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
    napi_value thisVar = 0;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    ElementName element;
    if (!CheckArgCountAndThrow(env, argc, ARGV_NUM_1) ||
        !ParseElementName(env, element, argv[ARGV_INDEX_0])) {
        ErrorLog("Stop hce: parse args failed");
        return CreateUndefined(env);
    }

    EventRegister::GetInstance().Unregister(env, element);
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}
napi_value NfcNapiHceAdapter::StartHCEDeprecated(napi_env env, napi_callback_info cbinfo)
{
    return CreateUndefined(env);
}
napi_value NfcNapiHceAdapter::StartHCE(napi_env env, napi_callback_info cbinfo)
{
    size_t argc = ARGV_NUM_2;
    napi_value argv[ARGV_NUM_2] = {0};
    napi_value thisVar = 0;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    ElementName element;
    std::vector<std::string> aidVec;
    if (!CheckArgCountAndThrow(env, argc, ARGV_NUM_2) || !ParseElementName(env, element, argv[ARGV_INDEX_0]) ||
        !ParseStringVector(env, aidVec, argv[ARGV_INDEX_1], MAX_AID_LIST_NUM_PER_APP)) {
        ErrorLog("Start hce: parse args failed");
        return CreateUndefined(env);
    }

    HceService hceService = HceService::GetInstance();
    ErrorCode ret = hceService.StartHce(element, aidVec);
    if (!CheckHceStatusCodeAndThrow(env, ret, "start")) {
        ErrorLog("StartHce, statusCode = %{public}d", ret);
    }
    return CreateUndefined(env);
}
napi_value NfcNapiHceAdapter::StopHCEDeprecated(napi_env env, napi_callback_info cbinfo)
{
    return CreateUndefined(env);
}
napi_value NfcNapiHceAdapter::SendResponse(napi_env env, napi_callback_info cbinfo)
{
    return CreateUndefined(env);
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS