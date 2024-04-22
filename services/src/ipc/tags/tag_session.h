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
#include <shared_mutex>
#include "element_name.h"
#include "itag_session.h"
#include "nfc_service.h"
#include "tag_dispatcher.h"
#include "tag_session_stub.h"
#include "nfc_polling_manager.h"
#include "inci_tag_interface.h"
#include "app_mgr_constants.h"
#include "infc_app_state_observer.h"

namespace OHOS {
namespace NFC {
namespace TAG {
using OHOS::AppExecFwk::ElementName;
class FgData {
public:
    // Indicates whether to enable the application to be foreground dispatcher
    bool isEnableForeground_ = false;
    ElementName element_;
    std::vector<uint32_t> techs_ = {};
    sptr<KITS::IForegroundCallback> cb_ = nullptr;

    explicit FgData(bool isEnable, ElementName element, const std::vector<uint32_t> &techs,
        sptr<KITS::IForegroundCallback> cb)
        : isEnableForeground_(isEnable),
        element_(element),
        techs_(techs),
        cb_(cb) {};
    ~FgData() {};
};

class ReaderData {
public:
    // Indicates whether to enable the application to be foreground dispatcher
    bool isEnabled_ = false;
    ElementName element_;
    std::vector<uint32_t> techs_ = {};
    sptr<KITS::IReaderModeCallback> cb_ = nullptr;

    explicit ReaderData(bool isEnable, ElementName element, const std::vector<uint32_t> &techs,
        sptr<KITS::IReaderModeCallback> cb)
        : isEnabled_(isEnable),
        element_(element),
        techs_(techs),
        cb_(cb) {};
    ~ReaderData() {};
};

class TagSession final : public TagSessionStub, public INfcAppStateObserver {
public:
    // Constructor/Destructor
    explicit TagSession(std::shared_ptr<NFC::NfcService> service);
    ~TagSession() override;
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
     * @brief Set the Timeout for tag operations
     *
     * @param timeout the timeout value to set for tag operations
     * @param technology the tag technology
     * @return true success of setting timeout value
     * @return false failure of setting timeout value
     */
    int SetTimeout(int tagRfDiscId, int timeout, int technology) override;
    /**
     * @brief Get the Timeout value of tag operations
     * @param tagRfDiscId the rf disc id of tag
     * @param technology the tag technology
     * @param timeout the output to read the timeout value.
     * @return the status code of function calling.
     */
    int GetTimeout(int tagRfDiscId, int technology, int &timeout) override;
    /**
     * @brief Reset the Timeout value of tag operations
     *
     * @param tagRfDiscId the rf disc id of tag
     */
    void ResetTimeout(int tagRfDiscId) override;
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

    int SendRawFrame(const int tagRfDiscId, std::string hexCmdData, bool raw, std::string &hexRespData) override;
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

    int CanMakeReadOnly(int ndefType, bool &canSetReadOnly) override;
    int GetMaxTransceiveLength(int technology, int &maxSize) override;
    int IsSupportedApdusExtended(bool &isSupported) override;

    /**
     * @brief register foreground dispatch
     *
     * @param element the element name of the hap that request to register foreground dispatch.
     * @param discTech the tag technologies in int array the the hap wants to discover.
     * @param callback the callback to be registered
     * @return The status code for register operation.
     */
    KITS::ErrorCode RegForegroundDispatch(ElementName &element,
        std::vector<uint32_t> &discTech, const sptr<KITS::IForegroundCallback> &callback) override;

    /**
     * @brief unregister foreground dispatch
     *
     * @param element the element name of the hap that request to unregister foreground dispatch.
     * @return The status code for unregister operation.
     */
    KITS::ErrorCode UnregForegroundDispatch(ElementName &element) override;

    /**
     * @brief register reader mode
     *
     * @param element the element name of the hap that request to register reader mode.
     * @param discTech the tag technologies in int array the the hap wants to discover.
     * @param callback the callback to be registered
     * @return The status code for register operation.
     */
    KITS::ErrorCode RegReaderMode(ElementName &element,
        std::vector<uint32_t> &discTech, const sptr<KITS::IReaderModeCallback> &callback) override;

    /**
     * @brief unregister reader mode
     *
     * @param element the element name of the hap that request to unregister reader mode
     * @return The status code for unregister operation.
     */
    KITS::ErrorCode UnregReaderMode(ElementName &element) override;

    int32_t Dump(int32_t fd, const std::vector<std::u16string>& args) override;

    /**
     * @brief Get numbers of apps in registration of foregroundDispatch.
     *
     * @return FgDataVector Size.
     */
    uint16_t GetFgDataVecSize();

    /**
     * @brief Get numbers of apps in registration of readerMode.
     *
     * @return readerDataVector Size.
     */
    uint16_t GetReaderDataVecSize();

    /**
     * @brief Handle app state changed.
     *
     * @param bundleName bundle name.
     * @param abilityName ability name.
     * @param abilityState ability state.
     */
    void HandleAppStateChanged(const std::string &bundleName, const std::string &abilityName,
                               int abilityState) override;

private:
    void CheckFgAppStateChanged(const std::string &bundleName, const std::string &abilityName, int abilityState);
    void CheckReaderAppStateChanged(const std::string &bundleName, const std::string &abilityName, int abilityState);
    bool IsFgRegistered(const ElementName &element, const std::vector<uint32_t> &discTech,
        const sptr<KITS::IForegroundCallback> &callback);
    bool IsFgUnregistered(const ElementName &element, bool isAppUnregister);
    KITS::ErrorCode RegForegroundDispatchInner(ElementName &element,
        const std::vector<uint32_t> &discTech, const sptr<KITS::IForegroundCallback> &callback);
    KITS::ErrorCode UnregForegroundDispatchInner(const ElementName &element, bool isAppUnregister);
    bool IsReaderRegistered(const ElementName &element, const std::vector<uint32_t> &discTech,
        const sptr<KITS::IReaderModeCallback> &callback);
    bool IsReaderUnregistered(const ElementName &element, bool isAppUnregistered);
    KITS::ErrorCode RegReaderModeInner(ElementName &element,
        std::vector<uint32_t> &discTech, const sptr<KITS::IReaderModeCallback> &callback);
    KITS::ErrorCode UnregReaderModeInner(ElementName &element, bool isAppUnregister);
    bool IsSameAppAbility(const ElementName &element, const ElementName &fgElement);
    std::string GetDumpInfo();
    std::weak_ptr<NFC::NfcService> nfcService_ {};
    std::weak_ptr<NCI::INciTagInterface> nciTagProxy_ {};
    // polling manager
    std::weak_ptr<NfcPollingManager> nfcPollingManager_ {};
    std::vector<FgData> fgDataVec_;
    std::vector<ReaderData> readerDataVec_;
    std::shared_mutex fgMutex_;
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_SESSION_H
