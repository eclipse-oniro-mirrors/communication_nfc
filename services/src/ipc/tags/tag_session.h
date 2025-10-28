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

#include <mutex>

#include "element_name.h"
#include "itag_session.h"
#include "nfc_service.h"
#include "tag_dispatcher.h"
#include "tag_session_stub.h"
#include "nfc_polling_manager.h"
#include "inci_tag_interface.h"
#include "app_mgr_constants.h"
#include "infc_app_state_observer.h"
#include "iforeground_callback.h"
#include "ireader_mode_callback.h"

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

    int32_t CallbackEnter(uint32_t code) override;
    int32_t CallbackExit(uint32_t code, int32_t result) override;

    /**
     * @brief To connect the tagRfDiscId by technology.
     * @param tagRfDiscId the rf disc id of tag
     * @param technology the tag technology
     * @return the result to connect the tag
     */
    ErrCode Connect(int32_t tagRfDiscId, int32_t technology) override;
    /**
    * @brief To get connection status of tag.
    * @param tagRfDiscId the rf disc id of tag
    * @param isConnected the connection status of tag
    * @return the result to get connection status of the tag
    */
    ErrCode IsConnected(int32_t tagRfDiscId, bool& isConnected) override;
    /**
     * @brief To reconnect the tagRfDiscId.
     * @param tagRfDiscId the rf disc id of tag
     * @return the result to reconnect the tag
     */
    ErrCode Reconnect(int32_t tagRfDiscId) override;
    /**
     * @brief To disconnect the tagRfDiscId.
     * @param tagRfDiscId the rf disc id of tag
     */
    ErrCode Disconnect(int32_t tagRfDiscId) override;
        /**
     * @brief Set the Timeout for tag operations
     *
     * @param timeout the timeout value to set for tag operations
     * @param technology the tag technology
     * @return true success of setting timeout value
     * @return false failure of setting timeout value
     */
    ErrCode SetTimeout(int32_t tagRfDiscId, int32_t timeout, int32_t technology) override;
    /**
     * @brief Get the Timeout value of tag operations
     * @param tagRfDiscId the rf disc id of tag
     * @param technology the tag technology
     * @param timeout the output to read the timeout value.
     * @return the status code of function calling.
     */
    ErrCode GetTimeout(int32_t tagRfDiscId, int32_t technology, int32_t& timeout) override;
    /**
     * @brief Reset the Timeout value of tag operations
     *
     * @param tagRfDiscId the rf disc id of tag
     */
    ErrCode ResetTimeout(int32_t tagRfDiscId) override;
    /**
     * @brief Get the TechList of the tagRfDiscId.
     * @param tagRfDiscId the rf disc id of tag
     * @return TechList
     */
    ErrCode GetTechList(int32_t tagRfDiscId, std::vector<int32_t>& funcResult) override;
    /**
     * @brief Checking the tagRfDiscId is present.
     * @param tagRfDiscId the rf disc id of tag
     * @return true - Presnet; the other - No Presnet
     */
    ErrCode IsTagFieldOn(int32_t tagRfDiscId, bool& funcResult) override;
    /**
     * @brief Checking the tagRfDiscId is a Ndef Tag.
     * @param tagRfDiscId the rf disc id of tag
     * @return true - Ndef Tag; the other - No Ndef Tag
     */
    ErrCode IsNdef(int32_t tagRfDiscId, bool& funcResult) override;

    ErrCode SendRawFrame(
        int32_t tagRfDiscId, const std::string& hexCmdData, bool raw, std::string& hexRespData) override;
    /**
     * @brief Reading from the host tag
     * @param tagRfDiscId the rf disc id of tag
     * @param ndefMessage the read data
     * @return the read Result
     */
    ErrCode NdefRead(int32_t tagRfDiscId, std::string& ndefMessage) override;
    /**
     * @brief Writing the data into the host tag.
     * @param tagRfDiscId the rf disc id of tag
     * @param msg the wrote data
     * @return the Writing Result
     */
    ErrCode NdefWrite(int32_t tagRfDiscId, const std::string& msg) override;
    /**
     * @brief Making the host tag to read only.
     * @param tagRfDiscId the rf disc id of tag
     * @return the making result
     */
    ErrCode NdefMakeReadOnly(int32_t tagRfDiscId) override;
    /**
     * @brief format the tag by Ndef
     * @param tagRfDiscId the rf disc id of tag
     * @param key the format key
     * @return the format result
     */
    ErrCode FormatNdef(int32_t tagRfDiscId, const std::string& key) override;

    ErrCode CanMakeReadOnly(int32_t ndefType, bool& canSetReadOnly) override;

    ErrCode GetMaxTransceiveLength(int32_t technology, int32_t& maxSize) override;

    ErrCode IsSupportedApdusExtended(bool& isSupported) override;

    /**
     * @brief register foreground dispatch
     *
     * @param element the element name of the hap that request to register foreground dispatch.
     * @param discTech the tag technologies in int array the the hap wants to discover.
     * @param callback the callback to be registered
     * @return The status code for register operation.
     */
    ErrCode RegForegroundDispatch(const ElementName& element,
        const std::vector<uint32_t>& discTech, const sptr<IForegroundCallback>& cb) override;

    /**
     * @brief unregister foreground dispatch
     *
     * @param element the element name of the hap that request to unregister foreground dispatch.
     * @return The status code for unregister operation.
     */
    ErrCode UnregForegroundDispatch(const ElementName& element) override;

    /**
     * @brief register reader mode
     *
     * @param element the element name of the hap that request to register reader mode.
     * @param discTech the tag technologies in int array the the hap wants to discover.
     * @param callback the callback to be registered
     * @return The status code for register operation.
     */
    ErrCode RegReaderMode(const ElementName& element,
        const std::vector<uint32_t>& discTech, const sptr<IReaderModeCallback>& cb) override;

    /**
     * @brief unregister reader mode
     *
     * @param element the element name of the hap that request to unregister reader mode
     * @return The status code for unregister operation.
     */
    ErrCode UnregReaderMode(const ElementName& element) override;

