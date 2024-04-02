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
#ifndef TAG_ABILITY_DISPATCH_H
#define TAG_ABILITY_DISPATCH_H
#include <map>
#include <mutex>
#include "ability_info.h"
#include "element_name.h"
#include "taginfo.h"
#include "want.h"

namespace OHOS {
namespace NFC {
namespace TAG {
class TagAbilityDispatcher final {
public:
    explicit TagAbilityDispatcher();
    ~TagAbilityDispatcher();

    static void SetWantExtraParam(std::shared_ptr<KITS::TagInfo>& tagInfo, AAFwk::Want &want);
    static void DispatchTagAbility(std::shared_ptr<KITS::TagInfo> tagInfo, OHOS::sptr<IRemoteObject> tagServiceIface);
    static void DispatchAbilityMultiApp(std::shared_ptr<KITS::TagInfo> tagInfo, AAFwk::Want& want);
    static void DispatchAbilitySingleApp(AAFwk::Want& want);
    static void StartVibratorOnce();

private:
    // there is only single tag application matched to be dispatched.
    const static int TAG_APP_MATCHED_SIZE_SINGLE = 1;

    const static int DEFAULT_MOTOR_VIBRATOR_ONCE = 500; // ms
};
}  // namespace TAG
}  // namespace NFC
}  // namespace OHOS
#endif  // TAG_ABILITY_DISPATCH_H
