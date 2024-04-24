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
#include "nfc_napi_foreground_dispatch.h"
#include <mutex>
#include <uv.h>
#include "nfc_napi_tag.h"
#include "nfc_sdk_common.h"
#include "loghelper.h"
#include "tag_foreground.h"
#include "taginfo_parcelable.h"

namespace OHOS {
namespace NFC {
namespace KITS {
constexpr uint32_t INVALID_REF_COUNT = 0xFF;
static std::mutex g_mutex {};
static RegObj g_foregroundRegInfo;
static RegObj g_readerModeRegInfo;
bool ForegroundEventRegister::isEvtRegistered = false;
bool ReaderModeEvtRegister::isReaderModeRegistered = false;
const std::string TYPE_FOREGROUND = "foreground";
const std::string TYPE_READER_MODE = "readerMode";

class NapiEvent {
public:
    napi_value CreateResult(const napi_env &env, TagInfoParcelable tagInfo);
    static bool IsForegroundRegistered();
    static bool IsReaderModeRegistered();
    void EventNotify(AsyncEventData *asyncEvent);

    template<typename T>
    void CheckAndNotify(const T& obj, const std::string &type)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        RegObj regObj;
        if (type.compare(TYPE_FOREGROUND) == 0) {
            if (!IsForegroundRegistered()) {
                ErrorLog("CheckAndNotify: foreground not registered.");
                return;
            }
            regObj = g_foregroundRegInfo;
        } else if (type.compare(TYPE_READER_MODE) == 0) {
            if (!IsReaderModeRegistered()) {
                ErrorLog("CheckAndNotify: reader mode not registered.");
                return;
            }
            regObj = g_readerModeRegInfo;
        } else {
            ErrorLog("CheckAndNotify: unknown type: %{public}s", type.c_str());
            return;
        }

