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
#ifndef OHOS_IPC_CMD_H
#define OHOS_IPC_CMD_H

/* ------------connected tag module message define--------- */
#define NFC_SVR_CMD_INIT 0x1001
#define NFC_SVR_CMD_UNINIT 0x1002
#define NFC_SVR_CMD_READ_NDEF_TAG 0x1003
#define NFC_SVR_CMD_WRITE_NDEF_TAG 0x1004

#define CMD_ON_NOTIFY 0x2001

/* ---------Feature service ability id */
#define NFC_CONNECTED_TAG_ABILITY_ID 1140

#endif