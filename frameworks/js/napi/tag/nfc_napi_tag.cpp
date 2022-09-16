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
#include "napi_remote_object.h"
#include "nfc_sdk_common.h"

namespace OHOS {
namespace NFC {
namespace KITS {
const std::string VAR_UID = "uid";
const std::string VAR_TECH = "technology";
const std::string VAR_EXTRA = "extrasData";
const std::string VAR_RF_ID = "tagRfDiscId";
const std::string VAR_SERVICE = "remoteTagService";
const std::string VAR_PROFILES = "supportedProfiles";

thread_local napi_ref nfcAConsRef_;
thread_local napi_ref nfcBConsRef_;
thread_local napi_ref nfcFConsRef_;
thread_local napi_ref nfcVConsRef_; // iso15693
thread_local napi_ref isoDepConsRef_;
thread_local napi_ref ndefConsRef_;
thread_local napi_ref mifareClassicConsRef_;
thread_local napi_ref mifareUltralightConsRef_;
thread_local napi_ref ndefFormatableConsRef_;
std::shared_ptr<TagInfo> nfcTaginfo;
const int INIT_REF = 1;

napi_value ParseTagTechArray(napi_env env, napi_value obj, std::vector<int> &typeArray)
{
    const int32_t ERROR_DEFAULT = -1;
    bool result = false;
    napi_status status = napi_is_array(env, obj, &result);
    if (status != napi_ok || !result) {
        ErrorLog("Invalid input parameter type!");
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
            DebugLog("tag tech array :%{public}d is %{public}d ", i, element);
        } else {
            ErrorLog("Invalid parameter type of array element!");
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
        DebugLog("SetPacMap keystr :%{public}s", valueString.c_str());
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
        ErrorLog("SetPacMapObject pacMap type error");
    }
}

void AnalysisPacMap(std::shared_ptr<AppExecFwk::PacMap> &pacMap, const napi_env &env, const napi_value &arg)
{
    DebugLog("AnalysisPacMap begin");
    napi_value keys = 0;
    napi_get_property_names(env, arg, &keys);
    uint32_t arrLen = 0;
    napi_status status = napi_get_array_length(env, keys, &arrLen);
    if (status != napi_ok) {
        ErrorLog("AnalysisPacMap errr");
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
        DebugLog("PacMap parse begin");
        AnalysisPacMap(tagTechExtrasData, env, obj);
    } else {
        ErrorLog("ParseExtrasData wrong arg!");
        return nullptr;
    }
    return CreateUndefined(env);
}

napi_value ParseTechAndExtraFromJsTagInfo(napi_env env, napi_value obj,
    std::vector<int> &tagTechList, std::vector<AppExecFwk::PacMap> &tagTechExtras)
{
    // prase tech and extras data from TagInfo Js Object from app.
    napi_value technologies = GetNamedProperty(env, obj, VAR_TECH);
    napi_value extras = GetNamedProperty(env, obj, VAR_EXTRA);

    bool result = false;
    napi_status status = napi_is_array(env, technologies, &result);
    if (status != napi_ok || !result) {
        ErrorLog("ParseTechAndExtraFromJsTagInfo, not array");
        return nullptr;
    }

    napi_value techValue = nullptr;
    napi_value extraValue = nullptr;
    napi_value extraKeyValue = nullptr;
    int32_t intTech = 0;
    uint32_t arrayLength = 0;
    NAPI_CALL(env, napi_get_array_length(env, technologies, &arrayLength));
    for (uint32_t i = 0; i < arrayLength; ++i) {
        NAPI_CALL(env, napi_get_element(env, technologies, i, &techValue));
        NAPI_CALL(env, napi_get_element(env, extras, i, &extraValue));
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, techValue, &valueType);
        if (valueType != napi_number) {
            ErrorLog("ParseTechAndExtraFromJsTagInfo, not number!");
            continue;
        }
        NAPI_CALL(env, napi_get_value_int32(env, techValue, &intTech));
        tagTechList.push_back(intTech);
        DebugLog("parsed tag tech array :%{public}d is %{public}d ", i, intTech);

        // parse extra data for this technology
        AppExecFwk::PacMap pacMap;
        if (intTech == static_cast<int>(TagTechnology::NFC_A_TECH)) {
            // for NFCA, parse extra SAK and ATQA
            napi_get_named_property(env, extraValue, KITS::TagInfo::SAK, &extraKeyValue);
            int32_t sak = 0;
            ParseInt32(env, sak, extraKeyValue);
            pacMap.PutIntValue(KITS::TagInfo::SAK, sak);

            napi_get_named_property(env, extraValue, KITS::TagInfo::ATQA, &extraKeyValue);
            std::string atqa = "";
            ParseString(env, atqa, extraKeyValue);
            pacMap.PutStringValue(KITS::TagInfo::ATQA, atqa);
        } else if (intTech == static_cast<int>(TagTechnology::NFC_ISODEP_TECH)) {
            // for ISODEP, parse extra HistoryBytes and HilayerResponse
            napi_get_named_property(env, extraValue, KITS::TagInfo::HISTORICAL_BYTES, &extraKeyValue);
            std::string historyByets = "";
            ParseString(env, historyByets, extraKeyValue);
            pacMap.PutStringValue(KITS::TagInfo::HISTORICAL_BYTES, historyByets);

            napi_get_named_property(env, extraValue, KITS::TagInfo::HILAYER_RESPONSE, &extraKeyValue);
            std::string hilyerResp = "";
            ParseString(env, hilyerResp, extraKeyValue);
            pacMap.PutStringValue(KITS::TagInfo::HILAYER_RESPONSE, hilyerResp);
        }
        tagTechExtras.push_back(pacMap);
    }
    return CreateUndefined(env);
}

std::shared_ptr<TagInfo> BuildNativeTagFromJsObj(napi_env env, napi_value obj)
{
    // parse uid: string from TagInfo object.
    std::string tagUid = GetNapiStringValue(env, obj, VAR_UID);
    DebugLog("BuildNativeTagFromJsObj, tag uid:%{public}s", tagUid.c_str());

    // parse technology: number[], extrasData: PacMap[] from TagInfo object.
    std::vector<int> tagTechList;
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    ParseTechAndExtraFromJsTagInfo(env, obj, tagTechList, tagTechExtras);
    DebugLog("BuildNativeTagFromJsObj, tech size %{public}zu, extra size %{public}zu", tagTechList.size(), tagTechExtras.size());

    // parse tagRfDiscId: number from TagInfo object.
    int tagRfDiscId = GetNapiInt32Value(env, obj, VAR_RF_ID);
    DebugLog("BuildNativeTagFromJsObj, tag RfDiscId:%{public}d", tagRfDiscId);

    // parse remoteTagService from TagInfo object.
    napi_value remoteTagSession = GetNamedProperty(env, obj, VAR_SERVICE);
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, remoteTagSession, &valueType);
    if (remoteTagSession == nullptr || valueType != napi_object) {
        ErrorLog("BuildNativeTagFromJsObj, invalid remoteTagService");
        return nullptr;
    }
    OHOS::sptr<IRemoteObject> remoteObject = nullptr;
    remoteObject = NAPI_ohos_rpc_getNativeRemoteObject(env, remoteTagSession);
    if (remoteObject == nullptr) {
        WarnLog("BuildNativeTagFromJsObj, prased remoteObject is nullptr.");
    }
    DebugLog("BuildNativeTagFromJsObj, taginfo parse finished.");
    return std::make_shared<TagInfo>(tagTechList, tagTechExtras, tagUid, tagRfDiscId, remoteObject);
}

template<typename T>
bool RegisterTag(NapiNfcTagSession *nfcTag, std::shared_ptr<TagInfo> nfcTaginfo)
{
    nfcTag->tagSession = T::GetTag(nfcTaginfo);
    return nfcTag->tagSession != nullptr ? true : false;
}

template<typename T, typename D>
napi_value JS_Constructor(napi_env env, napi_callback_info cbinfo)
{
    DebugLog("nfcTag JS_Constructor");
    // nfcTag is defined as a native instance that will be wrapped in the JS object
    NapiNfcTagSession *nfcTag = new T();
    size_t argc = 1;
    napi_value argv[1] = {0};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr));
    // check parameter number
    if (argc != static_cast<size_t>(JS_ARGV_INDEX::ARGV_INDEX_1)) {
        ErrorLog("Invalid number of arguments");
        return nullptr;
    }
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, argv[static_cast<size_t>(JS_ARGV_INDEX::ARGV_INDEX_0)], &valueType));
    // check parameter data type
    if (valueType != napi_object) {
        ErrorLog("invalid data type!");
        return nullptr;
    }
    // parse Taginfo parameters passed from JS
    nfcTaginfo = BuildNativeTagFromJsObj(env, argv[static_cast<size_t>(JS_ARGV_INDEX::ARGV_INDEX_0)]);
    if (nfcTaginfo == nullptr) {
        ErrorLog("taginfo parse failed.");
        return nullptr;
    }
    if (!RegisterTag<D>(nfcTag, nfcTaginfo)) {
        ErrorLog("Get Nfc Tag failed");
        return nullptr;
    }
    // wrap  data into thisVar
    napi_status status = napi_wrap(
        env, thisVar, nfcTag,
        [](napi_env env, void *data, void *hint) {
            if (data) {
                T *nfcTag = (T *)data;
                delete nfcTag;
            }
        },
        nullptr, nullptr);
    NAPI_ASSERT(env, status == napi_ok, "failed to get objectInfo");
    return thisVar;
}

