/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

export namespace tag {
  loadLibrary("nfc_tag_ani.z");

  export const NFC_A = 1;
  export const NFC_B = 2;
  export const ISO_DEP = 3;
  export const NFC_F = 4;
  export const NFC_V = 5;
  export const NDEF = 6;
  export const NDEF_FORMATABLE = 7;
  export const MIFARE_CLASSIC = 8;
  export const MIFARE_ULTRALIGHT = 9;
  export const NFC_BARCODE = 10;

  export enum NfcForumType {
    NFC_FORUM_TYPE_1 = 1,
    NFC_FORUM_TYPE_2 = 2,
    NFC_FORUM_TYPE_3 = 3,
    NFC_FORUM_TYPE_4 = 4,
    MIFARE_CLASSIC = 101
  }

  export native function isNfcAvailable(): boolean;
}