        auto result = [this, env = regObj.regEnv, obj] () -> napi_value {
            return CreateResult(env, obj);
        };
        AsyncEventData *asyncEvent =
            new (std::nothrow)AsyncEventData(regObj.regEnv, regObj.regHandlerRef, result);
        if (asyncEvent == nullptr) {
            return;
        }
        EventNotify(asyncEvent);
    }
};

static void ReleaseAfterWorkCb(uv_work_t *work, AsyncEventData *asyncData,
    napi_handle_scope &scope, uint32_t &refCount)
{
    napi_close_handle_scope(asyncData->env, scope);
    napi_reference_unref(asyncData->env, asyncData->callbackRef, &refCount);
    InfoLog("ReleaseAfterWorkCb unref, env: %{private}p, callbackRef: %{private}p, refCount: %{public}d",
        asyncData->env, asyncData->callbackRef, refCount);
    if (refCount == 0) {
        napi_delete_reference(asyncData->env, asyncData->callbackRef);
    }
    delete asyncData;
    delete work;
}

static void AfterWorkCb(uv_work_t *work, int status)
{
    AsyncEventData *asyncData = static_cast<AsyncEventData *>(work->data);
    InfoLog("AfterWorkCb, status: %{public}d", status);
    napi_handle_scope scope = nullptr;
    uint32_t refCount = INVALID_REF_COUNT;
    napi_open_handle_scope(asyncData->env, &scope);
    if (scope == nullptr) {
        ErrorLog("AfterWorkCb: scope is nullptr");
        ReleaseAfterWorkCb(work, asyncData, scope,  refCount);
        return;
    }

    napi_value callback = nullptr;
    napi_get_reference_value(asyncData->env, asyncData->callbackRef, &callback);
    if (callback == nullptr) {
        ErrorLog("AfterWorkCb: callback is nullptr");
        ReleaseAfterWorkCb(work, asyncData, scope,  refCount);
        return;
    }

    // build result arg for async callback in an array {error, tagInfo}
    napi_value resArgs[ARGV_INDEX_2];
    napi_get_undefined(asyncData->env, &resArgs[ARGV_INDEX_0]);
    resArgs[ARGV_INDEX_1] = asyncData->packResult();
    napi_value returnVal;
    napi_get_undefined(asyncData->env, &returnVal);
    if (napi_call_function(asyncData->env, nullptr, callback, ARGV_INDEX_2, resArgs, &returnVal) != napi_ok) {
        DebugLog("AfterWorkCb: Report event to Js failed");
    }
    ReleaseAfterWorkCb(work, asyncData, scope,  refCount);
}

void NapiEvent::EventNotify(AsyncEventData *asyncEvent)
{
    if (asyncEvent == nullptr) {
        DebugLog("foreground event notify: asyncEvent is null.");
        return;
    }
    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(asyncEvent->env, &loop);

    uv_work_t *work = new uv_work_t;
    if (work == nullptr) {
        DebugLog("foreground event notify: uv_work_t work is null.");
        delete asyncEvent;
        asyncEvent = nullptr;
        return;
    }

    InfoLog("foreground event notify: Get the event loop");
    uint32_t refCount = INVALID_REF_COUNT;
    napi_reference_ref(asyncEvent->env, asyncEvent->callbackRef, &refCount);
    work->data = asyncEvent;
    uv_after_work_cb tmpAfterWorkCb = AfterWorkCb;
    int ret = uv_queue_work(
        loop,
        work,
        [](uv_work_t *work) {},
        tmpAfterWorkCb);
    if (ret != 0) {
        ErrorLog("uv_queue_work failed");
        delete asyncEvent;
        delete work;
    }
}

static void SetTagExtraData(const napi_env &env, napi_value &tagInfoObj, TagInfoParcelable &tagInfo)
{
    uint32_t length = tagInfo.GetTechExtrasDataList().size();
    if (length > MAX_NUM_TECH_LIST) {
        ErrorLog("SetTagExtraData: invalid tag extras data length");
        return;
    }
    napi_value extrasData;
    napi_create_array_with_length(env, length, &extrasData);

    // parse extra data for this technology
    napi_value propValue;
    for (uint32_t i = 0; i < length; i++) {
        napi_value eachElement;
        napi_create_object(env, &eachElement);
        AppExecFwk::PacMap extra = tagInfo.GetTechExtrasDataList()[i];
        int technology = tagInfo.GetTechList()[i];
        if (technology == static_cast<int>(TagTechnology::NFC_A_TECH) ||
            technology == static_cast<int>(TagTechnology::NFC_MIFARE_CLASSIC_TECH)) {
            // for NFCA, parse extra SAK and ATQA
            napi_create_uint32(env, extra.GetIntValue(KITS::TagInfo::SAK, 0), &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::SAK, propValue);

            napi_create_string_utf8(env, extra.GetStringValue(KITS::TagInfo::ATQA, "").c_str(),
                NAPI_AUTO_LENGTH, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::ATQA, propValue);
        } else if (technology == static_cast<int>(TagTechnology::NFC_B_TECH)) {
            // parse app data and protocol info of nfcb.
            napi_create_string_utf8(env, extra.GetStringValue(KITS::TagInfo::APP_DATA, "").c_str(),
                NAPI_AUTO_LENGTH, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::APP_DATA, propValue);

            napi_create_string_utf8(env, extra.GetStringValue(KITS::TagInfo::PROTOCOL_INFO, "").c_str(),
                NAPI_AUTO_LENGTH, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::PROTOCOL_INFO, propValue);
        } else if (technology == static_cast<int>(TagTechnology::NFC_F_TECH)) {
            // parse pmm and sc of nfcf
            napi_create_string_utf8(env, extra.GetStringValue(KITS::TagInfo::NFCF_PMM, "").c_str(),
                NAPI_AUTO_LENGTH, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::NFCF_PMM, propValue);

            napi_create_string_utf8(env, extra.GetStringValue(KITS::TagInfo::NFCF_SC, "").c_str(),
                NAPI_AUTO_LENGTH, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::NFCF_SC, propValue);
        } else if (technology == static_cast<int>(TagTechnology::NFC_V_TECH)) {
            // parse response flag and dsf id of nfcv.
            napi_create_uint32(env, extra.GetIntValue(KITS::TagInfo::RESPONSE_FLAGS, 0), &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::RESPONSE_FLAGS, propValue);

            napi_create_uint32(env, extra.GetIntValue(KITS::TagInfo::DSF_ID, 0), &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::DSF_ID, propValue);
        } else if (technology == static_cast<int>(TagTechnology::NFC_ISODEP_TECH)) {
            // for ISODEP, parse extra HistoryBytes and HilayerResponse
            napi_create_string_utf8(env, extra.GetStringValue(KITS::TagInfo::HISTORICAL_BYTES, "").c_str(),
                NAPI_AUTO_LENGTH, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::HISTORICAL_BYTES, propValue);

            napi_create_string_utf8(env, extra.GetStringValue(KITS::TagInfo::HILAYER_RESPONSE, "").c_str(),
                NAPI_AUTO_LENGTH, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::HILAYER_RESPONSE, propValue);
        } else if (technology == static_cast<int>(TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH)) {
            napi_get_boolean(env, extra.GetBooleanValue(KITS::TagInfo::MIFARE_ULTRALIGHT_C_TYPE, false), &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::MIFARE_ULTRALIGHT_C_TYPE, propValue);
        } else if (technology == static_cast<int>(TagTechnology::NFC_NDEF_TECH)) {
            // parse ndef message/type/max size/read mode for ndef tag
            napi_create_string_utf8(env, extra.GetStringValue(KITS::TagInfo::NDEF_MSG, "").c_str(),
                NAPI_AUTO_LENGTH, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::NDEF_MSG, propValue);

            napi_create_uint32(env, extra.GetIntValue(KITS::TagInfo::NDEF_FORUM_TYPE, 0), &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::NDEF_FORUM_TYPE, propValue);

            napi_create_uint32(env, extra.GetIntValue(KITS::TagInfo::NDEF_TAG_LENGTH, 0), &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::NDEF_TAG_LENGTH, propValue);

            napi_create_uint32(env, extra.GetIntValue(KITS::TagInfo::NDEF_TAG_MODE, 0), &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::NDEF_TAG_MODE, propValue);
        } else {
            // set extrasData[i] empty if no tech matches to keep one-to-one mapping of techList and extras
        }
        napi_set_element(env, extrasData, i, eachElement);
    }
    napi_set_named_property(env, tagInfoObj, VAR_EXTRA.c_str(), extrasData);
}

napi_value NapiEvent::CreateResult(const napi_env &env, TagInfoParcelable tagInfo)
{
    // build tagInfo Js Object
    napi_value tagInfoObj = nullptr;
    napi_value uidValue;
    napi_value techValue;
    napi_value rfIdValue;
    napi_create_object(env, &tagInfoObj);

    std::string uid = tagInfo.GetUid();
    std::vector<int> techList = tagInfo.GetTechList();
    int rfId = tagInfo.GetDiscId();
    std::vector<unsigned char> uidBytes;
    NfcSdkCommon::HexStringToBytes(uid, uidBytes);
    BytesVectorToJS(env, uidValue, uidBytes);
    napi_create_array_with_length(env, techList.size(), &techValue);
    for (uint32_t i = 0; i < techList.size(); i++) {
        napi_value tech;
        napi_create_uint32(env, techList[i], &tech);
        napi_set_element(env, techValue, i, tech);
    }
    napi_create_uint32(env, rfId, &rfIdValue);
    napi_set_named_property(env, tagInfoObj, VAR_UID.c_str(), uidValue);
    napi_set_named_property(env, tagInfoObj, VAR_TECH.c_str(), techValue);
    napi_set_named_property(env, tagInfoObj, VAR_RF_ID.c_str(), rfIdValue);

    // set extras data from taginfo parcelable to taginfo js object
    SetTagExtraData(env, tagInfoObj, tagInfo);
    return tagInfoObj;
}

bool NapiEvent::IsForegroundRegistered()
{
    return (!g_foregroundRegInfo.IsEmpty());
}

bool NapiEvent::IsReaderModeRegistered()
{
    return (!g_readerModeRegInfo.IsEmpty());
}

class ForegroundListenerEvent : public IForegroundCallback, public NapiEvent {
public:
    ForegroundListenerEvent() {}
    virtual ~ForegroundListenerEvent() {}
public:
    void OnTagDiscovered(KITS::TagInfoParcelable* tagInfo) override
    {
        InfoLog("OnNotify rcvd tagInfo: %{public}s", tagInfo->ToString().c_str());
        CheckAndNotify(*(tagInfo), TYPE_FOREGROUND);
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

sptr<ForegroundListenerEvent> foregroundListenerEvent =
    sptr<ForegroundListenerEvent>(new (std::nothrow) ForegroundListenerEvent());

ErrorCode ForegroundEventRegister::RegisterForegroundEvents(ElementName &element,
    std::vector<uint32_t> &discTech)
{
    TagForeground tagForeground = TagForeground::GetInstance();
    ErrorCode ret = tagForeground.RegForeground(element, discTech, foregroundListenerEvent);
    if (ret != KITS::ERR_NONE) {
        DebugLog("ForegroundEventRegister foregroundListenerEvent failed!");
        return ret;
    }
    return ret;
}

ErrorCode ForegroundEventRegister::UnregisterForegroundEvents(ElementName &element)
{
    TagForeground tagForeground = TagForeground::GetInstance();
    ErrorCode ret = tagForeground.UnregForeground(element);
    if (ret != KITS::ERR_NONE) {
        DebugLog("UnregisterForegroundEvents nfcListenerEvent failed!");
        return ret;
    }
    return ret;
}

ForegroundEventRegister& ForegroundEventRegister::GetInstance()
{
    static ForegroundEventRegister inst;
    return inst;
}

ErrorCode ForegroundEventRegister::Register(const napi_env &env, ElementName &element,
    std::vector<uint32_t> &discTech, napi_value handler)
{
    InfoLog("ForegroundEventRegister::Register event, isEvtRegistered = %{public}d", isEvtRegistered);
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!isEvtRegistered) {
        ErrorCode ret = RegisterForegroundEvents(element, discTech);
        if (ret != ERR_NONE) {
            ErrorLog("ForegroundEventRegister::Register, reg event failed");
            return ret;
        }
        isEvtRegistered = true;
    }
    napi_ref handlerRef = nullptr;
    napi_create_reference(env, handler, 1, &handlerRef);
    RegObj regObj(env, handlerRef, element, discTech);
    g_foregroundRegInfo = regObj;
    if (env == regObj.regEnv) {
        napi_value handlerTemp = nullptr;
        napi_get_reference_value(regObj.regEnv, regObj.regHandlerRef, &handlerTemp);
    }
    return ERR_NONE;
}

void ForegroundEventRegister::DeleteRegisterObj(const napi_env &env, RegObj &regObj, napi_value &handler)
{
    if (env == regObj.regEnv) {
        uint32_t refCount = INVALID_REF_COUNT;
        napi_reference_unref(regObj.regEnv, regObj.regHandlerRef, &refCount);
        InfoLog("ForegroundEventRegister::DeleteRegisterObj: delete ref, refCount: %{public}d", refCount);
        if (refCount == 0) {
            napi_delete_reference(regObj.regEnv, regObj.regHandlerRef);
            DebugLog("ForegroundEventRegister::DeleteRegisterObj: ref obj deleted.");
        }
    }
}

ErrorCode ForegroundEventRegister::Unregister(const napi_env &env, ElementName &element, napi_value handler)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!g_foregroundRegInfo.IsEmpty()) {
        ErrorCode ret = UnregisterForegroundEvents(element);
        if (ret != ERR_NONE) {
            ErrorLog("ForegroundEventRegister::Unregister, unreg event failed.");
            return ret;
        }
    }
    if (handler != nullptr) {
        DeleteRegisterObj(env, g_foregroundRegInfo, handler);
    }
    if (!g_foregroundRegInfo.IsEmpty()) {
        g_foregroundRegInfo.Clear();
        isEvtRegistered = false;
    }
    InfoLog("ForegroundEventRegister::Unregister, isEvtRegistered = %{public}d", isEvtRegistered);
    return ERR_NONE;
}

bool ParseDiscTechVector(napi_env &env, std::vector<uint32_t> &dataVec, napi_value arg)
{
    if (!CheckArrayNumberAndThrow(env, arg, "discTech", "nummber[]") ||
        !ParseUInt32Vector(env, dataVec, arg)) {
        ErrorLog("ParseDiscTechVector: parse failed");
        return false;
    }
    if (dataVec.size() == 0) {
        ErrorLog("ParseDiscTechVector: size = 0");
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PARAM, BuildErrorMessage(BUSI_ERR_PARAM,
            "", "", "discTech", "nummber[]")));
        return false;
    }
    return true;
}

