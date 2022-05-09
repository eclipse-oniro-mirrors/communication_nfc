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
#ifndef TAG_H
#define TAG_H

#include "itag_session.h"
#include "nfc_basic_proxy.h"
#include "nfc_sdk_common.h"
#include "pac_map.h"
#include "parcel.h"

namespace OHOS {
namespace NFC {
namespace KITS {
class TagInfo final : public Parcelable {
public:
    const int MAX_TAG_TECH_NUM = 10;
    static const int SEND_COMMAND_HEAD_LEN_2 = 2;
    static const int SEND_COMMAND_MAX_LEN = 256;

    // define TagExternData Name
    static constexpr const auto TECH_EXTRA_DATA_PREFIX = "Tech_Extra_Data_";
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
    static constexpr const auto NDEF_TAG_MODE = "NdefTagMode";
    // MifareUltralight
    static constexpr const auto MIFARE_ULTRALIGHT_C_TYPE = "MifareUltralightC";
    // Iso15693
    static constexpr const auto RESPONSE_FLAGS = "ResponseFlags";
    static constexpr const auto DSF_ID = "DsfId";

public:
    TagInfo(std::vector<int> tagTechList,
        std::weak_ptr<AppExecFwk::PacMap> tagTechExtrasData,
        std::string& tagUid,
        int tagRfDiscId,
        OHOS::sptr<TAG::ITagSession> tagSession);
    ~TagInfo();

    std::string GetTagUid() const;
    std::vector<int> GetTagTechList() const;

    bool Marshalling(Parcel& parcel) const override;
    static TagInfo* Unmarshalling(Parcel& parcel);

    std::string GetStringExtrasData(AppExecFwk::PacMap& extraData, const std::string& extrasName);
    int GetIntExtrasData(AppExecFwk::PacMap extrasData, const std::string& extrasName);
    std::weak_ptr<AppExecFwk::PacMap> GetTagExtrasData() const;
    AppExecFwk::PacMap GetTechExtrasData(KITS::TagTechnology tech);
    bool IsTechSupported(KITS::TagTechnology tech);
    int GetTagRfDiscId() const;
    KITS::TagTechnology GetConnectedTagTech() const;
    void SetConnectedTagTech(KITS::TagTechnology connectedTagTech);

private:
    OHOS::sptr<TAG::ITagSession> GetRemoteTagSession() const;

private:
    int tagRfDiscId_;
    KITS::TagTechnology connectedTagTech_;
    std::string tagUid_;
    std::vector<int> tagTechList_;

    OHOS::sptr<TAG::ITagSession> remoteTagSession_;
    std::shared_ptr<AppExecFwk::PacMap> tagTechExtrasData_;
    friend class BasicTagSession;
    friend class NdefTag;
    friend class NdefFormatableTag;
};
}  // namespace KITS
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_H