napi_status InitNfcForumType(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "NFC_FORUM_TYPE_1 ", GetNapiValue(env, static_cast<int32_t>(NdefTag::EmNfcForumType::NFC_FORUM_TYPE_1))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "NFC_FORUM_TYPE_2 ", GetNapiValue(env, static_cast<int32_t>(NdefTag::EmNfcForumType::NFC_FORUM_TYPE_2))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "NFC_FORUM_TYPE_3 ", GetNapiValue(env, static_cast<int32_t>(NdefTag::EmNfcForumType::NFC_FORUM_TYPE_3))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "NFC_FORUM_TYPE_4 ", GetNapiValue(env, static_cast<int32_t>(NdefTag::EmNfcForumType::NFC_FORUM_TYPE_4))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "MIFARE_CLASSIC  ", GetNapiValue(env, static_cast<int32_t>(NdefTag::EmNfcForumType::MIFARE_CLASSIC))),
    };

    constexpr size_t arrSize = sizeof(desc) / sizeof(desc[0]);
    DefineEnumClassByName(env, exports, "NfcForumType ", arrSize, desc);
    return napi_define_properties(env, exports, arrSize, desc);
}

napi_status InitMifareClassicType(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_UNKOWN", GetNapiValue(env, static_cast<int32_t>(MifareClassicTag::EmMifareTagType::TYPE_UNKNOWN))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_CLASSIC", GetNapiValue(env, static_cast<int32_t>(MifareClassicTag::EmMifareTagType::TYPE_CLASSIC))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_PLUS", GetNapiValue(env, static_cast<int32_t>(MifareClassicTag::EmMifareTagType::TYPE_PLUS))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "TYPE_PRO", GetNapiValue(env, static_cast<int32_t>(MifareClassicTag::EmMifareTagType::TYPE_PRO))),
    };

    constexpr size_t arrSize = sizeof(desc) / sizeof(desc[0]);
    DefineEnumClassByName(env, exports, "MifareUltralightType", arrSize, desc);
    return napi_define_properties(env, exports, arrSize, desc);
}