bool CheckResultAndThrow(napi_env &env, int result, std::string funcName)
{
    if (result == ERR_NONE) {
        return true;
    }
    ErrorLog("CheckResultAndThrow, result = %{public}d", result);
    if (result == ERR_TAG_APP_NOT_FOREGROUND) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_ELEMENT_STATE_INVALID,
            BuildErrorMessage(BUSI_ERR_ELEMENT_STATE_INVALID, "", "", "", "")));
    } else if (result == ERR_NO_PERMISSION) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_PERM,
            BuildErrorMessage(BUSI_ERR_PERM, funcName, TAG_PERM_DESC, "", "")));
    } else if (result == ERR_TAG_APP_NOT_REGISTERED) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_REGISTER_STATE_INVALID,
            BuildErrorMessage(BUSI_ERR_REGISTER_STATE_INVALID, "", "", "", "")));
    } else if (result == ERR_TAG_STATE_NFC_CLOSED) {
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_TAG_STATE_INVALID,
            BuildErrorMessage(BUSI_ERR_TAG_STATE_INVALID, "", "", "", "")));
    }
    return false;
}

napi_value RegisterForegroundDispatch(napi_env env, napi_callback_info cbinfo)
{
    DebugLog("RegisterForegroundDispatch");
    size_t argc = ARGV_NUM_3;
    napi_value argv[ARGV_NUM_3] = {0};
    napi_value thisVar = 0;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    ElementName element;
    std::vector<uint32_t> dataVec;
    if (!CheckArgCountAndThrow(env, argc, ARGV_NUM_3) ||
        !CheckObjectAndThrow(env, argv[ARGV_INDEX_0], "elementName", "ElementName") ||
        !ParseElementName(env, element, argv[ARGV_INDEX_0]) ||
        !ParseDiscTechVector(env, dataVec, argv[ARGV_INDEX_1]) ||
        !CheckFunctionAndThrow(env, argv[ARGV_INDEX_2], "callback", "AsyncCallback<TagInfo>")) {
        ErrorLog("RegisterForegroundDispatch: parse args failed");
        return CreateUndefined(env);
    }
    int ret  = ForegroundEventRegister::GetInstance().Register(env, element, dataVec, argv[ARGV_INDEX_2]);
    CheckResultAndThrow(env, ret, "RegisterForegroundDispatch");
    return CreateUndefined(env);
}

