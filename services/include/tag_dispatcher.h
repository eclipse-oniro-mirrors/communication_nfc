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
#ifndef TAG_DISPATCH_H
#define TAG_DISPATCH_H

#include <map>
#include <mutex>

#include "indef_msg_callback.h"
#include "inci_tag_interface.h"
#include "isodep_card_handler.h"
#include "ndef_har_data_parser.h"
#include "nfc_service.h"
#include "taginfo.h"
#include "taginfo_parcelable.h"

namespace OHOS {
namespace NFC {
class NfcService;
namespace TAG {
class TagDispatcher final {
public:
    explicit TagDispatcher(std::shared_ptr<NfcService> nfcService);
    ~TagDispatcher();
    TagDispatcher(const TagDispatcher&) = delete;
    TagDispatcher& operator=(const TagDispatcher&) = delete;

    void HandleTagFound(uint32_t rfDiscId);
    void HandleTagDebounce();
    void HandleTagLost(uint32_t rfDiscId);
    void RegNdefMsgCb(const sptr<INdefMsgCallback> &callback);
    void OnNotificationButtonClicked(int notificationId);

private:
    std::shared_ptr<KITS::TagInfo> GetTagInfoFromTag(uint32_t rfDiscId);
    KITS::TagInfoParcelable* GetTagInfoParcelableFromTag(uint32_t rfDiscId);
    void DispatchTag(uint32_t rfDiscId);
    bool HandleNdefDispatch(uint32_t tagDiscId, std::string &msg);
    void PublishTagNotification(uint32_t tagDiscId, bool isIsoDep);

private:
    std::shared_ptr<NfcService> nfcService_ {};
    std::weak_ptr<NCI::INciTagInterface> nciTagProxy_ {};

    // tag field on checking
    const static int DEFAULT_FIELD_ON_CHECK_DURATION = 125; // ms
    const static int DEFAULT_ISO_DEP_FIELD_ON_CHECK_DURATION = 500; // ms

    // ndef message
    std::string lastNdefMsg_ {};
    sptr<INdefMsgCallback> ndefCb_;

    std::shared_ptr<IsodepCardHandler> isodepCardHandler_ {};
    std::shared_ptr<NdefHarDataParser> ndefHarDataParser_ {nullptr};

    std::shared_ptr<KITS::TagInfo> tagInfo_ {};
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_DISPATCH_H