napi_status InitMifareUltralightType(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_UNKOWN",
            GetNapiValue(env, static_cast<int32_t>(MifareUltralightTag::EmMifareUltralightType::TYPE_UNKOWN))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_ULTRALIGHT",
            GetNapiValue(env, static_cast<int32_t>(MifareUltralightTag::EmMifareUltralightType::TYPE_ULTRALIGHT))),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_ULTRALIGHT_C",
            GetNapiValue(env, static_cast<int32_t>(MifareUltralightTag::EmMifareUltralightType::TYPE_ULTRALIGHT_C))),
    };

    constexpr size_t arrSize = sizeof(desc) / sizeof(desc[0]);
    DefineEnumClassByName(env, exports, "MifareUltralightType", arrSize, desc);
    return napi_define_properties(env, exports, arrSize, desc);
}

void RegisterNfcAJSClass(napi_env env)
{
    DebugLog("DefineNfcAJSClass begin");
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getSak", NapiNfcATag::GetSak),
        DECLARE_NAPI_FUNCTION("getAtqa", NapiNfcATag::GetAtqa),
        DECLARE_NAPI_FUNCTION("connectTag", NapiNfcTagSession::ConnectTag),
        DECLARE_NAPI_FUNCTION("reset", NapiNfcTagSession::Reset),
        DECLARE_NAPI_FUNCTION("isTagConnected", NapiNfcTagSession::IsTagConnected),
        DECLARE_NAPI_FUNCTION("getMaxSendLength", NapiNfcTagSession::GetMaxSendLength),
    };

    // define JS class NfcATag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "NfcATag", NAPI_AUTO_LENGTH, JS_Constructor<NapiNfcATag, NfcATag>, nullptr,
        sizeof(desc) / sizeof(desc[0]), desc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &nfcAConsRef_);
    DebugLog("DefineNfcAJSClass end");
}