napi_value UnregisterForegroundDispatch(napi_env env, napi_callback_info cbinfo)
{
    DebugLog("UnregisterForegroundDispatch");
    size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
    napi_value thisVar = 0;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    ElementName element;
    if (!CheckArgCountAndThrow(env, argc, ARGV_NUM_1) ||
        !CheckObjectAndThrow(env, argv[ARGV_INDEX_0], "elementName", "ElementName") ||
        !ParseElementName(env, element, argv[ARGV_INDEX_0])) {
        ErrorLog("UnregisterForegroundDispatch: parse args failed");
        return CreateUndefined(env);
    }
    int ret = ForegroundEventRegister::GetInstance().Unregister(env, element, argv[ARGV_INDEX_0]);
    CheckResultAndThrow(env, ret, "UnregisterForegroundDispatch");
    return CreateUndefined(env);
}

class ReaderModeListenerEvt : public IReaderModeCallback, public NapiEvent {
public:
    ReaderModeListenerEvt() {}
    virtual ~ReaderModeListenerEvt() {}
public:
    void OnTagDiscovered(KITS::TagInfoParcelable* tagInfo) override
    {
        InfoLog("ReaderModeListenerEvt::OnNotify rcvd tagInfo: %{public}s", tagInfo->ToString().c_str());
        CheckAndNotify(*(tagInfo), TYPE_READER_MODE);
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override
    {
        return nullptr;
    }
};

sptr<ReaderModeListenerEvt> readerModeListenerEvt =
    sptr<ReaderModeListenerEvt>(new (std::nothrow) ReaderModeListenerEvt());

ErrorCode ReaderModeEvtRegister::RegReaderModeEvt(std::string &type, ElementName &element,
                                                  std::vector<uint32_t> &discTech)
{
    if (type.compare(TYPE_READER_MODE) != 0) {
        ErrorLog("RegReaderModeEvt invalid type: %{public}s", type.c_str());
        return KITS::ERR_NFC_PARAMETERS;
    }
    TagForeground tagForeground = TagForeground::GetInstance();
    ErrorCode ret = tagForeground.RegReaderMode(element, discTech, readerModeListenerEvt);
    if (ret != KITS::ERR_NONE) {
        DebugLog("RegReaderModeEvt register failed!");
        return ret;
    }
    return ret;
}

ErrorCode ReaderModeEvtRegister::UnregReaderModeEvt(std::string &type, ElementName &element)
{
    if (type.compare(TYPE_READER_MODE) != 0) {
        ErrorLog("UnregReaderModeEvt invalid type: %{public}s", type.c_str());
        return KITS::ERR_NFC_PARAMETERS;
    }
    TagForeground tagForeground = TagForeground::GetInstance();
    ErrorCode ret = tagForeground.UnregReaderMode(element);
    if (ret != KITS::ERR_NONE) {
        DebugLog("UnregReaderModeEvt nfcListenerEvent failed!");
        return ret;
    }
    return ret;
}

ReaderModeEvtRegister& ReaderModeEvtRegister::GetInstance()
{
    static ReaderModeEvtRegister inst;
    return inst;
}

ErrorCode ReaderModeEvtRegister::Register(const napi_env &env, std::string &type, ElementName &element,
                                          std::vector<uint32_t> &discTech, napi_value handler)
{
    InfoLog("ReaderModeEvtRegister::Register event, isReaderModeRegistered = %{public}d", isReaderModeRegistered);
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!isReaderModeRegistered) {
        ErrorCode ret = RegReaderModeEvt(type, element, discTech);
        if (ret != KITS::ERR_NONE) {
            ErrorLog("ReaderModeEvtRegister::Register, reg event failed");
            return ret;
        }
        isReaderModeRegistered = true;
    }
    napi_ref handlerRef = nullptr;
    napi_create_reference(env, handler, 1, &handlerRef);
    RegObj regObj(env, handlerRef, element, discTech);
    g_readerModeRegInfo = regObj;
    if (env == regObj.regEnv) {
        napi_value handlerTemp = nullptr;
        napi_get_reference_value(regObj.regEnv, regObj.regHandlerRef, &handlerTemp);
    }
    return ERR_NONE;
}

