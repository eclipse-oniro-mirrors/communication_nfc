/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#ifndef NDEF_HAR_DISPATCH_H
#define NDEF_HAR_DISPATCH_H

#include <shared_mutex>
#include <string>
#include "app_data_parser.h"
#include "if_system_ability_manager.h"
#include "taginfo.h"
#include "inci_nfcc_interface.h"
#include "iremote_object.h"
#ifdef NFC_LOCKED_HANDLE
#include "screenlock_manager.h"
#include "screenlock_common.h"
#include "screenlock_callback_stub.h"
#endif
namespace OHOS {
namespace NFC {
namespace TAG {
class NdefHarDispatch {
public:
    NdefHarDispatch(std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy);
    ~NdefHarDispatch() {}
    bool DispatchBundleAbility(const std::string &harPackage, const std::shared_ptr<KITS::TagInfo> &tagInfo,
        const std::string &mimeType, const std::string &uri, OHOS::sptr<IRemoteObject> tagServiceIface);
    bool DispatchBundleExtensionAbility(const std::string &harPackage, const std::shared_ptr<KITS::TagInfo> &tagInfo,
        const std::string &mimeType, const std::string &uri);
    bool DispatchUriToBundleAbility(const std::string &uri);
    bool DispatchMimeType(const std::string &type, const std::shared_ptr<KITS::TagInfo> &tagInfo);
    bool DispatchByAppLinkMode(const std::string &uriSchemeValue, const std::shared_ptr<KITS::TagInfo> &tagInfo,
        OHOS::sptr<IRemoteObject> tagServiceIface);
#ifdef NFC_LOCKED_HANDLE
    void UnlockStartTimer();
    static void UnlockStopTimer();
    void UnlockTimerCallback();
#endif

private:
    static sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy();
    std::shared_mutex mutex_ {};
    std::weak_ptr<NCI::INciNfccInterface> nciNfccProxy_ {};
};
#ifdef NFC_LOCKED_HANDLE
class NfcUnlockScreenCallback : public ScreenLock::ScreenLockCallbackStub {
public:
    explicit NfcUnlockScreenCallback();
    ~NfcUnlockScreenCallback() override;
    void OnCallBack(const int32_t screenLockResult) override;
};
#endif
} // namespace TAG
} // namespace NFC
} // namespace OHOS
#endif // NDEF_HAR_DISPATCH_H