void RegisterNfcBJSClass(napi_env env)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getRespAppData", NapiNfcBTag::GetRespAppData),
        DECLARE_NAPI_FUNCTION("getRespProtocol", NapiNfcBTag::GetRespProtocol),
        DECLARE_NAPI_FUNCTION("connectTag", NapiNfcTagSession::ConnectTag),
        DECLARE_NAPI_FUNCTION("reset", NapiNfcTagSession::Reset),
        DECLARE_NAPI_FUNCTION("isTagConnected", NapiNfcTagSession::IsTagConnected),
        DECLARE_NAPI_FUNCTION("getMaxSendLength", NapiNfcTagSession::GetMaxSendLength),
    };
    // define JS class NfcBTag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "NfcBTag", NAPI_AUTO_LENGTH, JS_Constructor<NapiNfcBTag, NfcBTag>, nullptr,
        sizeof(desc) / sizeof(desc[0]), desc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &nfcBConsRef_);
}

void RegisterNfcFJSClass(napi_env env)
{
    DebugLog("Register RegisterNfcFJSClass begin");
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getSystemCode", NapiNfcFTag::GetSystemCode),
        DECLARE_NAPI_FUNCTION("getPmm", NapiNfcFTag::GetPmm),
        DECLARE_NAPI_FUNCTION("connectTag", NapiNfcTagSession::ConnectTag),
        DECLARE_NAPI_FUNCTION("reset", NapiNfcTagSession::Reset),
        DECLARE_NAPI_FUNCTION("isTagConnected", NapiNfcTagSession::IsTagConnected),
        DECLARE_NAPI_FUNCTION("getMaxSendLength", NapiNfcTagSession::GetMaxSendLength),
    };
    // define JS class NfcFTag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "NfcFTag", NAPI_AUTO_LENGTH, JS_Constructor<NapiNfcFTag, NfcFTag>, nullptr,
        sizeof(desc) / sizeof(desc[0]), desc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &nfcFConsRef_);
}

void RegisterNfcVJSClass(napi_env env)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getResponseFlags", NapiNfcVTag::GetResponseFlags),
        DECLARE_NAPI_FUNCTION("getDsfId", NapiNfcVTag::GetDsfId),
        DECLARE_NAPI_FUNCTION("connectTag", NapiNfcTagSession::ConnectTag),
        DECLARE_NAPI_FUNCTION("reset", NapiNfcTagSession::Reset),
        DECLARE_NAPI_FUNCTION("isTagConnected", NapiNfcTagSession::IsTagConnected),
        DECLARE_NAPI_FUNCTION("getMaxSendLength", NapiNfcTagSession::GetMaxSendLength),
    };
    // define JS class NfcVTag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "NfcVTag", NAPI_AUTO_LENGTH, JS_Constructor<NapiNfcVTag, Iso15693Tag>, nullptr,
        sizeof(desc) / sizeof(desc[0]), desc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &nfcVConsRef_);
}