void ReaderModeEvtRegister::DeleteRegisteredObj(const napi_env &env, RegObj &regObj, napi_value &handler)
{
    if (env == regObj.regEnv) {
        uint32_t refCount = INVALID_REF_COUNT;
        napi_reference_unref(regObj.regEnv, regObj.regHandlerRef, &refCount);
        InfoLog("ReaderModeEvtRegister::DeleteRegisteredObj: delete ref, refCount: %{public}d", refCount);
        if (refCount == 0) {
            napi_delete_reference(regObj.regEnv, regObj.regHandlerRef);
            DebugLog("ReaderModeEvtRegister::DeleteRegisteredObj: ref obj deleted.");
        }
    }
}

ErrorCode ReaderModeEvtRegister::Unregister(const napi_env &env, std::string &type, ElementName &element,
                                            napi_value handler)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_readerModeRegInfo.IsEmpty()) {
        ErrorLog("ReaderModeEvtRegister::Unregister, reader not registered");
        return ERR_TAG_APP_NOT_REGISTERED;
    }
    ErrorCode ret = UnregReaderModeEvt(type, element);
    if (ret != ERR_NONE) {
        ErrorLog("ReaderModeEvtRegister::Unregister, unreg event failed.");
        return ret;
    }
    DeleteRegisteredObj(env, g_readerModeRegInfo, handler);
    if (!g_readerModeRegInfo.IsEmpty()) {
        g_readerModeRegInfo.Clear();
        isReaderModeRegistered = false;
    }
    InfoLog("ReaderModeEvtRegister::Unregister, isReaderModeRegistered = %{public}d", isReaderModeRegistered);
    return ERR_NONE;
}

