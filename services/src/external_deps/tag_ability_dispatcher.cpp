/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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
#include "tag_ability_dispatcher.h"
#include "ability_manager_client.h"
#include "app_data_parser.h"
#include "loghelper.h"
#include "nfc_sdk_common.h"
#include "vibrator_agent.h"

namespace OHOS {
namespace NFC {
namespace TAG {
using OHOS::NFC::KITS::TagTechnology;
using OHOS::AppExecFwk::ElementName;
TagAbilityDispatcher::TagAbilityDispatcher()
{
}

TagAbilityDispatcher::~TagAbilityDispatcher()
{
}

static void SetWantExtraParam(std::shared_ptr<KITS::TagInfo>& tagInfo, AAFwk::Want &want)
{
    // put extra data for all included technology, extra data used by 3rd party applications.
    want.SetParam("uid", tagInfo->GetTagUid());
    want.SetParam("technology", tagInfo->GetTagTechList());
    want.SetParam("tagRfDiscId", tagInfo->GetTagRfDiscId());

    std::vector<int> techList = tagInfo->GetTagTechList();
    for (size_t i = 0; i < techList.size(); i++) {
        AppExecFwk::PacMap extra = tagInfo->GetTechExtrasByIndex(i);
        if (techList[i] == static_cast<int>(TagTechnology::NFC_A_TECH)) {
            want.SetParam(KITS::TagInfo::SAK, extra.GetIntValue(KITS::TagInfo::SAK, 0));
            want.SetParam(KITS::TagInfo::ATQA, extra.GetStringValue(KITS::TagInfo::ATQA, ""));
        } else if (techList[i] == static_cast<int>(TagTechnology::NFC_B_TECH)) {
            want.SetParam(KITS::TagInfo::APP_DATA, extra.GetStringValue(KITS::TagInfo::APP_DATA, ""));
            want.SetParam(KITS::TagInfo::PROTOCOL_INFO, extra.GetStringValue(KITS::TagInfo::PROTOCOL_INFO, ""));
        } else if (techList[i] == static_cast<int>(TagTechnology::NFC_F_TECH)) {
            want.SetParam(KITS::TagInfo::NFCF_SC, extra.GetStringValue(KITS::TagInfo::NFCF_SC, ""));
            want.SetParam(KITS::TagInfo::NFCF_PMM, extra.GetStringValue(KITS::TagInfo::NFCF_PMM, ""));
        } else if (techList[i] == static_cast<int>(TagTechnology::NFC_V_TECH)) {
            want.SetParam(KITS::TagInfo::RESPONSE_FLAGS, extra.GetIntValue(KITS::TagInfo::RESPONSE_FLAGS, 0));
            want.SetParam(KITS::TagInfo::DSF_ID, extra.GetIntValue(KITS::TagInfo::DSF_ID, 0));
        } else if (techList[i] == static_cast<int>(TagTechnology::NFC_ISODEP_TECH)) {
            want.SetParam(KITS::TagInfo::HISTORICAL_BYTES, extra.GetStringValue(KITS::TagInfo::HISTORICAL_BYTES, ""));
            want.SetParam(KITS::TagInfo::HILAYER_RESPONSE, extra.GetStringValue(KITS::TagInfo::HILAYER_RESPONSE, ""));
        } else if (techList[i] == static_cast<int>(TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH)) {
            want.SetParam(KITS::TagInfo::MIFARE_ULTRALIGHT_C_TYPE,
                extra.GetBooleanValue(KITS::TagInfo::MIFARE_ULTRALIGHT_C_TYPE, false));
        } else if (techList[i] == static_cast<int>(TagTechnology::NFC_NDEF_TECH)) {
            // set ndef message/type/max size/read mode for ndef tag
            want.SetParam(KITS::TagInfo::NDEF_MSG, extra.GetStringValue(KITS::TagInfo::NDEF_MSG, ""));
            want.SetParam(KITS::TagInfo::NDEF_FORUM_TYPE, extra.GetIntValue(KITS::TagInfo::NDEF_FORUM_TYPE, 0));
            want.SetParam(KITS::TagInfo::NDEF_TAG_LENGTH, extra.GetIntValue(KITS::TagInfo::NDEF_TAG_LENGTH, 0));
            want.SetParam(KITS::TagInfo::NDEF_TAG_MODE, extra.GetIntValue(KITS::TagInfo::NDEF_TAG_MODE, 0));
        }
    }
}

void TagAbilityDispatcher::StartVibratorOnce()
{
    OHOS::Sensors::StartVibratorOnce(DEFAULT_MOTOR_VIBRATOR_ONCE);
}

void TagAbilityDispatcher::DispatchTagAbility(std::shared_ptr<KITS::TagInfo> tagInfo,
                                              OHOS::sptr<IRemoteObject> tagServiceIface)
{
    if (tagInfo == nullptr) {
        ErrorLog("DispatchTagAbility tagInfo is null");
        return;
    }
    if (tagServiceIface == nullptr) {
        WarnLog("DispatchTagAbility tagServiceIface is null");
    }

    std::vector<int> techList = tagInfo->GetTagTechList();
    std::vector<ElementName> elements = AppDataParser::GetInstance().GetDispatchTagAppsByTech(techList);
    InfoLog("DispatchTagAbility: try start ability elements size = %{public}zu", elements.size());
    if (elements.size() == 0) {
        return;
    }
#if 0
    std::vector<ElementName> vendorElements = AppDataParser::GetInstance().GetVendorDispatchTagAppsByTech(techList);
    bool isFromVendor = false;
    if (vendorElements.size() != 0) {
        isFromVendor = true;
        for (auto element : vendorElements) {
            elements.push_back(element);
        }
    }
    std::vector<std::string> elementNameList;
    for (auto element : elements) {
        std::string elementName = element.GetBundleName() + element.GetAbilityName();
        elementNameList.push_back(elementName);
    }
    AAFwk::Want want;
    const std::string PARAM_ABILITY_APPINFOS = "ohos.ability.params.appInfos";
    want.SetParam("remoteTagService", tagServiceIface);
    SetWantExtraParam(tagInfo, want);
    if (elementNameList.size() > TAG_APP_MATCHED_SIZE_SINGLE) {
        want.SetParam(PARAM_ABILITY_APPINFOS, elementNameList);
        DispatchAbilityMultiApp(tagInfo, want);
    } else if ((elements.size() == TAG_APP_MATCHED_SIZE_SINGLE) && isOH) {
        want.SetElement(elements[0]);
        DispatchAbilitySingleApp(want);
    } else if ((elements.size() == TAG_APP_MATCHED_SIZE_SINGLE) && isFromVendor) {}
#endif
    AAFwk::Want want;
    want.SetParam("remoteTagService", tagServiceIface);
    SetWantExtraParam(tagInfo, want);
    if (elements.size() == TAG_APP_MATCHED_SIZE_SINGLE) {
        want.SetElement(elements[0]);
        DispatchAbilitySingleApp(want);
    } else {
        DispatchAbilityMultiApp(tagInfo, want);
    }
}

void TagAbilityDispatcher::DispatchAbilityMultiApp(std::shared_ptr<KITS::TagInfo> tagInfo, AAFwk::Want& want)
{
    InfoLog("DispatchAbilityMultiApp for app");
    if (tagInfo == nullptr) {
        ErrorLog("DispatchTagAbility tagInfo is null");
        return;
    }

    // pull multi app page by skill.uris
    want.SetAction(KITS::ACTION_TAG_FOUND);
    std::vector<std::string> techArray;
    const std::string tagTechStr = "tag-tech/"; // exmaple: "tag-tech/NfcA"
    for (const auto& tagTech : tagInfo->GetTagTechList()) {
        if (tagTech < static_cast<int>(TagTechnology::NFC_A_TECH) ||
            tagTech > static_cast<int>(TagTechnology::NFC_MIFARE_ULTRALIGHT_TECH)) {
            WarnLog("DispatchAbilityMultiApp tagTech(%{public}d) out of range. ", tagTech);
            continue;
        }
        techArray.push_back(tagTechStr + KITS::TagInfo::GetStringTech(tagTech));
    }
    want.SetParam(AAFwk::Want::PARAM_ABILITY_URITYPES, techArray);

    if (AAFwk::AbilityManagerClient::GetInstance() == nullptr) {
        ErrorLog("DispatchAbilityMultiApp AbilityManagerClient is null");
        return;
    }

    AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    InfoLog("DispatchAbilityMultiApp call StartAbility end.");
}

void TagAbilityDispatcher::DispatchAbilitySingleApp(AAFwk::Want& want)
{
    if (AAFwk::AbilityManagerClient::GetInstance() == nullptr) {
        ErrorLog("DispatchAbilitySingleApp AbilityManagerClient is null");
        return;
    }
    AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want);
    InfoLog("DispatchAbilitySingleApp call StartAbility end.");
}
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
