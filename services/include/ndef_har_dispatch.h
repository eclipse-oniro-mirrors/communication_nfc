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

namespace OHOS {
namespace NFC {
namespace TAG {
class NdefHarDispatch {
public:
    NdefHarDispatch();
    ~NdefHarDispatch() {}
    static NdefHarDispatch& GetInstance();
    bool DispatchBundleAbility(
        const std::string &harPackage, std::shared_ptr<KITS::TagInfo> tagInfo, const std::string &mimeType);
    bool DispatchUriToBundleAbility(const std::string &uri);
    bool DispatchMimeType(const std::string &type, std::shared_ptr<KITS::TagInfo> tagInfo);
    bool DispatchWebLink(const std::string &webAddress, const std::string &browserBundleName);
    void OnBrowserOpenLink();

private:
    static sptr<AppExecFwk::IBundleMgr> GetBundleMgrProxy();
    std::shared_mutex mutex_ {};
};
} // namespace TAG
} // namespace NFC
} // namespace OHOS
#endif // NDEF_HAR_DISPATCH_H