napi_value On(napi_env env, napi_callback_info cbinfo)
{
    DebugLog("On ReaderMode");
    size_t argc = ARGV_NUM_4;
    napi_value argv[ARGV_NUM_4] = {0};
    napi_value thisVar = 0;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    std::string type = "";
    ElementName element;
    std::vector<uint32_t> dataVec;
    if (!CheckArgCountAndThrow(env, argc, ARGV_NUM_4) ||
        !CheckStringAndThrow(env, argv[ARGV_INDEX_0], "type", "String") ||
        !ParseString(env, type, argv[ARGV_INDEX_0]) ||
        !CheckObjectAndThrow(env, argv[ARGV_INDEX_1], "elementName", "ElementName") ||
        !ParseElementName(env, element, argv[ARGV_INDEX_1]) ||
        !ParseDiscTechVector(env, dataVec, argv[ARGV_INDEX_2]) ||
        !CheckFunctionAndThrow(env, argv[ARGV_INDEX_3], "callback", "AsyncCallback<TagInfo>")) {
        ErrorLog("On: parse args failed");
        return CreateUndefined(env);
    }
    int ret = ReaderModeEvtRegister::GetInstance().Register(env, type, element, dataVec, argv[ARGV_INDEX_3]);
    CheckResultAndThrow(env, ret, "On");
    return CreateUndefined(env);
}

