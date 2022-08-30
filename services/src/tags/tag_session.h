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
#ifndef TAG_SESSION_H
#define TAG_SESSION_H

#include "infcc_host.h"
#include "infc_service.h"
#include "itag_session.h"
#include "tag_dispatcher.h"
#include "tag_session_stub.h"

namespace OHOS {
namespace NFC {
namespace TAG {
class TagSession final : public TagSessionStub {
public:
    // Constructor/Destructor
    explicit TagSession(std::shared_ptr<NFC::INfcService> service);
    ~TagSession() override;
    // copy/operator functions
    TagSession(const TagSession&) = delete;
    TagSession& operator=(const TagSession&) = delete;
    /**
     * @brief To connect the tagRfDiscId by technology.
     * @param tagRfDiscId the rf disc id of tag
     * @param technology the tag technology
     * @return the result to connect the tag
     */
    int Connect(int tagRfDiscId, int technology) override;
    /**
     * @brief To reconnect the tagRfDiscId.
     * @param tagRfDiscId the rf disc id of tag
     * @return the result to reconnect the tag
     */
    int Reconnect(int tagRfDiscId) override;
    /**
     * @brief To disconnect the tagRfDiscId.
     * @param tagRfDiscId the rf disc id of tag
     */
    void Disconnect(int tagRfDiscId) override;
    /**
     * @brief Get the TechList of the tagRfDiscId.
     * @param tagRfDiscId the rf disc id of tag
     * @return TechList
     */
    std::vector<int> GetTechList(int tagRfDiscId) override;
    /**
     * @brief Checking the tagRfDiscId is present.
     * @param tagRfDiscId the rf disc id of tag
     * @return true - Presnet; the other - No Presnet
     */
    bool IsTagFieldOn(int tagRfDiscId) override;
    /**
     * @brief Checking the tagRfDiscId is a Ndef Tag.
     * @param tagRfDiscId the rf disc id of tag
     * @return true - Ndef Tag; the other - No Ndef Tag
     */
    bool IsNdef(int tagRfDiscId) override;
    /**
     * @brief To send the data to the tagRfDiscId.
     * @param tagRfDiscId the rf disc id of tag
     * @param data the sent data
     * @param raw to send whether original data or un-original data
     * @return The response result from the host tag
     */
    std::unique_ptr<ResResult> SendRawFrame(int tagRfDiscId, std::string data, bool raw) override;
    /**
     * @brief Reading from the host tag
     * @param tagRfDiscId the rf disc id of tag
     * @return the read data
     */
    std::string NdefRead(int tagRfDiscId) override;
    /**
     * @brief Writing the data into the host tag.
     * @param tagRfDiscId the rf disc id of tag
     * @param msg the wrote data
     * @return the Writing Result
     */
    int NdefWrite(int tagRfDiscId, std::string msg) override;
    /**
     * @brief Making the host tag to read only.
     * @param tagRfDiscId the rf disc id of tag
     * @return the making result
     */
    int NdefMakeReadOnly(int tagRfDiscId) override;
    /**
     * @brief format the tag by Ndef
     * @param tagRfDiscId the rf disc id of tag
     * @param key the format key
     * @return the format result
     */
    int FormatNdef(int tagRfDiscId, const std::string& key) override;
    /**
     * @brief Checking the host tag is Read only
     * @param technology the tag technology
     * @return true - ReadOnly; false - No Read Only
     */
    bool CanMakeReadOnly(int technology) override;
    /**
     * @brief Get Max Transceive Length
     * @param technology the tag technology
     * @return Max Transceive Length
     */
    int GetMaxTransceiveLength(int technology) override;
    /**
     * @brief Checking the NfccHost whether It supported the extended Apdus
     * @return true - yes; false - no
     */
    bool IsSupportedApdusExtended() override;

    int32_t Dump(int32_t fd, const std::vector<std::u16string>& args) override;
private:
    std::string GetDumpInfo();
    std::weak_ptr<NFC::INfcService> nfcService_ {};
    std::weak_ptr<NCI::INfccHost> nfccHost_ {};
    std::weak_ptr<TagDispatcher> tagDispatcher_ {};
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_SESSION_H