void RegisterIsoDepJSClass(napi_env env)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getHistoricalBytes", NapiIsoDepTag::GetHistoricalBytes),
        DECLARE_NAPI_FUNCTION("getHiLayerResponse", NapiIsoDepTag::GetHiLayerResponse),
        DECLARE_NAPI_FUNCTION("isExtendedApduSupported", NapiIsoDepTag::IsExtendedApduSupported),
        DECLARE_NAPI_FUNCTION("connectTag", NapiNfcTagSession::ConnectTag),
        DECLARE_NAPI_FUNCTION("reset", NapiNfcTagSession::Reset),
        DECLARE_NAPI_FUNCTION("isTagConnected", NapiNfcTagSession::IsTagConnected),
        DECLARE_NAPI_FUNCTION("getMaxSendLength", NapiNfcTagSession::GetMaxSendLength),
    };
    // define JS class IsoDepTag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "IsoDepTag", NAPI_AUTO_LENGTH, JS_Constructor<NapiIsoDepTag, IsoDepTag>, nullptr,
        sizeof(desc) / sizeof(desc[0]), desc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &isoDepConsRef_);
}

napi_value RegisterNdefJSClass(napi_env env, napi_value exports)
{
    // register NdefMessage object
    NapiNdefTag::RegisterNdefMessageObject(env, exports);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("createNdefMessage", NapiNdefTag::CreateNdefMessage),
        DECLARE_NAPI_FUNCTION("getNdefTagType", NapiNdefTag::GetNdefTagType),
        DECLARE_NAPI_FUNCTION("getNdefMessage", NapiNdefTag::GetNdefMessage),
        DECLARE_NAPI_FUNCTION("isNdefWritable", NapiNdefTag::IsNdefWritable),
        DECLARE_NAPI_FUNCTION("readNdef", NapiNdefTag::ReadNdef),
        DECLARE_NAPI_FUNCTION("writeNdef", NapiNdefTag::WriteNdef),
        DECLARE_NAPI_FUNCTION("canSetReadOnly", NapiNdefTag::CanSetReadOnly),
        DECLARE_NAPI_FUNCTION("setReadOnly", NapiNdefTag::SetReadOnly),
        DECLARE_NAPI_FUNCTION("getNdefTagTypeString", NapiNdefTag::GetNdefTagTypeString),
    };

    NAPI_CALL(env, InitNfcForumType(env, exports));

    // define JS class NdefTag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "NdefTag", NAPI_AUTO_LENGTH, JS_Constructor<NapiNdefTag, NdefTag>, nullptr,
        sizeof(desc) / sizeof(desc[0]), desc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &ndefConsRef_);
    return exports;
}

napi_value RegisterMifareClassicJSClass(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("authenticateSector", NapiMifareClassicTag::AuthenticateSector),
        DECLARE_NAPI_FUNCTION("readSingleBlock", NapiMifareClassicTag::ReadSingleBlock),
        DECLARE_NAPI_FUNCTION("writeSingleBlock", NapiMifareClassicTag::WriteSingleBlock),
        DECLARE_NAPI_FUNCTION("incrementBlock", NapiMifareClassicTag::IncrementBlock),
        DECLARE_NAPI_FUNCTION("decrementBlock", NapiMifareClassicTag::DecrementBlock),
        DECLARE_NAPI_FUNCTION("transferToBlock", NapiMifareClassicTag::TransferToBlock),
        DECLARE_NAPI_FUNCTION("restoreFromBlock", NapiMifareClassicTag::RestoreFromBlock),
        DECLARE_NAPI_FUNCTION("getSectorCount", NapiMifareClassicTag::GetSectorCount),
        DECLARE_NAPI_FUNCTION("getBlockCountInSector", NapiMifareClassicTag::GetBlockCountInSector),
        DECLARE_NAPI_FUNCTION("getType", NapiMifareClassicTag::GetType),
        DECLARE_NAPI_FUNCTION("getTagSize", NapiMifareClassicTag::GetTagSize),
        DECLARE_NAPI_FUNCTION("isEmulatedTag", NapiMifareClassicTag::IsEmulatedTag),
        DECLARE_NAPI_FUNCTION("getBlockIndex", NapiMifareClassicTag::GetBlockIndex),
        DECLARE_NAPI_FUNCTION("getSectorIndex", NapiMifareClassicTag::GetSectorIndex),
        DECLARE_NAPI_FUNCTION("reset", NapiNfcTagSession::Reset),
        DECLARE_NAPI_FUNCTION("isTagConnected", NapiNfcTagSession::IsTagConnected),
        DECLARE_NAPI_FUNCTION("getMaxSendLength", NapiNfcTagSession::GetMaxSendLength),
    };

    NAPI_CALL(env, InitMifareClassicType(env, exports));

    // define JS class MifareClassicTag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "MifareClassicTag", NAPI_AUTO_LENGTH,
        JS_Constructor<NapiMifareClassicTag, MifareClassicTag>, nullptr, sizeof(desc) / sizeof(desc[0]), desc,
        &constructor);
    napi_create_reference(env, constructor, INIT_REF, &mifareClassicConsRef_);
    return exports;
}

