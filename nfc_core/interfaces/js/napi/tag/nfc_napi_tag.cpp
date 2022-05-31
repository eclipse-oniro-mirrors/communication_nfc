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

#include "nfc_napi_tag.h"

#include "loghelper.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "nfc_napi_taga.h"
#include "nfc_napi_tag_context.h"
#include "nfc_napi_utils.h"

namespace OHOS {
namespace NFC {
namespace KITS {
napi_value nfcATagObject;
napi_value ParseIntArray(napi_env env, napi_value obj, std::vector<int> &typeArray)
{
    const int32_t ERROR_DEFAULT = -1;
    bool result = false;
    napi_status status = napi_is_array(env, obj, &result);
    if (status != napi_ok || !result) {
        InfoLog("Invalid input parameter type!");
        return nullptr;
    }

    napi_value elementValue = nullptr;
    int32_t element = ERROR_DEFAULT;
    uint32_t arrayLength = 0;
    NAPI_CALL(env, napi_get_array_length(env, obj, &arrayLength));
    typeArray.resize(arrayLength);
    for (uint32_t i = 0; i < arrayLength; ++i) {
        NAPI_CALL(env, napi_get_element(env, obj, i, &elementValue));
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, elementValue, &valueType);
        if (valueType == napi_number) {
            NAPI_CALL(env, napi_get_value_int32(env, elementValue, &element));
            typeArray[i] = element;
            InfoLog("tag tech array :%{public}d is %{public}d ", i, element);
        } else {
            InfoLog("Invalid parameter type of array element!");
            return nullptr;
        }
    }

    return CreateUndefined(env);
}

void SetPacMapObject(
    std::shared_ptr<AppExecFwk::PacMap> &pacMap, const napi_env &env, std::string keyStr, napi_value value)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType == napi_string) {
        std::string valueString = UnwrapStringFromJS(env, value);
        InfoLog("SetPacMap keystr :%{public}s", valueString.c_str());
        pacMap->PutStringValue(keyStr, valueString);
    } else if (valueType == napi_number) {
        double valueNumber = 0;
        napi_get_value_double(env, value, &valueNumber);
        pacMap->PutDoubleValue(keyStr, valueNumber);
    } else if (valueType == napi_boolean) {
        bool valueBool = false;
        napi_get_value_bool(env, value, &valueBool);
        pacMap->PutBooleanValue(keyStr, valueBool);
    } else if (valueType == napi_null) {
        pacMap->PutObject(keyStr, nullptr);
    } else if (valueType == napi_object) {
        pacMap->PutStringValueArray(keyStr, ConvertStringVector(env, value));
    } else {
        InfoLog("SetPacMapObject pacMap type error");
    }
}

void AnalysisPacMap(std::shared_ptr<AppExecFwk::PacMap> &pacMap, const napi_env &env, const napi_value &arg)
{
    InfoLog("AnalysisPacMap begin");
    napi_value keys = 0;
    napi_get_property_names(env, arg, &keys);
    uint32_t arrLen = 0;
    napi_status status = napi_get_array_length(env, keys, &arrLen);
    if (status != napi_ok) {
        InfoLog("AnalysisPacMap errr");
        return;
    }
    for (size_t i = 0; i < arrLen; ++i) {
        napi_value key = 0;
        status = napi_get_element(env, keys, i, &key);
        std::string keyStr = UnwrapStringFromJS(env, key);
        napi_value value = 0;
        napi_get_property(env, arg, key, &value);
        SetPacMapObject(pacMap, env, keyStr, value);
    }
}

napi_value ParseExtrasData(napi_env env, napi_value obj, std::shared_ptr<AppExecFwk::PacMap> &tagTechExtrasData)
{
    napi_valuetype valueType = napi_undefined;

    napi_typeof(env, obj, &valueType);

    if (valueType == napi_object) {
        InfoLog("PacMap parse begin");
        AnalysisPacMap(tagTechExtrasData, env, obj);
    } else {
        InfoLog("ParseExtrasData wrong arg!");
        return nullptr;
    }

    return CreateUndefined(env);
}

napi_value ParseTagSession(napi_env env, napi_value obj, OHOS::sptr<TAG::ITagSession> &tagSession)
{
    napi_valuetype valueType = napi_undefined;

    napi_typeof(env, obj, &valueType);

    if (valueType == napi_object) {
        InfoLog("TagSession is object");
        // wrap a native instance in a JS object tagSession
        napi_wrap(
            env, obj, tagSession,
            [](napi_env env, void *data, void *hint) {
                OHOS::sptr<TAG::ITagSession> *tagSession = (OHOS::sptr<TAG::ITagSession> *)data;
                delete tagSession;
            },
            nullptr, nullptr);
        InfoLog("wrap tagSession obj %{public}p", obj);
    } else {
        InfoLog("ParseTagSession arg err!");
        return nullptr;
    }

    return CreateUndefined(env);
}

