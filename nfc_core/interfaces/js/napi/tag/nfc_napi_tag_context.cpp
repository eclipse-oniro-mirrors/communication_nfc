/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "nfc_napi_tag_context.h"

#include <map>
#include "loghelper.h"

namespace OHOS {
namespace NFC {
namespace KITS {
std::map<NapiNfcATag *, std::shared_ptr<NfcATag>> tagMap;

NfcNapiTagContext &NfcNapiTagContext::GetInstance()
{
    DebugLog("NfcNapiTagContext::GetInstance");
    static NfcNapiTagContext instance;
    return instance;
}

void NfcNapiTagContext::Register(NapiNfcATag *jsObj, std::shared_ptr<NfcATag> &serviceObj)
{
    // map the service object with the js object
    tagMap[jsObj] = serviceObj;
}

std::shared_ptr<NfcATag> NfcNapiTagContext::Find(NapiNfcATag *jsObj)
{
    auto search = tagMap.find(jsObj);
    if (search != tagMap.end()) {
        return tagMap[jsObj];
    } else {
        return nullptr;
    }
}
} // namespace KITS
} // namespace NFC
} // namespace OHOS