napi_value RegisterMifareUltralightJSClass(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("readMultiplePages", NapiMifareUltralightTag::ReadMultiplePages),
        DECLARE_NAPI_FUNCTION("writeSinglePages", NapiMifareUltralightTag::WriteSinglePages),
        DECLARE_NAPI_FUNCTION("getType", NapiMifareUltralightTag::GetType),
        DECLARE_NAPI_FUNCTION("reset", NapiNfcTagSession::Reset),
        DECLARE_NAPI_FUNCTION("isTagConnected", NapiNfcTagSession::IsTagConnected),
        DECLARE_NAPI_FUNCTION("getMaxSendLength", NapiNfcTagSession::GetMaxSendLength),
    };

    NAPI_CALL(env, InitMifareUltralightType(env, exports));

    // define JS class MifareUltralightTag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "MifareUltralightTag", NAPI_AUTO_LENGTH,
        JS_Constructor<NapiMifareUltralightTag, MifareUltralightTag>, nullptr, sizeof(desc) / sizeof(desc[0]),
        desc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &mifareUltralightConsRef_);
    return exports;
}

napi_value RegisterNdefFormatableJSClass(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("format", NapiNdefFormatableTag::Format),
        DECLARE_NAPI_FUNCTION("formatReadOnly", NapiNdefFormatableTag::FormatReadOnly),
        DECLARE_NAPI_FUNCTION("reset", NapiNfcTagSession::Reset),
        DECLARE_NAPI_FUNCTION("isTagConnected", NapiNfcTagSession::IsTagConnected),
        DECLARE_NAPI_FUNCTION("getMaxSendLength", NapiNfcTagSession::GetMaxSendLength),
    };

    NAPI_CALL(env, InitMifareUltralightType(env, exports));

    // define JS class NdefFormatableTag , JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "NdefFormatableTag ", NAPI_AUTO_LENGTH,
        JS_Constructor<NapiNdefFormatableTag, NdefFormatableTag>, nullptr, sizeof(desc) / sizeof(desc[0]),
        desc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &ndefFormatableConsRef_);
    return exports;
}

napi_value GetSpecificTagObj(napi_env env, napi_callback_info info, napi_ref ref)
{
    if (ref == nullptr) {
        ErrorLog("GetSpecificTagObj error ref");
        return nullptr;
    }
    std::size_t argc = 1;
    napi_value argv[1] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[0], &valueType);
    if (argv[0] == nullptr || valueType != napi_object) {
        ErrorLog("GetSpecificTagObj error argv type");
        return nullptr;
    }

    // new instance of JS object NfcATag
    DebugLog("GetSpecificTagObj napi_get_reference_value start");
    napi_value result;
    napi_value constructor = nullptr;
    napi_get_reference_value(env, ref, &constructor);
    DebugLog("GetSpecificTagObj napi_get_reference_value end");
    napi_new_instance(env, constructor, argc, argv, &result);
    DebugLog("GetSpecificTagObj napi_new_instance end");
    return result;
}

napi_value GetNfcATag(napi_env env, napi_callback_info info)
{
    DebugLog("GetNfcATag begin");
    return GetSpecificTagObj(env, info, nfcAConsRef_);
}

napi_value GetNfcBTag(napi_env env, napi_callback_info info)
{
    DebugLog("nfcTag GetNfcBTag begin");
    return GetSpecificTagObj(env, info, nfcBConsRef_);
}

napi_value GetNfcFTag(napi_env env, napi_callback_info info)
{
    DebugLog("nfcFag GetNfcFTag begin");
    return GetSpecificTagObj(env, info, nfcFConsRef_);
}

napi_value GetNfcVTag(napi_env env, napi_callback_info info)
{
    DebugLog("nfcTag GetNfcVTag begin");
    return GetSpecificTagObj(env, info, nfcVConsRef_);
}

napi_value GetIsoDepTag(napi_env env, napi_callback_info info)
{
    DebugLog("nfcTag GetIsoDepTag begin");
    return GetSpecificTagObj(env, info, isoDepConsRef_);
}

napi_value GetNdefTag(napi_env env, napi_callback_info info)
{
    DebugLog("nfcTag GetNdefTag begin");
    return GetSpecificTagObj(env, info, ndefConsRef_);
}

napi_value GetMifareClassicTag(napi_env env, napi_callback_info info)
{
    DebugLog("nfcTag GetMifareClassicTag begin");
    return GetSpecificTagObj(env, info, mifareClassicConsRef_);
}

napi_value GetMifareUltralightTag(napi_env env, napi_callback_info info)
{
    DebugLog("nfcTag GetMifareUltralightTag begin");
    return GetSpecificTagObj(env, info, mifareUltralightConsRef_);
}

napi_value GetNdefFormatableTag(napi_env env, napi_callback_info info)
{
    DebugLog("nfcTag GetNdefFormatableTag begin");
    return GetSpecificTagObj(env, info, ndefFormatableConsRef_);
}

void BuildTagTechAndExtraData(napi_env env, napi_value &parameters, napi_value &tagInfoObj)
{
    napi_value propValue = nullptr;
    propValue = nullptr;
    bool isArray = false;
    napi_get_named_property(env, parameters, VAR_TECH.c_str(), &propValue);
    if (propValue == nullptr || napi_is_array(env, propValue, &isArray) != napi_ok || !isArray) {
        ErrorLog("BuildTagFromWantParams for technology error");
        return;
    }

    uint32_t length = 0;
    napi_get_array_length(env, propValue, &length);
    napi_value technologies = propValue;
    napi_set_named_property(env, tagInfoObj, VAR_TECH.c_str(), technologies);
    DebugLog("BuildTagFromWantParams for technology length %{public}d", length);

    // parse extras data for each technology
    napi_value extrasData;
    napi_create_array_with_length(env, length, &extrasData);
    for (uint32_t i = 0; i < length; i++) {
        propValue = nullptr;
        napi_get_element(env, technologies, i, &propValue);
        int32_t technology = 0;
        ParseInt32(env, technology, propValue);
        DebugLog("BuildTagFromWantParams extra for %{public}d", technology);

        napi_value eachElement;
        napi_create_object(env, &eachElement);
        if (technology == static_cast<int>(TagTechnology::NFC_A_TECH)) {
            // parse sak of nfca
            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::SAK, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::SAK, propValue);

            int32_t sak = 0;
            ParseInt32(env, sak, propValue);
            DebugLog("BuildTagFromWantParams sak %{public}x", sak);

            // parse atqa of nfca
            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::ATQA, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::ATQA, propValue);

            std::string atqa = "";
            ParseString(env, atqa, propValue);
            DebugLog("BuildTagFromWantParams atqa %{public}s", atqa.c_str());
        } else if (technology == static_cast<int>(TagTechnology::NFC_ISODEP_TECH)) {
            // parse history bytes of isodep.
            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::HISTORICAL_BYTES, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::HISTORICAL_BYTES, propValue);

            std::string historyBytes = "";
            ParseString(env, historyBytes, propValue);
            DebugLog("BuildTagFromWantParams historyBytes %{public}s", historyBytes.c_str());

            // parse hilayer response of isodep.
            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::HILAYER_RESPONSE, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::HILAYER_RESPONSE, propValue);

            std::string hilayerResp = "";
            ParseString(env, hilayerResp, propValue);
            DebugLog("BuildTagFromWantParams hilayerResp %{public}s", hilayerResp.c_str());
        } else {
            continue;
        }
        napi_set_element(env, extrasData, i, eachElement);
    }
    napi_set_named_property(env, tagInfoObj, VAR_EXTRA.c_str(), extrasData);
}

