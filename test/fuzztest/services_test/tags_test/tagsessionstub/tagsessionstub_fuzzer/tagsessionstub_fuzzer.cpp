/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
#include "tagsessionstub_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "tag_session_stub.h"
#include "tag_session.h"
#include "nfc_sdk_common.h"
#include "nfc_service_fuzz.h"
#include "nfc_service_ipc_interface_code.h"

namespace OHOS {
    using namespace OHOS::NFC::KITS;

    static constexpr const auto TAGSESSION_DESCRIPTOR = u"ohos.nfc.TAG.ITagSession";
    constexpr const auto FUZZER_THRESHOLD = 4;

    void ConvertToUint32s(const uint8_t* ptr, uint32_t* outPara, uint16_t outParaLen)
    {
        for (uint16_t i = 0 ; i < outParaLen ; i++) {
            // 4 uint8s compose 1 uint32 , 8 16 24 is bit operation, 2 3 4 are array subscripts.
            outPara[i] = (ptr[i * 4] << 24) | (ptr[(i * 4) + 1 ] << 16) | (ptr[(i * 4) + 2] << 8) | (ptr[(i * 4) + 3]);
        }
    }

    void FuzzHandleConnect(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_CONNECT),
            data2, reply, option);
    }

    void FuzzHandleReconnect(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_RECONNECT),
            data2, reply, option);
    }

    void FuzzHandleDisconnect(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_DISCONNECT),
            data2, reply, option);
    }

    void FuzzHandleSetTimeout(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_SET_TIMEOUT),
            data2, reply, option);
    }

    void FuzzHandleGetTimeout(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_GET_TIMEOUT),
            data2, reply, option);
    }

    void FuzzHandleGetTechList(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_GET_TECHLIST),
            data2, reply, option);
    }

    void FuzzHandleIsTagFieldOn(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_IS_PRESENT),
            data2, reply, option);
    }

    void FuzzHandleIsNdef(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_IS_NDEF),
            data2, reply, option);
    }

    void FuzzHandleSendRawFrame(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_SEND_RAW_FRAME),
            data2, reply, option);
    }

    void FuzzHandleNdefRead(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_NDEF_READ),
            data2, reply, option);
    }

    void FuzzHandleNdefWrite(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_NDEF_WRITE),
            data2, reply, option);
    }

    void FuzzHandleNdefMakeReadOnly(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_NDEF_MAKE_READ_ONLY),
            data2, reply, option);
    }

    void FuzzHandleFormatNdef(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_FORMAT_NDEF),
            data2, reply, option);
    }

    void FuzzHandleCanMakeReadOnly(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_CAN_MAKE_READ_ONLY),
            data2, reply, option);
    }

    void FuzzHandleGetMaxTransceiveLength(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(
            static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_GET_MAX_TRANSCEIVE_LENGTH),
            data2, reply, option);
    }

    void FuzzHandleIsSupportedApdusExtended(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(
            static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_IS_SUPPORTED_APDUS_EXTENDED),
            data2, reply, option);
    }

    void FuzzHandleRegForegroundDispatch(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(
            static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_REG_FOREGROUND),
            data2, reply, option);
    }

    void FuzzHandleUnregForegroundDispatch(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(
            static_cast<uint32_t>(NFC::NfcServiceIpcInterfaceCode::COMMAND_UNREG_FOREGROUND),
            data2, reply, option);
    }

    void FuzzOnRemoteRequest(const uint8_t* data, size_t size)
    {
        std::shared_ptr<NFC::NfcService> service = std::make_shared<NFC::NfcService>();
        sptr<NFC::TAG::TagSession> tagSession = new NFC::TAG::TagSession(service);
        uint32_t timeOutArray[1];
        ConvertToUint32s(data, timeOutArray, 1);
        MessageParcel data2;
        MessageParcel reply;
        MessageOption option;
        data2.WriteInterfaceToken(TAGSESSION_DESCRIPTOR);
        reply.WriteInt32(timeOutArray[0]);
        tagSession->OnRemoteRequest(static_cast<uint32_t>(size), data2, reply, option);
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size < OHOS::FUZZER_THRESHOLD) {
        return 0;
    }

    /* Run your code on data */
    OHOS::FuzzHandleConnect(data, size);
    OHOS::FuzzHandleReconnect(data, size);
    OHOS::FuzzHandleDisconnect(data, size);
    OHOS::FuzzHandleSetTimeout(data, size);
    OHOS::FuzzHandleGetTimeout(data, size);
    OHOS::FuzzHandleGetTechList(data, size);
    OHOS::FuzzHandleIsTagFieldOn(data, size);
    OHOS::FuzzHandleIsNdef(data, size);
    OHOS::FuzzHandleSendRawFrame(data, size);
    OHOS::FuzzHandleNdefRead(data, size);
    OHOS::FuzzHandleNdefWrite(data, size);
    OHOS::FuzzHandleNdefMakeReadOnly(data, size);
    OHOS::FuzzHandleFormatNdef(data, size);
    OHOS::FuzzHandleCanMakeReadOnly(data, size);
    OHOS::FuzzHandleGetMaxTransceiveLength(data, size);
    OHOS::FuzzHandleIsSupportedApdusExtended(data, size);
    OHOS::FuzzHandleRegForegroundDispatch(data, size);
    OHOS::FuzzHandleUnregForegroundDispatch(data, size);
    OHOS::FuzzOnRemoteRequest(data, size);

    return 0;
}

