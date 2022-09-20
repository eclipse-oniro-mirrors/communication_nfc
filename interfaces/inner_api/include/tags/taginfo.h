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
#ifndef TAG_INFO_H
#define TAG_INFO_H

#include "itag_session.h"
#include "nfc_basic_proxy.h"
#include "nfc_sdk_common.h"
#include "pac_map.h"
#include "parcel.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class TagInfo {
public:
    static const int MAX_TAG_TECH_NUM = 10;
    static const int SEND_COMMAND_HEAD_LEN_2 = 2;
    static const int SEND_COMMAND_MAX_LEN = 256;

    // ISODEP
    static constexpr const auto HISTORICAL_BYTES = "HistoricalBytes";
    static constexpr const auto HILAYER_RESPONSE = "HiLayerResponse";
    // iso 14443-3a
    static constexpr const auto SAK = "Sak";
    static constexpr const auto ATQA = "Atqa";
    // iso 14443-3b
    static constexpr const auto APP_DATA = "AppData";
    static constexpr const auto PROTOCOL_INFO = "ProtocolInfo";
    // NDEF
    static constexpr const auto NDEF_MSG = "NdefMsg";
    static constexpr const auto NDEF_FORUM_TYPE = "NdefForumType";
    static constexpr const auto NDEF_TAG_LENGTH = "NdefTagLength";
    static constexpr const auto NDEF_TAG_MODE = "NdefTagMode";
    // MifareUltralight
    static constexpr const auto MIFARE_ULTRALIGHT_C_TYPE = "MifareUltralightC";
    // Iso15693
    static constexpr const auto RESPONSE_FLAGS = "ResponseFlags";
    static constexpr const auto DSF_ID = "DsfId";
    // NfcF, Feilica
    static constexpr const auto NFCF_SC = "SystemCode";
    static constexpr const auto NFCF_PMM = "Pmm";

public:
    TagInfo(std::vector<int> tagTechList,
        std::vector<AppExecFwk::PacMap> tagTechExtrasData,
        std::string& tagUid,
        int tagRfDiscId,
        OHOS::sptr<IRemoteObject> tagServiceIface);
    ~TagInfo();

    std::string GetTagUid() const;
    std::vector<int> GetTagTechList() const;

    AppExecFwk::PacMap GetTechExtrasByIndex(size_t techIndex);
    AppExecFwk::PacMap GetTechExtrasByTech(KITS::TagTechnology tech);
    std::string GetStringExtrasData(AppExecFwk::PacMap& extrasData, const std::string& extrasName);
    int GetIntExtrasData(AppExecFwk::PacMap& extrasData, const std::string& extrasName);
    bool GetBoolExtrasData(AppExecFwk::PacMap& extrasData, const std::string& extrasName);

    bool IsTechSupported(KITS::TagTechnology tech);
    int GetTagRfDiscId() const;
    KITS::TagTechnology GetConnectedTagTech() const;
    void SetConnectedTagTech(KITS::TagTechnology connectedTagTech);
    OHOS::sptr<TAG::ITagSession> GetTagSessionProxy();
    static std::string GetStringTach(int tech);
private:
    int tagRfDiscId_;
    KITS::TagTechnology connectedTagTech_;
    std::string tagUid_;
    std::vector<int> tagTechList_;
    std::vector<AppExecFwk::PacMap> tagTechExtrasData_;

    OHOS::sptr<IRemoteObject> tagServiceIface_;
    OHOS::sptr<TAG::ITagSession> tagSessionProxy_;
    friend class BasicTagSession;
    friend class NdefTag;
    friend class NdefFormatableTag;
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_INFO_H