napi_value BuildTagFromWantParams(napi_env env, napi_value &parameters)
{
    // the parameters is from Want JS object, parse it to build the TagInfo JS Object.
    DebugLog("BuildTagFromWantParams begin");
    napi_value tagInfoObj = nullptr;
    napi_create_object(env, &tagInfoObj);

    napi_value propValue = nullptr;
    napi_valuetype valueType = napi_undefined;
    napi_get_named_property(env, parameters, VAR_UID.c_str(), &propValue);
    napi_typeof(env, propValue, &valueType);
    if (propValue != nullptr && valueType == napi_string) {
        napi_set_named_property(env, tagInfoObj, VAR_UID.c_str(), propValue);
        DebugLog("BuildTagFromWantParams for uid");
    }

    BuildTagTechAndExtraData(env, parameters, tagInfoObj);

    propValue = nullptr;
    valueType = napi_undefined;
    napi_get_named_property(env, parameters, VAR_RF_ID.c_str(), &propValue);
    napi_typeof(env, propValue, &valueType);
    if (propValue != nullptr && valueType == napi_number) {
        napi_set_named_property(env, tagInfoObj, VAR_RF_ID.c_str(), propValue);
        DebugLog("BuildTagFromWantParams for tagRfDiscId");
    }

    propValue = nullptr;
    valueType = napi_undefined;
    napi_get_named_property(env, parameters, VAR_SERVICE.c_str(), &propValue);
    napi_typeof(env, propValue, &valueType);
    if (propValue != nullptr && valueType == napi_object) {
        napi_set_named_property(env, tagInfoObj, VAR_SERVICE.c_str(), propValue);
        DebugLog("BuildTagFromWantParams for remoteTagService");
    }
    DebugLog("BuildTagFromWantParams end");
    return tagInfoObj;
}

napi_value GetTagInfo(napi_env env, napi_callback_info info)
{
    DebugLog("nfc_napi_tag::GetTagInfo start");
    // has only one arg, want: Want
    std::size_t argc = 1;
    napi_value argv[1] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, argv[0], &valueType);

    if (argv[0] == nullptr || valueType != napi_object) {
        ErrorLog("nfc_napi_tag::GetTagInfo error argv type");
        return nullptr;
    }

    // Get parameters?: {[key: string]: any} from want.
    napi_value want = argv[0];
    napi_value parameters = nullptr;
    valueType = napi_undefined;
    napi_create_object(env, &parameters);
    napi_get_named_property(env, want, "parameters", &parameters);
    napi_typeof(env, parameters, &valueType);
    if (parameters == nullptr || valueType != napi_object) {
        ErrorLog("nfc_napi_tag::GetTagInfo no parameters");
        return nullptr;
    }

    napi_value tagInfoObj = BuildTagFromWantParams(env, parameters);
    DebugLog("nfc_napi_tag::GetTagInfo end");
    return tagInfoObj;
}

static napi_value InitJs(napi_env env, napi_value exports)
{
    DebugLog("Init, nfc_napi_tag");
    // register all napi class for tag types.
    RegisterNfcAJSClass(env);
    RegisterNfcBJSClass(env);
    RegisterNfcFJSClass(env);
    RegisterNfcVJSClass(env);
    RegisterIsoDepJSClass(env);
    RegisterNdefJSClass(env, exports);
    RegisterMifareClassicJSClass(env, exports);
    RegisterMifareUltralightJSClass(env, exports);
    RegisterNdefFormatableJSClass(env, exports);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getNfcATag", GetNfcATag),
        DECLARE_NAPI_FUNCTION("getNfcBTag", GetNfcBTag),
        DECLARE_NAPI_FUNCTION("getNfcFTag", GetNfcFTag),
        DECLARE_NAPI_FUNCTION("getNfcVTag", GetNfcVTag),
        DECLARE_NAPI_FUNCTION("getIsoDepTag", GetIsoDepTag),
        DECLARE_NAPI_FUNCTION("getNdefTag", GetNdefTag),
        DECLARE_NAPI_FUNCTION("getMifareClassicTag", GetMifareClassicTag),
        DECLARE_NAPI_FUNCTION("getMifareUltralightTag", GetMifareUltralightTag),
        DECLARE_NAPI_FUNCTION("getNdefFormatableTag", GetNdefFormatableTag),
        DECLARE_NAPI_FUNCTION("getTagInfo", GetTagInfo),
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
