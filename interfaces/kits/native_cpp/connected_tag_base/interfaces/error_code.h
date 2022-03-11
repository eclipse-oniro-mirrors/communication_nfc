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

#ifndef OHOS_NFC_ERRCODE_H
#define OHOS_NFC_ERRCODE_H

namespace OHOS {
namespace ConnectedTag {
/* Nfc errcode defines */
enum ErrCode {
    NFC_OPT_SUCCESS = 0,             /* successfully */
    NFC_OPT_FAILED,                  /* failed */
    NFC_OPT_NOT_SUPPORTED,           /* not supported */
};
}  // namespace ConnectedTag
}  // namespace OHOS
#endif