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
#ifndef PERMISSION_TOOLS_H
#define PERMISSION_TOOLS_H

#include <string>

namespace OHOS {
namespace NFC {
// the app need request the permission for open/close nfc operations.
const std::string SYS_PERM = "ohos.permission.MANAGE_SECURE_SETTINGS";

// the app need request the permission for tag operations.
const std::string TAG_PERM = "ohos.permission.NFC_TAG";

// the app need request the permission for card emulation operations.
const std::string CARD_EMU_PERM = "ohos.permission.NFC_CARD_EMULATION";

class PermissionTools {
public:
    /**
     * @Description : Check the Permission.
     * @param permission - the permission to be checked
     * @return true: granted; false: not granted
     */
    static bool IsGranted(std::string permission);
};
}  // namespace NFC
}  // namespace OHOS
#endif // PERMISSION_TOOLS_H
