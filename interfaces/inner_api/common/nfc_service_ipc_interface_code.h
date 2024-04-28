/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
#ifndef NFC_SERVICE_IPC_INTERFACE_CODE_H
#define NFC_SERVICE_IPC_INTERFACE_CODE_H

/* said: 1140 */
namespace OHOS {
namespace NFC {
class NfcServiceIpcInterfaceCode {
public:
    enum NfcControllerCode {
        COMMAND_ID = 100,
        COMMAND_GET_STATE,
        COMMAND_TURN_ON,
        COMMAND_TURN_OFF,
        COMMAND_ON_NOTIFY,
        COMMAND_REGISTER_CALLBACK,
        COMMAND_UNREGISTER_CALLBACK,
        COMMAND_IS_NFC_OPEN,
        COMMAND_GET_TAG_INTERFACE,
        COMMAND_REG_FOREGROUND,
        COMMAND_UNREG_FOREGROUND,
        COMMAND_TAG_FOUND_FOREGROUND,
        COMMAND_REG_NDEF_MSG_CALLBACK,
        COMMAND_ON_NDEF_MSG_NOTIFY,
#ifdef VENDOR_APPLICATIONS_ENABLED
        COMMAND_QUERY_APP_INFO_MSG_CALLBACK,
#endif
        COMMAND_GET_HCE_INTERFACE,
#ifdef VENDOR_APPLICATIONS_ENABLED
        COMMAND_ON_CARD_EMULATION_NOTIFY,
        COMMAND_VENDOR_NOTIFY,
#endif
        COMMAND_REG_READER_MODE,
        COMMAND_UNREG_READER_MODE,
        COMMAND_TAG_FOUND_READER_MODE,
        // The last code, if you want to add a new code, please add it before this
        COMMAND_NFC_CONTROLLER_CALLBACK_STUB_BUTT
    };

    enum TagSessionCode {
        TAG_SESSION_START_ID = 200,
        COMMAND_CONNECT,
        COMMAND_RECONNECT,
        COMMAND_DISCONNECT,
        COMMAND_GET_TECHLIST,
        COMMAND_IS_PRESENT,
        COMMAND_IS_NDEF,
        COMMAND_SEND_RAW_FRAME,
        COMMAND_NDEF_READ,
        COMMAND_NDEF_WRITE,
        COMMAND_NDEF_MAKE_READ_ONLY,
        COMMAND_FORMAT_NDEF,
        COMMAND_CAN_MAKE_READ_ONLY,
        COMMAND_GET_MAX_TRANSCEIVE_LENGTH,
        COMMAND_IS_SUPPORTED_APDUS_EXTENDED,
        COMMAND_SET_TIMEOUT,
        COMMAND_GET_TIMEOUT,
        COMMAND_RESET_TIMEOUT
    };
    enum HceSessionCode {
        COMMAND_CE_UNKNOW = 300,
        COMMAND_CE_HCE_START,
        COMMAND_CE_HCE_STOP,
        COMMAND_CE_HCE_ON,
        COMMAND_ON_CE_APDU_DATA,
        COMMAND_CE_HCE_TRANSMIT,
        COMMAND_CE_HCE_GET_PAYMENT_SERVICES,
        COMMAND_CE_HCE_IS_DEFAULT_SERVICE,
        // The last code, if you want to add a new code, please add it before this
        COMMAND_CE_HCE_SESSION_BUTT
    };
}; // NfcServiceIpcInterfaceCode
} // NFC
} // OHOS
#endif // NFC_SERVICE_IPC_INTERFACE_CODE_H