std::shared_ptr<TagInfo> ParseTagInfo(napi_env env, napi_value obj)
{
    std::string tagUid = GetNapiStringValue(env, obj, "uid");
    InfoLog("tag uid:%{public}s", tagUid.c_str());
    std::vector<int> tagTechList;
    napi_value technology = GetNamedProperty(env, obj, "technology");
    if (technology) {
        if (ParseIntArray(env, technology, tagTechList) == nullptr) {
            InfoLog("parse tagTechList failed");
            return nullptr;
        }
    } else {
        // if technology is not set, check supportedProfiles
        napi_value supportedProfiles = GetNamedProperty(env, obj, "supportedProfiles");
        // if supportedProfiles is not null, set tagTechList with it
        if (supportedProfiles) {
            if (ParseIntArray(env, supportedProfiles, tagTechList) == nullptr) {
                InfoLog("parse supportedProfiles failed");
                return nullptr;
            }
        }
    }

    std::shared_ptr<AppExecFwk::PacMap> tagTechExtrasData = std::make_shared<AppExecFwk::PacMap>();
    napi_value extrasData = GetNamedProperty(env, obj, "extrasData");
    if (extrasData) {
        if (ParseExtrasData(env, extrasData, tagTechExtrasData) == nullptr) {
            InfoLog("parse tagTechExtrasData failed");
            return nullptr;
        }
    }

    int tagRfDiscId = GetNapiInt32Value(env, obj, "tagRfDiscId");
    InfoLog("tag RfDiscId:%{public}d", tagRfDiscId);

    OHOS::sptr<TAG::ITagSession> tagSession = nullptr;
    napi_value remoteTagSession = GetNamedProperty(env, obj, "remoteTagService");
    if (remoteTagSession) {
        if (ParseTagSession(env, remoteTagSession, tagSession) == nullptr) {
            InfoLog("parse tagSession failed");
            return nullptr;
        }
    }
    InfoLog("taginfo parse finished.");
    return std::make_shared<TagInfo>(tagTechList, tagTechExtrasData, tagUid, tagRfDiscId, tagSession);
}

void RegisterTagA(NapiNfcATag *nfcATag, std::shared_ptr<TagInfo> nfcATaginfo)
{
    std::shared_ptr<NfcATag> nfcATagPtr = NfcATag::GetTag(nfcATaginfo);
    if (nfcATagPtr == nullptr) {
        InfoLog("Get NfcA Tag failed");
        return;
    } else {
        NfcNapiTagContext instance = NfcNapiTagContext::GetInstance();
        instance.Register(nfcATag, nfcATagPtr);
    }
}

template<typename T, typename D>
napi_value JS_Constructor(napi_env env, napi_callback_info cbinfo)
{
    InfoLog("nfcTag JS_Constructor");
    std::shared_ptr<TagInfo> nfcATaginfo;
    // nfcTag is defined as a native instance that will be wrapped in the JS object
    T *nfcTag = new T();
    size_t argc = 1;
    napi_value argv[] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr));
    // check parameter number
    if (argc == static_cast<size_t>(JS_ARGV_INDEX::ARGV_INDEX_1)) {
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[static_cast<size_t>(JS_ARGV_INDEX::ARGV_INDEX_0)], &valueType));
        // check parameter data type
        if (valueType == napi_object) {
            // parse Taginfo parameters passed from JS
            nfcATaginfo = ParseTagInfo(env, argv[static_cast<size_t>(JS_ARGV_INDEX::ARGV_INDEX_0)]);
            if (nfcATaginfo) {
                InfoLog("taginfo parse succeed.");
                RegisterTagA(nfcTag, nfcATaginfo);
            } else {
                InfoLog("taginfo parse failed.");
                return nullptr;
            }
        } else {
            InfoLog("invalid data type!");
            return nullptr;
        }
    } else {
        InfoLog("Invalid number of arguments");
        return nullptr;
    }
    // wrap  data into thisVar
    napi_wrap(
        env, thisVar, nfcTag,
        [](napi_env env, void *data, void *hint) {
            if (data) {
                D *nfcTag = (D *)data;
                delete nfcTag;
            }
        },
        nullptr, nullptr);
    InfoLog("thisVar %{public}p", thisVar);
    return thisVar;
}

napi_value RegisternfcATagObject(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getSak", NapiNfcATag::GetSak),
        DECLARE_NAPI_FUNCTION("getAtqa", NapiNfcATag::GetAtqa),
        DECLARE_NAPI_FUNCTION("connectTag", NapiNfcTagSession::ConnectTag),
        DECLARE_NAPI_FUNCTION("reset", NapiNfcTagSession::Reset),
        DECLARE_NAPI_FUNCTION("isTagConnected", NapiNfcTagSession::IsTagConnected),
        DECLARE_NAPI_FUNCTION("getMaxSendLength", NapiNfcTagSession::GetMaxSendLength),
    };
    // define JS class NfcATag, JS_Constructor is the callback function that handles constructing instances of the class
    NAPI_CALL(env,
        napi_define_class(env, "NfcATag", NAPI_AUTO_LENGTH, JS_Constructor<NapiNfcATag, NfcATag>, nullptr,
            sizeof(desc) / sizeof(desc[0]), desc, &nfcATagObject));
    return exports;
}

napi_value GetNfcATag(napi_env env, napi_callback_info info)
{
    InfoLog("nfcTag GetNfcATag begin");
    std::size_t argc = 1;
    napi_value argv[] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    napi_value result = nullptr;
    // new instance of JS object NfcATag, call RegisternfcATagObject()
    NAPI_CALL(env, napi_new_instance(env, nfcATagObject, argc, argv, &result));
    return result;
}

static napi_value InitJs(napi_env env, napi_value exports)
{
    InfoLog("Init, nfc_napi_tag");
    // register NfcA tag object
    RegisternfcATagObject(env, exports);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getNfcATag", GetNfcATag),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(napi_property_descriptor), desc));
    return exports;
}

static napi_module nfcTagModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = InitJs,
    .nm_modname = "nfc.tag",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterNfcTagModule(void)
{
    napi_module_register(&nfcTagModule);
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS
