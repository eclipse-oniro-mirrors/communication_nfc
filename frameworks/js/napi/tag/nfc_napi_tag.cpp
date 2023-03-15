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
#include "ndef_message.h"
#include "ndef_tag.h"
#include "nfc_sdk_common.h"
#include "mifare_classic_tag.h"
#include "mifare_ultralight_tag.h"

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

static napi_value EnumConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisArg = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);
    napi_value global = nullptr;
    napi_get_global(env, &global);
    return thisArg;
}

static napi_value RegisterEnumTnfType(napi_env env, napi_value exports)
{
    napi_value tnfEmpty = nullptr;
    napi_value tnfKnown = nullptr;
    napi_value tnfMedia = nullptr;
    napi_value tnfUri = nullptr;
    napi_value tnfExtApp = nullptr;
    napi_value tnfUnknown = nullptr;
    napi_value tnfUnchanged = nullptr;
    napi_create_int32(env, static_cast<int32_t>(NdefMessage::EmTnfType::TNF_EMPTY), &tnfEmpty);
    napi_create_int32(env, static_cast<int32_t>(NdefMessage::EmTnfType::TNF_WELL_KNOWN), &tnfKnown);
    napi_create_int32(env, static_cast<int32_t>(NdefMessage::EmTnfType::TNF_MIME_MEDIA), &tnfMedia);
    napi_create_int32(env, static_cast<int32_t>(NdefMessage::EmTnfType::TNF_ABSOLUTE_URI), &tnfUri);
    napi_create_int32(env, static_cast<int32_t>(NdefMessage::EmTnfType::TNF_EXTERNAL_TYPE), &tnfExtApp);
    napi_create_int32(env, static_cast<int32_t>(NdefMessage::EmTnfType::TNF_UNKNOWN), &tnfUnknown);
    napi_create_int32(env, static_cast<int32_t>(NdefMessage::EmTnfType::TNF_UNCHANGED), &tnfUnchanged);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("TNF_EMPTY", tnfEmpty),
        DECLARE_NAPI_STATIC_PROPERTY("TNF_WELL_KNOWN", tnfKnown),
        DECLARE_NAPI_STATIC_PROPERTY("TNF_MEDIA", tnfMedia),
        DECLARE_NAPI_STATIC_PROPERTY("TNF_ABSOLUTE_URI", tnfUri),
        DECLARE_NAPI_STATIC_PROPERTY("TNF_EXT_APP", tnfExtApp),
        DECLARE_NAPI_STATIC_PROPERTY("TNF_UNKNOWN", tnfUnknown),
        DECLARE_NAPI_STATIC_PROPERTY("TNF_UNCHANGED", tnfUnchanged),
    };

    // define "TnfType" enum at @ohos.nfc.tag.d.ts
    napi_value result = nullptr;
    napi_define_class(env, "TnfType", NAPI_AUTO_LENGTH, EnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "TnfType", result);
    return exports;
}

static napi_value RegisterEnumNfcForumType(napi_env env, napi_value exports)
{
    napi_value type1 = nullptr;
    napi_value type2 = nullptr;
    napi_value type3 = nullptr;
    napi_value type4 = nullptr;
    napi_value typeMc = nullptr;
    napi_create_int32(env, static_cast<int32_t>(NdefTag::EmNfcForumType::NFC_FORUM_TYPE_1), &type1);
    napi_create_int32(env, static_cast<int32_t>(NdefTag::EmNfcForumType::NFC_FORUM_TYPE_2), &type2);
    napi_create_int32(env, static_cast<int32_t>(NdefTag::EmNfcForumType::NFC_FORUM_TYPE_3), &type3);
    napi_create_int32(env, static_cast<int32_t>(NdefTag::EmNfcForumType::NFC_FORUM_TYPE_4), &type4);
    napi_create_int32(env, static_cast<int32_t>(NdefTag::EmNfcForumType::MIFARE_CLASSIC), &typeMc);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("NFC_FORUM_TYPE_1", type1),
        DECLARE_NAPI_STATIC_PROPERTY("NFC_FORUM_TYPE_2", type2),
        DECLARE_NAPI_STATIC_PROPERTY("NFC_FORUM_TYPE_3", type3),
        DECLARE_NAPI_STATIC_PROPERTY("NFC_FORUM_TYPE_4", type4),
        DECLARE_NAPI_STATIC_PROPERTY("MIFARE_CLASSIC", typeMc),
    };

    // define "NfcForumType" enum at @ohos.nfc.tag.d.ts
    napi_value result = nullptr;
    napi_define_class(env, "NfcForumType", NAPI_AUTO_LENGTH, EnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "NfcForumType", result);
    return exports;
}