private:
    void CheckFgAppStateChanged(const std::string &bundleName, const std::string &abilityName, int abilityState);
    void CheckReaderAppStateChanged(const std::string &bundleName, const std::string &abilityName, int abilityState);
    bool IsFgRegistered(const ElementName &element, const std::vector<uint32_t> &discTech,
        const sptr<KITS::IForegroundCallback> &callback);
    bool IsFgUnregistered(const ElementName &element, bool isAppUnregister);
    int RegForegroundDispatchInner(const ElementName &element, const std::vector<uint32_t> &discTech,
        const sptr<KITS::IForegroundCallback> &callback, bool isVendorApp = false);
    int UnregForegroundDispatchInner(const ElementName &element, bool isAppUnregister);
    bool IsReaderRegistered(const ElementName &element, const std::vector<uint32_t> &discTech,
        const sptr<KITS::IReaderModeCallback> &callback);
    bool IsReaderUnregistered(const ElementName &element, bool isAppUnregistered);
    int RegReaderModeInner(const ElementName &element, const std::vector<uint32_t> &discTech,
        const sptr<KITS::IReaderModeCallback> &callback, bool isVendorApp = false);
    int UnregReaderModeInner(const ElementName &element, bool isAppUnregister);
    bool IsSameAppAbility(const ElementName &element, const ElementName &fgElement);
    bool IsSameDiscoveryPara(const std::vector<uint32_t> &discoveryPara, const std::vector<uint32_t> &discTech);

    uint16_t GetFgDataVecSize();
    uint16_t GetReaderDataVecSize();
    void HandleAppStateChanged(const std::string &bundleName, const std::string &abilityName,
                               int abilityState) override;

#ifdef VENDOR_APPLICATIONS_ENABLED
    bool IsVendorProcess();
#endif

    std::weak_ptr<NFC::NfcService> nfcService_ {};
    std::weak_ptr<NCI::INciTagInterface> nciTagProxy_ {};
    // polling manager
    std::weak_ptr<NfcPollingManager> nfcPollingManager_ {};
    std::vector<FgData> fgDataVec_;
    std::vector<ReaderData> readerDataVec_;
    std::mutex mutex_ {};

    sptr<KITS::IForegroundCallback> foregroundCallback_;
    sptr<KITS::IReaderModeCallback> readerModeCallback_;
    sptr<IRemoteObject::DeathRecipient> foregroundDeathRecipient_ {nullptr};
    sptr<IRemoteObject::DeathRecipient> readerModeDeathRecipient_ {nullptr};
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_SESSION_H