napi_value Off(napi_env env, napi_callback_info cbinfo)
{
    DebugLog("Off ReaderMode");
    size_t requiredArgc = ARGV_NUM_2;
    size_t requiredArgcWithCb = ARGV_NUM_3;
    size_t argc = ARGV_NUM_3;
    napi_value argv[ARGV_NUM_3] = {0};
    napi_value thisVar = 0;
    napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr);
    NAPI_ASSERT(env, argc >= requiredArgc, "requires at least 2 parameters");
    std::string type;
    ElementName element;
    if (!CheckStringAndThrow(env, argv[ARGV_INDEX_0], "type", "String") ||
        !ParseString(env, type, argv[ARGV_INDEX_0]) ||
        !CheckObjectAndThrow(env, argv[ARGV_INDEX_1], "elementName", "ElementName") ||
        !ParseElementName(env, element, argv[ARGV_INDEX_1])) {
        ErrorLog("Off ReaderMode: parse args failed");
        return CreateUndefined(env);
    }
    if (argc >= requiredArgcWithCb) {
        napi_valuetype handler = napi_undefined;
        napi_typeof(env, argv[ARGV_INDEX_2], &handler);
        if (handler == napi_null || handler == napi_undefined) {
            argc -= 1;
            DebugLog("argv[2] is null or undefined, handle as no argv[2] input");
        } else {
            if (!CheckFunctionAndThrow(env, argv[ARGV_INDEX_2], "callback", "AsyncCallback<TagInfo>")) {
                ErrorLog("Off ReaderMode: parse function failed");
                return CreateUndefined(env);
            }
        }
    }
    int ret = ReaderModeEvtRegister::GetInstance().Unregister(env, type, element,
        argc >= requiredArgcWithCb ? argv[ARGV_INDEX_2] : nullptr);
    CheckResultAndThrow(env, ret, "Off");
    return CreateUndefined(env);
}
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