static napi_value RegisterEnumRtdType(napi_env env, napi_value exports)
{
    napi_value rtdText = nullptr;
    std::string hexText = HEX_RTD_TYPE.at(NdefMessage::EmRtdType::RTD_TEXT);
    uint32_t textLen = NfcSdkCommon::GetHexStrBytesLen(hexText);
    napi_create_array_with_length(env, textLen, &rtdText);
    for (uint32_t i = 0; i < textLen; i++) {
        napi_value textByte;
        napi_create_uint32(env, NfcSdkCommon::GetByteFromHexStr(hexText, i), &textByte);
        napi_set_element(env, rtdText, i, textByte);
    }

    napi_value rtdUri = nullptr;
    std::string hexUri = HEX_RTD_TYPE.at(NdefMessage::EmRtdType::RTD_URI);
    uint32_t uriLen = NfcSdkCommon::GetHexStrBytesLen(hexUri);
    napi_create_array_with_length(env, uriLen, &rtdUri);
    for (uint32_t i = 0; i < uriLen; i++) {
        napi_value uriByte;
        napi_create_uint32(env, NfcSdkCommon::GetByteFromHexStr(hexUri, i), &uriByte);
        napi_set_element(env, rtdUri, i, uriByte);
    }

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("RTD_TEXT", rtdText),
        DECLARE_NAPI_STATIC_PROPERTY("RTD_URI", rtdUri),
    };

    // define "RtdType" enum at @ohos.nfc.tag.d.ts
    napi_value result = nullptr;
    napi_define_class(env, "RtdType", NAPI_AUTO_LENGTH, EnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "RtdType", result);
    return exports;
}

static napi_value RegisterEnumMifareClassicType(napi_env env, napi_value exports)
{
    napi_value typeUnknown = nullptr;
    napi_value typeClassic = nullptr;
    napi_value typePlus = nullptr;
    napi_value typePro = nullptr;
    napi_create_int32(env, static_cast<int32_t>(MifareClassicTag::EmType::TYPE_UNKNOWN), &typeUnknown);
    napi_create_int32(env, static_cast<int32_t>(MifareClassicTag::EmType::TYPE_CLASSIC), &typeClassic);
    napi_create_int32(env, static_cast<int32_t>(MifareClassicTag::EmType::TYPE_PLUS), &typePlus);
    napi_create_int32(env, static_cast<int32_t>(MifareClassicTag::EmType::TYPE_PRO), &typePro);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_UNKNOWN", typeUnknown),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_CLASSIC", typeClassic),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_PLUS", typePlus),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_PRO", typePro),
    };

    // define "MifareClassicType" enum at @ohos.nfc.tag.d.ts
    napi_value result = nullptr;
    napi_define_class(env, "MifareClassicType", NAPI_AUTO_LENGTH, EnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "MifareClassicType", result);
    return exports;
}

static napi_value RegisterEnumMifareClassicSize(napi_env env, napi_value exports)
{
    napi_value sizeMini = nullptr;
    napi_value size1K = nullptr;
    napi_value size2K = nullptr;
    napi_value size4K = nullptr;
    napi_create_int32(env, static_cast<int32_t>(MifareClassicTag::MC_SIZE_MINI), &sizeMini);
    napi_create_int32(env, static_cast<int32_t>(MifareClassicTag::MC_SIZE_1K), &size1K);
    napi_create_int32(env, static_cast<int32_t>(MifareClassicTag::MC_SIZE_2K), &size2K);
    napi_create_int32(env, static_cast<int32_t>(MifareClassicTag::MC_SIZE_4K), &size4K);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("MC_SIZE_MINI", sizeMini),
        DECLARE_NAPI_STATIC_PROPERTY("MC_SIZE_1K", size1K),
        DECLARE_NAPI_STATIC_PROPERTY("MC_SIZE_2K", size2K),
        DECLARE_NAPI_STATIC_PROPERTY("MC_SIZE_4K", size4K),
    };

    // define "MifareClassicSize" enum at @ohos.nfc.tag.d.ts
    napi_value result = nullptr;
    napi_define_class(env, "MifareClassicSize", NAPI_AUTO_LENGTH, EnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "MifareClassicSize", result);
    return exports;
}

static napi_value RegisterEnumMifareUlType(napi_env env, napi_value exports)
{
    napi_value typeUnknown = nullptr;
    napi_value typeUl = nullptr;
    napi_value typeUlC = nullptr;
    napi_create_int32(env, static_cast<int32_t>(MifareUltralightTag::EmType::TYPE_UNKNOWN), &typeUnknown);
    napi_create_int32(env, static_cast<int32_t>(MifareUltralightTag::EmType::TYPE_ULTRALIGHT), &typeUl);
    napi_create_int32(env, static_cast<int32_t>(MifareUltralightTag::EmType::TYPE_ULTRALIGHT_C), &typeUlC);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_UNKNOWN", typeUnknown),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_ULTRALIGHT", typeUl),
        DECLARE_NAPI_STATIC_PROPERTY("TYPE_ULTRALIGHT_C", typeUlC),
    };

    // define "MifareUltralightType" enum at @ohos.nfc.tag.d.ts
    napi_value result = nullptr;
    napi_define_class(env, "MifareUltralightType", NAPI_AUTO_LENGTH, EnumConstructor, nullptr,
        sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "MifareUltralightType", result);
    return exports;
}

napi_value ParseTechAndExtraFromJsTagInfo(napi_env env, napi_value obj,
    std::vector<int> &tagTechList, std::vector<AppExecFwk::PacMap> &tagTechExtras)
{
    // prase tech and extras data from TagInfo Js Object from app.
    napi_value technologies = GetNamedProperty(env, obj, VAR_TECH);
    napi_value extras = GetNamedProperty(env, obj, VAR_EXTRA);
    if (!CheckArrayNumberAndThrow(env, technologies, "tagInfo.technology", "number[]")) {
        return CreateUndefined(env);
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
            ErrorLog("ParseTechAndExtraFromJsTagInfo, tech not number!");
            continue;
        }
        NAPI_CALL(env, napi_get_value_int32(env, techValue, &intTech));
        tagTechList.push_back(intTech);
        DebugLog("parsed tech array idx %{public}d, tech is %{public}d ", i, intTech);

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
        } else if (intTech == static_cast<int>(TagTechnology::NFC_B_TECH)) {
            // parse app data and protocol info of nfcb.
            napi_get_named_property(env, extraValue, KITS::TagInfo::APP_DATA, &extraKeyValue);
            std::string appData = "";
            ParseString(env, appData, extraKeyValue);
            pacMap.PutStringValue(KITS::TagInfo::APP_DATA, appData);

            napi_get_named_property(env, extraValue, KITS::TagInfo::PROTOCOL_INFO, &extraKeyValue);
            std::string protocolInfo = "";
            ParseString(env, protocolInfo, extraKeyValue);
            pacMap.PutStringValue(KITS::TagInfo::PROTOCOL_INFO, protocolInfo);
        } else if (intTech == static_cast<int>(TagTechnology::NFC_F_TECH)) {
            // parse pmm and sc of nfcf
            napi_get_named_property(env, extraValue, KITS::TagInfo::NFCF_PMM, &extraKeyValue);
            std::string pmm = "";
            ParseString(env, pmm, extraKeyValue);
            pacMap.PutStringValue(KITS::TagInfo::NFCF_PMM, pmm);

            napi_get_named_property(env, extraValue, KITS::TagInfo::NFCF_SC, &extraKeyValue);
            std::string sysCode = "";
            ParseString(env, sysCode, extraKeyValue);
            pacMap.PutStringValue(KITS::TagInfo::NFCF_SC, sysCode);
        } else if (intTech == static_cast<int>(TagTechnology::NFC_V_TECH)) {
            // parse response flag and dsf id of nfcv.
            napi_get_named_property(env, extraValue, KITS::TagInfo::RESPONSE_FLAGS, &extraKeyValue);
            int32_t respFlag = 0;
            ParseInt32(env, respFlag, extraKeyValue);
            pacMap.PutIntValue(KITS::TagInfo::RESPONSE_FLAGS, respFlag);

            napi_get_named_property(env, extraValue, KITS::TagInfo::DSF_ID, &extraKeyValue);
            int32_t dsfId = 0;
            ParseInt32(env, dsfId, extraKeyValue);
            pacMap.PutIntValue(KITS::TagInfo::DSF_ID, dsfId);
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
        } else if (intTech == static_cast<int>(TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH)) {
            napi_get_named_property(env, extraValue, KITS::TagInfo::MIFARE_ULTRALIGHT_C_TYPE, &extraKeyValue);
            bool isUlC = false;
            ParseBool(env, isUlC, extraKeyValue);
            pacMap.PutBooleanValue(KITS::TagInfo::MIFARE_ULTRALIGHT_C_TYPE, isUlC);
        } else if (intTech == static_cast<int>(TagTechnology::NFC_NDEF_TECH)) {
            // parse ndef message/type/max size/read mode for ndef tag
            napi_get_named_property(env, extraValue, KITS::TagInfo::NDEF_MSG, &extraKeyValue);
            std::string ndefMessage = "";
            ParseString(env, ndefMessage, extraKeyValue);
            pacMap.PutStringValue(KITS::TagInfo::NDEF_MSG, ndefMessage);

            napi_get_named_property(env, extraValue, KITS::TagInfo::NDEF_FORUM_TYPE, &extraKeyValue);
            int32_t forumType = 0;
            ParseInt32(env, forumType, extraKeyValue);
            pacMap.PutIntValue(KITS::TagInfo::NDEF_FORUM_TYPE, forumType);

            napi_get_named_property(env, extraValue, KITS::TagInfo::NDEF_TAG_LENGTH, &extraKeyValue);
            int32_t maxNdefSize = 0;
            ParseInt32(env, maxNdefSize, extraKeyValue);
            pacMap.PutIntValue(KITS::TagInfo::NDEF_TAG_LENGTH, maxNdefSize);

            napi_get_named_property(env, extraValue, KITS::TagInfo::NDEF_TAG_MODE, &extraKeyValue);
            int32_t readMode = 0;
            ParseInt32(env, readMode, extraKeyValue);
            pacMap.PutIntValue(KITS::TagInfo::NDEF_TAG_MODE, readMode);
        } else {
        }
        // push the pacMap even if no extra data for this technology.
        tagTechExtras.push_back(pacMap);
    }
    return CreateUndefined(env);
}

std::shared_ptr<TagInfo> BuildNativeTagFromJsObj(napi_env env, napi_value obj)
{
    // parse uid: string from TagInfo object.
    napi_value uidValue = GetNamedProperty(env, obj, VAR_UID);
    std::vector<unsigned char> bytes;
    ParseBytesVector(env, bytes, uidValue);
    std::string tagUid = NfcSdkCommon::BytesVecToHexString(static_cast<unsigned char *>(bytes.data()), bytes.size());
    DebugLog("BuildNativeTagFromJsObj, tag uid:%{public}s", tagUid.c_str());

    // parse technology: number[], extrasData: PacMap[] from TagInfo object.
    std::vector<int> tagTechList;
    std::vector<AppExecFwk::PacMap> tagTechExtras;
    ParseTechAndExtraFromJsTagInfo(env, obj, tagTechList, tagTechExtras);
    DebugLog("BuildNativeTagFromJsObj, tech size %{public}zu, extra size %{public}zu",
        tagTechList.size(), tagTechExtras.size());

    // parse tagRfDiscId: number from TagInfo object.
    int tagRfDiscId = GetNapiInt32Value(env, obj, VAR_RF_ID);
    DebugLog("BuildNativeTagFromJsObj, tag RfDiscId:%{public}d", tagRfDiscId);

    // parse remoteTagService from TagInfo object.
    napi_value remoteTagSession = GetNamedProperty(env, obj, VAR_SERVICE);
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, remoteTagSession, &valueType);
    OHOS::sptr<IRemoteObject> remoteObject = nullptr;
    if (remoteTagSession != nullptr && valueType == napi_object) {
        remoteObject = NAPI_ohos_rpc_getNativeRemoteObject(env, remoteTagSession);
    }

    // remoteObject is allowed to be null, remote nfc service will initialize it again.
    if (remoteObject == nullptr) {
        WarnLog("BuildNativeTagFromJsObj, prased remoteObject is nullptr.");
    }
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
    size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, cbinfo, &argc, argv, &thisVar, nullptr));

    // check parameter number
    if (!CheckArgCountAndThrow(env, argc, ARGV_NUM_1)) {
        return CreateUndefined(env);
    }

    // check parameter data type
    napi_value tagInfoJsObj = argv[static_cast<size_t>(JS_ARGV_INDEX::ARGV_INDEX_0)];
    if (!CheckObjectAndThrow(env, tagInfoJsObj, "tagInfo", "TagInfo")) {
        return CreateUndefined(env);
    }

    // parse Taginfo parameters passed from JS
    nfcTaginfo = BuildNativeTagFromJsObj(env, tagInfoJsObj);

    // nfcTag is defined as a native instance that will be wrapped in the JS object
    NapiNfcTagSession *nfcTag = new T();
    if (!RegisterTag<D>(nfcTag, nfcTaginfo)) {
        delete nfcTag;
        napi_throw(env, GenerateBusinessError(env, BUSI_ERR_TAG_STATE_INVALID,
            BuildErrorMessage(BUSI_ERR_TAG_STATE_INVALID, "", "", "", "")));
        return CreateUndefined(env);
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
    if (!CheckUnwrapStatusAndThrow(env, status, BUSI_ERR_TAG_STATE_INVALID)) {
        return CreateUndefined(env);
    }
    return thisVar;
}

// the functions of base class for all tag sub class.
static napi_property_descriptor g_baseClassDesc[] = {
    DECLARE_NAPI_FUNCTION("getTagInfo", NapiNfcTagSession::GetTagInfo),
    DECLARE_NAPI_FUNCTION("connectTag", NapiNfcTagSession::ConnectTag),
    DECLARE_NAPI_FUNCTION("connect", NapiNfcTagSession::Connect),
    DECLARE_NAPI_FUNCTION("reset", NapiNfcTagSession::Reset),
    DECLARE_NAPI_FUNCTION("resetConnection", NapiNfcTagSession::ResetConnection),
    DECLARE_NAPI_FUNCTION("isTagConnected", NapiNfcTagSession::IsTagConnected),
    DECLARE_NAPI_FUNCTION("isConnected", NapiNfcTagSession::IsConnected),
    DECLARE_NAPI_FUNCTION("getMaxSendLength", NapiNfcTagSession::GetMaxSendLength),
    DECLARE_NAPI_FUNCTION("getMaxTransmitSize", NapiNfcTagSession::GetMaxTransmitSize),
    DECLARE_NAPI_FUNCTION("getSendDataTimeout", NapiNfcTagSession::GetSendDataTimeout),
    DECLARE_NAPI_FUNCTION("getTimeout", NapiNfcTagSession::GetTimeout),
    DECLARE_NAPI_FUNCTION("setSendDataTimeout", NapiNfcTagSession::SetSendDataTimeout),
    DECLARE_NAPI_FUNCTION("setTimeout", NapiNfcTagSession::SetTimeout),
    DECLARE_NAPI_FUNCTION("sendData", NapiNfcTagSession::SendData),
    DECLARE_NAPI_FUNCTION("transmit", NapiNfcTagSession::Transmit),
};

// merge the functions of sub class and the functions of base class.
static void MergeAllDesc(const napi_property_descriptor* subDesc, size_t subSize,
    napi_property_descriptor* allFuncDesc, size_t allDescSize)
{
    for (size_t i = 0; i < allDescSize; i++) {
        if (i < subSize) {
            allFuncDesc[i] = subDesc[i];
        } else {
            allFuncDesc[i] = g_baseClassDesc[i - subSize];
        }
    }
}

void RegisterNfcAJSClass(napi_env env)
{
    napi_property_descriptor nfcASubDesc[] = {
        DECLARE_NAPI_FUNCTION("getSak", NapiNfcATag::GetSak),
        DECLARE_NAPI_FUNCTION("getAtqa", NapiNfcATag::GetAtqa),
    };

    size_t allDescSize = (sizeof(nfcASubDesc) / sizeof(nfcASubDesc[0]))
        + (sizeof(g_baseClassDesc) / sizeof(g_baseClassDesc[0]));
    napi_property_descriptor allFuncDesc[allDescSize];
    MergeAllDesc(nfcASubDesc, (sizeof(nfcASubDesc) / sizeof(nfcASubDesc[0])), allFuncDesc, allDescSize);

    // define JS class NfcATag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "NfcATag", NAPI_AUTO_LENGTH, JS_Constructor<NapiNfcATag, NfcATag>, nullptr,
        allDescSize, allFuncDesc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &nfcAConsRef_);
}

void RegisterNfcBJSClass(napi_env env)
{
    napi_property_descriptor nfcBSubDesc[] = {
        DECLARE_NAPI_FUNCTION("getRespAppData", NapiNfcBTag::GetRespAppData),
        DECLARE_NAPI_FUNCTION("getRespProtocol", NapiNfcBTag::GetRespProtocol),
    };
    size_t allDescSize = (sizeof(nfcBSubDesc) / sizeof(nfcBSubDesc[0]))
        + (sizeof(g_baseClassDesc) / sizeof(g_baseClassDesc[0]));
    napi_property_descriptor allFuncDesc[allDescSize];
    MergeAllDesc(nfcBSubDesc, (sizeof(nfcBSubDesc) / sizeof(nfcBSubDesc[0])), allFuncDesc, allDescSize);

    // define JS class NfcBTag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "NfcBTag", NAPI_AUTO_LENGTH, JS_Constructor<NapiNfcBTag, NfcBTag>, nullptr,
        allDescSize, allFuncDesc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &nfcBConsRef_);
}

void RegisterNfcFJSClass(napi_env env)
{
    napi_property_descriptor nfcFSubDesc[] = {
        DECLARE_NAPI_FUNCTION("getSystemCode", NapiNfcFTag::GetSystemCode),
        DECLARE_NAPI_FUNCTION("getPmm", NapiNfcFTag::GetPmm),
    };
    size_t allDescSize = (sizeof(nfcFSubDesc) / sizeof(nfcFSubDesc[0]))
        + (sizeof(g_baseClassDesc) / sizeof(g_baseClassDesc[0]));
    napi_property_descriptor allFuncDesc[allDescSize];
    MergeAllDesc(nfcFSubDesc, (sizeof(nfcFSubDesc) / sizeof(nfcFSubDesc[0])), allFuncDesc, allDescSize);

    // define JS class NfcFTag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "NfcFTag", NAPI_AUTO_LENGTH, JS_Constructor<NapiNfcFTag, NfcFTag>, nullptr,
        allDescSize, allFuncDesc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &nfcFConsRef_);
}

void RegisterNfcVJSClass(napi_env env)
{
    napi_property_descriptor nfcVSubDesc[] = {
        DECLARE_NAPI_FUNCTION("getResponseFlags", NapiNfcVTag::GetResponseFlags),
        DECLARE_NAPI_FUNCTION("getDsfId", NapiNfcVTag::GetDsfId),
    };
    size_t allDescSize = (sizeof(nfcVSubDesc) / sizeof(nfcVSubDesc[0]))
        + (sizeof(g_baseClassDesc) / sizeof(g_baseClassDesc[0]));
    napi_property_descriptor allFuncDesc[allDescSize];
    MergeAllDesc(nfcVSubDesc, (sizeof(nfcVSubDesc) / sizeof(nfcVSubDesc[0])), allFuncDesc, allDescSize);

    // define JS class NfcVTag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "NfcVTag", NAPI_AUTO_LENGTH, JS_Constructor<NapiNfcVTag, Iso15693Tag>, nullptr,
        allDescSize, allFuncDesc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &nfcVConsRef_);
}

void RegisterIsoDepJSClass(napi_env env)
{
    napi_property_descriptor isoDepSubDesc[] = {
        DECLARE_NAPI_FUNCTION("getHistoricalBytes", NapiIsoDepTag::GetHistoricalBytes),
        DECLARE_NAPI_FUNCTION("getHiLayerResponse", NapiIsoDepTag::GetHiLayerResponse),
        DECLARE_NAPI_FUNCTION("isExtendedApduSupported", NapiIsoDepTag::IsExtendedApduSupported),
    };
    size_t allDescSize = (sizeof(isoDepSubDesc) / sizeof(isoDepSubDesc[0]))
        + (sizeof(g_baseClassDesc) / sizeof(g_baseClassDesc[0]));
    napi_property_descriptor allFuncDesc[allDescSize];
    MergeAllDesc(isoDepSubDesc, (sizeof(isoDepSubDesc) / sizeof(isoDepSubDesc[0])), allFuncDesc, allDescSize);

    // define JS class IsoDepTag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "IsoDepTag", NAPI_AUTO_LENGTH, JS_Constructor<NapiIsoDepTag, IsoDepTag>, nullptr,
        allDescSize, allFuncDesc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &isoDepConsRef_);
}

napi_value RegisterNdefJSClass(napi_env env, napi_value exports)
{
    // register NdefMessage object
    NapiNdefTag::RegisterNdefMessageJSClass(env, exports);

    napi_property_descriptor ndefSubDesc[] = {
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
    size_t allDescSize = (sizeof(ndefSubDesc) / sizeof(ndefSubDesc[0]))
        + (sizeof(g_baseClassDesc) / sizeof(g_baseClassDesc[0]));
    napi_property_descriptor allFuncDesc[allDescSize];
    MergeAllDesc(ndefSubDesc, (sizeof(ndefSubDesc) / sizeof(ndefSubDesc[0])), allFuncDesc, allDescSize);

    // define JS class NdefTag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "NdefTag", NAPI_AUTO_LENGTH, JS_Constructor<NapiNdefTag, NdefTag>, nullptr,
        allDescSize, allFuncDesc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &ndefConsRef_);
    return exports;
}

napi_value RegisterNdefStaticFunctions(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("createNdefMessage", NapiNdefTag::CreateNdefMessage),
        DECLARE_NAPI_FUNCTION("makeUriRecord", NapiNdefMessage::MakeUriRecord),
        DECLARE_NAPI_FUNCTION("makeTextRecord", NapiNdefMessage::MakeTextRecord),
        DECLARE_NAPI_FUNCTION("makeMimeRecord", NapiNdefMessage::MakeMimeRecord),
        DECLARE_NAPI_FUNCTION("makeExternalRecord", NapiNdefMessage::MakeExternalRecord),
        DECLARE_NAPI_FUNCTION("messageToBytes", NapiNdefMessage::MessageToBytes),
    };

    napi_value ndef = nullptr;
    napi_create_object(env, &ndef);
    napi_define_properties(env, ndef, sizeof(desc) / sizeof(desc[0]), desc);
    napi_set_named_property(env, exports, "ndef", ndef);
    return exports;
}

napi_value RegisterMifareClassicJSClass(napi_env env, napi_value exports)
{
    napi_property_descriptor mcSubDesc[] = {
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
    };
    size_t allDescSize = (sizeof(mcSubDesc) / sizeof(mcSubDesc[0]))
        + (sizeof(g_baseClassDesc) / sizeof(g_baseClassDesc[0]));
    napi_property_descriptor allFuncDesc[allDescSize];
    MergeAllDesc(mcSubDesc, (sizeof(mcSubDesc) / sizeof(mcSubDesc[0])), allFuncDesc, allDescSize);

    // define JS class MifareClassicTag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "MifareClassicTag", NAPI_AUTO_LENGTH,
        JS_Constructor<NapiMifareClassicTag, MifareClassicTag>, nullptr, allDescSize, allFuncDesc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &mifareClassicConsRef_);
    return exports;
}

napi_value RegisterMifareUltralightJSClass(napi_env env, napi_value exports)
{
    napi_property_descriptor muSubDesc[] = {
        DECLARE_NAPI_FUNCTION("readMultiplePages", NapiMifareUltralightTag::ReadMultiplePages),
        DECLARE_NAPI_FUNCTION("writeSinglePage", NapiMifareUltralightTag::WriteSinglePage),
        DECLARE_NAPI_FUNCTION("getType", NapiMifareUltralightTag::GetType),
    };
    size_t allDescSize = (sizeof(muSubDesc) / sizeof(muSubDesc[0]))
        + (sizeof(g_baseClassDesc) / sizeof(g_baseClassDesc[0]));
    napi_property_descriptor allFuncDesc[allDescSize];
    MergeAllDesc(muSubDesc, (sizeof(muSubDesc) / sizeof(muSubDesc[0])), allFuncDesc, allDescSize);

    // define JS class MifareUltralightTag, JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "MifareUltralightTag", NAPI_AUTO_LENGTH,
        JS_Constructor<NapiMifareUltralightTag, MifareUltralightTag>, nullptr, allDescSize, allFuncDesc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &mifareUltralightConsRef_);
    return exports;
}

napi_value RegisterNdefFormatableJSClass(napi_env env, napi_value exports)
{
    napi_property_descriptor formatSubDesc[] = {
        DECLARE_NAPI_FUNCTION("format", NapiNdefFormatableTag::Format),
        DECLARE_NAPI_FUNCTION("formatReadOnly", NapiNdefFormatableTag::FormatReadOnly),
    };
    size_t allDescSize = (sizeof(formatSubDesc) / sizeof(formatSubDesc[0]))
        + (sizeof(g_baseClassDesc) / sizeof(g_baseClassDesc[0]));
    napi_property_descriptor allFuncDesc[allDescSize];
    MergeAllDesc(formatSubDesc, (sizeof(formatSubDesc) / sizeof(formatSubDesc[0])), allFuncDesc, allDescSize);

    // define JS class NdefFormatableTag , JS_Constructor is the callback function
    napi_value constructor = nullptr;
    napi_define_class(env, "NdefFormatableTag ", NAPI_AUTO_LENGTH,
        JS_Constructor<NapiNdefFormatableTag, NdefFormatableTag>, nullptr, allDescSize, allFuncDesc, &constructor);
    napi_create_reference(env, constructor, INIT_REF, &ndefFormatableConsRef_);
    return exports;
}

napi_value GetSpecificTagObj(napi_env env, napi_callback_info info, napi_ref ref)
{
    if (ref == nullptr) {
        ErrorLog("GetSpecificTagObj error ref");
        return CreateUndefined(env);
    }
    std::size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {0};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    // new instance of JS object NfcATag
    napi_value result;
    napi_value constructor = nullptr;
    napi_get_reference_value(env, ref, &constructor);
    napi_new_instance(env, constructor, argc, argv, &result);
    return result;
}

napi_value GetNfcATag(napi_env env, napi_callback_info info)
{
    return GetSpecificTagObj(env, info, nfcAConsRef_);
}

napi_value GetNfcBTag(napi_env env, napi_callback_info info)
{
    return GetSpecificTagObj(env, info, nfcBConsRef_);
}

napi_value GetNfcFTag(napi_env env, napi_callback_info info)
{
    return GetSpecificTagObj(env, info, nfcFConsRef_);
}

napi_value GetNfcVTag(napi_env env, napi_callback_info info)
{
    return GetSpecificTagObj(env, info, nfcVConsRef_);
}

napi_value GetIsoDepTag(napi_env env, napi_callback_info info)
{
    return GetSpecificTagObj(env, info, isoDepConsRef_);
}

napi_value GetNdefTag(napi_env env, napi_callback_info info)
{
    return GetSpecificTagObj(env, info, ndefConsRef_);
}

napi_value GetMifareClassicTag(napi_env env, napi_callback_info info)
{
    return GetSpecificTagObj(env, info, mifareClassicConsRef_);
}

napi_value GetMifareUltralightTag(napi_env env, napi_callback_info info)
{
    return GetSpecificTagObj(env, info, mifareUltralightConsRef_);
}

napi_value GetNdefFormatableTag(napi_env env, napi_callback_info info)
{
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
            // parse sak and atqa of nfca
            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::SAK, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::SAK, propValue);

            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::ATQA, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::ATQA, propValue);
        } else if (technology == static_cast<int>(TagTechnology::NFC_B_TECH)) {
            // parse app data and protocol info of nfcb.
            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::APP_DATA, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::APP_DATA, propValue);

            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::PROTOCOL_INFO, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::PROTOCOL_INFO, propValue);
        } else if (technology == static_cast<int>(TagTechnology::NFC_F_TECH)) {
            // parse pmm and sc of nfcf
            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::NFCF_PMM, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::NFCF_PMM, propValue);

            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::NFCF_SC, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::NFCF_SC, propValue);
        } else if (technology == static_cast<int>(TagTechnology::NFC_V_TECH)) {
            // parse response flag and dsf id of nfcv.
            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::RESPONSE_FLAGS, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::RESPONSE_FLAGS, propValue);

            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::DSF_ID, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::DSF_ID, propValue);
        } else if (technology == static_cast<int>(TagTechnology::NFC_ISODEP_TECH)) {
            // parse history bytes and hilayer response of isodep.
            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::HISTORICAL_BYTES, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::HISTORICAL_BYTES, propValue);

            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::HILAYER_RESPONSE, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::HILAYER_RESPONSE, propValue);
        } else if (technology == static_cast<int>(TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH)) {
            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::MIFARE_ULTRALIGHT_C_TYPE, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::MIFARE_ULTRALIGHT_C_TYPE, propValue);
        } else if (technology == static_cast<int>(TagTechnology::NFC_NDEF_TECH)) {
            // parse ndef message/type/max size/read mode for ndef tag
            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::NDEF_MSG, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::NDEF_MSG, propValue);

            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::NDEF_FORUM_TYPE, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::NDEF_FORUM_TYPE, propValue);

            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::NDEF_TAG_LENGTH, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::NDEF_TAG_LENGTH, propValue);

            propValue = nullptr;
            napi_get_named_property(env, parameters, KITS::TagInfo::NDEF_TAG_MODE, &propValue);
            napi_set_named_property(env, eachElement, KITS::TagInfo::NDEF_TAG_MODE, propValue);
        } else {
            // set empty eachElement into extrasData to let the size same with technologies array.
        }
        napi_set_element(env, extrasData, i, eachElement);
    }
    napi_set_named_property(env, tagInfoObj, VAR_EXTRA.c_str(), extrasData);
}

napi_value BuildTagFromWantParams(napi_env env, napi_value &parameters)
{
    // the parameters is from Want JS object, parse it to build the TagInfo JS Object.
    napi_value tagInfoObj = nullptr;
    napi_create_object(env, &tagInfoObj);

    napi_value propValue = nullptr;
    napi_valuetype valueType = napi_undefined;
    napi_get_named_property(env, parameters, VAR_UID.c_str(), &propValue);
    napi_typeof(env, propValue, &valueType);
    if (propValue != nullptr && valueType == napi_string) {
        std::vector<unsigned char> bytes;
        JsStringToBytesVector(env, propValue, bytes);
        BytesVectorToJS(env, propValue, bytes);
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
    // has only one arg, want: Want
    std::size_t argc = ARGV_NUM_1;
    napi_value argv[ARGV_NUM_1] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    if (!CheckArgCountAndThrow(env, argc, ARGV_NUM_1) ||
        !CheckObjectAndThrow(env, argv[0], "want", "Want")) {
        return CreateUndefined(env);
    }

    // Get parameters?: {[key: string]: any} from want.
    napi_value want = argv[0];
    napi_value parameters = nullptr;
    napi_create_object(env, &parameters);
    napi_get_named_property(env, want, "parameters", &parameters);
    if (!CheckObjectAndThrow(env, parameters, "", "")) {
        return CreateUndefined(env);
    }

    napi_value tagInfoObj = BuildTagFromWantParams(env, parameters);
    return tagInfoObj;
}

static napi_value InitJs(napi_env env, napi_value exports)
{
    // register enum types
    RegisterEnumTnfType(env, exports);
    RegisterEnumNfcForumType(env, exports);
    RegisterEnumRtdType(env, exports);
    RegisterEnumMifareClassicType(env, exports);
    RegisterEnumMifareClassicSize(env, exports);
    RegisterEnumMifareUlType(env, exports);

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

    // register namespace 'ndef' functions
    RegisterNdefStaticFunctions(env, exports);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getNfcATag", GetNfcATag), // deprecated since 9
        DECLARE_NAPI_FUNCTION("getNfcA", GetNfcATag),
        DECLARE_NAPI_FUNCTION("getNfcBTag", GetNfcBTag), // deprecated since 9
        DECLARE_NAPI_FUNCTION("getNfcB", GetNfcBTag),
        DECLARE_NAPI_FUNCTION("getNfcFTag", GetNfcFTag), // deprecated since 9
        DECLARE_NAPI_FUNCTION("getNfcF", GetNfcFTag),
        DECLARE_NAPI_FUNCTION("getNfcVTag", GetNfcVTag), // deprecated since 9
        DECLARE_NAPI_FUNCTION("getNfcV", GetNfcVTag),
        DECLARE_NAPI_FUNCTION("getIsoDep", GetIsoDepTag),
        DECLARE_NAPI_FUNCTION("getNdef", GetNdefTag),
        DECLARE_NAPI_FUNCTION("getMifareClassic", GetMifareClassicTag),
        DECLARE_NAPI_FUNCTION("getMifareUltralight", GetMifareUltralightTag),
        DECLARE_NAPI_FUNCTION("getNdefFormatable", GetNdefFormatableTag),
        DECLARE_NAPI_FUNCTION("getTagInfo", GetTagInfo),
        DECLARE_NAPI_STATIC_PROPERTY("NFC_A", GetNapiValue(env, static_cast<int32_t>(TagTechnology::NFC_A_TECH))),
        DECLARE_NAPI_STATIC_PROPERTY("NFC_B", GetNapiValue(env, static_cast<int32_t>(TagTechnology::NFC_B_TECH))),
        DECLARE_NAPI_STATIC_PROPERTY("ISO_DEP",
            GetNapiValue(env, static_cast<int32_t>(TagTechnology::NFC_ISODEP_TECH))),
        DECLARE_NAPI_STATIC_PROPERTY("NFC_F", GetNapiValue(env, static_cast<int32_t>(TagTechnology::NFC_F_TECH))),
        DECLARE_NAPI_STATIC_PROPERTY("NFC_V", GetNapiValue(env, static_cast<int32_t>(TagTechnology::NFC_V_TECH))),
        DECLARE_NAPI_STATIC_PROPERTY("NDEF", GetNapiValue(env, static_cast<int32_t>(TagTechnology::NFC_NDEF_TECH))),
        DECLARE_NAPI_STATIC_PROPERTY("MIFARE_CLASSIC",
            GetNapiValue(env, static_cast<int32_t>(TagTechnology::NFC_MIFARE_CLASSIC_TECH))),
        DECLARE_NAPI_STATIC_PROPERTY("MIFARE_ULTRALIGHT",
            GetNapiValue(env, static_cast<int32_t>(TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH))),
        DECLARE_NAPI_STATIC_PROPERTY("NDEF_FORMATABLE",
            GetNapiValue(env, static_cast<int32_t>(TagTechnology::NFC_NDEF_FORMATABLE_TECH))),
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
